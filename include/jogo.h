#ifndef JOGO_H
#define JOGO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "tipos.h"
#include "recursos.h"
#include "constantes.h"

// Agrupa todo o estado de uma partida em andamento (jogador, inimigos, onda atual,
// transicoes visuais, etc). Antes essas eram mais de vinte variaveis soltas dentro de
// main(), o que obrigava a passar dezenas de parametros para reiniciar o jogo.
typedef struct {
    Cavaleiro cavaleiro;
    Inimigo *inimigos[MAX_INIMIGOS];
    int num_inimigos_ativos;
    int onda_atual;

    int vitoria;

    // Transicao de tela preta entre a ultima onda comum e a onda do chefe.
    int em_transicao_onda;
    float alfa_transicao;
    int estado_fade; // 0 = nada, 1 = escurecendo, 2 = esperando, 3 = clareando.
    int timer_transicao;

    // Pausa curta entre ondas comuns, com pequena cura para o jogador.
    int interludio_onda;
    int timer_interludio;

    float alfa_fim_de_jogo; // Escurece a tela na derrota.

    int screen_shake_timer;
    float screen_shake_intensidade;

    int parry_timer;
    float parry_x;
    float parry_y;

    // Sequencia de vitoria ao derrotar o chefe (texto -> tela escurece -> tela final).
    int vitoria_em_andamento;
    int timer_vitoria;
    float alfa_vitoria;

    int jogo_pausado;
} EstadoPartida;

// Funcao de colisao basica, usando retangulos (AABB).
// Checa se as "caixas" de colisao de dois objetos estao uma por cima da outra.
int verificar_colisao(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

// Funcao para o efeito de tremer a tela.
void ativar_screen_shake(int *timer, float *intensidade, int duracao, float forca);

// Prepara uma nova onda de inimigos, limpando os antigos e criando os novos.
void iniciar_onda(int onda, Inimigo *inimigos[], int *num_inimigos_ativos);

// Aloca os MAX_INIMIGOS inimigos usados durante toda a execucao do jogo (reaproveitados
// a cada onda/partida) e prepara o estado inicial da primeira partida.
// Encerra o programa com uma mensagem de erro se a alocacao falhar.
void inicializar_partida(EstadoPartida *partida);

// Libera a memoria alocada por inicializar_partida. Deve ser chamada uma unica vez,
// ao encerrar o programa.
void finalizar_partida(EstadoPartida *partida);

// Reseta o estado de uma partida (chamado ao escolher "Iniciar Jogo" no menu).
void reiniciar_partida(EstadoPartida *partida);

// Mostra o texto "Parry!" no local onde o parry aconteceu, desaparecendo com o tempo.
void desenhar_feedback_parry(int timer, float x, float y, ALLEGRO_FONT *fonte);

// Telas do jogo. Cada uma cuida de desenhar um estado (EstadoJogo) inteiro.
void desenhar_menu(RecursosJogo *recursos, int opcao_selecionada, const char *opcoes[], int num_opcoes, int contador_frames);
void desenhar_instrucoes(RecursosJogo *recursos);
void desenhar_cena_jogo(RecursosJogo *recursos, EstadoPartida *partida, EstadoJogo estado_jogo);

#endif // JOGO_H
