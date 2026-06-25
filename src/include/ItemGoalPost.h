#pragma once
#include "Tipos.h"

ItemGoalPost *criarItemGoalPost( Rectangle ret, Color cor );
void atualizarItemGoalPost( ItemGoalPost *gp, float delta );
void desenharItemGoalPost( ItemGoalPost *gp );
void destruirItemGoalPost( ItemGoalPost *gp );