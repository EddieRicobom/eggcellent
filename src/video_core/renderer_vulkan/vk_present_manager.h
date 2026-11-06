// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <boost/container/deque.hpp>

#include "common/common_types.h"
#include "common/polyfill_thread.h"
#include "video_core/vulkan_common/vulkan_memory_allocator.h"
#include "video_core/vulkan_common/vulkan_wrapper.h"

namespace Core::Frontend {
class EmuWindow;
} // namespace Core::Frontend

namespace Vulkan {

class Device;
class Scheduler;
class Swapchain;

struct Frame {
    u32 width;
    u32 height;
    u32 index;
    vk::Image image;
    vk::ImageView image_view;
    vk::ImageView storage_view;
    vk::Framebuffer framebuffer;
    vk::CommandBuffer cmdbuf;
    vk::Semaphore render_ready;
    vk::Fence present_done;
    /// The present thread holds the frame until this time (spaces generated frames).
    std::chrono::steady_clock::time_point present_after{};
    /// Generated frames are not counted by the swapchain's frame rate target.
    bool generated{};
    bool storage_capable{};
};

class PresentManager {
public:
    PresentManager(const vk::Instance& instance,
                   Core::Frontend::EmuWindow& render_window,
                   const Device& device,
                   MemoryAllocator& memory_allocator,
                   Scheduler& scheduler,
                   Swapchain& swapchain,
                   vk::SurfaceKHR& surface);
    ~PresentManager();

    /// Returns the last used presentation frame
    Frame* GetRenderFrame();

    /// Pushes a frame for presentation
    void Present(Frame* frame);

    /// Recreates the present frame to match the provided parameters
    void RecreateFrame(Frame* frame, u32 width, u32 height, VkFormat image_view_format,
                       VkRenderPass rd, bool storage);

    [[nodiscard]] bool NeedsStorage(const Frame* frame, bool required) const;

    /// Waits for the present thread to finish presenting all queued frames.
    void WaitPresent();

    /// How many additional frames can be queued without stalling the render thread
    [[nodiscard]] size_t MaxExtraFrames() const;

    /// Starts the present thread for frame generation if frames are still presented on the
    /// render thread.
    void StartPresentThread();

    /// Goes back to presenting on the render thread once frame generation is off, if only frame
    /// generation started the present thread.
    void StopFrameGenPresentThread();

    /// Stops the present thread; frames still queued are not presented. For teardown, before
    /// the device is waited on.
    void StopPresentThread();

    /// How many of the wanted generated frames can be queued now without holding back the next
    /// real frame: none while the present thread is behind, and one free frame is always left.
    [[nodiscard]] size_t GeneratedFramesAllowed(size_t wanted);

    /// Frames that reached the swapchain since the last call
    [[nodiscard]] u32 TakePresentedFrames() {
        return presented_frames.exchange(0, std::memory_order_relaxed);
    }

    [[nodiscard]] std::size_t SwapchainImageCount() const { return swapchain_image_count; }
    [[nodiscard]] VkFormat SwapchainImageFormat() const { return swapchain_image_format; }

private:
    void PresentThread(std::stop_token token);

    void CopyToSwapchain(Frame* frame);

    void CopyToSwapchainImpl(Frame* frame);

    void RecreateSwapchain(Frame* frame);

    void DiscardFrame(Frame* frame);

    void SetImageCount();

    void UpdateImageCount();

    void WaitPresentTime(const Frame* frame) const;

private:
    const vk::Instance& instance;
    Core::Frontend::EmuWindow& render_window;
    const Device& device;
    MemoryAllocator& memory_allocator;
    Scheduler& scheduler;
    Swapchain& swapchain;
    vk::SurfaceKHR& surface;
    vk::CommandPool cmdpool;
    std::vector<Frame> frames;
    boost::container::deque<Frame*> present_queue;
    boost::container::deque<Frame*> free_queue;
    std::condition_variable_any frame_cv;
    std::condition_variable free_cv;
    std::mutex swapchain_mutex;
    std::mutex queue_mutex;
    std::mutex free_mutex;
    std::jthread present_thread;
    /// frames[0, activated_frames) have been put in use at least once (guarded by free_mutex)
    std::size_t activated_frames{};
    /// Frames cycling through the free and present queues (guarded by free_mutex)
    std::size_t circulating_frames{};
    /// Idle frames taken out of use after image_count dropped; they keep their resources
    std::vector<Frame*> parked_frames;
    bool blit_supported;
    bool storage_supported;
    bool use_present_thread;
    /// The present thread runs only because frame generation started it
    bool present_thread_for_frame_gen{};
    /// present_after of the frame the present thread is presenting, max() while it has none
    std::atomic<std::chrono::steady_clock::rep> presenting_after{
        std::chrono::steady_clock::time_point::max().time_since_epoch().count()};
    std::atomic<std::size_t> image_count{};
    std::atomic<std::size_t> swapchain_image_count{};
    std::atomic<VkFormat> swapchain_image_format{};
    std::atomic<u32> presented_frames{};
};

} // namespace Vulkan
