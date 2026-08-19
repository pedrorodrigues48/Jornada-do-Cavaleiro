#ifndef CAVALEIRO_H
#define CAVALEIRO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include "tipos.h"

// Prepara o cavaleiro com seus valores iniciais.
// Atribui os ponteiros dos sprites (globais, carregados em recursos.c) e reseta o status.
void inicializar_cavaleiro(Cavaleiro *cavaleiro);

// Processa entrada de teclado, fisica, combate e animacao do cavaleiro em um frame.
// Recebe a lista de inimigos para poder checar a colisao do proprio ataque.
void atualizar_cavaleiro(Cavaleiro *cavaleiro, ALLEGRO_KEYBOARD_STATE *estado_teclado, Inimigo *inimigos[], int num_inimigos,
                         int *shake_timer, float *shake_intensidade, ALLEGRO_SAMPLE *som_ataque, ALLEGRO_SAMPLE *som_dano_jogador,
                         ALLEGRO_SAMPLE *som_esquiva, ALLEGRO_SAMPLE *som_dano_esqueleto, ALLEGRO_SAMPLE *som_dano_minotauro);

// Desenha o cavaleiro na tela, alem da barra de vida (coracoes) e da barra de estamina.
void desenhar_cavaleiro(Cavaleiro *cavaleiro, ALLEGRO_BITMAP *imagem_coracao);

#endif // CAVALEIRO_H
