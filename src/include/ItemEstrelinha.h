#pragma once
#include "Tipos.h"

ItemEstrelinha *criarItemEstrelinha( Rectangle ret, Color cor );
void destruirItemEstrelinha( ItemEstrelinha *item );
void atualizarItemEstrelinha( ItemEstrelinha *item, float delta );
void desenharItemEstrelinha( ItemEstrelinha *item );
QuadroAnimacao *getQuadroAnimacaoAtualItemEstrelinha( ItemEstrelinha *item );