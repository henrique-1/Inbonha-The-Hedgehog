#pragma once
#include "Tipos.h"

ItemEscudo *criarItemEscudo( Rectangle ret, Color cor );
void destruirItemEscudo( ItemEscudo *item );
void atualizarItemEscudo( ItemEscudo *item, float delta );
void desenharItemEscudo( ItemEscudo *item );
QuadroAnimacao *getQuadroAnimacaoAtualItemEscudo( ItemEscudo *item );