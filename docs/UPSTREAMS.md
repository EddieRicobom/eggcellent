# Upstreams do Eggcellent

O Eggcellent junta vários forks do Eglantine/Eden num só emulador de Switch 2. Este arquivo diz de
onde vem o código, como trazer novidades para a `main` e o que não pode se perder no caminho. Está
escrito para quem (pessoa ou IA) continuar o trabalho sem o histórico da conversa.

Última revisão: 2026-11-11. `main` em `0b03d47f63` = Fritchii v5.7 + extras do Eggcellent.

## Regras do dono do repositório

- Pode mesclar e dar push direto na `main` (autorizado). Trabalhe numa branch de rascunho e só faça
  fast-forward na `main` depois de compilar.
- As otimizações valem para **todos os jogos**. Não filtre PRs por jogo. Os jogos que o dono testa:
  Zelda Ocarina of Time (OOT, `0400486024DEC000`), Mario Kart World (MKW), Pokopia, Nintendo Switch
  Sports Resort e Fire Emblem: Fortune's Weave.
- PC do dono: Ryzen 7 5700X, RTX 4060 8 GB, 16 GB de RAM, Windows 11. Correções só para AMD que mexem
  no estado dos pipelines podem piorar o preparo de shaders na NVIDIA: avalie antes de aplicar.
- `E:\Eden Switch 2\eggcellent` é a pasta onde o dono testa os executáveis. Nunca crie repositório
  git nela e não sobrescreva arquivos dela sem perguntar. O clone de código é
  `E:\Eden Switch 2\Fast\eggcellent`.
- Nome e ícones (marca) ficam para depois: não mude agora.
- Todo commit termina com `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Repositórios

| Prioridade | Repositório | Como trazer | Observação |
|---|---|---|---|
| Base | [Frizti/Fritchii](https://github.com/Frizti/Fritchii) | Merge da **tag** `fritchii-vX.Y` | A árvore da `main` é o Fritchii. O código novo só existe nas tags; a branch `nx2` parou em 2026-11-06. Já mesclado: v4.1, v5.3, v5.5, v5.7. |
| Alta | [zortos293/Lichii](https://github.com/zortos293/Lichii) | Cherry-pick de commits do `nx2` e de PRs (`refs/pull/N/head`) | Upstream do Fritchii. Releases `build-N`. Nunca mescle o `nx2` inteiro: conflita em centenas de trechos. |
| Média | [Djihads80/Eglantine](https://github.com/Djihads80/Eglantine) | Cherry-pick | Upstream antigo. Pouca novidade desde 2026-11-08. |
| Baixa | [MythosOfs/Orchid](https://github.com/MythosOfs/Orchid) | Cherry-pick caso a caso | Diverge muito; a maioria dos commits é de Xenoblade, Tennis, Wonder e documentação. |
| Baixa | [sylvieistaken/selevet-eglantine](https://github.com/sylvieistaken/selevet-eglantine), [PokeDinner](https://git.eden-emu.dev/Franseven/PokeDinner-a-Eglantine-Fork), [nemyu](https://gitlab.com/nemyu-emulator/nemyu), [Frizti/Lichii](https://github.com/Frizti/Lichii) | Cherry-pick | Já mesclados uma vez; olhar só quando citados. |

Os remotes não ficam salvos: busque por URL para `refs/up/*`.

```bash
export MSYS_NO_PATHCONV=1   # Git Bash no Windows: sem isso "ref:arquivo" vira caminho
git fetch -q --no-tags https://github.com/Frizti/Fritchii.git "+refs/tags/fritchii-v5.7:refs/up/fritchii-v5.7"
git fetch -q --no-tags https://github.com/zortos293/Lichii.git "+nx2:refs/up/lichii"
git fetch -q --no-tags https://github.com/zortos293/Lichii.git "+refs/pull/1143/head:refs/up/pr/1143"
git ls-remote --tags https://github.com/Frizti/Fritchii.git     # tags novas
curl -s -H "User-Agent: x" "https://api.github.com/repos/Frizti/Fritchii/releases?per_page=3"
```

Sem token, a API do GitHub aceita 60 requisições por hora.

## Como trazer uma versão nova do Fritchii

1. `git switch -c mergeNN main` e `git merge --no-commit --no-ff refs/up/fritchii-vX.Y`.
2. **Antes do merge, reverta as cópias que o Fritchii novo já traz.** Se aplicamos um PR do Lichii
   e o Fritchii também o portou, os dois lados conflitam em dezenas de trechos. Veja na lista de
   commits da tag (`git log refs/up/<tag-anterior>..refs/up/<tag-nova>`) o que ele portou, reverta a
   nossa versão e refaça o merge. Na v5.7 isso derrubou os conflitos de 103 para cerca de 40 trechos
   fáceis.
3. Resolva **por trecho**, nunca o arquivo inteiro com `--theirs`. Em geral vale o lado do Fritchii;
   mantenha o nosso só nos itens da seção abaixo.
4. Atualize a URL da receita de shaders em `.github/workflows/eggcellent-windows.yml` para a tag nova
   (`fritchii-vX.Y/Fritchii-vX.Y-Windows-x64.zip` e a pasta `Fritchii-vX.Y/recipes`).
5. Compile (seção Compilação), corrija e faça um commit "Fit ..." com os ajustes.

## O que é só do Eggcellent e precisa sobreviver aos merges

- **Frame generation visível e funcionando.** O Fritchii esconde e desliga. Mantenha
  `HidesSetting` em `src/yuzu/fritchii/fritchii_ui.cpp` escondendo só `frame_gen_dump_flow`, e
  `Settings::FrameGenActive()` em `src/common/settings.cpp` sem o teste `g_build_name == "Fritchii"`.
  O dono usa LSFG (`Lossless.dll` em `%APPDATA%\eden\lossless`).
- **Água do MKW (Lichii #690).** `PipelineCache::CurrentComputePipeline(u64& native_key)` e o
  `publish_collision_map` em `vk_rasterizer.cpp`; `src/video_core/mkw_wave_workaround.h`.
- **Proteções contra crash no cache de texturas.** `try/catch` em volta de `slot_images.insert` e o
  teste `OutOfMemory()` em `texture_cache.h`; `rt_control.Map(index) == index` na regra de clear.
- **Upload pulado em joins** (Lichii #817 e #1079): em `texture_cache.h` o `RefreshContents` fica
  depois da ordenação de `join_copies_to_do`, com `copied_whole`. Não traga de volta o bloco
  `overwritten_by_copies` do Fritchii.
- **Render targets transitórios descartados com a VRAM apertada** (Lichii #1023, para placas de 8 GB
  no Market do OOT): `transient_targets` em `texture_cache_base.h`.
- **`aud:u` com os gerenciadores registrados** (Lichii #600): `SharedManager` em
  `src/core/hle/service/audio/nx2_audio_user_client.h`. Sem isso o áudio de alguns jogos trava.
- **Zona morta da câmera do OOT no DLSS** (Lichii #678) em
  `src/video_core/host_shaders/ocarina_camera_motion.comp`.
- `frizti_settings.cpp`: o parâmetro chama `settings_values` (não `values`, que sombreia o global).
  Ao pegar a lista nova do Fritchii, troque `&values.` por `&settings_values.`.
- `src/CMakeLists.txt`: avisos de sombreamento (`/wd4456`–`/wd4459`, `/wd5245`) não são erro. O
  Fritchii compila com clang e traz esses avisos aos montes.
- CI: o passo que baixa a receita do OOT (`recipes/OoT v131072 (cache v54).nx2recipe`) do zip do
  Fritchii. O zip final precisa ter a pasta `recipes/`.

## Armadilhas conhecidas

- `src/shader_recompiler/info_serialize.cpp` tem `static_assert(sizeof(Info) == N)`. Quando um campo
  novo entra em `Shader::Info`, liste-o em `VisitInfo` e ajuste N (o nosso tem um campo a mais que o
  Fritchii, `removed_conditional_barrier`, então N é 8 maior que o dele). Se mudar o layout, mude também
  o carimbo `NX2KEPT` em `vk_pipeline_cache.cpp` e o `Magic` em `vk_precompiled_kept.cpp`.
- Depois de resolver conflitos, procure `#ifdef`/`#else`/`#endif` sem par e variáveis declaradas em
  trechos descartados.
- `git cherry` e os testes de ancestralidade mentem às vezes: o commit `235c6082b3` trocou a árvore
  pelo Fritchii v4.1 mantendo o histórico. Confira no código se algo já está lá.
- Para PRs, liste os commits com `git rev-list --reverse --no-merges $(git merge-base <base> <pr>)..<pr>`.
  Nunca use `HEAD..pr`, que arrasta a base inteira do PR.
- `-X theirs` gera híbridos (declaração sem definição). Prefira resolver à mão.

## Já avaliado: não aplicar (ou só com teste)

- Lichii #161 (geometria errada em Kokiri Forest), #818, #820 (neblina branca em Hyrule Field; o
  Fritchii reverteu), #858, #769 (não ligar por padrão), #337 (custa FPS), #1069 (fechado sem merge,
  113 commits, medido em AMD).
- Lichii #885 (RAM ao ler `.nup`), #949 e #979 (semáforos), #1197 (NUP corrompido): grandes ou
  conflitantes, só com teste em jogo.
- Fritchii PR #11 e #2 (flashes brancos), #24 (tudo opcional), #38 (DLSS Frame Generation, 9 mil
  linhas), #51 (conflita em `lower_to_maxwell.h`), #4 e #6 (rumble).

## Compilação local

- `G:\eggcellent-build\build.bat` (compila `-j 14` a partir do clone). No Git Bash:
  `cd /g/eggcellent-build && cmd //c ".\\build.bat" > buildNN.log 2>&1`. Um build completo passa de
  10 minutos: rode em segundo plano.
- Falha local conhecida e ignorável: `FAILED: bin/eglantine.exe` com `__std_*` não resolvidos (Qt 6.11
  contra MSVC 14.44). O que importa é `release/bin/eden-cli.exe` linkar e não haver `error C`.
- `error D8027` (não executa `c1xx.dll`) é falta de RAM momentânea: rode o build de novo.
- As mensagens do MSVC estão em português: procure `error C`, `LNK2019`, `LNK2001`.

## CI e release

O push na `main` dispara `.github/workflows/eggcellent-windows.yml`, que publica o prerelease
`eggcellent-build-<id>-<tentativa>` com `eggcellent-windows-x86_64.zip`. Um push novo cancela o build
em andamento: espere o anterior terminar. Se o configure falhar por 504 no download do `sha512sum`
do OpenSSL (`crueter-ci/OpenSSL`), é instabilidade do GitHub: tente de novo mais tarde.
