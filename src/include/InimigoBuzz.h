#pragma once
#include "Tipos.h"

InimigoBuzz *criarInimigoBuzz( Rectangle ret, Color cor );
void destruirInimigoBuzz( InimigoBuzz *inimigo );
void atualizarInimigoBuzz( InimigoBuzz *inimigo, GameWorld *gw, float delta );
void desenharInimigoBuzz( InimigoBuzz *inimigo );
QuadroAnimacao *getQuadroAnimacaoAtualInimigoBuzz( InimigoBuzz *inimigo );