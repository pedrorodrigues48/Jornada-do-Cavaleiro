#ifndef CONSTANTES_H
#define CONSTANTES_H

// Dimensoes da janela e do cenario.
#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define POSICAO_Y_CHAO 500
#define GRAVIDADE 1.0
#define VELOCIDADE_MOVIMENTO 5.0

// Valores para a mecanica de esquiva.
// Precisei testar alguns valores ate o tempo e a distancia ficarem bons.
#define VELOCIDADE_ESQUIVA 12.0
#define DURACAO_ESQUIVA 8
#define COOLDOWN_ESQUIVA 50

// Atributos de combate do jogador e dos inimigos.
// Balancear a estamina foi necessario, para evitar que o jogador atacasse ou esquivasse sem parar.
#define DURACAO_INVENCIBILIDADE 60 // Frames de invencibilidade apos levar dano.
#define DURACAO_ATORDOAMENTO 90    // Duracao do atordoamento no inimigo apos um parry.
#define COOLDOWN_ATAQUE_JOGADOR 20
#define VIDA_MAXIMA_CAVALEIRO 8
#define STAMINA_MAXIMA_CAVALEIRO 100.0
#define STAMINA_CUSTO_ATAQUE 25.0
#define STAMINA_CUSTO_ESQUIVA 35.0
#define STAMINA_TAXA_REGENERACAO 0.8
#define STAMINA_COOLDOWN_REGENERACAO 40 // Tempo ate a estamina comecar a regenerar.

// Dimensoes do sprite do cavaleiro na tela.
#define LARGURA_EXIBICAO_CAVALEIRO 128
#define ALTURA_EXIBICAO_CAVALEIRO 128

// Detalhes dos spritesheets do Cavaleiro (jogador).
#define COLUNAS_FRAME_PARADO 4
#define TOTAL_FRAMES_PARADO 4
#define COLUNAS_FRAME_CORRER 7
#define TOTAL_FRAMES_CORRER 7
#define COLUNAS_FRAME_ATAQUE 5
#define TOTAL_FRAMES_ATAQUE 5
#define COLUNAS_FRAME_ESQUIVA 6
#define TOTAL_FRAMES_ESQUIVA 6
#define COLUNAS_FRAME_DEFESA 5
#define TOTAL_FRAMES_DEFESA 5
#define COLUNAS_FRAME_CAVALEIRO_FERIDO 2
#define TOTAL_FRAMES_CAVALEIRO_FERIDO 2
#define COLUNAS_FRAME_CAVALEIRO_MORTO 6
#define TOTAL_FRAMES_CAVALEIRO_MORTO 6

// Detalhes dos spritesheets do Minotauro (chefe).
#define LARGURA_EXIBICAO_MINOTAURO 128
#define ALTURA_EXIBICAO_MINOTAURO 128
#define COLUNAS_FRAME_MINOTAURO_PARADO 10
#define TOTAL_FRAMES_MINOTAURO_PARADO 10
#define COLUNAS_FRAME_MINOTAURO_ANDAR 12
#define TOTAL_FRAMES_MINOTAURO_ANDAR 12
#define COLUNAS_FRAME_MINOTAURO_ATAQUE 5
#define TOTAL_FRAMES_MINOTAURO_ATAQUE 5
#define COLUNAS_FRAME_MINOTAURO_FERIDO 3
#define TOTAL_FRAMES_MINOTAURO_FERIDO 3
#define COLUNAS_FRAME_MINOTAURO_MORTO 5
#define TOTAL_FRAMES_MINOTAURO_MORTO 5

// Detalhes dos spritesheets dos esqueletos (inimigos comuns).
#define LARGURA_EXIBICAO_ESQUELETO 128
#define ALTURA_EXIBICAO_ESQUELETO 128
#define COLUNAS_FRAME_ESQUELETO_PARADO 7
#define TOTAL_FRAMES_ESQUELETO_PARADO 7
#define COLUNAS_FRAME_ESQUELETO_ANDAR 7
#define TOTAL_FRAMES_ESQUELETO_ANDAR 7
#define COLUNAS_FRAME_ESQUELETO_ATAQUE 5
#define TOTAL_FRAMES_ESQUELETO_ATAQUE 5
#define COLUNAS_FRAME_ESQUELETO_FERIDO 2
#define TOTAL_FRAMES_ESQUELETO_FERIDO 2
#define COLUNAS_FRAME_ESQUELETO_MORTO 4
#define TOTAL_FRAMES_ESQUELETO_MORTO 4

// Configuracoes das ondas de inimigos e do cenario.
#define MAX_INIMIGOS 3
#define NUM_ONDAS 3
#define TAMANHO_TILE 32
#define VIDA_MAXIMA_ESQUELETO 5
#define VIDA_MAXIMA_MINOTAURO 20

// Caminhos dos arquivos de assets, relativos a raiz do projeto.
// O jogo precisa ser executado com essa pasta como diretorio de trabalho
// (e o que o Code::Blocks faz automaticamente ao rodar com F9/Ctrl+F10).
#define CAMINHO_FONTE "assets/OptimusPrinceps.ttf"
#define CAMINHO_IMAGENS "assets/imagens/"
#define CAMINHO_SONS "assets/sons/"

#endif // CONSTANTES_H
