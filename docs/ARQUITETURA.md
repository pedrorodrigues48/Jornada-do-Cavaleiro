# Arquitetura do projeto

Este documento explica, em linhas gerais, como o programa funciona por dentro. É um material
de apoio para estudo/apresentação — não é necessário decorar, só entender o fluxo.

## Visão geral

O jogo é organizado em cinco arquivos-fonte, cada um com uma responsabilidade:

| Arquivo         | Responsabilidade                                                        |
|-----------------|---------------------------------------------------------------------------|
| `src/main.c`     | Inicializa o Allegro, cria a janela, carrega os recursos, roda o loop principal (a "máquina de estados" do jogo) e libera tudo no final. |
| `src/cavaleiro.c`| Tudo relacionado ao jogador: inicialização, entrada de teclado, física, combate e desenho. |
| `src/inimigo.c`  | Tudo relacionado aos inimigos (esqueleto e o chefe Minotauro): IA simples, física, combate e desenho. |
| `src/jogo.c`     | Regras que não pertencem a uma entidade específica: colisão (AABB), ondas de inimigos, ciclo de vida da partida (`EstadoPartida`) e o desenho das telas (menu, instruções, cena do jogo). |
| `src/recursos.c` | Carrega e libera todas as imagens, fontes e sons usados pelo jogo. |

Cada `.c` tem um `.h` correspondente em `include/` com a "interface pública" (as funções e
tipos que os outros arquivos podem usar). Isso é o que permite que, por exemplo, `main.c`
chame `atualizar_cavaleiro(...)` sem precisar saber como a função é implementada por dentro.

```
main.c
  ├─ inclui recursos.h  -> carregar_recursos(), carregar_spritesheets(), RecursosJogo
  ├─ inclui cavaleiro.h -> inicializar_cavaleiro(), atualizar_cavaleiro(), desenhar_cavaleiro()
  ├─ inclui inimigo.h   -> inicializar_inimigo(), atualizar_inimigo(), desenhar_inimigo()
  └─ inclui jogo.h      -> EstadoPartida, iniciar_onda(), reiniciar_partida(), desenhar_menu()...

cavaleiro.c e inimigo.c
  └─ usam jogo.h (verificar_colisao, ativar_screen_shake) e recursos.h (spritesheets globais)

jogo.c
  └─ usa cavaleiro.h e inimigo.h para inicializar/desenhar as entidades dentro das telas
```

Não há dependência circular: `tipos.h` e `constantes.h` não dependem de nada do projeto, e os
módulos "de entidade" (`cavaleiro`, `inimigo`) não dependem de `main.c`.

## Fluxo principal (`main.c`)

1. **Inicialização**: `al_init()` e os addons do Allegro (imagem, primitivas, fonte, TTF,
   áudio, codecs de áudio, diálogos nativos) são inicializados. A janela (`ALLEGRO_DISPLAY`),
   a fila de eventos (`ALLEGRO_EVENT_QUEUE`) e o timer de 60 FPS (`ALLEGRO_TIMER`) são criados.
   Se algum desses passos essenciais falhar, `mostrar_erro_fatal()` mostra uma caixa de
   diálogo nativa do sistema operacional explicando o problema e encerra o programa —
   em vez de continuar e travar mais adiante com um ponteiro nulo.
2. **Carregamento de recursos**: `carregar_spritesheets()` e `carregar_recursos()` carregam
   todas as imagens, fontes e sons (ver seção seguinte).
3. **Loop principal**: um `while (executando)` que:
   - Espera o próximo evento com `al_wait_for_event()` (a fila recebe eventos de teclado, de
     fechamento da janela e um "tick" do timer, 60 vezes por segundo).
   - Direciona o evento para a lógica do estado atual do jogo (`switch (estado_jogo)`):
     `ESTADO_MENU`, `ESTADO_INSTRUCOES`, `ESTADO_JOGANDO` ou `ESTADO_FIM_DE_JOGO`.
   - Quando o evento é um "tick" do timer, a lógica daquele estado é atualizada (posição do
     cavaleiro, IA dos inimigos, transições de tela, etc.) e a flag `redesenhar` é ligada.
   - Se `redesenhar` estiver ligada e não houver mais eventos pendentes na fila, a tela
     correspondente ao estado atual é desenhada (`desenhar_menu`, `desenhar_instrucoes` ou
     `desenhar_cena_jogo`) e `al_flip_display()` mostra o resultado na tela.
4. **Encerramento**: ao sair do loop (janela fechada ou "Sair" escolhido no menu), todos os
   recursos são liberados na ordem inversa em que foram criados (spritesheets, fontes, sons,
   memória dos inimigos, timer, fila de eventos, janela).

Esse padrão ("esperar evento → atualizar estado → desenhar se necessário") é comum em jogos
2D simples feitos com Allegro: ele evita desenhar a tela sem necessidade (por exemplo, um
evento de tecla que não muda nada visualmente não precisa gerar um novo frame).

## Como os dados são armazenados

- **`Cavaleiro`** (`include/tipos.h`): uma única instância, dentro de `EstadoPartida`,
  guarda posição, velocidade, vida, estamina, estado de animação atual e os ponteiros para
  os spritesheets do jogador.
- **`Inimigo`** (`include/tipos.h`): existe um vetor de **ponteiros** para até `MAX_INIMIGOS`
  (3) inimigos, alocados dinamicamente uma única vez no início do programa
  (`inicializar_partida`, em `jogo.c`) e **reaproveitados** a cada onda — a função
  `inicializar_inimigo()` reconfigura o mesmo bloco de memória para representar um esqueleto
  ou o Minotauro, dependendo da onda. Isso evita ter que alocar/desalocar memória repetidas
  vezes durante o jogo.
- **`EstadoPartida`** (`include/jogo.h`): agrupa o cavaleiro, o vetor de inimigos e todas as
  variáveis de controle de uma partida em andamento (onda atual, temporizadores de transição,
  screen shake, parry, etc.). Antes essas eram dezenas de variáveis soltas dentro de
  `main()`; agrupá-las em uma struct simplificou muito as assinaturas de função (por exemplo,
  reiniciar uma partida passou de uma função com 14 parâmetros para
  `reiniciar_partida(EstadoPartida *partida)`).
- **`RecursosJogo`** (`include/recursos.h`): agrupa fontes, imagens de cenário/HUD e todos os
  `ALLEGRO_SAMPLE`/`ALLEGRO_SAMPLE_INSTANCE` de áudio. Os spritesheets dos personagens
  (`global_cavaleiro_*`, `global_minotauro_*`, `global_esqueleto_*`) continuam como variáveis
  globais em `recursos.c`, porque são compartilhadas entre todas as instâncias daquele tipo de
  personagem (por exemplo, todo `Inimigo` do tipo esqueleto aponta para os mesmos bitmaps).

## Como o Allegro é utilizado

- **`allegro5/allegro.h`**: núcleo (janela, eventos, timer, teclado, bitmaps, transformações).
- **`allegro_image`**: carregar `.png` (`al_load_bitmap`).
- **`allegro_font` / `allegro_ttf`**: carregar e desenhar texto com a fonte `.ttf` do projeto.
- **`allegro_audio` / `allegro_acodec`**: carregar e tocar `.ogg` (efeitos com `al_play_sample`,
  músicas em loop com `ALLEGRO_SAMPLE_INSTANCE`).
- **`allegro_primitives`**: desenhar retângulos (barras de vida/estamina, fades de tela).
- **`allegro_native_dialog`**: mostrar uma caixa de mensagem nativa do sistema operacional
  quando um recurso essencial não pode ser carregado (`mostrar_erro_fatal`, em `recursos.c`).

Cada personagem é desenhado recortando um retângulo (frame) de dentro de um spritesheet
maior, usando `al_draw_scaled_bitmap` com a região de origem calculada a partir do frame
atual (`frame_col`, `frame_row`) e do número de colunas daquela animação (definido em
`include/constantes.h`).

## Como os principais sistemas se comunicam

- `main.c` é o único lugar que conhece a estrutura completa do loop de eventos; ele chama
  funções de `cavaleiro.c`, `inimigo.c` e `jogo.c` passando ponteiros para `EstadoPartida` e
  `RecursosJogo`, em vez de essas funções acessarem variáveis globais do jogo.
- A comunicação entre o ataque do cavaleiro e o dano no inimigo (e vice-versa) acontece por
  **checagem de colisão de retângulos** (`verificar_colisao`, em `jogo.c`) no frame exato em
  que a animação de ataque mostra a arma/golpe — não existe uma "lista de projéteis" ou
  similar, o dano é aplicado diretamente nos campos `vida` da struct do alvo.
- Efeitos que várias entidades podem disparar (screen shake, mensagem de "Parry!") usam
  **ponteiros de saída** (`int *shake_timer`, `float *parry_x`, etc.) para que a função de
  `cavaleiro.c`/`inimigo.c` que detectou o evento possa alterar o estado guardado em
  `EstadoPartida`, sem precisar retornar múltiplos valores.
