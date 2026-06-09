#include <stdlib.h>
#include "raylib/raylib.h"
#include "Animacao.h"
#include "ItemEscudo.h"
#include "ResourceManager.h"

static void desenharQuadroAnimacaoItemEscudo( ItemEscudo *item, QuadroAnimacao *qa, Color tonalidade );
static Animacao *getAnimacaoAtualItemEscudo( ItemEscudo *item );

ItemEscudo *criarItemEscudo( Rectangle ret, Color cor ) {
    ItemEscudo *novo = (ItemEscudo*) malloc( sizeof( ItemEscudo ) );
    
    novo->ret = ret;
    novo->cor = cor;
    novo->estado = ESTADO_ITEM_ESCUDO_PARADO;
    novo->ativo = true;

    // Animação 1: TV Intacta (1 Quadro)
    novo->animacaoParado.quantidadeQuadros = 1;
    novo->animacaoParado.quadroAtual = 0;
    novo->animacaoParado.contadorTempoQuadro = 0.0f;
    novo->animacaoParado.pararNoUltimoQuadro = true;
    novo->animacaoParado.executarUmaVez = false;
    novo->animacaoParado.finalizada = false;
    criarQuadrosAnimacao( &novo->animacaoParado, novo->animacaoParado.quantidadeQuadros );
    inicializarQuadrosAnimacao( novo->animacaoParado.quadros, 1, 1000,
        1, 69, 32, 32, 0, false, (Rectangle) { 0, 0, 32, 32 } );

    // Animação 2: TV Destruída (1 Quadro)
    novo->animacaoColetando.quantidadeQuadros = 1;
    novo->animacaoColetando.quadroAtual = 0;
    novo->animacaoColetando.contadorTempoQuadro = 0.0f;
    novo->animacaoColetando.pararNoUltimoQuadro = true;
    novo->animacaoColetando.executarUmaVez = false;
    novo->animacaoColetando.finalizada = false;
    criarQuadrosAnimacao( &novo->animacaoColetando, novo->animacaoColetando.quantidadeQuadros );
    inicializarQuadrosAnimacao( novo->animacaoColetando.quadros, 1, 1000,
        34, 85, 32, 16, 0, false, (Rectangle) { 0 } );

    novo->animacoes[ESTADO_ITEM_ESCUDO_PARADO] = &novo->animacaoParado;
    novo->animacoes[ESTADO_ITEM_ESCUDO_COLETADO] = &novo->animacaoColetando;
    novo->quantidadeAnimacoes = 2;

    return novo;
}

void destruirItemEscudo( ItemEscudo *item ) {
    if ( item != NULL ) {
        for ( int i = 0; i < item->quantidadeAnimacoes; i++ ) destruirQuadrosAnimacao( item->animacoes[i] );
        free( item );
    }
}

void atualizarItemEscudo( ItemEscudo *item, float delta ) {
    if ( item->ativo ) {
        atualizarAnimacao( getAnimacaoAtualItemEscudo( item ), delta );
    }
}

void desenharItemEscudo( ItemEscudo *item ) {
    if ( item->ativo ) {
        desenharQuadroAnimacaoItemEscudo( item, getQuadroAnimacaoAtualItemEscudo( item ), WHITE );
    }
}

QuadroAnimacao *getQuadroAnimacaoAtualItemEscudo( ItemEscudo *item ) {
    return getQuadroAtualAnimacao( getAnimacaoAtualItemEscudo( item ) );
}

static Animacao *getAnimacaoAtualItemEscudo( ItemEscudo *item ) {
    return item->animacoes[item->estado];
}

static void desenharQuadroAnimacaoItemEscudo( ItemEscudo *item, QuadroAnimacao *qa, Color tonalidade ) {
    if ( qa != NULL ) {
        if ( item->estado == ESTADO_ITEM_ESCUDO_PARADO ) {
            // 1. Desenha a carcaça da TV
            DrawTexturePro( rm.texturaItens, qa->fonte, item->ret, (Vector2){ 0 }, 0.0f, tonalidade );
            
            // 2. Desenha o Ícone do Escudo no centro da tela da TV (Offset X+8, Y+6)
            Rectangle fonteIcone = { 69, 102, 16, 16 };
            Rectangle destIcone = { item->ret.x + 8, item->ret.y + 6, 16, 16 };
            DrawTexturePro( rm.texturaItens, fonteIcone, destIcone, (Vector2){ 0 }, 0.0f, tonalidade );
        } else {
            Rectangle destDestruida = { item->ret.x, item->ret.y + 16, 32, 16 };
            DrawTexturePro( rm.texturaItens, qa->fonte, destDestruida, (Vector2){ 0 }, 0.0f, tonalidade );
        }
    }
}