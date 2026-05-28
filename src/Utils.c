/**
 * @file Utils.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação das funções utilitárias.
 *
 * @copyright Copyright (c) 2026
 */
#include "raylib/raylib.h"

#include "Utils.h"
#include <stdio.h>

/**
 * @brief Carrega uma textura trocando cores.
 */
Texture2D carregarTexturaAlterandoCores( const char *caminhoArquivo, Color *coresFonte, Color *coresAlvo, int quantidadeCores ) {
    Image imagem = LoadImage( caminhoArquivo );
    ImageFormat( &imagem, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 );
    for ( int i = 0; i < quantidadeCores; i++ ) {
        ImageColorReplace( &imagem, coresFonte[i], coresAlvo[i] );
    }
    Texture2D textura = LoadTextureFromImage( imagem );
    UnloadImage( imagem );
    return textura;
}

/**
 * Desenha elementos estáticos da HUD ignorando a margem de 1px.
 */
void desenharElementoHUD(Texture2D textura, Vector2 posicao, float largura, float altura) {
    Rectangle sourceRec = {
        .x = 1.0f,
        .y = 1.0f,
        .width = largura,
        .height = altura
    };
    DrawTextureRec(textura, sourceRec, posicao, WHITE);
}

/**
 * Extrai e desenha números individuais a partir do spritesheet, pulando os gaps de 1px.
 */
void desenharNumeroHUD(Texture2D textura, int valor, Vector2 posicao, int minDigitos, float larguraNum, float alturaNum) {
    char valorStr[16];
    
    // snprintf é a forma segura de formatar a string, garantindo limite de memória
    snprintf(valorStr, sizeof(valorStr), "%0*d", minDigitos, valor);
    
    for (int i = 0; valorStr[i] != '\0'; i++) {
        int digitoAtual = valorStr[i] - '0'; // Converte char para int
        
        Rectangle sourceRec = {
            .x = 1.0f + (digitoAtual * (larguraNum + 1.0f)), // Salta 1px de borda + (tamanho do dígito + 1px de espaço)
            .y = 1.0f,
            .width = larguraNum,
            .height = alturaNum
        };
        
        Vector2 posDestino = {
            .x = posicao.x + (i * larguraNum), // Renderiza os números lado a lado na tela
            .y = posicao.y
        };
        
        DrawTextureRec(textura, sourceRec, posDestino, WHITE);
    }
}