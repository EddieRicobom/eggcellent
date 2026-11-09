# Repositórios acompanhados (upstreams)

O Eggcellent é um fork que une outros forks do Eglantine/Eden. Este arquivo lista de onde vêm as
mudanças, o que olhar em cada um e como trazê-las para a `main`.

Última revisão da lista: 2026-11-09.

## Lista

| Prioridade | Repositório | O que acompanhar | Por quê |
|---|---|---|---|
| Alta | [Djihads80/Eglantine](https://github.com/Djihads80/Eglantine) | Branch `nx2`; releases `daily-AAAAMMDD`; releases de branch (`branch-fix-*`, `branch-exp-*`); PRs abertos de OOT (#614, #613, #663, #678, #685) | Upstream principal. Quase tudo que o Eggcellent mescla vem daqui. |
| Alta | [Frizti/Lichii](https://github.com/Frizti/Lichii) | Releases `frizti-vN` e commits | Performance e menos stutter de shader no Ocarina of Time (OOT). |
| Alta | [Frizti/Fritchii](https://github.com/Frizti/Fritchii) | Releases `fritchii-vN` (v5.3 em 2026-11-09; o código novo fica nas tags, a branch `nx2` parou em 2026-11-06) | Fork do Lichii focado no OOT: correção do pisca-pisca da água, exposição em Hyrule Field, DLSS, HMMA em cooperative matrix e botão de 60/30 FPS. Testado em RTX 4080. |
| Média | [zortos293/Lichii](https://github.com/zortos293/Lichii) | PRs mergeados e commits | Fonte dos merges do Frizti. |
| Média | [sylvieistaken/selevet-eglantine](https://github.com/sylvieistaken/selevet-eglantine) | Releases `selevet-eglantine-performance-N` | Ganhos de GPU portados de várias forks. |
| Baixa | [PokeDinner-a-Eglantine-Fork](https://git.eden-emu.dev/Franseven/PokeDinner-a-Eglantine-Fork) (Forgejo) | Branch `master` e PRs | Mouse do Joy-Con 2, DK Bananza e Mario Kart World. Já mesclado uma vez. |
| Baixa | [MythosOfs/Orchid](https://github.com/MythosOfs/Orchid) | Branch `main`, PRs e docs de desempenho (Hyrule, Pokopia, Bananza) | Emulador de Switch 2 experimental, compartilha história com o Eden, mas diverge do Eglantine. Revisar commits caso a caso (cherry-pick), não mesclar a branch inteira. |
| Baixa | [nemyu-emulator/nemyu](https://gitlab.com/nemyu-emulator/nemyu) (GitLab) | Releases e changelog | Origem do frame generation já portado. |
| Só por PR | [A-Man-With-A-Plan-dev/Eglantine](https://github.com/A-Man-With-A-Plan-dev/Eglantine) | Branches e PRs | Autor do trabalho de shader do OOT (PR 614). |

Cadência sugerida: Alta, todo dia. Média, uma vez por semana. Baixa, a cada duas semanas ou quando
um PR deles aparecer no upstream.

## Candidatos ainda não mesclados

Itens do Lichii que a revisão de 2026-11-06 considerou úteis e que ainda não entraram:

- zortos293 #194: melhorias de performance portadas do Frizti, mais correções de FSR3.
- zortos293 #184: promove os 17 caminhos de performance ativos (`perf9`). Pode se sobrepor ao #194.
- zortos293 #103 e #104: menos downloads de buffer e menos stalls de GPU.
- zortos293 `7b953db74e`: pacing de presents para o processo que apresenta.
- Frizti #54, #59 e #70: menos stalls na thread GPU split. Conferir se já estão na base.

Já cobertos pela branch `fix/oot-blackscreen` do Djihads80, que foi mesclada: #121 (flashes brancos),
#100 (NaN na porta), #137 e #120 (UMWAIT/MWAITX).

Não usar: zortos293 #161 (foi excluído da Frizti v2 por geometria errada em Kokiri Forest).

## Como verificar novidades

Sem autenticação a API do GitHub limita a 60 requisições por hora.

```bash
# Últimas releases de um repositório
curl -s -H "User-Agent: x" https://api.github.com/repos/Djihads80/Eglantine/releases | head -c 3000

# PRs abertos que citam Ocarina
curl -s -H "User-Agent: x" "https://api.github.com/search/issues?q=repo:Djihads80/Eglantine+is:pr+is:open+Ocarina"
```

## Como trazer mudanças para a `main`

```bash
git fetch https://github.com/Djihads80/Eglantine.git nx2:up-nx2
git rev-list --count HEAD..up-nx2         # quantos commits faltam
git merge --no-ff up-nx2
```

Regras usadas até agora ao resolver conflitos:

1. **Shader, GPU e kernel/serviços:** vale o lado do upstream. O código do Eggcellent costuma ser uma
   versão mais antiga do mesmo arquivo.
2. **Funcionalidades só do fork:** manter o nosso lado. São as configurações de MKW, DK Bananza e OOT
   (`settings.h`, `shared_translation.cpp`), o aviso de configurações recomendadas do MKW, o patch de
   30 fps do DK em `nso.cpp`, o `README.md` e a correção das esperas de 64 bits do OOT
   (`k_address_arbiter.cpp`).
3. **Evite `git merge-file --theirs` em arquivo inteiro** quando só uma parte conflita. Resolva por
   trecho, para não perder o que só existe no nosso lado.
4. **Nunca aplique cherry-pick de um PR sobre uma base atrasada.** O PR assume código que a base ainda
   não tem e os conflitos explodem. Mescle primeiro o `nx2` e depois a branch de fix.
5. **Depois de resolver, procure sobras que não compilam**, como variáveis declaradas dentro de um
   trecho descartado (`grep` por nomes usados fora do trecho).

## Nome e ícones

Os merges do upstream trazem o nome e os ícones do Eglantine. A marca Eggcellent fica em
`dist/eggcellent.png`, `dist/eggcellent.ico` e `dist/qt_themes/default/icons/256x256/eggcellent.png`.
Um merge não deve apagar esses arquivos. Reaplicar o nome do executável e as strings de tradução é um
passo à parte, feito localmente na compilação.

## Compilação e release

O push na `main` dispara `.github/workflows/eggcellent-windows.yml`, que compila `eglantine.exe` e
`eden-cli.exe` e publica um prerelease `eggcellent-build-<id>-<tentativa>` com o
`eggcellent-windows-x86_64.zip`. **Mescle em branch de teste antes quando houver muitos conflitos**,
porque um build quebrado na `main` não gera release.
