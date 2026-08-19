// Regras gerais do jogo: colisao, ondas de inimigos, ciclo de vida da partida
// e desenho das telas de menu, instrucoes e do mundo do jogo (HUD, transicoes, etc).

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include "../include/jogo.h"
#include "../include/cavaleiro.h"
#include "../include/inimigo.h"
#include "../include/recursos.h"
#include "../include/constantes.h"

int verificar_colisao(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2;
}

void ativar_screen_shake(int *timer, float *intensidade, int duracao, float forca) {
    *timer = duracao;
    *intensidade = forca;
}

void iniciar_onda(int onda, Inimigo *inimigos[], int *num_inimigos_ativos) {
    int i;
    // Primeiro, marca todos os inimigos antigos como mortos (a memoria e reaproveitada).
    for (i = 0; i < MAX_INIMIGOS; i++) {
        inimigos[i]->morto = 1;
        inimigos[i]->animacao_morte_concluida = 1;
    }

    if (onda == 0) { // Primeira onda.
        *num_inimigos_ativos = 2;
        inicializar_inimigo(inimigos[0], 500, TIPO_INIMIGO_ESQUELETO);
        inicializar_inimigo(inimigos[1], 700, TIPO_INIMIGO_ESQUELETO);
    } else if (onda == 1) { // Segunda onda.
        *num_inimigos_ativos = 3;
        inicializar_inimigo(inimigos[0], 400, TIPO_INIMIGO_ESQUELETO);
        inicializar_inimigo(inimigos[1], 600, TIPO_INIMIGO_ESQUELETO);
        inicializar_inimigo(inimigos[2], 800, TIPO_INIMIGO_ESQUELETO);
    } else if (onda == 2) { // Terceira onda (onda do chefe).
        *num_inimigos_ativos = 1;
        inicializar_inimigo(inimigos[0], 600, TIPO_INIMIGO_MINOTAURO);
    }
}

void inicializar_partida(EstadoPartida *partida) {
    int i;
    memset(partida, 0, sizeof(EstadoPartida));

    // Aloca memoria para os MAX_INIMIGOS inimigos que serao reaproveitados durante todo
    // o jogo (a cada onda/partida eles sao reconfigurados por inicializar_inimigo).
    for (i = 0; i < MAX_INIMIGOS; i++) {
        partida->inimigos[i] = (Inimigo *)malloc(sizeof(Inimigo));
        if (!partida->inimigos[i]) {
            mostrar_erro_fatal("Memoria insuficiente para iniciar o jogo (falha ao alocar inimigos).");
        }
    }
}

void finalizar_partida(EstadoPartida *partida) {
    int i;
    for (i = 0; i < MAX_INIMIGOS; i++) {
        free(partida->inimigos[i]);
        partida->inimigos[i] = NULL;
    }
}

void reiniciar_partida(EstadoPartida *partida) {
    partida->vitoria = 0;
    partida->onda_atual = 0;
    partida->em_transicao_onda = 0;
    partida->estado_fade = 0;
    partida->alfa_transicao = 0;
    partida->timer_transicao = 0;
    partida->alfa_fim_de_jogo = 0;
    partida->interludio_onda = 0;
    partida->timer_interludio = 0;
    partida->vitoria_em_andamento = 0;
    partida->timer_vitoria = 0;
    partida->alfa_vitoria = 0;
    partida->screen_shake_timer = 0;
    partida->screen_shake_intensidade = 0;
    partida->parry_timer = 0;
    partida->parry_x = 0;
    partida->parry_y = 0;
    partida->jogo_pausado = 0;

    inicializar_cavaleiro(&partida->cavaleiro);
    iniciar_onda(partida->onda_atual, partida->inimigos, &partida->num_inimigos_ativos);
}

void desenhar_feedback_parry(int timer, float x, float y, ALLEGRO_FONT *fonte) {
    if (timer > 0) {
        float alpha = (float)timer / 30.0; // O alfa diminui conforme o timer passa.
        al_draw_text(fonte, al_map_rgba_f(1, 1, 0.8, alpha), x, y - 50, ALLEGRO_ALIGN_CENTER, "Parry!");
    }
}

void desenhar_menu(RecursosJogo *recursos, int opcao_selecionada, const char *opcoes[], int num_opcoes, int contador_frames) {
    int i;
    float pulsacao = sin(contador_frames * 0.1) * 5; // Efeitinho de pulsar para o seletor.
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_scaled_bitmap(recursos->fundo_ceu, 0, 0, al_get_bitmap_width(recursos->fundo_ceu), al_get_bitmap_height(recursos->fundo_ceu),
                          0, 0, LARGURA_TELA, ALTURA_TELA, 0);
    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 100)); // Escurece um pouco o fundo.

    // Desenha o texto com uma sombra para dar mais destaque.
    al_draw_text(recursos->fonte_titulo, al_map_rgb(20, 20, 20), LARGURA_TELA / 2 + 4, 104, ALLEGRO_ALIGN_CENTER, "Jornada do Cavaleiro");
    al_draw_text(recursos->fonte_titulo, al_map_rgb(255, 223, 0), LARGURA_TELA / 2, 100, ALLEGRO_ALIGN_CENTER, "Jornada do Cavaleiro");

    for (i = 0; i < num_opcoes; i++) {
        ALLEGRO_COLOR cor_texto = (i == opcao_selecionada) ? al_map_rgb(255, 255, 255) : al_map_rgb(180, 180, 180);
        al_draw_text(recursos->fonte, cor_texto, LARGURA_TELA / 2, 300 + i * 50, ALLEGRO_ALIGN_CENTER, opcoes[i]);
    }

    al_draw_text(recursos->fonte, al_map_rgb(255, 223, 0), LARGURA_TELA / 2 - 150 + pulsacao, 300 + opcao_selecionada * 50, ALLEGRO_ALIGN_RIGHT, ">");
    al_draw_text(recursos->fonte, al_map_rgb(255, 223, 0), LARGURA_TELA / 2 + 150 - pulsacao, 300 + opcao_selecionada * 50, ALLEGRO_ALIGN_LEFT, "<");
    al_draw_text(recursos->fonte_onda, al_map_rgb(200, 200, 200), LARGURA_TELA - 10, ALTURA_TELA - 30, ALLEGRO_ALIGN_RIGHT, "'Enter' para selecionar");
}

void desenhar_instrucoes(RecursosJogo *recursos) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_scaled_bitmap(recursos->fundo_ceu, 0, 0, al_get_bitmap_width(recursos->fundo_ceu), al_get_bitmap_height(recursos->fundo_ceu),
                          0, 0, LARGURA_TELA, ALTURA_TELA, 0);
    al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 100));
    al_draw_text(recursos->fonte, al_map_rgb(255, 223, 0), LARGURA_TELA / 2, 100, ALLEGRO_ALIGN_CENTER, "Como Jogar");
    al_draw_text(recursos->fonte_onda, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 250, ALLEGRO_ALIGN_CENTER, "Setas Esquerda/Direita - Mover");
    al_draw_text(recursos->fonte_onda, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 300, ALLEGRO_ALIGN_CENTER, "Z - Atacar");
    al_draw_text(recursos->fonte_onda, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 350, ALLEGRO_ALIGN_CENTER, "X - Defender / Aparar (Parry)");
    al_draw_text(recursos->fonte_onda, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 400, ALLEGRO_ALIGN_CENTER, "Espaco - Esquivar");
    al_draw_text(recursos->fonte_onda, al_map_rgb(200, 200, 200), LARGURA_TELA / 2, 500, ALLEGRO_ALIGN_CENTER, "Pressione 'Enter' para voltar");
}

void desenhar_cena_jogo(RecursosJogo *recursos, EstadoPartida *partida, EstadoJogo estado_jogo) {
    int i, j;
    char texto_onda[50];
    ALLEGRO_TRANSFORM transform_base, transform_shake;

    al_copy_transform(&transform_base, al_get_current_transform());
    if (partida->screen_shake_timer > 0) {
        // Garante um divisor valido para o "% "; ativar_screen_shake sempre usa forca > 0,
        // mas essa checagem evita uma divisao por zero (comportamento indefinido) em qualquer cenario.
        int intervalo_shake = (int)(partida->screen_shake_intensidade * 2);
        if (intervalo_shake < 1) intervalo_shake = 1;
        float offset_x = (rand() % intervalo_shake) - partida->screen_shake_intensidade;
        float offset_y = (rand() % intervalo_shake) - partida->screen_shake_intensidade;
        al_identity_transform(&transform_shake);
        al_translate_transform(&transform_shake, offset_x, offset_y);
        al_use_transform(&transform_shake);
        partida->screen_shake_timer--;
    }

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_scaled_bitmap(recursos->fundo_ceu, 0, 0, al_get_bitmap_width(recursos->fundo_ceu), al_get_bitmap_height(recursos->fundo_ceu),
                          0, 0, LARGURA_TELA, ALTURA_TELA, 0);

    // Desenha o fundo de montanhas estaticamente (camada decorativa opcional).
    if (recursos->fundo_montanhas) {
        al_draw_bitmap(recursos->fundo_montanhas, 0, 0, 0);
    }

    // Desenha o chao usando tiles do tileset.
    int num_tiles_x = LARGURA_TELA / TAMANHO_TILE + 1;
    int num_tiles_y = (ALTURA_TELA - POSICAO_Y_CHAO) / TAMANHO_TILE + 1;
    for (i = 0; i < num_tiles_x; i++) {
        al_draw_bitmap_region(recursos->tileset_chao, TAMANHO_TILE, 0, TAMANHO_TILE, TAMANHO_TILE, i * TAMANHO_TILE, POSICAO_Y_CHAO, 0); // Grama.
        for (j = 1; j < num_tiles_y; j++) {
            al_draw_bitmap_region(recursos->tileset_chao, TAMANHO_TILE, TAMANHO_TILE, TAMANHO_TILE, TAMANHO_TILE,
                                  i * TAMANHO_TILE, POSICAO_Y_CHAO + (j * TAMANHO_TILE), 0); // Terra.
        }
    }

    for (i = 0; i < partida->num_inimigos_ativos; i++) {
        desenhar_inimigo(partida->inimigos[i]);
    }
    desenhar_cavaleiro(&partida->cavaleiro, recursos->imagem_coracao);
    desenhar_feedback_parry(partida->parry_timer, partida->parry_x, partida->parry_y, recursos->fonte_onda);
    if (partida->parry_timer > 0) partida->parry_timer--;

    al_use_transform(&transform_base);

    // Desenha a barra de vida do chefe, se ele estiver na tela.
    for (i = 0; i < partida->num_inimigos_ativos; i++) {
        if (partida->inimigos[i]->e_boss && !partida->inimigos[i]->morto) {
            desenhar_barra_vida_boss(partida->inimigos[i], recursos->fonte_onda);
            break;
        }
    }

    if (!partida->em_transicao_onda && !partida->interludio_onda && estado_jogo == ESTADO_JOGANDO &&
        !partida->jogo_pausado && partida->vitoria_em_andamento == 0) {
        if (partida->onda_atual == NUM_ONDAS - 1) {
            snprintf(texto_onda, sizeof(texto_onda), "Onda Final!");
        } else {
            snprintf(texto_onda, sizeof(texto_onda), "Onda %d / %d", partida->onda_atual + 1, NUM_ONDAS - 1);
        }
        al_draw_text(recursos->fonte_onda, al_map_rgb(255, 255, 255), LARGURA_TELA - 10, 10, ALLEGRO_ALIGN_RIGHT, texto_onda);
    }

    if (partida->interludio_onda) {
        al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 150));
        if (partida->timer_interludio > 90) {
            al_draw_text(recursos->fonte, al_map_rgb(255, 223, 0), LARGURA_TELA / 2, ALTURA_TELA / 2 - 60, ALLEGRO_ALIGN_CENTER, "Onda Concluida!");
        } else {
            snprintf(texto_onda, sizeof(texto_onda), "Proxima Onda!");
            al_draw_text(recursos->fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 - 30, ALLEGRO_ALIGN_CENTER, texto_onda);
        }
    }

    if (partida->em_transicao_onda) {
        al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, (int)partida->alfa_transicao));
        // Durante o estado de espera da transicao (tela preta), mostra a mensagem.
        if (partida->estado_fade == 2) {
            al_draw_text(recursos->fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 - 30, ALLEGRO_ALIGN_CENTER, "Onda Final!");
        }
    }

    if (partida->vitoria_em_andamento > 0 && estado_jogo == ESTADO_JOGANDO) {
        if (partida->vitoria_em_andamento == 1) {
            al_draw_text(recursos->fonte, al_map_rgb(255, 223, 0), LARGURA_TELA / 2, ALTURA_TELA / 2 - 30,
                           ALLEGRO_ALIGN_CENTER, "CHEFE DERROTADO!");
        } else if (partida->vitoria_em_andamento >= 2) {
            al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, (int)partida->alfa_vitoria));
        }
    }

    if (estado_jogo == ESTADO_FIM_DE_JOGO) {
        if (partida->vitoria) {
            al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgb(0, 0, 0));
            al_draw_text(recursos->fonte, al_map_rgb(255, 223, 0), LARGURA_TELA / 2, ALTURA_TELA / 2 - 40, ALLEGRO_ALIGN_CENTER, "VITORIA!");
            al_draw_text(recursos->fonte_onda, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 20, ALLEGRO_ALIGN_CENTER, "Pressione 'Enter' para voltar ao Menu");
        } else {
            al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, (int)partida->alfa_fim_de_jogo));
            if (partida->alfa_fim_de_jogo > 20) {
                al_draw_text(recursos->fonte, al_map_rgb(255, 0, 0), LARGURA_TELA / 2, ALTURA_TELA / 2 - 40, ALLEGRO_ALIGN_CENTER, "VOCE MORREU!");
            }
            if (partida->alfa_fim_de_jogo >= 180) {
                al_draw_text(recursos->fonte_onda, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 + 20, ALLEGRO_ALIGN_CENTER, "Pressione 'Enter' para voltar ao Menu");
            }
        }
    }

    if (partida->jogo_pausado) {
        al_draw_filled_rectangle(0, 0, LARGURA_TELA, ALTURA_TELA, al_map_rgba(0, 0, 0, 180));
        al_draw_text(recursos->fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2 - 30, ALLEGRO_ALIGN_CENTER, "PAUSADO");
    }
}
