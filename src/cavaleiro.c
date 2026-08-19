// Logica do cavaleiro (jogador): inicializacao, atualizacao (entrada + fisica + combate) e desenho.

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include "../include/cavaleiro.h"
#include "../include/inimigo.h"
#include "../include/recursos.h"
#include "../include/jogo.h"
#include "../include/constantes.h"

void inicializar_cavaleiro(Cavaleiro *cavaleiro) {
    cavaleiro->spritesheet_parado = global_cavaleiro_parado;
    cavaleiro->spritesheet_correr = global_cavaleiro_correr;
    cavaleiro->spritesheet_ataque = global_cavaleiro_ataque;
    cavaleiro->spritesheet_esquiva = global_cavaleiro_esquiva;
    cavaleiro->spritesheet_defesa = global_cavaleiro_defesa;
    cavaleiro->spritesheet_ferido = global_cavaleiro_ferido;
    cavaleiro->spritesheet_morte = global_cavaleiro_morte;
    cavaleiro->largura = LARGURA_EXIBICAO_CAVALEIRO;
    cavaleiro->altura = ALTURA_EXIBICAO_CAVALEIRO;
    cavaleiro->x = 100;
    cavaleiro->y = POSICAO_Y_CHAO - cavaleiro->altura;
    cavaleiro->vx = 0;
    cavaleiro->vy = 0;
    cavaleiro->no_chao = 1;
    cavaleiro->frame_atual = 0;
    cavaleiro->contador_frame = 0;
    cavaleiro->direcao = 1;
    cavaleiro->atacando = 0;
    cavaleiro->defendendo = 0;
    cavaleiro->vida = VIDA_MAXIMA_CAVALEIRO;
    cavaleiro->stamina = STAMINA_MAXIMA_CAVALEIRO;
    cavaleiro->timer_regeneracao_stamina = 0;
    cavaleiro->ferido = 0;
    cavaleiro->morto = 0;
    cavaleiro->animacao_morte_concluida = 0;
    cavaleiro->pode_levar_dano = 1;
    cavaleiro->pode_aparar = 0;
    cavaleiro->timer_cooldown_ataque = 0;
    cavaleiro->timer_esquiva = 0;
    cavaleiro->cooldown_esquiva = 0;
    // Pequeno delay para evitar bugs no comeco (o jogador nao pode agir nos primeiros frames).
    cavaleiro->input_delay_timer = 5;
    cavaleiro->estado_anim_atual = ANIM_ESTADO_PARADO;
    cavaleiro->estado_anim_anterior = ANIM_ESTADO_PARADO;
}

void atualizar_cavaleiro(Cavaleiro *cavaleiro, ALLEGRO_KEYBOARD_STATE *estado_teclado, Inimigo *inimigos[], int num_inimigos,
                         int *shake_timer, float *shake_intensidade, ALLEGRO_SAMPLE *som_ataque, ALLEGRO_SAMPLE *som_dano_jogador,
                         ALLEGRO_SAMPLE *som_esquiva, ALLEGRO_SAMPLE *som_dano_esqueleto, ALLEGRO_SAMPLE *som_dano_minotauro) {
    int j;
    // Se o jogador esta morto, a unica coisa que acontece e sua animacao de morte.
    if (cavaleiro->morto) {
        if (cavaleiro->estado_anim_atual != ANIM_ESTADO_MORTO) {
            cavaleiro->estado_anim_atual = ANIM_ESTADO_MORTO;
            cavaleiro->frame_atual = 0;
            cavaleiro->contador_frame = 0;
        }
        cavaleiro->contador_frame++;
        int velocidade_animacao = 10;
        if (cavaleiro->contador_frame >= velocidade_animacao) {
            cavaleiro->contador_frame = 0;
            cavaleiro->frame_atual++;
            if (cavaleiro->frame_atual >= TOTAL_FRAMES_CAVALEIRO_MORTO) {
                cavaleiro->frame_atual = TOTAL_FRAMES_CAVALEIRO_MORTO - 1;
                cavaleiro->animacao_morte_concluida = 1;
            }
        }
        cavaleiro->vx = 0;
        // Aplica a gravidade mesmo depois de morto, para o corpo cair.
        cavaleiro->y += cavaleiro->vy;
        if (cavaleiro->y + cavaleiro->altura >= POSICAO_Y_CHAO) {
            cavaleiro->y = POSICAO_Y_CHAO - cavaleiro->altura;
            cavaleiro->vy = 0;
            cavaleiro->no_chao = 1;
        } else {
            cavaleiro->no_chao = 0;
            cavaleiro->vy += GRAVIDADE;
        }
        return;
    }

    // Estamina se regenera apos um cooldown se o jogador nao estiver defendendo.
    if (cavaleiro->timer_regeneracao_stamina > 0) {
        cavaleiro->timer_regeneracao_stamina--;
    } else if (cavaleiro->stamina < STAMINA_MAXIMA_CAVALEIRO && !cavaleiro->defendendo) {
        cavaleiro->stamina += STAMINA_TAXA_REGENERACAO;
        if (cavaleiro->stamina > STAMINA_MAXIMA_CAVALEIRO) {
            cavaleiro->stamina = STAMINA_MAXIMA_CAVALEIRO;
        }
    }

    // Controla os cooldowns da esquiva e do ataque.
    if (cavaleiro->cooldown_esquiva > 0) {
        cavaleiro->cooldown_esquiva--;
    }
    if (cavaleiro->timer_cooldown_ataque > 0) {
        cavaleiro->timer_cooldown_ataque--;
    }

    cavaleiro->estado_anim_anterior = cavaleiro->estado_anim_atual;
    if (!cavaleiro->no_chao)
        cavaleiro->vy += GRAVIDADE;
    if (!cavaleiro->ferido && !cavaleiro->atacando && !cavaleiro->defendendo && cavaleiro->estado_anim_atual != ANIM_ESTADO_ESQUIVAR) {
        cavaleiro->vx = 0;
    }

    if (!cavaleiro->defendendo) {
        cavaleiro->pode_aparar = 0;
    }

    // Processa os inputs do teclado.
    if (cavaleiro->input_delay_timer > 0) {
        cavaleiro->input_delay_timer--;
    } else {
        // Maquina de estados para as acoes do jogador.
        if (cavaleiro->estado_anim_atual == ANIM_ESTADO_ESQUIVAR) {
            cavaleiro->vx = VELOCIDADE_ESQUIVA * cavaleiro->direcao;
            cavaleiro->timer_esquiva--;
            if (cavaleiro->timer_esquiva <= 0) {
                cavaleiro->pode_levar_dano = 1; // Fim da invencibilidade da esquiva.
                cavaleiro->estado_anim_atual = ANIM_ESTADO_PARADO;
            }
        } else if (cavaleiro->ferido) {
            cavaleiro->estado_anim_atual = ANIM_ESTADO_FERIDO;
            cavaleiro->vx = 0;
        } else if (cavaleiro->atacando) {
            cavaleiro->estado_anim_atual = ANIM_ESTADO_ATACAR;
            cavaleiro->vx = 0;
        } else if (cavaleiro->defendendo) {
            cavaleiro->estado_anim_atual = ANIM_ESTADO_DEFENDER;
            cavaleiro->vx = 0;
        } else if (cavaleiro->no_chao) {
            // Movimentacao.
            if (al_key_down(estado_teclado, ALLEGRO_KEY_RIGHT)) {
                cavaleiro->vx = VELOCIDADE_MOVIMENTO;
                cavaleiro->direcao = 1;
                cavaleiro->estado_anim_atual = ANIM_ESTADO_CORRER;
            } else if (al_key_down(estado_teclado, ALLEGRO_KEY_LEFT)) {
                cavaleiro->vx = -VELOCIDADE_MOVIMENTO;
                cavaleiro->direcao = -1;
                cavaleiro->estado_anim_atual = ANIM_ESTADO_CORRER;
            } else {
                cavaleiro->estado_anim_atual = ANIM_ESTADO_PARADO;
            }

            // Esquiva.
            if (al_key_down(estado_teclado, ALLEGRO_KEY_SPACE) && cavaleiro->cooldown_esquiva == 0 && cavaleiro->stamina >= STAMINA_CUSTO_ESQUIVA) {
                cavaleiro->stamina -= STAMINA_CUSTO_ESQUIVA;
                cavaleiro->timer_regeneracao_stamina = STAMINA_COOLDOWN_REGENERACAO;
                cavaleiro->estado_anim_atual = ANIM_ESTADO_ESQUIVAR;
                cavaleiro->timer_esquiva = DURACAO_ESQUIVA;
                cavaleiro->cooldown_esquiva = COOLDOWN_ESQUIVA;
                cavaleiro->pode_levar_dano = 0;
                cavaleiro->frame_atual = 0;
                cavaleiro->contador_frame = 0;
                if (som_esquiva) {
                    al_play_sample(som_esquiva, 0.7, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
            }

            // Ataque.
            if (al_key_down(estado_teclado, ALLEGRO_KEY_Z) && cavaleiro->timer_cooldown_ataque == 0 && cavaleiro->stamina >= STAMINA_CUSTO_ATAQUE) {
                cavaleiro->stamina -= STAMINA_CUSTO_ATAQUE;
                cavaleiro->timer_regeneracao_stamina = STAMINA_COOLDOWN_REGENERACAO;
                cavaleiro->atacando = 1;
                cavaleiro->estado_anim_atual = ANIM_ESTADO_ATACAR;
                cavaleiro->frame_atual = 0;
                cavaleiro->contador_frame = 0;
                if (som_ataque) {
                    al_play_sample(som_ataque, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
            }

            // Defesa.
            if (al_key_down(estado_teclado, ALLEGRO_KEY_X)) {
                cavaleiro->defendendo = 1;
                cavaleiro->pode_aparar = 1; // A janela de tempo para parry comeca aqui.
                cavaleiro->estado_anim_atual = ANIM_ESTADO_DEFENDER;
                cavaleiro->frame_atual = 0;
                cavaleiro->contador_frame = 0;
                cavaleiro->timer_regeneracao_stamina = STAMINA_COOLDOWN_REGENERACAO; // Defendendo tambem pausa a regeneracao.
            }
        } else {
            cavaleiro->estado_anim_atual = ANIM_ESTADO_PARADO; // Se estiver no ar, fica parado.
        }
    }

    // Aplica a velocidade (vx, vy) a posicao do jogador.
    cavaleiro->x += cavaleiro->vx;
    cavaleiro->y += cavaleiro->vy;
    if (cavaleiro->y + cavaleiro->altura >= POSICAO_Y_CHAO) {
        cavaleiro->y = POSICAO_Y_CHAO - cavaleiro->altura;
        cavaleiro->vy = 0;
        if (!cavaleiro->no_chao) {
            cavaleiro->no_chao = 1;
            if (!cavaleiro->atacando && !cavaleiro->defendendo && !cavaleiro->ferido) {
                 cavaleiro->estado_anim_atual = (cavaleiro->vx == 0) ? ANIM_ESTADO_PARADO : ANIM_ESTADO_CORRER;
            }
        }
    } else {
        cavaleiro->no_chao = 0;
    }

    // Impede que o jogador saia da tela.
    if (cavaleiro->x < 0) cavaleiro->x = 0;
    if (cavaleiro->x + cavaleiro->largura > LARGURA_TELA) cavaleiro->x = LARGURA_TELA - cavaleiro->largura;

    // Se a animacao mudou em relacao ao quadro anterior, reinicia o contador de frames.
    if (cavaleiro->estado_anim_atual != cavaleiro->estado_anim_anterior) {
        cavaleiro->frame_atual = 0;
        cavaleiro->contador_frame = 0;
    }

    // Avanca o frame da animacao atual.
    cavaleiro->contador_frame++;
    int velocidade_animacao = 7;
    if (cavaleiro->contador_frame >= velocidade_animacao) {
        cavaleiro->contador_frame = 0;
        switch (cavaleiro->estado_anim_atual) {
            case ANIM_ESTADO_PARADO:
                cavaleiro->frame_atual = (cavaleiro->frame_atual + 1) % TOTAL_FRAMES_PARADO;
                break;
            case ANIM_ESTADO_CORRER:
                cavaleiro->frame_atual = (cavaleiro->frame_atual + 1) % TOTAL_FRAMES_CORRER;
                break;
            case ANIM_ESTADO_ATACAR:
                cavaleiro->frame_atual++;
                if (cavaleiro->frame_atual >= TOTAL_FRAMES_ATAQUE) {
                    cavaleiro->frame_atual = 0;
                    cavaleiro->atacando = 0;
                    cavaleiro->timer_cooldown_ataque = COOLDOWN_ATAQUE_JOGADOR;
                    cavaleiro->estado_anim_atual = (cavaleiro->vx == 0) ? ANIM_ESTADO_PARADO : ANIM_ESTADO_CORRER;
                }
                // A colisao do ataque so e checada no frame em que o sprite de ataque aparece,
                // isso sincroniza o dano com a animacao.
                if (cavaleiro->frame_atual == 2 && cavaleiro->atacando) {
                    float offset_x_ataque = (cavaleiro->direcao == 1) ? cavaleiro->largura * 0.6 : -cavaleiro->largura * 0.25;
                    float ataque_x = cavaleiro->x + offset_x_ataque;
                    float ataque_y = cavaleiro->y + cavaleiro->altura * 0.3;
                    float ataque_w = cavaleiro->largura * 0.25;
                    float ataque_h = cavaleiro->altura * 0.4;
                    for (j = 0; j < num_inimigos; j++) {
                        Inimigo *inimigo_atual = inimigos[j];
                        if (verificar_colisao(ataque_x, ataque_y, ataque_w, ataque_h, inimigo_atual->x, inimigo_atual->y, inimigo_atual->largura, inimigo_atual->altura) && !inimigo_atual->morto && inimigo_atual->pode_levar_dano) {

                            if (inimigo_atual->tipo == TIPO_INIMIGO_MINOTAURO) {
                                if (som_dano_minotauro) al_play_sample(som_dano_minotauro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                            } else {
                                if (som_dano_esqueleto) al_play_sample(som_dano_esqueleto, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                            }

                            inimigo_atual->vida -= 2;
                            inimigo_atual->pode_levar_dano = 0; // Inimigo fica invencivel por um instante.
                            inimigo_atual->timer_invencibilidade = DURACAO_INVENCIBILIDADE;
                            if (inimigo_atual->e_boss) {
                                ativar_screen_shake(shake_timer, shake_intensidade, 10, 2.0);
                            }

                            if (!inimigo_atual->atacando) {
                                inimigo_atual->ferido = 1; // Inicia a animacao de dano do inimigo.
                                inimigo_atual->frame_atual = 0;
                                inimigo_atual->contador_frame = 0;
                            }

                            if (inimigo_atual->vida <= 0) {
                                inimigo_atual->morto = 1;
                            }
                        }
                    }
                }
                break;
            case ANIM_ESTADO_DEFENDER:
                cavaleiro->frame_atual++;
                // A janela de parry so existe nos primeiros frames da animacao de defesa.
                if (cavaleiro->frame_atual > 1) {
                    cavaleiro->pode_aparar = 0;
                }
                if (cavaleiro->frame_atual >= TOTAL_FRAMES_DEFESA) {
                    cavaleiro->frame_atual = 0;
                    cavaleiro->defendendo = 0;
                    cavaleiro->estado_anim_atual = (cavaleiro->vx == 0) ? ANIM_ESTADO_PARADO : ANIM_ESTADO_CORRER;
                }
                break;
            case ANIM_ESTADO_ESQUIVAR:
                cavaleiro->frame_atual = (cavaleiro->frame_atual + 1) % TOTAL_FRAMES_ESQUIVA;
                break;
            case ANIM_ESTADO_FERIDO:
                cavaleiro->frame_atual++;
                if (cavaleiro->frame_atual >= TOTAL_FRAMES_CAVALEIRO_FERIDO) {
                    cavaleiro->frame_atual = 0;
                    cavaleiro->ferido = 0;
                    cavaleiro->pode_levar_dano = 1; // Jogador pode levar dano de novo.
                    cavaleiro->estado_anim_atual = (cavaleiro->vx == 0) ? ANIM_ESTADO_PARADO : ANIM_ESTADO_CORRER;
                }
                break;
            case ANIM_ESTADO_MORTO:
                // Se o jogador esta morto, a animacao nao entra em loop, ela para no ultimo frame.
                break;
        }
    }

    // Toca o som de dano no momento exato em que o jogador leva dano.
    if (cavaleiro->ferido && !cavaleiro->pode_levar_dano) {
         if (som_dano_jogador) {
            al_play_sample(som_dano_jogador, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
         }
    }

    // Checa se a vida do jogador chegou a zero para iniciar o estado de morte.
    if (cavaleiro->vida <= 0 && !cavaleiro->morto) {
        cavaleiro->morto = 1;
        cavaleiro->estado_anim_atual = ANIM_ESTADO_MORTO;
        cavaleiro->frame_atual = 0;
        cavaleiro->contador_frame = 0;
        cavaleiro->vx = 0;
    }
}

void desenhar_cavaleiro(Cavaleiro *cavaleiro, ALLEGRO_BITMAP *imagem_coracao) {
    ALLEGRO_BITMAP *spritesheet_atual = NULL;
    int frame_w = 0, frame_h = 0, frame_col, frame_row;
    int i;
    int tamanho_barra_stamina, largura_atual_stamina;

    // Seleciona qual spritesheet usar com base no estado atual do cavaleiro.
    switch (cavaleiro->estado_anim_atual) {
        case ANIM_ESTADO_PARADO:
            spritesheet_atual = cavaleiro->spritesheet_parado;
            frame_w = al_get_bitmap_width(spritesheet_atual) / COLUNAS_FRAME_PARADO;
            frame_h = al_get_bitmap_height(spritesheet_atual);
            break;
        case ANIM_ESTADO_CORRER:
            spritesheet_atual = cavaleiro->spritesheet_correr;
            frame_w = al_get_bitmap_width(spritesheet_atual) / COLUNAS_FRAME_CORRER;
            frame_h = al_get_bitmap_height(spritesheet_atual);
            break;
        case ANIM_ESTADO_ATACAR:
            spritesheet_atual = cavaleiro->spritesheet_ataque;
            frame_w = al_get_bitmap_width(spritesheet_atual) / COLUNAS_FRAME_ATAQUE;
            frame_h = al_get_bitmap_height(spritesheet_atual);
            break;
        case ANIM_ESTADO_DEFENDER:
            spritesheet_atual = cavaleiro->spritesheet_defesa;
            frame_w = al_get_bitmap_width(spritesheet_atual) / COLUNAS_FRAME_DEFESA;
            frame_h = al_get_bitmap_height(spritesheet_atual);
            break;
        case ANIM_ESTADO_ESQUIVAR:
            spritesheet_atual = cavaleiro->spritesheet_esquiva;
            frame_w = al_get_bitmap_width(spritesheet_atual) / COLUNAS_FRAME_ESQUIVA;
            frame_h = al_get_bitmap_height(spritesheet_atual);
            break;
        case ANIM_ESTADO_FERIDO:
            spritesheet_atual = cavaleiro->spritesheet_ferido;
            frame_w = al_get_bitmap_width(spritesheet_atual) / COLUNAS_FRAME_CAVALEIRO_FERIDO;
            frame_h = al_get_bitmap_height(spritesheet_atual);
            break;
        case ANIM_ESTADO_MORTO:
            spritesheet_atual = cavaleiro->spritesheet_morte;
            frame_w = al_get_bitmap_width(spritesheet_atual) / COLUNAS_FRAME_CAVALEIRO_MORTO;
            frame_h = al_get_bitmap_height(spritesheet_atual);
            break;
        default:
            break;
    }

    // Protecao extra: se por algum motivo o spritesheet nao estiver carregado, nao desenha
    // (evita travar o jogo tentando ler dimensoes de um bitmap inexistente).
    if (spritesheet_atual) {
        // Calcula qual frame da spritesheet desenhar e usa a flag para espelhar o sprite na horizontal.
        frame_col = cavaleiro->frame_atual % (al_get_bitmap_width(spritesheet_atual) / frame_w);
        frame_row = cavaleiro->frame_atual / (al_get_bitmap_width(spritesheet_atual) / frame_w);
        int flip_flags = (cavaleiro->direcao == -1) ? ALLEGRO_FLIP_HORIZONTAL : 0;
        al_draw_scaled_bitmap(
            spritesheet_atual,
            frame_col * frame_w, frame_row * frame_h,
            frame_w, frame_h,
            cavaleiro->x, cavaleiro->y,
            cavaleiro->largura, cavaleiro->altura,
            flip_flags
        );
    }

    // Coracoes para representar a barra de vida.
    int tamanho_exibicao_coracao = 24;
    int espacamento_coracao = 15;
    int coracoes_cheios_atuais = cavaleiro->vida / 2; // Cada coracao vale 2 de vida.
    int tem_meio_coracao = cavaleiro->vida % 2;
    for (i = 0; i < coracoes_cheios_atuais; i++) {
        al_draw_scaled_bitmap(imagem_coracao, 0, 0, al_get_bitmap_width(imagem_coracao), al_get_bitmap_height(imagem_coracao),
                              10 + i * (tamanho_exibicao_coracao + espacamento_coracao), 10, tamanho_exibicao_coracao, tamanho_exibicao_coracao, 0);
    }

    if (tem_meio_coracao) {
        float largura_fonte = 0.5 * al_get_bitmap_width(imagem_coracao);
        float largura_exibicao = 0.5 * tamanho_exibicao_coracao;
        al_draw_scaled_bitmap(imagem_coracao, 0, 0, largura_fonte, al_get_bitmap_height(imagem_coracao),
                              10 + coracoes_cheios_atuais * (tamanho_exibicao_coracao + espacamento_coracao), 10,
                              largura_exibicao, tamanho_exibicao_coracao, 0);
    }

    // Barra simples de estamina.
    tamanho_barra_stamina = 150;
    largura_atual_stamina = (int)((cavaleiro->stamina / STAMINA_MAXIMA_CAVALEIRO) * tamanho_barra_stamina);
    al_draw_filled_rectangle(10, 40, 10 + tamanho_barra_stamina, 50, al_map_rgb(50, 50, 50));
    al_draw_filled_rectangle(10, 40, 10 + largura_atual_stamina, 50, al_map_rgb(255, 215, 0));
    al_draw_rectangle(10, 40, 10 + tamanho_barra_stamina, 50, al_map_rgb(255, 255, 255), 1);
}
