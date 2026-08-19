// Carregamento e liberacao de todos os recursos externos do jogo (imagens, fontes e audio).
// Centralizar isso aqui evita duplicar chamadas ao Allegro espalhadas pelo main().

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include "../include/recursos.h"
#include "../include/constantes.h"

ALLEGRO_BITMAP *global_cavaleiro_parado = NULL;
ALLEGRO_BITMAP *global_cavaleiro_correr = NULL;
ALLEGRO_BITMAP *global_cavaleiro_ataque = NULL;
ALLEGRO_BITMAP *global_cavaleiro_esquiva = NULL;
ALLEGRO_BITMAP *global_cavaleiro_defesa = NULL;
ALLEGRO_BITMAP *global_cavaleiro_ferido = NULL;
ALLEGRO_BITMAP *global_cavaleiro_morte = NULL;

ALLEGRO_BITMAP *global_minotauro_parado = NULL;
ALLEGRO_BITMAP *global_minotauro_andar = NULL;
ALLEGRO_BITMAP *global_minotauro_ataque = NULL;
ALLEGRO_BITMAP *global_minotauro_ferido = NULL;
ALLEGRO_BITMAP *global_minotauro_morte = NULL;

ALLEGRO_BITMAP *global_esqueleto_parado = NULL;
ALLEGRO_BITMAP *global_esqueleto_andar = NULL;
ALLEGRO_BITMAP *global_esqueleto_ataque = NULL;
ALLEGRO_BITMAP *global_esqueleto_ferido = NULL;
ALLEGRO_BITMAP *global_esqueleto_morte = NULL;

void mostrar_erro_fatal(const char *mensagem) {
    al_show_native_message_box(NULL, "Jornada do Cavaleiro - Erro", "Nao foi possivel iniciar o jogo", mensagem,
                                NULL, ALLEGRO_MESSAGEBOX_ERROR);
    exit(1);
}

int carregar_spritesheets(void) {
    global_cavaleiro_parado = al_load_bitmap(CAMINHO_IMAGENS "Idle.png");
    global_cavaleiro_correr = al_load_bitmap(CAMINHO_IMAGENS "Run.png");
    global_cavaleiro_ataque = al_load_bitmap(CAMINHO_IMAGENS "Attack1.png");
    global_cavaleiro_esquiva = al_load_bitmap(CAMINHO_IMAGENS "Jump.png");
    global_cavaleiro_defesa = al_load_bitmap(CAMINHO_IMAGENS "Defend.png");
    global_cavaleiro_ferido = al_load_bitmap(CAMINHO_IMAGENS "HurtC.png");
    global_cavaleiro_morte = al_load_bitmap(CAMINHO_IMAGENS "DeadC.png");

    global_minotauro_parado = al_load_bitmap(CAMINHO_IMAGENS "idlemonster.png");
    global_minotauro_andar = al_load_bitmap(CAMINHO_IMAGENS "Walk.png");
    global_minotauro_ataque = al_load_bitmap(CAMINHO_IMAGENS "Attack.png");
    global_minotauro_ferido = al_load_bitmap(CAMINHO_IMAGENS "Hurt.png");
    global_minotauro_morte = al_load_bitmap(CAMINHO_IMAGENS "Dead.png");

    global_esqueleto_parado = al_load_bitmap(CAMINHO_IMAGENS "Skeleton_Idle.png");
    global_esqueleto_andar = al_load_bitmap(CAMINHO_IMAGENS "Skeleton_Walk.png");
    global_esqueleto_ataque = al_load_bitmap(CAMINHO_IMAGENS "Skeleton_Attack_1.png");
    global_esqueleto_ferido = al_load_bitmap(CAMINHO_IMAGENS "Skeleton_Hurt.png");
    global_esqueleto_morte = al_load_bitmap(CAMINHO_IMAGENS "Skeleton_Dead.png");

    return global_cavaleiro_parado && global_cavaleiro_correr && global_cavaleiro_ataque &&
           global_cavaleiro_esquiva && global_cavaleiro_defesa && global_cavaleiro_ferido &&
           global_cavaleiro_morte && global_minotauro_parado && global_minotauro_andar &&
           global_minotauro_ataque && global_minotauro_ferido && global_minotauro_morte &&
           global_esqueleto_parado && global_esqueleto_andar && global_esqueleto_ataque &&
           global_esqueleto_ferido && global_esqueleto_morte;
}

void liberar_spritesheets(void) {
    al_destroy_bitmap(global_cavaleiro_parado);
    al_destroy_bitmap(global_cavaleiro_correr);
    al_destroy_bitmap(global_cavaleiro_ataque);
    al_destroy_bitmap(global_cavaleiro_esquiva);
    al_destroy_bitmap(global_cavaleiro_defesa);
    al_destroy_bitmap(global_cavaleiro_ferido);
    al_destroy_bitmap(global_cavaleiro_morte);

    al_destroy_bitmap(global_minotauro_parado);
    al_destroy_bitmap(global_minotauro_andar);
    al_destroy_bitmap(global_minotauro_ataque);
    al_destroy_bitmap(global_minotauro_ferido);
    al_destroy_bitmap(global_minotauro_morte);

    al_destroy_bitmap(global_esqueleto_parado);
    al_destroy_bitmap(global_esqueleto_andar);
    al_destroy_bitmap(global_esqueleto_ataque);
    al_destroy_bitmap(global_esqueleto_ferido);
    al_destroy_bitmap(global_esqueleto_morte);
}

// Cria uma ALLEGRO_SAMPLE_INSTANCE em loop para uma musica, pronta para tocar.
// Retorna NULL se a amostra original nao existir (om_ som nao carregado).
static ALLEGRO_SAMPLE_INSTANCE *criar_instancia_musica(ALLEGRO_SAMPLE *amostra) {
    if (!amostra) {
        return NULL;
    }
    ALLEGRO_SAMPLE_INSTANCE *instancia = al_create_sample_instance(amostra);
    if (!instancia) {
        return NULL;
    }
    al_set_sample_instance_playmode(instancia, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(instancia, 0.6);
    al_attach_sample_instance_to_mixer(instancia, al_get_default_mixer());
    return instancia;
}

int carregar_recursos(RecursosJogo *recursos) {
    memset(recursos, 0, sizeof(RecursosJogo));

    recursos->fonte = al_load_font(CAMINHO_FONTE, 36, 0);
    recursos->fonte_onda = al_load_font(CAMINHO_FONTE, 24, 0);
    recursos->fonte_titulo = al_load_font(CAMINHO_FONTE, 72, 0);

    recursos->tileset_chao = al_load_bitmap(CAMINHO_IMAGENS "tilesetgrass.png");
    recursos->fundo_ceu = al_load_bitmap(CAMINHO_IMAGENS "sky.png");
    recursos->imagem_coracao = al_load_bitmap(CAMINHO_IMAGENS "heart.png");

    // Recursos opcionais: o jogo funciona normalmente mesmo se eles nao existirem
    // (ver README, secao "Limitacoes conhecidas"). O codigo em toda parte que usa
    // esses ponteiros checa por NULL antes de desenhar/tocar.
    recursos->fundo_montanhas = al_load_bitmap(CAMINHO_IMAGENS "fundo_montanhas.png");
    recursos->icone_janela = al_load_bitmap(CAMINHO_IMAGENS "icone.png");

    recursos->som_ataque = al_load_sample(CAMINHO_SONS "ataque.ogg");
    recursos->som_dano_jogador = al_load_sample(CAMINHO_SONS "player_hurt.ogg");
    recursos->som_parry = al_load_sample(CAMINHO_SONS "parry.ogg");
    recursos->som_morte_inimigo = al_load_sample(CAMINHO_SONS "enemy_death.ogg");
    recursos->som_esquiva_jogador = al_load_sample(CAMINHO_SONS "player_dodge.ogg");
    recursos->som_cura = al_load_sample(CAMINHO_SONS "heal.ogg");
    recursos->som_vitoria = al_load_sample(CAMINHO_SONS "victory.ogg");
    recursos->som_derrota = al_load_sample(CAMINHO_SONS "defeat.ogg");
    recursos->som_menu_navegar = al_load_sample(CAMINHO_SONS "menu_nav.ogg");
    recursos->som_menu_selecionar = al_load_sample(CAMINHO_SONS "menu_select.ogg");
    recursos->som_ataque_esqueleto = al_load_sample(CAMINHO_SONS "esqueleto_ataque.ogg");
    recursos->som_dano_esqueleto = al_load_sample(CAMINHO_SONS "esqueleto_dano.ogg");
    recursos->som_ataque_minotauro = al_load_sample(CAMINHO_SONS "minotauro_ataque.ogg");
    recursos->som_dano_minotauro = al_load_sample(CAMINHO_SONS "minotauro_dano.ogg");

    recursos->musica_menu = al_load_sample(CAMINHO_SONS "menu_music.ogg");
    recursos->musica_jogo = al_load_sample(CAMINHO_SONS "game_music.ogg");
    recursos->musica_boss = al_load_sample(CAMINHO_SONS "boss_music.ogg");
    recursos->musica_menu_instancia = criar_instancia_musica(recursos->musica_menu);
    recursos->musica_jogo_instancia = criar_instancia_musica(recursos->musica_jogo);
    recursos->musica_boss_instancia = criar_instancia_musica(recursos->musica_boss);

    return recursos->fonte && recursos->fonte_onda && recursos->fonte_titulo &&
           recursos->tileset_chao && recursos->fundo_ceu && recursos->imagem_coracao;
}

void liberar_recursos(RecursosJogo *recursos) {
    if (recursos->musica_menu_instancia) al_destroy_sample_instance(recursos->musica_menu_instancia);
    if (recursos->musica_jogo_instancia) al_destroy_sample_instance(recursos->musica_jogo_instancia);
    if (recursos->musica_boss_instancia) al_destroy_sample_instance(recursos->musica_boss_instancia);

    if (recursos->musica_menu) al_destroy_sample(recursos->musica_menu);
    if (recursos->musica_jogo) al_destroy_sample(recursos->musica_jogo);
    if (recursos->musica_boss) al_destroy_sample(recursos->musica_boss);

    if (recursos->som_ataque) al_destroy_sample(recursos->som_ataque);
    if (recursos->som_dano_jogador) al_destroy_sample(recursos->som_dano_jogador);
    if (recursos->som_parry) al_destroy_sample(recursos->som_parry);
    if (recursos->som_morte_inimigo) al_destroy_sample(recursos->som_morte_inimigo);
    if (recursos->som_esquiva_jogador) al_destroy_sample(recursos->som_esquiva_jogador);
    if (recursos->som_cura) al_destroy_sample(recursos->som_cura);
    if (recursos->som_vitoria) al_destroy_sample(recursos->som_vitoria);
    if (recursos->som_derrota) al_destroy_sample(recursos->som_derrota);
    if (recursos->som_menu_navegar) al_destroy_sample(recursos->som_menu_navegar);
    if (recursos->som_menu_selecionar) al_destroy_sample(recursos->som_menu_selecionar);
    if (recursos->som_ataque_esqueleto) al_destroy_sample(recursos->som_ataque_esqueleto);
    if (recursos->som_dano_esqueleto) al_destroy_sample(recursos->som_dano_esqueleto);
    if (recursos->som_ataque_minotauro) al_destroy_sample(recursos->som_ataque_minotauro);
    if (recursos->som_dano_minotauro) al_destroy_sample(recursos->som_dano_minotauro);

    if (recursos->fonte) al_destroy_font(recursos->fonte);
    if (recursos->fonte_onda) al_destroy_font(recursos->fonte_onda);
    if (recursos->fonte_titulo) al_destroy_font(recursos->fonte_titulo);

    if (recursos->tileset_chao) al_destroy_bitmap(recursos->tileset_chao);
    if (recursos->fundo_ceu) al_destroy_bitmap(recursos->fundo_ceu);
    if (recursos->fundo_montanhas) al_destroy_bitmap(recursos->fundo_montanhas);
    if (recursos->imagem_coracao) al_destroy_bitmap(recursos->imagem_coracao);
    if (recursos->icone_janela) al_destroy_bitmap(recursos->icone_janela);
}
