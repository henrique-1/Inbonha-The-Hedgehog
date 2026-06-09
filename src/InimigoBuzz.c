#include <stdio.h>
#include <stdlib.h>
#include "raylib/raylib.h"
#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoBuzz.h"
#include "Macros.h"
#include "ResourceManager.h"

static void desenharQuadroAnimacaoInimigoBuzz( InimigoBuzz *inimigo, QuadroAnimacao *qa, Color tonalidade );
static void desenharQuadroAnimacaoInimigoBuzzMorrendo( InimigoBuzz *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade );
static Animacao *getAnimacaoAtualInimigoBuzz( InimigoBuzz *inimigo );

static const bool MOSTRAR_RETANGULOS = false;

InimigoBuzz *criarInimigoBuzz( Rectangle ret, Color cor ) {
    InimigoBuzz *novo = (InimigoBuzz*) malloc( sizeof( InimigoBuzz ) );

    novo->ret = ret;
    novo->vel = (Vector2) { 0 };
    novo->cor = cor;
    novo->velVoando = 150; // Velocidade de voo horizontal
    novo->estado = ESTADO_INIMIGO_BUZZ_VOANDO;
    novo->ativo = true;
    novo->olhandoParaDireita = false;

    // Configuração das animações (Ajuste as posições X e Y da fonte conforme sua spritesheet)
    novo->animacaoVoando.quantidadeQuadros = 4;
    novo->animacaoVoando.quadroAtual = 0;
    novo->animacaoVoando.contadorTempoQuadro = 0.0f;
    novo->animacaoVoando.pararNoUltimoQuadro = false;
    novo->animacaoVoando.executarUmaVez = false;
    novo->animacaoVoando.finalizada = false;
    criarQuadrosAnimacao( &novo->animacaoVoando, novo->animacaoVoando.quantidadeQuadros );
    inicializarQuadrosAnimacao( novo->animacaoVoando.quadros, novo->animacaoVoando.quantidadeQuadros, 150,
        1, 72, 48, 32, 1, false, (Rectangle) { 4, 12, 72, 44 } );

    novo->animacaoMorrendo.quantidadeQuadros = 4;
    novo->animacaoMorrendo.quadroAtual = 0;
    novo->animacaoMorrendo.contadorTempoQuadro = 0.0f;
    novo->animacaoMorrendo.pararNoUltimoQuadro = false;
    novo->animacaoMorrendo.executarUmaVez = true;
    novo->animacaoMorrendo.finalizada = false;
    criarQuadrosAnimacao( &novo->animacaoMorrendo, novo->animacaoMorrendo.quantidadeQuadros );
    inicializarQuadrosAnimacao( novo->animacaoMorrendo.quadros, novo->animacaoMorrendo.quantidadeQuadros, 100,
        169, 1, 32, 32, 1, false, (Rectangle) { 0 } );

    novo->animacoes[ESTADO_INIMIGO_BUZZ_VOANDO] = &novo->animacaoVoando;
    novo->animacoes[ESTADO_INIMIGO_BUZZ_MORRENDO] = &novo->animacaoMorrendo;
    novo->quantidadeAnimacoes = 2;

    return novo;
}

/**
 * @brief Obtém o quadro de animação atual de um inimigo (Buzz).
 */
QuadroAnimacao *getQuadroAnimacaoAtualInimigoBuzz( InimigoBuzz *inimigo ) {
    return getQuadroAtualAnimacao( getAnimacaoAtualInimigoBuzz( inimigo ) );
}

void destruirInimigoBuzz( InimigoBuzz *inimigo ) {
    if ( inimigo != NULL ) {
        for ( int i = 0; i < inimigo->quantidadeAnimacoes; i++ ) destruirQuadrosAnimacao( inimigo->animacoes[i] );
        free( inimigo );
    }
}

void atualizarInimigoBuzz( InimigoBuzz *inimigo, GameWorld *gw, float delta ) {
    if ( inimigo->ativo ) {
        if ( inimigo->estado == ESTADO_INIMIGO_BUZZ_VOANDO ) {
            atualizarAnimacao( getAnimacaoAtualInimigoBuzz( inimigo ), delta );

            Inimigo ini = { .objeto = inimigo, .tipo = TIPO_INIMIGO_BUZZ };

            inimigo->vel.x = inimigo->olhandoParaDireita ? inimigo->velVoando : -inimigo->velVoando;

            // Atualiza X e resolve colisões apenas no eixo X (rebate nas paredes)
            inimigo->ret.x += inimigo->vel.x * delta;
            resolverColisaoInimigoObstaculosMapaX( &ini, gw->mapa );

            // Sem gravidade! Inimigo flutua na posição Y original.
        } else if ( inimigo->estado == ESTADO_INIMIGO_BUZZ_MORRENDO ) {
            atualizarAnimacao( &inimigo->animacaoMorrendo, delta );
            if ( inimigo->animacaoMorrendo.finalizada ) inimigo->ativo = false;
        }
    }
}

void desenharInimigoBuzz( InimigoBuzz *inimigo ) {
    if ( inimigo->ativo ) {
        if ( inimigo->estado == ESTADO_INIMIGO_BUZZ_VOANDO ) {
            QuadroAnimacao *qa = getQuadroAnimacaoAtualInimigoBuzz( inimigo );
            desenharQuadroAnimacaoInimigoBuzz( inimigo, qa, WHITE );
        } else if ( inimigo->estado == ESTADO_INIMIGO_BUZZ_MORRENDO ) {
            desenharQuadroAnimacaoInimigoBuzzMorrendo( inimigo, getQuadroAtualAnimacao( &inimigo->animacaoMorrendo ), 2.0f, WHITE );
        }

        if ( MOSTRAR_RETANGULOS ) {
            DrawRectangleRec( inimigo->ret, Fade( inimigo->cor, 0.5f ) );
            DrawRectangleLines( inimigo->ret.x, inimigo->ret.y, inimigo->ret.width, inimigo->ret.height, BLACK );
        }
    }
}

static void desenharQuadroAnimacaoInimigoBuzz( InimigoBuzz *inimigo, QuadroAnimacao *qa, Color tonalidade ) {
    if ( qa != NULL ) {
        DrawTexturePro(
            rm.texturaBadniks,
            (Rectangle) {
                qa->fonte.x,
                qa->fonte.y,
                // O Buzz inverte a sprite dependendo de onde está olhando
                inimigo->olhandoParaDireita ? -qa->fonte.width : qa->fonte.width, 
                qa->fonte.height
            },
            inimigo->ret,
            (Vector2) { 0 },
            0.0f,
            tonalidade
        );

        if ( MOSTRAR_RETANGULOS ) {
            float xDesenho = inimigo->olhandoParaDireita
                ? inimigo->ret.x + inimigo->ret.width - qa->retColisao.x - qa->retColisao.width
                : inimigo->ret.x + qa->retColisao.x;
            float yDesenho = inimigo->ret.y + qa->retColisao.y;
            DrawRectangle( xDesenho, yDesenho, qa->retColisao.width, qa->retColisao.height, Fade( GREEN, 0.5f ) );
        }
    }
}

static void desenharQuadroAnimacaoInimigoBuzzMorrendo( InimigoBuzz *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade ) {
    if ( qa != NULL ) {
        DrawTexturePro(
            rm.texturaBadniks,
            qa->fonte,
            (Rectangle) {
                inimigo->ret.x,
                inimigo->ret.y,
                qa->fonte.width * escala,
                qa->fonte.height * escala
            },
            (Vector2) { 0 },
            0.0f,
            tonalidade
        );
    }
}

static Animacao *getAnimacaoAtualInimigoBuzz( InimigoBuzz *inimigo ) {
    return inimigo->animacoes[inimigo->estado];
}