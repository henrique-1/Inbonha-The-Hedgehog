/**
 * @file InimigoCrabmeat.c
 * @brief Implementação do Inimigo (Crabmeat).
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoCrabmeat.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacaoInimigoCrabmeat( InimigoCrabmeat *inimigo, QuadroAnimacao *qa, Color tonalidade );
static void desenharQuadroAnimacaoInimigoCrabmeatMorrendo( InimigoCrabmeat *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade );
static Animacao *getAnimacaoAtualInimigoCrabmeat( InimigoCrabmeat *inimigo );

static const bool MOSTRAR_RETANGULOS = false;

InimigoCrabmeat *criarInimigoCrabmeat( Rectangle ret, Color cor ) {

    InimigoCrabmeat *novoInimigo = (InimigoCrabmeat*) malloc( sizeof( InimigoCrabmeat ) );
    if (novoInimigo == NULL) return NULL; // Boas práticas: verificar alocação

    novoInimigo->ret = ret;
    novoInimigo->vel = (Vector2) { 0 };
    novoInimigo->cor = cor;

    novoInimigo->velAndando = 80; // Crabmeat costuma ser um pouco mais lento
    novoInimigo->velMaxQueda = 600;

    novoInimigo->estado = ESTADO_INIMIGO_CRABMEAT_ANDANDO;
    novoInimigo->ativo = true;
    novoInimigo->olhandoParaDireita = false;

    // Configuração das animações
    novoInimigo->animacaoAndando.quantidadeQuadros = 3; // Ajuste conforme sua Sprite
    novoInimigo->animacaoAndando.quadroAtual = 0;
    novoInimigo->animacaoAndando.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoAndando.pararNoUltimoQuadro = false;
    novoInimigo->animacaoAndando.executarUmaVez = false;
    novoInimigo->animacaoAndando.finalizada = false;
    criarQuadrosAnimacao( &novoInimigo->animacaoAndando, novoInimigo->animacaoAndando.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoInimigo->animacaoAndando.quadros,
        novoInimigo->animacaoAndando.quantidadeQuadros,
        300,             // duração
        1, 104,          // INÍCIO (Ajuste a coordenada Y para bater com a sprite do Crabmeat)
        48, 32,          // dimensões (Ajuste largura e altura)
        1,               // separação
        false,           
        (Rectangle) { 2, 2, 44, 56 } // Retângulo de colisão
    );

    // Animação de Morte (Padrão para badniks)
    novoInimigo->animacaoMorrendo.quantidadeQuadros = 4;
    novoInimigo->animacaoMorrendo.quadroAtual = 0;
    novoInimigo->animacaoMorrendo.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoMorrendo.pararNoUltimoQuadro = false;
    novoInimigo->animacaoMorrendo.executarUmaVez = true;
    novoInimigo->animacaoMorrendo.finalizada = false;
    criarQuadrosAnimacao( &novoInimigo->animacaoMorrendo, novoInimigo->animacaoMorrendo.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoInimigo->animacaoMorrendo.quadros,
        novoInimigo->animacaoMorrendo.quantidadeQuadros,
        100,
        169, 1, 32, 32, 1, false, (Rectangle) { 0 }
    );

    novoInimigo->animacoes[ESTADO_INIMIGO_CRABMEAT_ANDANDO] = &novoInimigo->animacaoAndando;
    novoInimigo->animacoes[ESTADO_INIMIGO_CRABMEAT_MORRENDO] = &novoInimigo->animacaoMorrendo;
    novoInimigo->quantidadeAnimacoes = 2;

    return novoInimigo;
}

void destruirInimigoCrabmeat( InimigoCrabmeat *inimigo ) {
    if ( inimigo != NULL ) {
        for ( int i = 0; i < inimigo->quantidadeAnimacoes; i++ ) {
            destruirQuadrosAnimacao( inimigo->animacoes[i] );
        }
        free( inimigo );
    }
}

void atualizarInimigoCrabmeat( InimigoCrabmeat *inimigo, GameWorld *gw, float delta ) {
    if ( inimigo->ativo ) {
        if ( inimigo->estado == ESTADO_INIMIGO_CRABMEAT_ANDANDO ) {
            atualizarAnimacao( getAnimacaoAtualInimigoCrabmeat( inimigo ), delta );

            Inimigo ini = { .objeto = inimigo, .tipo = TIPO_INIMIGO_CRABMEAT };

            inimigo->vel.x = inimigo->olhandoParaDireita ? inimigo->velAndando : -inimigo->velAndando;

            // Eixo X
            inimigo->ret.x += inimigo->vel.x * delta;
            resolverColisaoInimigoObstaculosMapaX( &ini, gw->mapa );

            // Gravidade e Eixo Y
            inimigo->vel.y += gw->gravidade * delta;
            if ( inimigo->vel.y > inimigo->velMaxQueda ) inimigo->vel.y = inimigo->velMaxQueda;
            
            inimigo->ret.y += inimigo->vel.y * delta;
            resolverColisaoInimigoObstaculosMapaY( &ini, gw->mapa );

        } else if ( inimigo->estado == ESTADO_INIMIGO_CRABMEAT_MORRENDO ) {
            atualizarAnimacao( &inimigo->animacaoMorrendo, delta );
            if ( inimigo->animacaoMorrendo.finalizada ) inimigo->ativo = false;
        }
    }
}

void desenharInimigoCrabmeat( InimigoCrabmeat *inimigo ) {
    if ( inimigo->ativo ) {
        if ( inimigo->estado == ESTADO_INIMIGO_CRABMEAT_ANDANDO ) {
            desenharQuadroAnimacaoInimigoCrabmeat( inimigo, getQuadroAnimacaoAtualInimigoCrabmeat( inimigo ), WHITE );
        } else if ( inimigo->estado == ESTADO_INIMIGO_CRABMEAT_MORRENDO ) {
            desenharQuadroAnimacaoInimigoCrabmeatMorrendo( inimigo, getQuadroAtualAnimacao( &inimigo->animacaoMorrendo ), 2.0f, WHITE );
        }

        if ( MOSTRAR_RETANGULOS ) {
            DrawRectangleRec( inimigo->ret, Fade( inimigo->cor, 0.5f ) );
            DrawRectangleLines( inimigo->ret.x, inimigo->ret.y, inimigo->ret.width, inimigo->ret.height, BLACK );
        }
    }
}

QuadroAnimacao *getQuadroAnimacaoAtualInimigoCrabmeat( InimigoCrabmeat *inimigo ) {
    return getQuadroAtualAnimacao( getAnimacaoAtualInimigoCrabmeat( inimigo ) );
}

static void desenharQuadroAnimacaoInimigoCrabmeat( InimigoCrabmeat *inimigo, QuadroAnimacao *qa, Color tonalidade ) {
    if ( qa != NULL ) {
        DrawTexturePro(
            rm.texturaBadniks,
            (Rectangle) {
                qa->fonte.x, qa->fonte.y,
                inimigo->olhandoParaDireita ? -qa->fonte.width : qa->fonte.width, 
                qa->fonte.height
            },
            inimigo->ret, (Vector2) { 0 }, 0.0f, tonalidade
        );
    }
}

static void desenharQuadroAnimacaoInimigoCrabmeatMorrendo( InimigoCrabmeat *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade ) {
    if ( qa != NULL ) {
        DrawTexturePro(
            rm.texturaBadniks, qa->fonte,
            (Rectangle) { inimigo->ret.x, inimigo->ret.y, qa->fonte.width * escala, qa->fonte.height * escala },
            (Vector2) { 0 }, 0.0f, tonalidade
        );
    }
}

static Animacao *getAnimacaoAtualInimigoCrabmeat( InimigoCrabmeat *inimigo ) {
    return inimigo->animacoes[inimigo->estado];
}