/**
 * @file GameWorld.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do GameWorld.
 *
 * @copyright Copyright (c) 2026
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "GameWorld.h"
#include "Jogador.h"
#include "Macros.h"
#include "Mapa.h"
#include "Obstaculo.h"
#include "Tipos.h"
#include "ResourceManager.h"
#include "Utils.h"

#include "raylib/raylib.h"
//#include "raylib/raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raylib/raygui.h"       // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

// Definição da ordem das fases
static const char *ARQUIVOS_MAPAS[] = {
    "resources/mapas/mapa01.txt",
    "resources/mapas/mapa02.txt"
    // Adicione mapa03.txt, etc, conforme for criando
};
static const int TOTAL_FASES = 2; // Atualize esse número conforme adicionar mapas

static void desenharFundo( GameWorld *gw );
static void atualizarCamera( GameWorld *gw );

static void inicializar( GameWorld *gw );
static void reiniciar( GameWorld *gw, bool gameOver );

static void desenharHUD( GameWorld *gw );

/**
 * @brief Cria uma instância alocada dinamicamente da struct GameWorld.
 */
GameWorld *createGameWorld( void ) {
    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );
    
    gw->faseAtual = 0; 
    gw->transicaoFase = false;
    gw->contadorTransicao = 0.0f;
    
    inicializar( gw );
    return gw;
}

/**
 * @brief Destrói um objeto GameWorld e suas dependências.
 */
void destroyGameWorld( GameWorld *gw ) {
    if ( gw != NULL ) {
        destruirMapa( gw->mapa );
        destruirJogador( gw->jogador );
        free( gw );
    }
}

/**
 * @brief Lê a entrada do usuário e atualiza o estado do jogo.
 */
void updateGameWorld( GameWorld *gw, float delta ) {

    Jogador *j = gw->jogador;

    // ========================================================================
    // 1. ESTADO DE GAME OVER OU TIME OVER ATIVO
    // ========================================================================
    if ( gw->gameOver || gw->timeOver ) {
        // Pausa as músicas
        if ( IsMusicStreamPlaying( rm.musicaFase01 ) ) StopMusicStream( rm.musicaFase01 );
        if ( IsMusicStreamPlaying( rm.musicaInvencibilidade ) ) StopMusicStream( rm.musicaInvencibilidade );

        // Continua atualizando apenas a gravidade do jogador (para ele cair fora da tela)
        if ( j->ret.y <= (float)calcularAlturaMapa(gw->mapa) + 1000.0f ) {
            atualizarJogador( j, gw, delta );
        }

        // Aguarda jogador apertar qualquer tecla para reiniciar todo o jogo
        if ( GetKeyPressed() != 0 || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) ) {
            gw->faseAtual = 0; // Retorna para a primeira fase
            gw->gameOver = false;
            gw->timeOver = false;
            reiniciar( gw, true ); // Reinicia zerando vidas, pontos e recriando mapa
        }
        
        return; // Retorna cedo para CONGELAR tempo, HUD, inimigos e câmera
    }

    if ( gw->jogador->temEstrela && gw->jogador->estado != ESTADO_JOGADOR_MORTO ) {
        
        // 1. Pausa a música normal da fase
        PauseMusicStream( rm.musicaFase01 ); 
        
        // 2. Se a música de invencibilidade não estiver tocando, dê Play nela
        if ( !IsMusicStreamPlaying( rm.musicaInvencibilidade ) ) {
            PlayMusicStream( rm.musicaInvencibilidade );
        }
        
        // 3. Atualiza o Stream da música de invencibilidade para ela tocar
        UpdateMusicStream( rm.musicaInvencibilidade );
        
    } else {
        
        // 1. Se a música de invencibilidade estiver tocando (o poder acabou de acabar)
        if ( IsMusicStreamPlaying( rm.musicaInvencibilidade ) ) {
            StopMusicStream( rm.musicaInvencibilidade ); // Para a invencibilidade
            ResumeMusicStream( rm.musicaFase01 );        // Retoma a música normal de onde parou
        }
        
        // 2. Atualiza o Stream da música da fase normalmente (Seu código original)
        if ( !IsMusicStreamPlaying( rm.musicaFase01 ) ) {
            PlayMusicStream( rm.musicaFase01 );
        } else {
            UpdateMusicStream( rm.musicaFase01 );
        }
    }

    if ( IsKeyPressed( KEY_R ) ) {
        reiniciar( gw, false ); // O botão R apenas reinicia o mapa mantendo os status
        return;
    }

    gw->tempoDecorrido += delta;

    // ========================================================================
    // 2. CHECAGEM DE TIME OVER
    // ========================================================================
    // Em Sonic clássico, o limite é 9:59 (599 segundos).
    if ( gw->tempoDecorrido >= 599.0f ) {
        matarJogador( j );
        gw->timeOver = true;
    }

    // ========================================================================
    // 3. CHECAGEM DE GAME OVER (Morto por Inimigo / Espinho)
    // ========================================================================
    if ( j->estado == ESTADO_JOGADOR_MORTO && j->quantidadeVidas < 0 && !gw->timeOver ) {
        gw->gameOver = true;
    }

    // 1. CHECAGEM DO GOAL POST
    ElementoMapa *el = gw->mapa->itens;
    while ( el != NULL ) {
        Item *item = (Item*) el->objeto;
        if ( item->tipo == TIPO_ITEM_GOAL_POST ) {
            ItemGoalPost *gp = (ItemGoalPost*) item->objeto;
            
            // Ativa o Goal Post se o jogador correr passando pela placa
            if ( gp->estado == ESTADO_ITEM_GOAL_POST_PARADO && j->ret.x >= gp->ret.x ) {
                gp->estado = ESTADO_ITEM_GOAL_POST_GIRANDO;
                gw->transicaoFase = true;
                gw->contadorTransicao = 0.0f;
            }
        }
        el = el->proximo;
    }

    // 2. CONTROLE DA MUDANÇA DE FASE
    if ( gw->transicaoFase ) {
        gw->contadorTransicao += delta;
        
        // Bloqueia controle do jogador e o faz sair correndo sozinho da tela
        j->vel.x = j->velAndandoRapido; 
        j->olhandoParaDireita = true;
        if( j->estado != ESTADO_JOGADOR_PULANDO && j->estado != ESTADO_JOGADOR_PULANDO_CORRENDO ) {
            j->estado = ESTADO_JOGADOR_CORRENDO;
        }

        // Aguarda 4 segundos para o Sonic sair da tela e a Placa terminar de rodar
        if ( gw->contadorTransicao >= 4.0f ) {
            gw->faseAtual++;
            
            // Sistema circular de fases
            if ( gw->faseAtual >= TOTAL_FASES ) {
                gw->faseAtual = 0; 
            }
            
            gw->transicaoFase = false;
            gw->contadorTransicao = 0.0f;
            
            reiniciar( gw, false ); // Aproveita sua arquitetura de preservação de saldo
            return;
        }
    } else {
        // Apenas escuta os controles normais se NÃO estiver finalizando a fase
        entradaJogador( j, delta );
    }

    atualizarMapa( gw->mapa, gw, delta );
    entradaJogador( j, delta );
    atualizarJogador( j, gw, delta );
    atualizarCamera( gw );

    float alturaMapa = (float) calcularAlturaMapa( gw->mapa );
    if ( j->ret.y > alturaMapa + 100.0f ) {
        if ( j->estado != ESTADO_JOGADOR_MORTO ) {
            matarJogador( j ); // Desconta vida se caiu num buraco sem ter tocado inimigo
        }

        if ( j->quantidadeVidas >= 0 ) {
            // Perdeu uma vida, mas ainda tem saldo. Recarrega o mapa preservando as vidas.
            reiniciar( gw, false ); 
        } else {
            // Vidas caíram abaixo de 0 (Game Over). Reseta a fase inteira (vidas voltam pra 3).
            reiniciar( gw, true ); 
        }
    }
}

/**
 * @brief Desenha o estado do jogo.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( (Color) { 36, 0, 180, 255 } );

    BeginMode2D( gw->camera );
    desenharFundo( gw );
    desenharMapa( gw->mapa );
    desenharJogador( gw->jogador );
    EndMode2D();

    desenharHUD( gw );
    DrawFPS( GetScreenWidth() - 100, 10 );

    EndDrawing();
}

static void desenharHUD( GameWorld *gw ) {
    Jogador *j = gw->jogador;

    // Trabalhar com floats facilita a passagem para os Vector2 do Raylib
    float margemEsquerda = 16.0f;
    float margemTopo = 16.0f;
    float espacamentoLinhas = 24.0f; 

    // ========================================================================
    // --- SCORE ---
    // ========================================================================
    Vector2 posScoreImg = { margemEsquerda, margemTopo };
    desenharElementoHUD( rm.texturaHudScore, posScoreImg, 40.0f, 16.0f );
    
    // Regra: 16px de distância entre a imagem útil (40px) e o primeiro número
    Vector2 posScoreNum = { margemEsquerda + 40.0f + 16.0f, margemTopo };
    desenharNumeroHUD( rm.texturaHudNumeros, j->pontuacao, posScoreNum, 6, 8.0f, 16.0f );

    // ========================================================================
    // --- TIME ---
    // ========================================================================
    float yTime = margemTopo + espacamentoLinhas;
    Texture2D texTimeAtual = rm.texturaHudTime;
    int minutosParaMostrar;
    int segundosParaMostrar;

    // Regra: Se invulnerável, texto vermelho e mostra cronômetro decrescente
    if ( j->invulneravel ) {
        texTimeAtual = rm.texturaHudTimeRed; // Substitui pela imagem vermelha
        float tempoRestante = j->tempoInvulnerabilidade - j->contadorTempoInvulnerabilidade;
        minutosParaMostrar = (int)tempoRestante / 60;
        segundosParaMostrar = (int)tempoRestante % 60;
    } else {
        minutosParaMostrar = (int)(gw->tempoDecorrido) / 60;
        segundosParaMostrar = (int)(gw->tempoDecorrido) % 60;
    }

    Vector2 posTimeImg = { margemEsquerda, yTime };
    desenharElementoHUD( texTimeAtual, posTimeImg, 32.0f, 16.0f );
    
    // Regra: 16px de distância entre a imagem TIME útil (32px) e o primeiro número (Minuto)
    Vector2 posTimeMin = { margemEsquerda + 32.0f + 16.0f, yTime };
    desenharNumeroHUD( rm.texturaHudNumeros, minutosParaMostrar, posTimeMin, 1, 8.0f, 16.0f );
    
    // Regra: O ':' deve estar logo após o primeiro número (largura do número é 8.0f)
    Vector2 posTimeColon = { posTimeMin.x + 8.0f, yTime }; 
    desenharElementoHUD( rm.texturaHudTimeColon, posTimeColon, 8.0f, 16.0f );
    
    // Desenha os segundos logo após a imagem do ':' (largura do ':' é 8.0f)
    Vector2 posTimeSeg = { posTimeColon.x + 8.0f, yTime };
    desenharNumeroHUD( rm.texturaHudNumeros, segundosParaMostrar, posTimeSeg, 2, 8.0f, 16.0f );

    // ========================================================================
    // --- RINGS ---
    // ========================================================================
    float yRings = margemTopo + espacamentoLinhas * 2.0f;
    Texture2D texRingsAtual = rm.texturaHudRings;

    // Regras de piscar a imagem para o estado Red
    if ( j->invulneravel ) {
        if ( (int)(j->contadorTempoInvulnerabilidade * 2) % 2 == 0 ) {
            texRingsAtual = rm.texturaHudRingsRed;
        }
    } else if ( j->quantidadeAneis == 0 && ((int)(gw->tempoDecorrido * 4) % 2 == 0) ) {
        texRingsAtual = rm.texturaHudRingsRed;
    }

    Vector2 posRingsImg = { margemEsquerda, yRings };
    desenharElementoHUD( texRingsAtual, posRingsImg, 40.0f, 16.0f );
    
    // Regra: 8px de distância da imagem RINGS útil (40px)
    Vector2 posRingsNum = { margemEsquerda + 40.0f + 8.0f, yRings };
    desenharNumeroHUD( rm.texturaHudNumeros, j->quantidadeAneis, posRingsNum, 1, 8.0f, 16.0f );

    // ========================================================================
    // --- VIDAS ---
    // ========================================================================
    // Consideramos apenas os 16px úteis de altura do scoreboard para alinhar
    float yVidas = GetScreenHeight() - 16.0f - 16.0f; 
    
    Vector2 posVidasImg = { margemEsquerda, yVidas };
    desenharElementoHUD( rm.texturaHudVidas, posVidasImg, 49.0f, 16.0f ); // O scoreboard útil tem 49x16
    
    // Regra rigorosa de posicionamento interno do HUD de vidas:
    // 35x10 para o 1º número e 43x10 para o 2º número (baseado no canto superior esquerdo do elemento)
    int dezenaVidas = j->quantidadeVidas / 10;
    int unidadeVidas = j->quantidadeVidas % 10;

    // Se o jogador possuir 10 ou mais vidas, a dezena aparece na posição X=35 relativa
    if ( j->quantidadeVidas >= 10 ) {
        Vector2 posVidasDezena = { margemEsquerda + 35.0f, yVidas + 10.0f };
        // Atenção: As vidas usam uma textura de números menores (8x8)
        desenharNumeroHUD( rm.texturaHudNumerosPontos, dezenaVidas, posVidasDezena, 1, 8.0f, 8.0f ); 
    }
    
    // A unidade sempre aparece na posição absoluta 43 relativa, alinhando visualmente à direita
    Vector2 posVidasUnidade = { margemEsquerda + 43.0f, yVidas + 10.0f };
    desenharNumeroHUD( rm.texturaHudNumerosPontos, unidadeVidas, posVidasUnidade, 1, 8.0f, 8.0f );

    // ========================================================================
    // --- MENSAGENS DE GAME OVER E TIME OVER ---
    // ========================================================================
    if ( gw->gameOver || gw->timeOver ) {
        
        Rectangle fonteOrigem;
        
        // Aplica os recortes que você forneceu
        if ( gw->gameOver ) {
            fonteOrigem = (Rectangle){ 456, 95, 144, 16 };
        } else { 
            fonteOrigem = (Rectangle){ 456, 119, 136, 16 };
        }

        // Multiplicador de escala da interface (ajuste se necessário para padronizar com o seu HUD)
        float escala = 2.0f; 
        
        // Lógica para centralizar as sprites perfeitamente no meio da janela
        Vector2 posicao = {
            (GetScreenWidth() - (fonteOrigem.width * escala)) / 2.0f,
            (GetScreenHeight() - (fonteOrigem.height * escala)) / 2.0f
        };

        DrawTexturePro( 
            rm.texturaObjetos, 
            fonteOrigem, 
            (Rectangle){ posicao.x, posicao.y, fonteOrigem.width * escala, fonteOrigem.height * escala }, 
            (Vector2){ 0 }, 
            0.0f, 
            WHITE 
        );
    }
}

static void desenharFundo( GameWorld *gw ) {

    int larguraFundo = rm.texturaFundo.width;
    int larguraMapa = calcularLarguraMapa( gw->mapa );
    int alturaMapa = calcularAlturaMapa( gw->mapa );
    int repeticoes = larguraMapa / larguraFundo;

    int deslocamentoParallax = (int) ( ( gw->camera.target.x / (float) larguraMapa ) * 200 );

    for ( int i = 0; i <= repeticoes; i++ ) {
        DrawTexture( rm.texturaFundo, larguraFundo * i - deslocamentoParallax, alturaMapa - rm.texturaFundo.height, WHITE );
    }

}

static void atualizarCamera( GameWorld *gw ) {

    Jogador *j = gw->jogador;
    Camera2D *c = &gw->camera;

    c->offset.x = GetScreenWidth() / 2;
    c->offset.y = GetScreenHeight() / 2;

    // O target é arredondado para o inteiro mais próximo para garantir que a
    // translação da câmera ocorra sempre em posições inteiras de pixel. Sem esse
    // arredondamento, o valor float contínuo de ret.x faz os tiles serem
    // renderizados em posições subpixel, causando frestas visíveis entre eles.
    c->target.x = roundf( j->ret.x + j->ret.width / 2.0f );
    c->target.y = roundf( j->ret.y + j->ret.height / 2.0f );

    int minX = GetScreenWidth() / 2;
    int maxX = calcularLarguraMapa( gw->mapa ) - GetScreenWidth() / 2;
    int maxY = calcularAlturaMapa( gw->mapa ) - GetScreenHeight() / 2;

    if ( c->target.x < minX ) {
        c->target.x = minX;
    } else if ( c->target.x > maxX ) {
        c->target.x = maxX;
    }

    if ( c->target.y > maxY ) {
        c->target.y = maxY;
    }

}

static void inicializar( GameWorld *gw ) {

    //gw->mapa = carregarMapa( "resources/mapas/mapaTeste.txt" );
    gw->mapa = carregarMapa( ARQUIVOS_MAPAS[gw->faseAtual] );
    gw->jogador = criarJogador( GetScreenWidth() / 2 + 144, calcularAlturaMapa( gw->mapa ) - 196, 96, 96 );

    gw->camera = (Camera2D) {
        .offset = { 0 },    // deslocamento relativo da câmera em relação ao alvo
        .target = { 0 },    // o alvo da câmera, ou seja, a coordenada em que ela está centralizada
        .rotation = 0.0f,   // rotação da câmera em graus. o pivô é o alvo.
        .zoom = 1.0f        // zoom da câmera. 1.0f significa sem escala
    };

    gw->gravidade = 900;
    gw->tempoDecorrido = 0.0f;

    gw->gameOver = false;
    gw->timeOver = false;

}

static void reiniciar( GameWorld *gw, bool gameOver ) {

    // 1. Salva o estado atual (vidas e pontos) antes de destruir o objeto
    int vidasRestantes = gw->jogador->quantidadeVidas;
    int pontuacaoAtual = gw->jogador->pontuacao;

    destruirMapa( gw->mapa );
    destruirJogador( gw->jogador );

    if ( IsMusicStreamPlaying( rm.musicaFase01 ) ) {
        StopMusicStream( rm.musicaFase01 );
    }

    // 2. Cria um novo mapa e um novo jogador (que virá com 3 vidas e 0 pontos por padrão)
    inicializar( gw );

    // 3. Se NÃO for Game Over, injetamos as vidas e os pontos salvos de volta no novo jogador
    if ( !gameOver ) {
        gw->jogador->quantidadeVidas = vidasRestantes;
        gw->jogador->pontuacao = pontuacaoAtual;
    }

}