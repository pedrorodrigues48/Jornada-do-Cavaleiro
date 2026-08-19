// Logica dos inimigos (esqueletos e o chefe Minotauro): inicializacao, IA/atualizacao e desenho.

#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include "../include/inimigo.h"
#include "../include/cavaleiro.h"
#include "../include/recursos.h"
#include "../include/jogo.h"
#include "../include/constantes.h"

void inicializar_inimigo(Inimigo *inimigo, float pos_x_inicial, TipoInimigo tipo) {
    inimigo->tipo = tipo;
    inimigo->e_boss = (tipo == TIPO_INIMIGO_MINOTAURO);
    if (tipo == TIPO_INIMIGO_MINOTAURO) {
        inimigo->spritesheet_parado = global_minotauro_parado;
        inimigo->spritesheet_andar = global_minotauro_andar;
        inimigo->spritesheet_ataque = global_minotauro_ataque;
        inimigo->spritesheet_ferido = global_minotauro_ferido;
        inimigo->spritesheet_morte = global_minotauro_morte;
        inimigo->largura = (int)(LARGURA_EXIBICAO_MINOTAURO * 1.5);
        inimigo->altura = (int)(ALTURA_EXIBICAO_MINOTAURO * 1.5);
        inimigo->vida = VIDA_MAXIMA_MINOTAURO;
        inimigo->vida_maxima = VIDA_MAXIMA_MINOTAURO;
    } else {
        inimigo->spritesheet_parado = global_esqueleto_parado;
        inimigo->spritesheet_andar = global_esqueleto_andar;
        inimigo->spritesheet_ataque = global_esqueleto_ataque;
        inimigo->spritesheet_ferido = global_esqueleto_ferido;
        inimigo->spritesheet_morte = global_esqueleto_morte;
        inimigo->largura = LARGURA_EXIBICAO_ESQUELETO;
        inimigo->altura = ALTURA_EXIBICAO_ESQUELETO;
        inimigo->vida = VIDA_MAXIMA_ESQUELETO;
        inimigo->vida_maxima = VIDA_MAXIMA_ESQUELETO;
    }

    inimigo->x = pos_x_inicial;
    inimigo->y = POSICAO_Y_CHAO - inimigo->altura;
    inimigo->vx = 0;
    inimigo->vy = 0;
    inimigo->no_chao = 1;
    inimigo->frame_atual = 0;
    inimigo->contador_frame = 0;
    inimigo->direcao = (pos_x_inicial < LARGURA_TELA / 2) ? 1 : -1;
    inimigo->atacando = 0;
    inimigo->ferido = 0;
    inimigo->morto = 0;
    inimigo->animacao_morte_concluida = 0;
    inimigo->pode_levar_dano = 1;
    inimigo->timer_invencibilidade = 0;
    inimigo->atordoado = 0;
    inimigo->timer_atordoamento = 0;
    inimigo->estado_anim_atual = INIMIGO_ANIM_ESTADO_PARADO;
    inimigo->estado_anim_anterior = INIMIGO_ANIM_ESTADO_PARADO;
}

void atualizar_inimigo(Inimigo *inimigo, Cavaleiro *cavaleiro, Inimigo *outros_inimigos[], int num_inimigos, int indice_atual,
                       int *parry_timer, float *parry_x, float *parry_y, int *shake_timer, float *shake_intensidade,
                       ALLEGRO_SAMPLE *som_parry, ALLEGRO_SAMPLE *som_morte_inimigo, ALLEGRO_SAMPLE *som_ataque_esqueleto,
                       ALLEGRO_SAMPLE *som_ataque_minotauro) {
    int i;
    int total_frames_morto, total_frames_ferido, total_frames_parado, total_frames_andar, total_frames_ataque;

    // Pega o numero de frames de cada animacao com base no tipo do inimigo.
    // Fizemos isso para que cada tipo de inimigo tenha animacoes de duracoes diferentes.
    if (inimigo->tipo == TIPO_INIMIGO_MINOTAURO) {
        total_frames_morto = TOTAL_FRAMES_MINOTAURO_MORTO;
        total_frames_ferido = TOTAL_FRAMES_MINOTAURO_FERIDO;
        total_frames_parado = TOTAL_FRAMES_MINOTAURO_PARADO;
        total_frames_andar = TOTAL_FRAMES_MINOTAURO_ANDAR;
        total_frames_ataque = TOTAL_FRAMES_MINOTAURO_ATAQUE;
    } else {
        total_frames_morto = TOTAL_FRAMES_ESQUELETO_MORTO;
        total_frames_ferido = TOTAL_FRAMES_ESQUELETO_FERIDO;
        total_frames_parado = TOTAL_FRAMES_ESQUELETO_PARADO;
        total_frames_andar = TOTAL_FRAMES_ESQUELETO_ANDAR;
        total_frames_ataque = TOTAL_FRAMES_ESQUELETO_ATAQUE;
    }

    // Logica de morte do inimigo.
    if (inimigo->morto) {
        if (inimigo->estado_anim_atual != INIMIGO_ANIM_ESTADO_MORTO) {
            inimigo->estado_anim_atual = INIMIGO_ANIM_ESTADO_MORTO;
            inimigo->frame_atual = 0;
            inimigo->contador_frame = 0;
        }
        inimigo->contador_frame++;
        int velocidade_animacao = 10;
        if (inimigo->contador_frame >= velocidade_animacao) {
            inimigo->contador_frame = 0;
            inimigo->frame_atual++;
            if (inimigo->frame_atual >= total_frames_morto) {
                inimigo->frame_atual = total_frames_morto - 1;
                inimigo->animacao_morte_concluida = 1;
            }
        }
        inimigo->vx = 0;
        inimigo->y += inimigo->vy;
        if (inimigo->y + inimigo->altura >= POSICAO_Y_CHAO) {
            inimigo->y = POSICAO_Y_CHAO - inimigo->altura;
            inimigo->vy = 0;
            inimigo->no_chao = 1;
        } else {
            inimigo->no_chao = 0;
            inimigo->vy += GRAVIDADE;
        }
        return;
    }

    // Se o inimigo foi acertado por um parry, ele fica atordoado por um tempo.
    if (inimigo->atordoado) {
        inimigo->timer_atordoamento--;
        if (inimigo->timer_atordoamento <= 0) {
            inimigo->atordoado = 0;
        }
        inimigo->vx = 0;
        inimigo->estado_anim_atual = INIMIGO_ANIM_ESTADO_FERIDO; // Reutiliza a animacao de ferido para o atordoamento.
        inimigo->contador_frame++;
        int velocidade_animacao = 10;
        if (inimigo->contador_frame >= velocidade_animacao) {
            inimigo->contador_frame = 0;
            inimigo->frame_atual = (inimigo->frame_atual + 1) % total_frames_ferido;
        }
        return;
    }

    // Invencibilidade do inimigo, para ele nao levar dano varias vezes seguidas.
    if (inimigo->timer_invencibilidade > 0) {
        inimigo->timer_invencibilidade--;
        if (inimigo->timer_invencibilidade == 0) {
            inimigo->pode_levar_dano = 1;
        }
    }

    inimigo->estado_anim_anterior = inimigo->estado_anim_atual;
    if (!inimigo->no_chao) inimigo->vy += GRAVIDADE;
    if (!inimigo->ferido && !inimigo->atacando) inimigo->vx = 0;

    // Comportamento do inimigo: o inimigo persegue o jogador e ataca quando chega perto.
    if (inimigo->ferido) {
        inimigo->estado_anim_atual = INIMIGO_ANIM_ESTADO_FERIDO;
        inimigo->vx = 0;
    } else if (inimigo->atacando) {
        inimigo->estado_anim_atual = INIMIGO_ANIM_ESTADO_ATACAR;
        inimigo->vx = 0;
    } else {
        float distancia_x = cavaleiro->x - inimigo->x;
        float alcance_ataque = inimigo->e_boss ? 90.0 : 70.0;

        if (cavaleiro->morto) {
            inimigo->vx = 0;
            // Se o jogador morrer, o inimigo para de se mover.
            inimigo->estado_anim_atual = INIMIGO_ANIM_ESTADO_PARADO;
        } else if (fabs(distancia_x) > alcance_ataque) {
            // Se o jogador esta longe, o inimigo anda na direcao dele.
            float velocidade_movimento_inimigo = inimigo->e_boss ? 2.5 : 2.0;
            inimigo->vx = (distancia_x > 0) ? velocidade_movimento_inimigo : -velocidade_movimento_inimigo;
            inimigo->direcao = (distancia_x > 0) ? 1 : -1;
            inimigo->estado_anim_atual = INIMIGO_ANIM_ESTADO_ANDAR;
        } else {
            // Se o jogador esta perto, o inimigo ataca.
            if (!inimigo->atacando) {
                inimigo->atacando = 1;
                inimigo->estado_anim_atual = INIMIGO_ANIM_ESTADO_ATACAR;
                inimigo->frame_atual = 0;
                inimigo->contador_frame = 0;
                inimigo->direcao = (distancia_x > 0) ? 1 : -1;
                if (inimigo->tipo != TIPO_INIMIGO_MINOTAURO) {
                    if (som_ataque_esqueleto) al_play_sample(som_ataque_esqueleto, 0.8, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
            } else {
                inimigo->estado_anim_atual = INIMIGO_ANIM_ESTADO_ATACAR;
            }
        }
    }

    // Logica simples para impedir que os inimigos fiquem um dentro do outro.
    float proximo_x = inimigo->x + inimigo->vx;
    for (i = 0; i < num_inimigos; i++) {
        if (i == indice_atual) continue;
        Inimigo *outro_inimigo = outros_inimigos[i];
        if (!outro_inimigo->morto && verificar_colisao(proximo_x, inimigo->y, inimigo->largura, inimigo->altura, outro_inimigo->x, outro_inimigo->y, outro_inimigo->largura, outro_inimigo->altura)) {
            inimigo->vx = 0; // Se for encostar no outro, para de se mover.
            break;
        }
    }

    // Aplica a fisica e o movimento.
    inimigo->x += inimigo->vx;
    inimigo->y += inimigo->vy;
    if (inimigo->estado_anim_atual == INIMIGO_ANIM_ESTADO_ANDAR && inimigo->vx == 0) {
        inimigo->estado_anim_atual = INIMIGO_ANIM_ESTADO_PARADO;
    }

    if (inimigo->y + inimigo->altura >= POSICAO_Y_CHAO) {
        inimigo->y = POSICAO_Y_CHAO - inimigo->altura;
        inimigo->vy = 0;
        inimigo->no_chao = 1;
    } else {
        inimigo->no_chao = 0;
    }

    if (inimigo->estado_anim_atual != inimigo->estado_anim_anterior) {
        inimigo->frame_atual = 0;
        inimigo->contador_frame = 0;
    }

    // Avanca os frames da animacao do inimigo.
    inimigo->contador_frame++;
    int velocidade_animacao_inimigo = 10;
    switch (inimigo->estado_anim_atual) {
        case INIMIGO_ANIM_ESTADO_PARADO:
            if (inimigo->contador_frame >= velocidade_animacao_inimigo) {
                inimigo->contador_frame = 0;
                inimigo->frame_atual = (inimigo->frame_atual + 1) % total_frames_parado;
            }
            break;
        case INIMIGO_ANIM_ESTADO_ANDAR:
            if (inimigo->contador_frame >= velocidade_animacao_inimigo) {
                inimigo->contador_frame = 0;
                inimigo->frame_atual = (inimigo->frame_atual + 1) % total_frames_andar;
            }
            break;
        case INIMIGO_ANIM_ESTADO_ATACAR:
            velocidade_animacao_inimigo = inimigo->e_boss ? 12 : 14;
            if (inimigo->contador_frame >= velocidade_animacao_inimigo) {
                inimigo->contador_frame = 0;
                inimigo->frame_atual++;
                if (inimigo->frame_atual >= total_frames_ataque) {
                    inimigo->frame_atual = 0;
                    inimigo->atacando = 0;
                    inimigo->estado_anim_atual = (inimigo->vx == 0) ? INIMIGO_ANIM_ESTADO_PARADO : INIMIGO_ANIM_ESTADO_ANDAR;
                }

                // Sincronizei melhor o som do ataque do Minotauro com a animacao.
                if (inimigo->frame_atual == 2 && inimigo->tipo == TIPO_INIMIGO_MINOTAURO) {
                    if (som_ataque_minotauro) al_play_sample(som_ataque_minotauro, 1.0, 0.0, 1.2, ALLEGRO_PLAYMODE_ONCE, NULL);
                }

                // A checagem de dano tambem ocorre em um frame especifico do ataque.
                if (inimigo->frame_atual == 3 && inimigo->atacando) {
                    float offset_x_ataque = (inimigo->direcao == 1) ? inimigo->largura * 0.6 : -inimigo->largura * 0.25;
                    float ataque_x = inimigo->x + offset_x_ataque;
                    float ataque_y = inimigo->y + inimigo->altura * 0.3;
                    float ataque_w = inimigo->largura * 0.25;
                    float ataque_h = inimigo->altura * 0.4;
                    if (verificar_colisao(ataque_x, ataque_y, ataque_w, ataque_h, cavaleiro->x, cavaleiro->y, cavaleiro->largura, cavaleiro->altura) && !cavaleiro->morto && cavaleiro->pode_levar_dano) {

                        // Se o jogador defende no momento exato do ataque, ele consegue um parry.
                        if (cavaleiro->pode_aparar) {
                            inimigo->atordoado = 1;
                            inimigo->timer_atordoamento = DURACAO_ATORDOAMENTO;
                            inimigo->atacando = 0;

                            *parry_timer = 30; // Ativa a mensagem de "Parry!".
                            *parry_x = cavaleiro->x + cavaleiro->largura * 0.5;
                            *parry_y = cavaleiro->y + cavaleiro->altura * 0.5;
                            if (som_parry) {
                                al_play_sample(som_parry, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                            }

                        } else {
                            // Se nao for parry, o jogador leva dano.
                            if (cavaleiro->defendendo) {
                                cavaleiro->vida -= 1; // O dano e reduzido se o jogador estiver defendendo.
                            } else {
                                int dano = inimigo->e_boss ? 4 : 2; // O chefe da mais dano.
                                cavaleiro->vida -= dano;
                            }
                            cavaleiro->ferido = 1;
                            cavaleiro->frame_atual = 0;
                            cavaleiro->contador_frame = 0;
                            cavaleiro->pode_levar_dano = 0;

                            if (inimigo->e_boss) {
                                ativar_screen_shake(shake_timer, shake_intensidade, 20, 5.0);
                            } else {
                                ativar_screen_shake(shake_timer, shake_intensidade, 15, 4.0);
                            }

                            if (cavaleiro->vida <= 0) {
                                cavaleiro->morto = 1;
                            }
                        }
                    }
                }
           }
            break;
        case INIMIGO_ANIM_ESTADO_FERIDO:
            if (inimigo->contador_frame >= velocidade_animacao_inimigo) {
                inimigo->contador_frame = 0;
                inimigo->frame_atual++;
                if (inimigo->frame_atual >= total_frames_ferido) {
                    inimigo->frame_atual = 0;
                    inimigo->ferido = 0;
                    inimigo->estado_anim_atual = (inimigo->vx == 0) ? INIMIGO_ANIM_ESTADO_PARADO : INIMIGO_ANIM_ESTADO_ANDAR;
                }
            }
            break;
        case INIMIGO_ANIM_ESTADO_MORTO:
            break;
    }

    // Checa se a vida do inimigo chegou a zero para fazer ele morrer.
    if (inimigo->vida <= 0 && !inimigo->morto) {
        inimigo->morto = 1;
        inimigo->estado_anim_atual = INIMIGO_ANIM_ESTADO_MORTO;
        inimigo->frame_atual = 0;
        inimigo->contador_frame = 0;
        inimigo->vx = 0;
        if (som_morte_inimigo) {
            al_play_sample(som_morte_inimigo, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
    }
}

void desenhar_inimigo(Inimigo *inimigo) {
    // Nao desenha o inimigo se a animacao de morte dele ja tiver acabado.
    if (inimigo->animacao_morte_concluida) {
        return;
    }
    ALLEGRO_BITMAP *spritesheet_atual = NULL;
    int frame_w = 0, frame_h = 0, frame_col, frame_row;
    int colunas_parado, colunas_andar, colunas_ataque, colunas_ferido, colunas_morto;

    // Pega os dados do spritesheet certo para o tipo de inimigo (esqueleto ou minotauro).
    if (inimigo->tipo == TIPO_INIMIGO_MINOTAURO) {
        colunas_parado = COLUNAS_FRAME_MINOTAURO_PARADO;
        colunas_andar = COLUNAS_FRAME_MINOTAURO_ANDAR;
        colunas_ataque = COLUNAS_FRAME_MINOTAURO_ATAQUE;
        colunas_ferido = COLUNAS_FRAME_MINOTAURO_FERIDO;
        colunas_morto = COLUNAS_FRAME_MINOTAURO_MORTO;
    } else {
        colunas_parado = COLUNAS_FRAME_ESQUELETO_PARADO;
        colunas_andar = COLUNAS_FRAME_ESQUELETO_ANDAR;
        colunas_ataque = COLUNAS_FRAME_ESQUELETO_ATAQUE;
        colunas_ferido = COLUNAS_FRAME_ESQUELETO_FERIDO;
        colunas_morto = COLUNAS_FRAME_ESQUELETO_MORTO;
    }

    // Seleciona o spritesheet correto para o estado de animacao atual do inimigo.
    switch (inimigo->estado_anim_atual) {
        case INIMIGO_ANIM_ESTADO_PARADO:
            spritesheet_atual = inimigo->spritesheet_parado;
            frame_w = al_get_bitmap_width(spritesheet_atual) / colunas_parado;
            frame_h = al_get_bitmap_height(spritesheet_atual);
            break;
        case INIMIGO_ANIM_ESTADO_ANDAR:
            spritesheet_atual = inimigo->spritesheet_andar;
            frame_w = al_get_bitmap_width(spritesheet_atual) / colunas_andar;
            frame_h = al_get_bitmap_height(spritesheet_atual);
            break;
        case INIMIGO_ANIM_ESTADO_ATACAR:
            spritesheet_atual = inimigo->spritesheet_ataque;
            frame_w = al_get_bitmap_width(spritesheet_atual) / colunas_ataque;
            frame_h = al_get_bitmap_height(spritesheet_atual);
            break;
        case INIMIGO_ANIM_ESTADO_FERIDO:
            spritesheet_atual = inimigo->spritesheet_ferido;
            frame_w = al_get_bitmap_width(spritesheet_atual) / colunas_ferido;
            frame_h = al_get_bitmap_height(spritesheet_atual);
            break;
        case INIMIGO_ANIM_ESTADO_MORTO:
            spritesheet_atual = inimigo->spritesheet_morte;
            frame_w = al_get_bitmap_width(spritesheet_atual) / colunas_morto;
            frame_h = al_get_bitmap_height(spritesheet_atual);
            break;
        default:
            break;
    }

    if (!spritesheet_atual) {
        return; // Protecao extra: spritesheet nao carregado, nao ha o que desenhar.
    }

    // Desenha o frame do inimigo.
    frame_col = inimigo->frame_atual % (al_get_bitmap_width(spritesheet_atual) / frame_w);
    frame_row = inimigo->frame_atual / (al_get_bitmap_width(spritesheet_atual) / frame_w);
    int flip_flags = (inimigo->direcao == -1) ? ALLEGRO_FLIP_HORIZONTAL : 0;
    al_draw_scaled_bitmap(
        spritesheet_atual,
        frame_col * frame_w, frame_row * frame_h, frame_w, frame_h,
        inimigo->x, inimigo->y, inimigo->largura, inimigo->altura, flip_flags
    );
}

void desenhar_barra_vida_boss(Inimigo *boss, ALLEGRO_FONT *fonte) {
    float largura_total_barra = 400;
    float altura_barra = 20;
    float barra_x = LARGURA_TELA / 2 - largura_total_barra / 2;
    float barra_y = ALTURA_TELA - 40;
    float perc_vida = (float)boss->vida / boss->vida_maxima;
    float largura_atual_vida = largura_total_barra * perc_vida;
    al_draw_filled_rectangle(barra_x, barra_y, barra_x + largura_total_barra, barra_y + altura_barra, al_map_rgb(50, 0, 0));
    al_draw_filled_rectangle(barra_x, barra_y, barra_x + largura_atual_vida, barra_y + altura_barra, al_map_rgb(200, 0, 0));
    al_draw_rectangle(barra_x, barra_y, barra_x + largura_total_barra, barra_y + altura_barra, al_map_rgb(255, 255, 255), 2);
    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, barra_y - 25, ALLEGRO_ALIGN_CENTER, "MINOTAURO");
}
