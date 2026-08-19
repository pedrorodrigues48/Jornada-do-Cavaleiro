# Conceitos de C utilizados no projeto

Material de estudo: onde cada conceito da disciplina aparece no código, com exemplos reais
(arquivo e função). Use junto com `docs/ARQUITETURA.md` e `docs/PERGUNTAS_APRESENTACAO.md`.

## 1. Structs

- `Cavaleiro` e `Inimigo` (`include/tipos.h`): agrupam todos os atributos de uma entidade do
  jogo (posição, velocidade, vida, estado de animação, ponteiros para sprites) em um único
  tipo, em vez de dezenas de variáveis soltas.
- `RecursosJogo` (`include/recursos.h`): agrupa todos os recursos externos (fontes, imagens,
  sons) carregados uma única vez no início do jogo.
- `EstadoPartida` (`include/jogo.h`): agrupa todo o estado de uma partida em andamento
  (cavaleiro, inimigos, onda atual, temporizadores de transição). Struct dentro de struct:
  `EstadoPartida` contém um `Cavaleiro` por valor e um vetor de ponteiros para `Inimigo`.

## 2. Enums

- `EstadoJogo` (menu, jogando, instruções, fim de jogo) — controla qual "tela" está ativa.
- `EstadoAnimacao` / `EstadoAnimacaoInimigo` — qual spritesheet/animação está tocando.
- `TipoInimigo` (esqueleto ou minotauro) — usado para decidir sprites, vida máxima, dano e
  velocidade do inimigo em várias funções de `inimigo.c`.

Enums deixam o `switch` sobre o estado atual muito mais legível do que usar números "mágicos"
(`0`, `1`, `2`...) espalhados pelo código.

## 3. Ponteiros

- **Ponteiro para struct**: quase todas as funções recebem `Cavaleiro *cavaleiro` ou
  `Inimigo *inimigo` em vez de copiar a struct inteira a cada chamada — mais rápido e permite
  que a função altere a entidade original.
- **Ponteiro para ponteiro / vetor de ponteiros**: `Inimigo *inimigos[MAX_INIMIGOS]`
  (declarado em `EstadoPartida`) é um vetor onde cada posição é um ponteiro para um `Inimigo`
  alocado dinamicamente. Passado para funções como `Inimigo *outros_inimigos[]`
  (`atualizar_inimigo`, em `inimigo.c`), que decai para `Inimigo **`.
- **Ponteiros como parâmetros de saída**: como C só retorna um valor por `return`, funções que
  precisam alterar mais de uma variável do chamador recebem ponteiros para elas. Exemplo:
  `void ativar_screen_shake(int *timer, float *intensidade, int duracao, float forca)`
  (`jogo.c`) altera diretamente `partida->screen_shake_timer` e
  `partida->screen_shake_intensidade` através dos ponteiros recebidos.
- **Ponteiros para função implícitos via Allegro**: não usados diretamente no código do aluno,
  mas o próprio Allegro usa ponteiros opacos (`ALLEGRO_BITMAP *`, `ALLEGRO_FONT *`, etc.) para
  representar recursos gerenciados pela biblioteca.

## 4. Arrays

- `Inimigo *inimigos[MAX_INIMIGOS]` — vetor de ponteiros, tamanho fixo definido pela constante
  `MAX_INIMIGOS` (`include/constantes.h`).
- `const char *opcoes_menu[] = {"Iniciar Jogo", "Como Jogar", "Sair"};` (`main.c`) — vetor de
  strings usado para desenhar e navegar o menu.
- Spritesheets são tratados como uma "grade" (matriz implícita) de frames: a posição do frame
  a desenhar é calculada com `frame_col = frame_atual % colunas` e
  `frame_row = frame_atual / colunas` (`cavaleiro.c`, `inimigo.c`) — divisão e módulo inteiro
  para transformar um índice linear (0, 1, 2, ...) em coordenadas de linha/coluna.

## 5. Alocação dinâmica

- `inicializar_partida()` (`jogo.c`) usa `malloc(sizeof(Inimigo))` para alocar cada um dos
  `MAX_INIMIGOS` inimigos **uma única vez**, no início do programa. A cada onda, a mesma
  memória é reconfigurada por `inicializar_inimigo()` — não há `malloc`/`free` repetidos
  durante o jogo, só na inicialização e no encerramento.
- A alocação é **verificada**: se `malloc` retornar `NULL` (memória insuficiente), o programa
  chama `mostrar_erro_fatal()` em vez de continuar e travar com um ponteiro nulo.
- `finalizar_partida()` (`jogo.c`) libera essa memória com `free()` ao encerrar o programa.

## 6. Manipulação de arquivos

O jogo não lê/escreve arquivos de texto próprios, mas carrega arquivos binários do disco via
Allegro: imagens `.png` (`al_load_bitmap`), a fonte `.ttf` (`al_load_font`) e os áudios `.ogg`
(`al_load_sample`), todos em `src/recursos.c`. O retorno dessas funções é sempre checado
(`if (!recursos->fonte) ...`) porque `NULL` significa que o arquivo não foi encontrado ou não
pôde ser lido — o mesmo princípio de qualquer leitura de arquivo em C (`fopen` retornando
`NULL`), só que encapsulado pela biblioteca.

## 7. Passagem de parâmetros

- **Por valor**: tipos simples (`int`, `float`) quando a função só precisa *ler* o valor,
  como `TipoInimigo tipo` em `inicializar_inimigo`.
- **Por referência (ponteiro)**: sempre que a função precisa *alterar* o dado do chamador —
  praticamente todas as funções de atualização (`atualizar_cavaleiro`, `atualizar_inimigo`)
  recebem ponteiros para a entidade que vão modificar.

## 8. Estruturas condicionais e loops

- `switch` sobre enums para as máquinas de estado (estado do jogo, estado de animação).
- `for` para percorrer o vetor de inimigos, desenhar tiles do chão e colunas do menu.
- `while (executando)` como loop principal do jogo.
- Condições compostas (`&&`, `||`, `!`) para as regras de combate (ex.: só pode atacar se não
  estiver ferido, atacando ou defendendo, e tiver estamina suficiente).

## 9. Modularização

Divisão em `src/`/`include/` por responsabilidade (jogador, inimigos, regras do jogo,
recursos), cada `.c` com um `.h` correspondente declarando sua interface pública — ver
`docs/ARQUITETURA.md` para o detalhamento completo.

## 10. Algoritmos

- **Detecção de colisão AABB** (`verificar_colisao`, em `jogo.c`): algoritmo clássico de
  colisão entre dois retângulos alinhados aos eixos, usado tanto para acertar ataques quanto
  para impedir que os inimigos se sobreponham.
- **Máquina de estados finita**: tanto o estado geral do jogo quanto o estado de animação de
  cada personagem são modelados como uma máquina de estados (enum + `switch`), onde cada
  estado define quais transições são possíveis.
- **Animação por spritesheet**: conversão de um índice de frame (contador linear) para
  coordenadas de recorte na imagem (linha/coluna), usando divisão e módulo inteiro.
