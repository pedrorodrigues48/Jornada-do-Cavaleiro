// Trabalho Final de Algoritmos e Programacao
// Autor: Pedro Lopes Rodrigues
// Professor: Marcelo Serrano Zanetti
//
// Universidade Federal de Santa Maria (UFSM) - Engenharia de Computacao
// Disciplina de Algoritmos e Programacao, 1o semestre de 2025.
//
// "Jornada do Cavaleiro" - jogo de acao 2D feito em C com a biblioteca Allegro 5.
// Este arquivo cuida apenas da inicializacao do Allegro, do carregamento de recursos,
// do loop principal (maquina de estados do jogo) e da limpeza final. A logica do
// jogador, dos inimigos e das regras/telas do jogo fica em cavaleiro.c, inimigo.c e jogo.c
// (veja include/ para as respectivas assinaturas). Mais detalhes em docs/ARQUITETURA.md.

#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "../include/constantes.h"
#include "../include/tipos.h"
#include "../include/recursos.h"
#include "../include/cavaleiro.h"
#include "../include/inimigo.h"
#include "../include/jogo.h"

// Inicializa o Allegro e todos os addons usados pelo jogo. Encerra o programa com uma
// mensagem de erro caso algum passo essencial falhe (sem isso, o resto do jogo travaria
// sem explicacao caso a biblioteca nao esteja instalada corretamente).
static void inicializar_allegro(void) {
    if (!al_init()) {
        mostrar_erro_fatal("Falha ao inicializar o Allegro.");
    }
    al_init_image_addon();
    al_init_primitives_addon();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_init_native_dialog_addon();
    al_reserve_samples(16); // Reserva canais de audio para tocar varios sons ao mesmo tempo.
}

int main(void) {
    int i;

    srand((unsigned int)time(NULL));
    inicializar_allegro();

    ALLEGRO_DISPLAY *tela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!tela) {
        mostrar_erro_fatal("Falha ao criar a janela do jogo.");
    }
    al_set_window_title(tela, "Jornada do Cavaleiro");

    ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
    ALLEGRO_TIMER *temporizador = al_create_timer(1.0 / 60.0); // 60 FPS.
    if (!fila_eventos || !temporizador) {
        mostrar_erro_fatal("Falha ao criar a fila de eventos ou o timer do jogo.");
    }

    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(tela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(temporizador));

    // Carrega os spritesheets dos personagens e os demais recursos (fontes, cenario, audio).
    // Se algum recurso essencial (fonte ou imagem de cenario) nao carregar, o jogo nao
    // teria como ser exibido corretamente, entao encerramos com uma mensagem clara em vez
    // de travar mais adiante tentando usar um ponteiro nulo.
    if (!carregar_spritesheets()) {
        mostrar_erro_fatal(
            "Nao foi possivel carregar as imagens dos personagens.\n"
            "Verifique se a pasta 'assets/imagens' esta presente ao lado do executavel\n"
            "(ou execute o jogo pelo Code::Blocks, que ja usa a pasta correta como diretorio de trabalho).");
    }

    RecursosJogo recursos;
    if (!carregar_recursos(&recursos)) {
        mostrar_erro_fatal(
            "Nao foi possivel carregar as fontes ou as imagens de cenario do jogo.\n"
            "Verifique se as pastas 'assets/imagens' e o arquivo 'assets/OptimusPrinceps.ttf'\n"
            "estao presentes ao lado do executavel.");
    }
    if (recursos.icone_janela) {
        al_set_display_icon(tela, recursos.icone_janela);
    }

    EstadoPartida partida;
    inicializar_partida(&partida);

    // Variaveis de controle do loop principal e do menu.
    int redesenhar = 1;
    int executando = 1;
    EstadoJogo estado_jogo = ESTADO_MENU;
    int opcao_menu_selecionada = 0;
    const int NUM_OPCOES_MENU = 3;
    const char *opcoes_menu[] = {"Iniciar Jogo", "Como Jogar", "Sair"};
    int contador_frames_geral = 0;

    ALLEGRO_EVENT ev;
    ALLEGRO_KEYBOARD_STATE estado_teclado;

    al_start_timer(temporizador);
    if (recursos.musica_menu_instancia) al_play_sample_instance(recursos.musica_menu_instancia);

    while (executando) {
        al_wait_for_event(fila_eventos, &ev);

        // Se fechar a janela, o jogo termina.
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            executando = 0;
        }

        // Maquina de estados principal, que direciona a logica para o estado atual do jogo.
        switch (estado_jogo) {
            case ESTADO_MENU:
                if (ev.type == ALLEGRO_EVENT_TIMER) {
                    contador_frames_geral++;
                    redesenhar = 1;
                } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
                        opcao_menu_selecionada--;
                        if (opcao_menu_selecionada < 0) {
                            opcao_menu_selecionada = NUM_OPCOES_MENU - 1;
                        }
                        if (recursos.som_menu_navegar) al_play_sample(recursos.som_menu_navegar, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    }
                    if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                        opcao_menu_selecionada++;
                        if (opcao_menu_selecionada >= NUM_OPCOES_MENU) {
                            opcao_menu_selecionada = 0;
                        }
                        if (recursos.som_menu_navegar) al_play_sample(recursos.som_menu_navegar, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    }
                    if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                        if (recursos.som_menu_selecionar) al_play_sample(recursos.som_menu_selecionar, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        switch (opcao_menu_selecionada) {
                            case 0: // Iniciar Jogo
                                reiniciar_partida(&partida);
                                if (recursos.musica_menu_instancia) al_stop_sample_instance(recursos.musica_menu_instancia);
                                if (recursos.musica_boss_instancia) al_stop_sample_instance(recursos.musica_boss_instancia);
                                if (recursos.musica_jogo_instancia) al_play_sample_instance(recursos.musica_jogo_instancia);
                                estado_jogo = ESTADO_JOGANDO;
                                break;
                            case 1: // Como Jogar
                                estado_jogo = ESTADO_INSTRUCOES;
                                break;
                            case 2: // Sair
                                executando = 0;
                                break;
                        }
                    }
                }
                break;

            case ESTADO_INSTRUCOES:
                if (ev.type == ALLEGRO_EVENT_TIMER) {
                    redesenhar = 1;
                } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE || ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                        estado_jogo = ESTADO_MENU;
                    }
                }
                break;

            case ESTADO_JOGANDO:
                if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_P) {
                        partida.jogo_pausado = !partida.jogo_pausado;
                        // Diminui o volume da musica quando o jogo esta pausado.
                        float ganho = partida.jogo_pausado ? 0.3f : 0.6f;
                        if (recursos.musica_jogo_instancia) al_set_sample_instance_gain(recursos.musica_jogo_instancia, ganho);
                        if (recursos.musica_boss_instancia) al_set_sample_instance_gain(recursos.musica_boss_instancia, ganho);
                    }
                }

                if (ev.type == ALLEGRO_EVENT_TIMER) {
                    if (!partida.jogo_pausado) {

                        // Logica para a animacao de vitoria.
                        if (partida.vitoria_em_andamento > 0) {
                            if (partida.vitoria_em_andamento == 1) { // Texto "CHEFE DERROTADO!" na tela.
                                partida.timer_vitoria--;
                                if (partida.timer_vitoria <= 0) {
                                    partida.vitoria_em_andamento = 2;
                                }
                            } else if (partida.vitoria_em_andamento == 2) { // A tela escurece.
                                partida.alfa_vitoria += 2.0;
                                if (partida.alfa_vitoria >= 255.0) {
                                    partida.alfa_vitoria = 255.0;
                                    partida.vitoria_em_andamento = 3;
                                }
                            } else if (partida.vitoria_em_andamento == 3) { // Muda para a tela final de vitoria.
                                partida.vitoria = 1;
                                if (recursos.som_vitoria) al_play_sample(recursos.som_vitoria, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                                estado_jogo = ESTADO_FIM_DE_JOGO;
                            }
                        }

                        // Atualiza o jogador se nao estivermos em uma transicao de onda.
                        if (!partida.em_transicao_onda && partida.vitoria_em_andamento < 2) {
                            al_get_keyboard_state(&estado_teclado);
                            atualizar_cavaleiro(&partida.cavaleiro, &estado_teclado, partida.inimigos, partida.num_inimigos_ativos,
                                                &partida.screen_shake_timer, &partida.screen_shake_intensidade,
                                                recursos.som_ataque, recursos.som_dano_jogador,
                                                recursos.som_esquiva_jogador, recursos.som_dano_esqueleto, recursos.som_dano_minotauro);
                        }

                        if (partida.cavaleiro.animacao_morte_concluida) {
                            partida.vitoria = 0;
                            if (recursos.musica_jogo_instancia) al_stop_sample_instance(recursos.musica_jogo_instancia);
                            if (recursos.musica_boss_instancia) al_stop_sample_instance(recursos.musica_boss_instancia);
                            if (recursos.som_derrota) al_play_sample(recursos.som_derrota, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                            estado_jogo = ESTADO_FIM_DE_JOGO;
                        }

                        // Verifica se a onda atual terminou.
                        if (!partida.em_transicao_onda && !partida.interludio_onda && partida.vitoria_em_andamento == 0) {
                            int inimigos_mortos_na_onda = 0;
                            int animacoes_de_morte_concluidas = 0;

                            for (i = 0; i < partida.num_inimigos_ativos; i++) {
                                atualizar_inimigo(partida.inimigos[i], &partida.cavaleiro, partida.inimigos, partida.num_inimigos_ativos, i,
                                                  &partida.parry_timer, &partida.parry_x, &partida.parry_y,
                                                  &partida.screen_shake_timer, &partida.screen_shake_intensidade,
                                                  recursos.som_parry, recursos.som_morte_inimigo,
                                                  recursos.som_ataque_esqueleto, recursos.som_ataque_minotauro);
                            }

                            for (i = 0; i < partida.num_inimigos_ativos; i++) {
                                if (partida.inimigos[i]->morto) {
                                    inimigos_mortos_na_onda++;
                                }
                            }

                            if (partida.num_inimigos_ativos > 0 && inimigos_mortos_na_onda == partida.num_inimigos_ativos) {
                                animacoes_de_morte_concluidas = 1;
                                for (i = 0; i < partida.num_inimigos_ativos; i++) {
                                    if (!partida.inimigos[i]->animacao_morte_concluida) {
                                        animacoes_de_morte_concluidas = 0;
                                        break;
                                    }
                                }

                                if (animacoes_de_morte_concluidas) {
                                    if (partida.onda_atual == NUM_ONDAS - 1) {
                                        partida.vitoria_em_andamento = 1; // Inicia a sequencia de vitoria.
                                        partida.timer_vitoria = 300;
                                        if (recursos.musica_jogo_instancia) al_stop_sample_instance(recursos.musica_jogo_instancia);
                                        if (recursos.musica_boss_instancia) al_stop_sample_instance(recursos.musica_boss_instancia);
                                    } else if (partida.onda_atual == NUM_ONDAS - 2) {
                                        partida.em_transicao_onda = 1; // Transicao para a onda do chefe.
                                        partida.estado_fade = 1; // Comeca a escurecer a tela.
                                    } else {
                                        partida.interludio_onda = 1; // Pausa normal entre as ondas comuns.
                                        partida.timer_interludio = 180;
                                        partida.cavaleiro.vida += 2; // Pequena cura para o jogador.
                                        if (partida.cavaleiro.vida > VIDA_MAXIMA_CAVALEIRO) partida.cavaleiro.vida = VIDA_MAXIMA_CAVALEIRO;
                                        if (recursos.som_cura) al_play_sample(recursos.som_cura, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                                    }
                                }
                            }
                        } else if (partida.interludio_onda) {
                            partida.timer_interludio--;
                            if (partida.timer_interludio <= 0) {
                                partida.interludio_onda = 0;
                                partida.onda_atual++;
                                iniciar_onda(partida.onda_atual, partida.inimigos, &partida.num_inimigos_ativos);
                            }
                        } else if (partida.em_transicao_onda) {
                            // Logica da transicao entre a onda 2 e a onda final.
                            if (partida.estado_fade == 1) { // Escurecendo a tela.
                                partida.alfa_transicao += 2;
                                if (partida.alfa_transicao >= 255) {
                                    partida.alfa_transicao = 255;

                                    // Quando a tela esta preta, prepara a proxima onda (a do chefe).
                                    partida.onda_atual++;
                                    iniciar_onda(partida.onda_atual, partida.inimigos, &partida.num_inimigos_ativos);
                                    // Troca a musica e reposiciona o jogador.
                                    if (recursos.musica_jogo_instancia) al_stop_sample_instance(recursos.musica_jogo_instancia);
                                    if (recursos.musica_boss_instancia) al_play_sample_instance(recursos.musica_boss_instancia);
                                    partida.cavaleiro.x = 100;
                                    partida.cavaleiro.y = POSICAO_Y_CHAO - partida.cavaleiro.altura;
                                    partida.cavaleiro.vx = 0;
                                    partida.cavaleiro.direcao = 1;
                                    partida.cavaleiro.estado_anim_atual = ANIM_ESTADO_PARADO;

                                    partida.cavaleiro.vida += 2;
                                    if (partida.cavaleiro.vida > VIDA_MAXIMA_CAVALEIRO) {
                                        partida.cavaleiro.vida = VIDA_MAXIMA_CAVALEIRO;
                                    }
                                    if (recursos.som_cura) al_play_sample(recursos.som_cura, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

                                    // Inicia um timer para segurar a mensagem "Onda Final" na tela.
                                    partida.timer_transicao = 150; // Aproximadamente 2.5 segundos.
                                    partida.estado_fade = 2; // Muda para o estado de espera.
                                }
                            } else if (partida.estado_fade == 2) { // Esperando com a tela preta.
                                partida.timer_transicao--;
                                if (partida.timer_transicao <= 0) {
                                    partida.estado_fade = 3; // Acabou a espera, comeca a clarear a tela.
                                }
                            } else if (partida.estado_fade == 3) { // Clareando a tela.
                                partida.alfa_transicao -= 2; // Fade mais lento.
                                if (partida.alfa_transicao <= 0) {
                                    partida.alfa_transicao = 0;
                                    partida.estado_fade = 0;
                                    partida.em_transicao_onda = 0; // Fim da transicao.
                                }
                            }
                        }
                    }
                    redesenhar = 1;
                }
                break;

            case ESTADO_FIM_DE_JOGO:
                if (ev.type == ALLEGRO_EVENT_TIMER) {
                    if (!partida.vitoria && partida.alfa_fim_de_jogo < 180) {
                        partida.alfa_fim_de_jogo += 1.0; // Efeito de transicao para a tela de "Voce Morreu".
                    }
                    redesenhar = 1;
                } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                        if (recursos.musica_menu_instancia) al_play_sample_instance(recursos.musica_menu_instancia);
                        partida.alfa_fim_de_jogo = 0;
                        estado_jogo = ESTADO_MENU;
                    }
                }
                break;
        }

        // Secao de renderizacao. O jogo so desenha na tela quando a flag redesenhar e verdadeira
        // e nao ha mais eventos pendentes (evita desenhar mais de uma vez por frame).
        if (redesenhar && al_is_event_queue_empty(fila_eventos)) {
            redesenhar = 0;
            if (estado_jogo == ESTADO_MENU) {
                desenhar_menu(&recursos, opcao_menu_selecionada, opcoes_menu, NUM_OPCOES_MENU, contador_frames_geral);
            } else if (estado_jogo == ESTADO_INSTRUCOES) {
                desenhar_instrucoes(&recursos);
            } else if (estado_jogo == ESTADO_JOGANDO || estado_jogo == ESTADO_FIM_DE_JOGO) {
                desenhar_cena_jogo(&recursos, &partida, estado_jogo);
            }
            al_flip_display();
        }
    }

    // Liberando todos os recursos que foram alocados durante a execucao do jogo.
    liberar_spritesheets();
    liberar_recursos(&recursos);
    finalizar_partida(&partida);

    al_destroy_timer(temporizador);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(tela);

    return 0;
}
