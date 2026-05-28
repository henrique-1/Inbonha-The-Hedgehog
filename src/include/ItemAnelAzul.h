/**
 * @file Obstaculo.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações das funções do Item (Anel Azul).
 * 
 * (1,35)
 * (1, 52)
 * h: 16
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria um novo item (anel).
 */
ItemAnelAzul *criarItemAnelAzul( Rectangle ret, Color cor );

/**
 * @brief Destroi um item (anel).
 */
void destruirItemAnelAzul( ItemAnelAzul *item );

/**
 * @brief Atualiza um item (anel).
 */
void atualizarItemAnelAzul( ItemAnelAzul *item, float delta );

/**
 * @brief Desenha um item (anel).
 */
void desenharItemAnelAzul( ItemAnelAzul *item );

/**
 * @brief Obtém o quadro de animação atual de um item (anel).
 */
QuadroAnimacao *getQuadroAnimacaoAtualItemAnelAzul( ItemAnelAzul *item );