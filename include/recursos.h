#ifndef RECURSOS_H
#define RECURSOS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>

// Spritesheets do cavaleiro (jogador). Ficam em variaveis globais porque sao
// compartilhadas por todas as instancias do personagem (so existe um Cavaleiro no jogo).
extern ALLEGRO_BITMAP *global_cavaleiro_parado;
extern ALLEGRO_BITMAP *global_cavaleiro_correr;
extern ALLEGRO_BITMAP *global_cavaleiro_ataque;
extern ALLEGRO_BITMAP *global_cavaleiro_esquiva;
extern ALLEGRO_BITMAP *global_cavaleiro_defesa;
extern ALLEGRO_BITMAP *global_cavaleiro_ferido;
extern ALLEGRO_BITMAP *global_cavaleiro_morte;

// Spritesheets do Minotauro (chefe). Compartilhadas por todo inimigo do tipo TIPO_INIMIGO_MINOTAURO.
extern ALLEGRO_BITMAP *global_minotauro_parado;
extern ALLEGRO_BITMAP *global_minotauro_andar;
extern ALLEGRO_BITMAP *global_minotauro_ataque;
extern ALLEGRO_BITMAP *global_minotauro_ferido;
extern ALLEGRO_BITMAP *global_minotauro_morte;

// Spritesheets do Esqueleto. Compartilhadas por todo inimigo do tipo TIPO_INIMIGO_ESQUELETO.
extern ALLEGRO_BITMAP *global_esqueleto_parado;
extern ALLEGRO_BITMAP *global_esqueleto_andar;
extern ALLEGRO_BITMAP *global_esqueleto_ataque;
extern ALLEGRO_BITMAP *global_esqueleto_ferido;
extern ALLEGRO_BITMAP *global_esqueleto_morte;

// Agrupa todos os recursos "de cena" (fontes, cenario, HUD, audio) que antes
// eram dezenas de variaveis soltas dentro de main(). Passar um ponteiro para
// essa struct entre as funcoes evita ter que passar cada asset como parametro.
typedef struct {
    ALLEGRO_FONT *fonte;
    ALLEGRO_FONT *fonte_onda;
    ALLEGRO_FONT *fonte_titulo;

    ALLEGRO_BITMAP *tileset_chao;
    ALLEGRO_BITMAP *fundo_ceu;
    ALLEGRO_BITMAP *fundo_montanhas; // Camada decorativa opcional; ver README (asset nao incluso).
    ALLEGRO_BITMAP *imagem_coracao;
    ALLEGRO_BITMAP *icone_janela;

    ALLEGRO_SAMPLE *som_ataque;
    ALLEGRO_SAMPLE *som_dano_jogador;
    ALLEGRO_SAMPLE *som_parry;
    ALLEGRO_SAMPLE *som_morte_inimigo;
    ALLEGRO_SAMPLE *som_esquiva_jogador; // Opcional; ver README (asset nao incluso).
    ALLEGRO_SAMPLE *som_cura;            // Opcional; ver README (asset nao incluso).
    ALLEGRO_SAMPLE *som_vitoria;
    ALLEGRO_SAMPLE *som_derrota;
    ALLEGRO_SAMPLE *som_menu_navegar;
    ALLEGRO_SAMPLE *som_menu_selecionar;
    ALLEGRO_SAMPLE *som_ataque_esqueleto;
    ALLEGRO_SAMPLE *som_dano_esqueleto;  // Opcional; ver README (asset nao incluso).
    ALLEGRO_SAMPLE *som_ataque_minotauro;
    ALLEGRO_SAMPLE *som_dano_minotauro;

    ALLEGRO_SAMPLE *musica_menu;
    ALLEGRO_SAMPLE *musica_jogo;
    ALLEGRO_SAMPLE *musica_boss;
    ALLEGRO_SAMPLE_INSTANCE *musica_menu_instancia;
    ALLEGRO_SAMPLE_INSTANCE *musica_jogo_instancia;
    ALLEGRO_SAMPLE_INSTANCE *musica_boss_instancia;
} RecursosJogo;

// Carrega todos os spritesheets dos personagens (variaveis globais acima).
// Retorna 1 se todos os spritesheets essenciais foram carregados, 0 caso contrario.
int carregar_spritesheets(void);

// Libera os spritesheets dos personagens. Seguro chamar mesmo se algum nao foi carregado.
void liberar_spritesheets(void);

// Zera a struct e carrega fontes, cenario, HUD e audio.
// Retorna 1 se os recursos essenciais (fontes e imagens de cenario) foram carregados,
// 0 se algum recurso essencial faltou (o chamador deve encerrar o jogo nesse caso).
// Efeitos sonoros e a camada decorativa de fundo sao opcionais: se o arquivo nao existir,
// o jogo continua funcionando normalmente, apenas sem aquele som/imagem.
int carregar_recursos(RecursosJogo *recursos);

// Libera todos os recursos carregados por carregar_recursos.
void liberar_recursos(RecursosJogo *recursos);

// Mostra uma caixa de dialogo nativa do sistema operacional com a mensagem de erro
// e encerra o programa (exit(1)). Usada quando um recurso essencial nao pode ser
// carregado e o jogo nao tem como continuar (ex.: fonte ou spritesheet ausente).
void mostrar_erro_fatal(const char *mensagem);

#endif // RECURSOS_H
