#include <stdlib.h>
#include "raylib/raylib.h"
#include "Animacao.h"
#include "ItemEstrelinha.h"
#include "ResourceManager.h"

static void desenharQuadroAnimacaoItemEstrelinha( ItemEstrelinha *item, QuadroAnimacao *qa, Color tonalidade );
static Animacao *getAnimacaoAtualItemEstrelinha( ItemEstrelinha *item );

ItemEstrelinha *criarItemEstrelinha( Rectangle ret, Color cor ) {
    ItemEstrelinha *novo = (ItemEstrelinha*) malloc( sizeof( ItemEstrelinha ) );
    
    novo->ret = ret;
    novo->cor = cor;
    novo->estado = ESTADO_ITEM_ESTRELINHA_PARADO;
    novo->ativo = true;

    // Animação 1: TV Intacta (1 Quadro)
    novo->animacaoParado.quantidadeQuadros = 1;
    novo->animacaoParado.quadroAtual = 0;
    novo->animacaoParado.contadorTempoQuadro = 0.0f;
    novo->animacaoParado.pararNoUltimoQuadro = true;
    novo->animacaoParado.executarUmaVez = false;
    novo->animacaoParado.finalizada = false;
    criarQuadrosAnimacao( &novo->animacaoParado, novo->animacaoParado.quantidadeQuadros );
    // Fonte da TV Intacta: 1, 69, 32, 32
    inicializarQuadrosAnimacao( novo->animacaoParado.quadros, 1, 1000,
        1, 69, 32, 32, 0, false, (Rectangle) { 0, 0, 64, 64 } );

    // Animação 2: TV Destruída (1 Quadro)
    novo->animacaoColetando.quantidadeQuadros = 1;
    novo->animacaoColetando.quadroAtual = 0;
    novo->animacaoColetando.contadorTempoQuadro = 0.0f;
    novo->animacaoColetando.pararNoUltimoQuadro = true;
    novo->animacaoColetando.executarUmaVez = false;
    novo->animacaoColetando.finalizada = false;
    criarQuadrosAnimacao( &novo->animacaoColetando, novo->animacaoColetando.quantidadeQuadros );
    // Fonte da TV Destruída: 34, 85, 32, 16
    inicializarQuadrosAnimacao( novo->animacaoColetando.quadros, 1, 1000,
        34, 85, 32, 16, 0, false, (Rectangle) { 0 } );

    novo->animacoes[ESTADO_ITEM_ESTRELINHA_PARADO] = &novo->animacaoParado;
    novo->animacoes[ESTADO_ITEM_ESTRELINHA_COLETADO] = &novo->animacaoColetando;
    novo->quantidadeAnimacoes = 2;

    return novo;
}

void destruirItemEstrelinha( ItemEstrelinha *item ) {
    if ( item != NULL ) {
        for ( int i = 0; i < item->quantidadeAnimacoes; i++ ) destruirQuadrosAnimacao( item->animacoes[i] );
        free( item );
    }
}

void atualizarItemEstrelinha( ItemEstrelinha *item, float delta ) {
    if ( item->ativo ) {
        atualizarAnimacao( getAnimacaoAtualItemEstrelinha( item ), delta );
    }
}

void desenharItemEstrelinha( ItemEstrelinha *item ) {
    if ( item->ativo ) {
        desenharQuadroAnimacaoItemEstrelinha( item, getQuadroAnimacaoAtualItemEstrelinha( item ), WHITE );
    }
}

QuadroAnimacao *getQuadroAnimacaoAtualItemEstrelinha( ItemEstrelinha *item ) {
    return getQuadroAtualAnimacao( getAnimacaoAtualItemEstrelinha( item ) );
}

static Animacao *getAnimacaoAtualItemEstrelinha( ItemEstrelinha *item ) {
    return item->animacoes[item->estado];
}

static void desenharQuadroAnimacaoItemEstrelinha( ItemEstrelinha *item, QuadroAnimacao *qa, Color tonalidade ) {
   if ( qa != NULL ) {
        if ( item->estado == ESTADO_ITEM_ESTRELINHA_PARADO ) {
            // 1. Desenha a carcaça da TV (item->ret agora já é perfeitamente 64x64 no mapa)
            DrawTexturePro( rm.texturaItens, qa->fonte, item->ret, (Vector2){ 0 }, 0.0f, tonalidade );
            
            // 2. Desenha o Ícone da Estrelinha
            Rectangle fonteIcone = { 35, 102, 16, 16 };
            Rectangle destIcone = { 
                item->ret.x + 16,  // Offset alinhado à tela da TV
                item->ret.y + 12, 
                32, 
                32 
            };
            DrawTexturePro( rm.texturaItens, fonteIcone, destIcone, (Vector2){ 0 }, 0.0f, tonalidade );
            
        } else {
            // TV Destruída (Empurramos o Y +32 para colar a base destruída no chão)
            Rectangle destDestruida = { 
                item->ret.x, 
                item->ret.y + 32, 
                64, 
                32 
            };
            DrawTexturePro( rm.texturaItens, qa->fonte, destDestruida, (Vector2){ 0 }, 0.0f, tonalidade );
        }
    }
}