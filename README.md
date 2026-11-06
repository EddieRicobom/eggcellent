# Eggcellent

Eggcellent is an experimental Nintendo Switch emulator based on [PokeDinner](https://git.eden-emu.dev/Franseven/PokeDinner-a-Eglantine-Fork/), a fork of [Eglantine](https://github.com/Djihads80/Eglantine), which builds on [Eden](https://git.eden-emu.dev/eden-emu/eden) and yuzu. It also includes an optional patch from [Frizti/Lichii](https://github.com/Frizti/Lichii). Switch 2 support is experimental, and compatibility and performance vary.

## Windows builds

Download the Windows x86_64 ZIP from [Releases](https://github.com/EddieRicobom/eggcellent/releases), extract it, and run `eglantine.exe`. The [Windows workflow](https://github.com/EddieRicobom/eggcellent/actions/workflows/eggcellent-windows.yml) publishes an experimental release after each successful build pushed to `main`; manual runs provide a temporary build artifact. Games, firmware, and keys are not included.

## Experimental Ocarina of Time Dynamic FPS

An experimental **Ocarina of Time: Dynamic FPS** option is available in Graphics > Advanced Graphics. It is off by default. The built-in patch only applies when enabled and when the game matches title ID `0400486024DEC000` and main build ID `E0F3CCFAF50065AE4EA01EB5AEB13B1A45DA3348`. Other builds are not patched. This feature is experimental and may not work as expected.

## Attribution and licenses

Eggcellent retains upstream copyright and license notices. The source includes code under different licenses, including Eden's GPL-3.0-or-later and yuzu's GPL-2.0-or-later terms; check each file's SPDX headers and the [`LICENSES`](LICENSES) directory for applicable terms. [`LICENSE.txt`](LICENSE.txt) contains the GNU GPL version 3 text.
