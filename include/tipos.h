#ifndef TIPOS_H
#define TIPOS_H

#include <allegro5/allegro.h>

// Enum para gerenciar as telas do jogo.
typedef enum {
    ESTADO_MENU,
    ESTADO_JOGANDO,
    ESTADO_INSTRUCOES,
    ESTADO_FIM_DE_JOGO
} EstadoJogo;

// Enum para os estados de animacao do cavaleiro.
typedef enum {
    ANIM_ESTADO_PARADO,
    ANIM_ESTADO_CORRER,
    ANIM_ESTADO_ESQUIVAR,
    ANIM_ESTADO_ATACAR,
    ANIM_ESTADO_DEFENDER,
    ANIM_ESTADO_FERIDO,
    ANIM_ESTADO_MORTO
} EstadoAnimacao;

// Enum para os tipos de inimigos.
typedef enum {
    TIPO_INIMIGO_ESQUELETO,
    TIPO_INIMIGO_MINOTAURO
} TipoInimigo;

// Struct do Cavaleiro.
// Aqui vao todas as variaveis relacionadas ao jogador.
typedef struct Cavaleiro {
    float x, y;
    float vx, vy;
    int largura, altura;
    int no_chao;
    int frame_atual;
    int contador_frame;
    int direcao;
    int atacando;
    int defendendo;
    int vida;
    float stamina;
    int timer_regeneracao_stamina;
    int ferido;
    int morto;
    int animacao_morte_concluida;
    int pode_levar_dano;
    int pode_aparar;
    int timer_cooldown_ataque;
    int timer_esquiva;
    int cooldown_esquiva;
    int input_delay_timer;
    EstadoAnimacao estado_anim_atual;
    EstadoAnimacao estado_anim_anterior;
    ALLEGRO_BITMAP *spritesheet_parado;
    ALLEGRO_BITMAP *spritesheet_correr;
    ALLEGRO_BITMAP *spritesheet_ataque;
    ALLEGRO_BITMAP *spritesheet_esquiva;
    ALLEGRO_BITMAP *spritesheet_defesa;
    ALLEGRO_BITMAP *spritesheet_ferido;
    ALLEGRO_BITMAP *spritesheet_morte;
} Cavaleiro;

// Enum para os estados de animacao dos inimigos.
typedef enum {
    INIMIGO_ANIM_ESTADO_PARADO,
    INIMIGO_ANIM_ESTADO_ANDAR,
    INIMIGO_ANIM_ESTADO_ATACAR,
    INIMIGO_ANIM_ESTADO_FERIDO,
    INIMIGO_ANIM_ESTADO_MORTO
} EstadoAnimacaoInimigo;

// Struct do Inimigo. Usada tanto para os esqueletos quanto para o minotauro (chefe),
// o campo "tipo" e a flag "e_boss" e que diferenciam o comportamento de cada um.
typedef struct Inimigo {
    TipoInimigo tipo;
    float x, y;
    float vx, vy;
    int largura, altura;
    int no_chao;
    int frame_atual;
    int contador_frame;
    int direcao;
    int atacando;
    int ferido;
    int morto;
    int animacao_morte_concluida;
    int vida;
    int vida_maxima;
    int pode_levar_dano;
    int timer_invencibilidade;
    int atordoado;
    int timer_atordoamento;
    int e_boss;
    EstadoAnimacaoInimigo estado_anim_atual;
    EstadoAnimacaoInimigo estado_anim_anterior;
    ALLEGRO_BITMAP *spritesheet_parado;
    ALLEGRO_BITMAP *spritesheet_andar;
    ALLEGRO_BITMAP *spritesheet_ataque;
    ALLEGRO_BITMAP *spritesheet_ferido;
    ALLEGRO_BITMAP *spritesheet_morte;
} Inimigo;

#endif // TIPOS_H
