/**
 * @file Utils.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações das funções utilitárias.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "raylib/raylib.h"

/**
 * @brief Carrega uma textura trocando cores.
 */
Texture2D carregarTexturaAlterandoCores( const char *caminhoArquivo, Color *coresFonte, Color *coresAlvo, int quantidadeCores );

void desenharElementoHUD(Texture2D textura, Vector2 posicao, float largura, float altura);
void desenharNumeroHUD(Texture2D textura, int valor, Vector2 posicao, int minDigitos, float larguraNum, float alturaNum);