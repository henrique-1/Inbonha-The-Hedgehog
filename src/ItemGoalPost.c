#include <stdlib.h>
#include "raylib/raylib.h"
#include "ItemGoalPost.h"
#include "ResourceManager.h"

ItemGoalPost *criarItemGoalPost( Rectangle ret, Color cor ) {
    ItemGoalPost *gp = (ItemGoalPost*) malloc( sizeof( ItemGoalPost ) );
    
    gp->ret = ret;
    gp->cor = cor;
    gp->estado = ESTADO_ITEM_GOAL_POST_PARADO;
    gp->ativo = true;
    
    gp->contadorTotalGiros = 0;
    gp->contadorTempoQuadro = 0.0f;
    gp->quadroAtualGiro = 1; // Inicia no índice 1 (Quadro 2) para o giro

    // Mapeamento preciso das coordenadas do sprite fornecido
    gp->quadros[0] = (Rectangle){ 1, 119, 48, 48 };   // Quadro 1: Robotnik
    gp->quadros[1] = (Rectangle){ 50, 119, 32, 48 };  // Quadro 2: Giro
    gp->quadros[2] = (Rectangle){ 83, 119, 8, 48 };   // Quadro 3: Fina
    gp->quadros[3] = (Rectangle){ 92, 119, 32, 48 };  // Quadro 4: Giro
    gp->quadros[4] = (Rectangle){ 125, 119, 48, 48 }; // Quadro 5: Sonic

    return gp;
}

void atualizarItemGoalPost( ItemGoalPost *gp, float delta ) {
    if ( gp->estado == ESTADO_ITEM_GOAL_POST_GIRANDO ) {
        gp->contadorTempoQuadro += delta;
        
        // Controle da velocidade da animação (0.04s por quadro de giro)
        if ( gp->contadorTempoQuadro >= 0.04f ) { 
            gp->contadorTempoQuadro = 0.0f;
            gp->quadroAtualGiro++;
            
            // Loop entre os quadros de giro (índices 1, 2 e 3)
            if ( gp->quadroAtualGiro > 3 ) {
                gp->quadroAtualGiro = 1;
                gp->contadorTotalGiros++;
                
                // 16 giros totais = 8 do Robotnik + 8 do Sonic
                if ( gp->contadorTotalGiros >= 16 ) {
                    gp->estado = ESTADO_ITEM_GOAL_POST_FINALIZADO;
                }
            }
        }
    }
}

void desenharItemGoalPost( ItemGoalPost *gp ) {
    Rectangle fonte;

    if ( gp->estado == ESTADO_ITEM_GOAL_POST_PARADO ) {
        fonte = gp->quadros[0];
    } else if ( gp->estado == ESTADO_ITEM_GOAL_POST_FINALIZADO ) {
        fonte = gp->quadros[4];
    } else {
        fonte = gp->quadros[gp->quadroAtualGiro];
    }

    // Calcula a posição do X para que o pivô de giro seja o centro
    Rectangle dest = gp->ret;
    dest.x = dest.x + (48.0f / 2.0f) - (fonte.width / 2.0f);
    dest.width = fonte.width;
    dest.height = fonte.height;

    // Utilize rm.texturaItens ou o nome que represente seu arquivo "itens.png"
    DrawTexturePro( rm.texturaItens, fonte, dest, (Vector2){0}, 0.0f, WHITE );
}

void destruirItemGoalPost( ItemGoalPost *gp ) {
    if ( gp != NULL ) {
        free( gp );
    }
}