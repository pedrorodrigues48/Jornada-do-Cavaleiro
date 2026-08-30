# Jornada do Cavaleiro

![Build](https://github.com/pedrorodrigues48/Jornada-do-Cavaleiro/actions/workflows/build.yml/badge.svg)
![Linguagem](https://img.shields.io/badge/linguagem-C-blue)
![Biblioteca](https://img.shields.io/badge/biblioteca-Allegro%205-orange)
![Licença](https://img.shields.io/badge/licença-MIT-green)
![Plataforma](https://img.shields.io/badge/plataforma-Windows-lightgrey)

Jogo de ação 2D em C, feito com a biblioteca [Allegro 5](https://liballeg.org/), no qual um
cavaleiro enfrenta ondas de esqueletos e, por fim, um chefe (Minotauro), usando ataques,
defesa e um sistema de esquiva/parry inspirado em jogos de ação como *Dark Souls*.

Trabalho final da disciplina de **Algoritmos e Programação** (1º semestre de 2025), do curso
de **Engenharia de Computação** da **Universidade Federal de Santa Maria (UFSM)**.

### ▶️ Jogar sem compilar

Baixe o executável pronto para Windows (64 bits) na página de
[**Releases**](https://github.com/pedrorodrigues48/Jornada-do-Cavaleiro/releases/latest),
extraia o `.zip` e rode `JornadaDoCavaleiro.exe` — não precisa instalar Code::Blocks, Allegro
nem nada. Para compilar a partir do código-fonte, veja [Como executar](#como-executar).

<!--
Adicione aqui uma screenshot ou GIF do jogo, se quiser:
![Menu principal](docs/screenshots/menu.png)
-->

## Descrição

O jogador controla um cavaleiro que precisa sobreviver a três ondas de inimigos:
duas ondas de esqueletos e uma onda final contra o chefe Minotauro. O combate é baseado em
posicionamento, tempo de reação e gerenciamento de estamina: atacar e esquivar consomem
estamina, que se regenera com o tempo, e é possível **aparar (parry)** um ataque inimigo no
momento certo para atordoar o oponente.

## Objetivo

- **Acadêmico:** aplicar, em um programa completo e funcional, os principais conceitos de
  programação em C vistos na disciplina — structs, ponteiros, alocação dinâmica, enums,
  modularização em múltiplos arquivos, manipulação de arquivos de mídia, etc. (veja a seção
  [Conceitos de C utilizados](#conceitos-de-c-utilizados) e `docs/CONCEITOS_C.md`).
- **Do jogo:** derrotar as três ondas de inimigos e vencer o Minotauro sem perder toda a vida
  do cavaleiro.

## Funcionalidades

- Menu principal navegável (Iniciar Jogo / Como Jogar / Sair) e tela de instruções.
- Cavaleiro com movimentação, ataque, defesa, esquiva com invencibilidade temporária e
  sistema de parry (aparar um ataque no momento exato atordoa o inimigo).
- Sistema de estamina que limita o uso de ataque/esquiva e regenera com o tempo.
- Três ondas de inimigos (duas de esqueletos e uma do chefe Minotauro), com transições,
  interlúdios entre ondas e uma pequena cura ao final de cada onda comum.
- Barra de vida do chefe e HUD com corações (vida) e barra de estamina do jogador.
- Animações por spritesheet para cada ação (parado, correr, atacar, defender, esquivar,
  ferido, morto) do cavaleiro e de cada tipo de inimigo.
- Efeitos de tela (*screen shake* ao levar/causar dano, fade in/out nas transições de onda).
- Trilhas sonoras diferentes para menu, fase normal e fase do chefe, além de efeitos sonoros
  de ataque, dano, parry e navegação de menu.
- Pausa (tecla `P`), telas de vitória e derrota, com opção de reiniciar pelo menu.

## Tecnologias

- **C** (padrão C99/GNU), compilado com **GCC** (MinGW no Windows).
- **[Allegro 5](https://liballeg.org/)** e os addons: `image`, `primitives`, `font`, `ttf`,
  `audio`, `acodec` (Ogg Vorbis) e `native_dialog`.
- **Code::Blocks** (`JornadaDoCavaleiro.cbp`) e/ou **VSCode** (`.vscode/`) como IDE — veja as duas opções
  abaixo.

## Como executar

Duas formas de compilar, escolha a que preferir.

### Opção A — Code::Blocks (a original do projeto)

Pré-requisito: **Allegro 5.0.10 (MinGW, build estática)** instalado em
`C:/Allegro/AllegroMinGw` — caminho já configurado em `JornadaDoCavaleiro.cbp`. Se o Allegro estiver em
outro local na sua máquina, atualize em **Project → Build options → Search directories /
Linker settings**.

1. Abra `JornadaDoCavaleiro.cbp` no Code::Blocks.
2. Selecione o alvo **Debug** ou **Release**.
3. Compile e execute com **F9**.

### Opção B — VSCode + MSYS2/MinGW-w64

Pré-requisito: [MSYS2](https://www.msys2.org/) instalado, com o pacote do Allegro5:
```
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-allegro mingw-w64-x86_64-pkgconf mingw-w64-x86_64-gdb
```

1. Abra a pasta `jornada-do-cavaleiro/` no VSCode (extensão **C/C++** da Microsoft recomendada).
2. **Ctrl+Shift+B** compila (`build/JornadaDoCavaleiro.exe`); a task **"Rodar"** compila e
   executa; **F5** compila e abre com o depurador (gdb) anexado.

As tasks assumem o MSYS2 instalado em `C:\msys64` — ajuste `.vscode/tasks.json` se o seu
estiver em outro caminho.

### Observação sobre os assets

O programa carrega os assets com caminhos relativos (ex.: `assets/imagens/Idle.png`), então
o **diretório de trabalho precisa ser a raiz do projeto** (`jornada-do-cavaleiro/`) ao executar — é o que tanto o
Code::Blocks quanto as tasks do VSCode já fazem automaticamente. Se mover o `.exe` gerado
(`bin/Debug/`, `bin/Release/` ou `build/`) para fora dessa estrutura, copie a pasta `assets/`
junto, ou ele não vai encontrar as imagens/sons.

## Controles

| Tecla                | Ação                        |
|-----------------------|-----------------------------|
| Seta Esquerda / Direita | Mover o cavaleiro          |
| `Z`                    | Atacar                      |
| `X`                    | Defender / Aparar (parry)   |
| `Espaço`               | Esquivar (rolar)            |
| `P`                    | Pausar / despausar          |
| `Enter`                | Confirmar no menu / voltar ao menu |
| Setas Cima / Baixo     | Navegar no menu             |
| `Esc`                  | Voltar (na tela de instruções) |
| Fechar a janela        | Sair do jogo                |

## Estrutura do projeto

```
jornada-do-cavaleiro/
├── src/                  # Código-fonte (.c)
│   ├── main.c            # Inicializacao do Allegro, loop principal, maquina de estados
│   ├── cavaleiro.c       # Logica do jogador (entrada, fisica, combate, animacao)
│   ├── inimigo.c         # Logica dos inimigos (esqueleto e chefe Minotauro)
│   ├── jogo.c            # Regras gerais: colisao, ondas, ciclo da partida, telas (menu/HUD)
│   └── recursos.c        # Carregamento e liberacao de imagens, fontes e audio
├── include/               # Headers (.h) correspondentes a cada modulo acima
│   ├── constantes.h       # Constantes do jogo (tamanhos, velocidades, vida, etc.)
│   ├── tipos.h            # Enums e structs principais (Cavaleiro, Inimigo, estados)
│   ├── cavaleiro.h
│   ├── inimigo.h
│   ├── jogo.h
│   └── recursos.h
├── assets/
│   ├── imagens/           # Spritesheets (.png) do cavaleiro, esqueletos, minotauro e cenario
│   ├── sons/               # Efeitos sonoros e musicas (.ogg)
│   └── OptimusPrinceps.ttf # Fonte usada nos textos do jogo
├── docs/                  # Documentacao de apoio para estudo/apresentacao
│   ├── ARQUITETURA.md
│   ├── CONCEITOS_C.md
│   └── PERGUNTAS_APRESENTACAO.md
├── .vscode/               # Tasks de build/run e config de debug para quem usa VSCode
├── bin/, obj/, build/     # Saida da compilacao (gerados pelo compilador; ignorados no git)
├── JornadaDoCavaleiro.cbp               # Projeto do Code::Blocks
├── LICENSE                # Licenca MIT do codigo-fonte original
└── LICENSE-ASSETS.txt     # Licenca encontrada junto aos assets originais (ver observacao abaixo)
```

## Conceitos de C utilizados

Resumo rápido — a explicação completa, com trechos de código e localização exata, está em
`docs/CONCEITOS_C.md`:

- **Structs**: `Cavaleiro`, `Inimigo`, `RecursosJogo` e `EstadoPartida` (`include/tipos.h`,
  `include/recursos.h`, `include/jogo.h`).
- **Enums**: `EstadoJogo`, `EstadoAnimacao`, `EstadoAnimacaoInimigo`, `TipoInimigo`.
- **Ponteiros**: ponteiros para struct (`Cavaleiro *`, `Inimigo *`), ponteiros para ponteiro
  (`Inimigo *inimigos[]`), ponteiros usados como parâmetros de saída (`int *timer`, `float *x`)
  para uma função alterar variáveis do chamador.
- **Arrays**: vetor de ponteiros para os inimigos (`Inimigo *inimigos[MAX_INIMIGOS]`), vetor
  de strings do menu (`const char *opcoes_menu[]`), spritesheets tratados como grades
  (linhas/colunas) de frames.
- **Alocação dinâmica**: `malloc`/`free` dos inimigos em `jogo.c` (`inicializar_partida` /
  `finalizar_partida`), com checagem de falha de alocação.
- **Modularização**: separação em `src/`/`include/` por responsabilidade (jogador, inimigos,
  regras do jogo, recursos), com um `.h` por `.c` declarando sua interface pública.
- **Manipulação de arquivos**: carregamento de imagens (`.png`), fontes (`.ttf`) e áudio
  (`.ogg`) do disco via Allegro (`al_load_bitmap`, `al_load_font`, `al_load_sample`).
- **Máquinas de estado com enum + switch**: estado do jogo (menu/jogando/instruções/fim de
  jogo) e estado de animação de cada personagem.

## Licença

O código-fonte original (`src/`, `include/`) está sob a licença **MIT** — veja o arquivo
[`LICENSE`](LICENSE).

Os assets em `assets/` (spritesheets e efeitos sonoros) são de terceiros. O arquivo
[`LICENSE-ASSETS.txt`](LICENSE-ASSETS.txt) (GPLv3) veio junto com eles na entrega original,
mas estava como um template genérico sem preenchimento — **confirme a licença/atribuição
correta de cada asset (sprite pack e efeitos sonoros) antes de redistribuir o projeto
publicamente**, e substitua os assets por outros com licença clara se for necessário.

## Limitações conhecidas

- Três recursos opcionais não estão incluídos na pasta `assets/`: o efeito sonoro de esquiva
  (`player_dodge.ogg`), o efeito sonoro de cura entre ondas (`heal.ogg`) e o efeito sonoro de
  dano do esqueleto (`esqueleto_dano.ogg`). O jogo detecta que o arquivo não existe e
  simplesmente não toca aquele som — não é um bug, é o tratamento defensivo de recursos
  ausentes (ver `docs/PERGUNTAS_APRESENTACAO.md`). Se quiser o som completo, basta colocar um
  arquivo `.ogg` com esse nome em `assets/sons/`.
- O mesmo vale para uma camada decorativa de fundo (`fundo_montanhas.png`, não incluída).

## Autor

- Pedro Lopes Rodrigues

Trabalho final da disciplina de Algoritmos e Programação, 1º semestre de 2025 — Engenharia de
Computação, Universidade Federal de Santa Maria (UFSM).
Professor: Marcelo Serrano Zanetti
