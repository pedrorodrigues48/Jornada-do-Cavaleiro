#ifndef INIMIGO_H
#define INIMIGO_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include "tipos.h"

// Configura um inimigo com base no tipo dele (esqueleto ou minotauro): atribui os
// spritesheets certos, a vida maxima e a posicao inicial.
void inicializar_inimigo(Inimigo *inimigo, float pos_x_inicial, TipoInimigo tipo);

// Atualiza o comportamento (perseguir/atacar o jogador), fisica e animacao de um inimigo.
// "outros_inimigos" e usado apenas para eles nao ficarem um em cima do outro.
void atualizar_inimigo(Inimigo *inimigo, Cavaleiro *cavaleiro, Inimigo *outros_inimigos[], int num_inimigos, int indice_atual,
                       int *parry_timer, float *parry_x, float *parry_y, int *shake_timer, float *shake_intensidade,
                       ALLEGRO_SAMPLE *som_parry, ALLEGRO_SAMPLE *som_morte_inimigo, ALLEGRO_SAMPLE *som_ataque_esqueleto,
                       ALLEGRO_SAMPLE *som_ataque_minotauro);

// Desenha um inimigo na tela (nao desenha nada se a animacao de morte dele ja tiver acabado).
void desenhar_inimigo(Inimigo *inimigo);

// Desenha a barra de vida do chefe (Minotauro) na parte inferior da tela.
void desenhar_barra_vida_boss(Inimigo *boss, ALLEGRO_FONT *fonte);

#endif // INIMIGO_H
