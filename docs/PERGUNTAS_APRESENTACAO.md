# Perguntas e respostas para a apresentação

Perguntas que um professor poderia fazer sobre este projeto especificamente, com respostas
curtas. Use como roteiro de estudo, não como texto para decorar palavra por palavra.

### Por que o projeto foi dividido em vários arquivos `.c`/`.h`?

Para separar responsabilidades: `cavaleiro.c` só sabe sobre o jogador, `inimigo.c` só sobre os
inimigos, `jogo.c` sobre as regras gerais (colisão, ondas, telas) e `recursos.c` sobre
carregar/liberar arquivos externos. Isso facilita achar onde mexer quando algo precisa mudar,
e permite entender cada arquivo sem precisar ler o projeto inteiro de uma vez. Cada `.h`
funciona como um "contrato": mostra o que aquele módulo oferece para os outros, sem expor
como é implementado por dentro.

### Por que a struct `Inimigo` é usada tanto para o esqueleto quanto para o Minotauro (chefe)?

Os dois compartilham o mesmo conjunto de atributos (posição, vida, animação, spritesheets),
só os *valores* mudam (vida máxima, velocidade, dano, tamanho na tela). O campo `tipo`
(`TipoInimigo`) e a flag `e_boss` são o que diferenciam o comportamento nas funções de
atualização — por exemplo, em `atualizar_inimigo` o alcance de ataque e a velocidade de
movimento são maiores se `inimigo->e_boss` for verdadeiro. Criar uma struct separada só para
o chefe duplicaria praticamente todo o código de IA/animação.

### Onde existe alocação dinâmica e por quê?

Em `jogo.c`, `inicializar_partida()` aloca com `malloc` os `MAX_INIMIGOS` (3) inimigos usados
durante todo o jogo. Poderia ser um array estático (`Inimigo inimigos[MAX_INIMIGOS];`), mas
como o resto do código já trabalha com `Inimigo *` (ponteiro) em todo lugar — inclusive
passando o vetor para várias funções — optamos por alocar dinamicamente e manter tudo
consistente com ponteiros. A alocação acontece uma única vez; as ondas seguintes só
*reconfiguram* essa mesma memória (`inicializar_inimigo`), sem novos `malloc`/`free`.

### Por que várias funções recebem ponteiros em vez de retornar um valor?

Porque uma função em C só pode retornar um valor. Quando uma ação precisa alterar mais de uma
variável do chamador — por exemplo, `ativar_screen_shake` precisa alterar tanto o temporizador
quanto a intensidade do tremor de tela — a função recebe ponteiros (`int *timer`,
`float *intensidade`) para essas variáveis e as altera diretamente através do ponteiro
(`*timer = duracao;`).

### Como funciona o loop principal do jogo?

`main()` roda um `while (executando)` que espera o próximo evento com `al_wait_for_event()`
(a fila recebe eventos de teclado, fechamento da janela, e um "tick" do timer 60 vezes por
segundo). O evento é direcionado para a lógica do estado atual (`switch (estado_jogo)`), e
quando o evento é um "tick" do timer a lógica daquele estado avança um frame e a flag
`redesenhar` é ligada. A tela só é efetivamente redesenhada quando `redesenhar` está ligada e
não há mais eventos pendentes — isso evita desenhar mais de um frame por "tick" e evita
desenhar sem necessidade quando chega um evento que não muda nada visualmente.

### Como o Allegro trata os eventos?

O Allegro usa uma `ALLEGRO_EVENT_QUEUE`: diferentes fontes de eventos (teclado, timer, janela)
são registradas nela com `al_register_event_source`. O programa consome um evento por vez com
`al_wait_for_event` (que bloqueia até haver um evento, sem gastar CPU em um loop de polling) e
decide o que fazer olhando o campo `ev.type` (`ALLEGRO_EVENT_TIMER`, `ALLEGRO_EVENT_KEY_DOWN`,
`ALLEGRO_EVENT_DISPLAY_CLOSE`, etc.).

### Como os arquivos (assets) são utilizados?

Imagens (`.png`), a fonte (`.ttf`) e os áudios (`.ogg`) ficam em `assets/` e são carregados do
disco uma única vez, no início do programa, por funções em `recursos.c`
(`al_load_bitmap`, `al_load_font`, `al_load_sample`). Cada carregamento é checado: se o
arquivo não existir ou não puder ser lido, a função retorna `NULL`. Recursos essenciais
(fontes, cenário) sem os quais o jogo não pode nem desenhar a tela fazem o programa mostrar um
erro e encerrar (`mostrar_erro_fatal`); recursos opcionais (alguns efeitos sonoros — ver
README, "Limitações conhecidas") simplesmente não são tocados se o arquivo não existir, sem
travar o jogo.

### Como funciona a detecção de colisão?

Com o algoritmo clássico de **AABB** (*Axis-Aligned Bounding Box*): cada entidade tem uma
caixa retangular invisível (posição + largura + altura), e duas caixas colidem se elas se
sobrepõem nos eixos X e Y ao mesmo tempo (`verificar_colisao`, em `jogo.c`). O ataque do
cavaleiro, por exemplo, cria uma caixa de colisão temporária na frente dele, só ativa no
frame exato da animação em que a espada aparece, e testa contra a caixa de cada inimigo vivo.

### Como funciona o sistema de animação por spritesheet?

Cada spritesheet é uma imagem única com vários "frames" lado a lado, formando linhas e
colunas. Para desenhar o frame `N` de uma animação com `C` colunas, calculamos
`coluna = N % C` e `linha = N / C` (divisão e módulo inteiro), e usamos essas coordenadas
para recortar (`al_draw_scaled_bitmap`) só aquele pedaço da imagem. Um contador de frames
avança a cada poucos "ticks" do timer, dando a velocidade da animação.

### O que acontece se um asset (imagem/som/fonte) não carregar?

Depende de quão essencial ele é: fontes e as imagens de cenário/HUD são essenciais — sem elas
o jogo não tem como desenhar a tela, então o programa mostra uma caixa de mensagem nativa do
sistema operacional (`mostrar_erro_fatal`, em `recursos.c`, usando o addon
`allegro_native_dialog`) explicando o problema e encerra de forma controlada. Já efeitos
sonoros específicos e uma camada decorativa de fundo são opcionais: o ponteiro fica `NULL` e
todo lugar que toca aquele som ou desenha aquela imagem primeiro checa `if (recurso) { ... }`
antes de usar — então o jogo continua funcionando normalmente, só sem aquele som/imagem
específica. Isso evita um `NULL dereference` (uso de ponteiro nulo), que travaria o programa.

### Por que existe uma struct `EstadoPartida` e uma struct `RecursosJogo`?

Antes, o `main()` tinha mais de vinte variáveis soltas (posição do jogador, timers de
transição, ponteiros de som, etc.) e uma função para reiniciar o jogo que recebia 14
parâmetros. Agrupar esse estado em duas structs — uma para a partida em andamento
(`EstadoPartida`) e outra para os recursos carregados (`RecursosJogo`) — simplificou as
assinaturas de função (passar um único ponteiro para a struct, em vez de dezenas de
parâmetros) e deixou mais claro o que pertence a "uma partida" versus o que é carregado uma
única vez e reaproveitado entre partidas.

### Por que o sistema de parry existe e como ele foi implementado?

É a mecânica que recompensa reação rápida: se o jogador pressionar defender (`X`) e o ataque
do inimigo acertar a caixa de colisão do jogador enquanto a flag `pode_aparar` ainda está
ativa (só verdadeira nos primeiros frames da animação de defesa), o inimigo fica atordoado em
vez do jogador levar dano. Isso é checado dentro de `atualizar_inimigo` (`inimigo.c`), no
mesmo ponto onde o dano normal seria aplicado — é uma condição a mais antes de decidir entre
"atordoar o inimigo" ou "aplicar dano no jogador".
