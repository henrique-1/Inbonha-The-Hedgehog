/**
 * @file InimigoCrabmeat.h
 * @author Prof. Dr. David Buzatto / Contribuição
 * @brief Declarações das funções do Inimigo (Crabmeat).
 */
#pragma once

#include "Tipos.h"

InimigoCrabmeat *criarInimigoCrabmeat( Rectangle ret, Color cor );
void destruirInimigoCrabmeat( InimigoCrabmeat *inimigo );
void atualizarInimigoCrabmeat( InimigoCrabmeat *inimigo, GameWorld *gw, float delta );
void desenharInimigoCrabmeat( InimigoCrabmeat *inimigo );
QuadroAnimacao *getQuadroAnimacaoAtualInimigoCrabmeat( InimigoCrabmeat *inimigo );