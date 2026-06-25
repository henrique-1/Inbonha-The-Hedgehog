/**
 * @file GameWindow.c
 * @author Prof. Dr. David Buzatto
 * @brief GameWindow implementation.
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdlib.h>
#include <stdbool.h>

#include "Animacao.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "ResourceManager.h"
#include "raylib/raylib.h"

/**
 * @brief Creates a dinamically allocated GameWindow struct instance.
 */
GameWindow *createGameWindow( 
        int width, 
        int height, 
        const char *title, 
        int targetFPS,
        bool antialiasing, 
        bool resizable, 
        bool fullScreen,
        bool undecorated, 
        bool alwaysOnTop, 
        bool invisibleBackground, 
        bool alwaysRun, 
        bool loadResources, 
        bool initAudio ) {

    GameWindow *gameWindow = (GameWindow*) malloc( sizeof( GameWindow ) );

    gameWindow->width = width;
    gameWindow->height = height;
    gameWindow->title = title;
    gameWindow->targetFPS = targetFPS;
    gameWindow->antialiasing = antialiasing;
    gameWindow->resizable = resizable;
    gameWindow->fullScreen = fullScreen;
    gameWindow->undecorated = undecorated;
    gameWindow->alwaysOnTop = alwaysOnTop;
    gameWindow->invisibleBackground = invisibleBackground;
    gameWindow->alwaysRun = alwaysRun;
    gameWindow->loadResources = loadResources;
    gameWindow->initAudio = initAudio;
    gameWindow->gw = NULL;
    gameWindow->initialized = false;
    
    // Define que o jogo sempre abre na tela de título
    gameWindow->naTelaTitulo = true;

    // Configuração da Animação da Tela de Título (18 quadros)
    gameWindow->animacaoTelaTitulo.quantidadeQuadros = 18;
    gameWindow->animacaoTelaTitulo.quadroAtual = 0;
    gameWindow->animacaoTelaTitulo.contadorTempoQuadro = 0.0f;
    gameWindow->animacaoTelaTitulo.pararNoUltimoQuadro = true;
    gameWindow->animacaoTelaTitulo.executarUmaVez = false;
    gameWindow->animacaoTelaTitulo.finalizada = false;
    criarQuadrosAnimacao( &gameWindow->animacaoTelaTitulo, 18 );

    // Mapeamento matemático do Grid da Sprite (6 Linhas x 3 Colunas)
    int frameIndex = 0;
    for ( int linha = 0; linha < 6; linha++ ) {
        for ( int coluna = 0; coluna < 3; coluna++ ) {
            gameWindow->animacaoTelaTitulo.quadros[frameIndex].duracao = 0.1f;
            gameWindow->animacaoTelaTitulo.quadros[frameIndex].fonte = (Rectangle) {
                24 + coluna * (320 + 8), // Coordenada X dinâmica
                520 + linha * (224 + 8), // Coordenada Y dinâmica
                320, 224                 // Dimensão de cada quadro
            };
            // gameWindow->animacaoTelaTitulo.quadros[frameIndex].espelhado = false;
            gameWindow->animacaoTelaTitulo.quadros[frameIndex].retColisao = (Rectangle){0};
            frameIndex++;
        }
    }

    return gameWindow;
}

/**
 * @brief Initializes the Window, starts the game loop and, when it
 * finishes, the window will be finished and destroyed too.
 */
void initGameWindow( GameWindow *gameWindow ) {

    if ( !gameWindow->initialized ) {

        gameWindow->initialized = true;

        if ( gameWindow->antialiasing ) SetConfigFlags( FLAG_MSAA_4X_HINT );
        if ( gameWindow->resizable ) SetConfigFlags( FLAG_WINDOW_RESIZABLE );
        if ( gameWindow->fullScreen ) SetConfigFlags( FLAG_FULLSCREEN_MODE );
        if ( gameWindow->undecorated ) SetConfigFlags( FLAG_WINDOW_UNDECORATED );
        if ( gameWindow->alwaysOnTop ) SetConfigFlags( FLAG_WINDOW_TOPMOST );
        if ( gameWindow->invisibleBackground ) SetConfigFlags( FLAG_WINDOW_TRANSPARENT );
        if ( gameWindow->alwaysRun ) SetConfigFlags( FLAG_WINDOW_ALWAYS_RUN );

        InitWindow( gameWindow->width, gameWindow->height, gameWindow->title );

        if ( gameWindow->initAudio ) {
            InitAudioDevice();
        }

        SetTargetFPS( gameWindow->targetFPS );    

        if ( gameWindow->loadResources ) {
            loadResourcesResourceManager();
        }

        gameWindow->gw = createGameWorld();

        // game loop
        while ( !WindowShouldClose() ) {

            // O delta time é limitado a 1/30s para evitar tunneling
            float delta = GetFrameTime();
            if ( delta > 1.0f / 30.0f ) {
                delta = 1.0f / 30.0f;
            }

            // MÁQUINA DE ESTADOS DA JANELA (TÍTULO VS JOGO)
            if ( gameWindow->naTelaTitulo ) {
                
                // 1. Atualiza Animação com Regra de Loop Customizada
                int quadroAnterior = gameWindow->animacaoTelaTitulo.quadroAtual;
                
                atualizarAnimacao( &gameWindow->animacaoTelaTitulo, delta );

                // Como a animação tem 18 imagens (índices de 0 a 17), se o quadro 
                // anterior era o 17 e o motor de animação o reiniciou para o 0, 
                // nós o interceptamos e forçamos a voltar para o índice 15 (as 3 últimas).
                if ( quadroAnterior == 17 && gameWindow->animacaoTelaTitulo.quadroAtual == 0 ) {
                    gameWindow->animacaoTelaTitulo.quadroAtual = 15;
                    gameWindow->animacaoTelaTitulo.contadorTempoQuadro = 0.0f;
                }

                // 2. Escuta Entradas do Jogador para iniciar a fase
                if ( GetKeyPressed() != 0 || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) ) {
                    gameWindow->naTelaTitulo = false;
                    PlaySound( rm.somAnel ); // Feedback sonoro ao dar Play
                }

                // 3. Desenha a Tela de Título
                BeginDrawing();
                ClearBackground( BLACK );

                // ====================================================================
                // ANIMAÇÃO 1: A Arte Principal da Tela de Título (Roda desde o início)
                // ====================================================================
                QuadroAnimacao *qa = getQuadroAtualAnimacao( &gameWindow->animacaoTelaTitulo );
                float escala = 2.0f; // Multiplicador de tamanho
                
                // Centraliza a imagem da tela de título na janela
                Rectangle dest = {
                    (gameWindow->width / 2.0f) - ((320 * escala) / 2.0f),
                    (gameWindow->height / 2.0f) - ((224 * escala) / 2.0f),
                    320 * escala, 224 * escala
                };

                // Desenha a tela de título (Fundo + Sonic)
                DrawTexturePro( rm.texturaTelaTitulo, qa->fonte, dest, (Vector2){0}, 0.0f, WHITE );


                // ====================================================================
                // ANIMAÇÃO 2: O Texto "PRESS START" (Executa em paralelo perfeitamente)
                // ====================================================================
                // Removida a trava de quadros para que o pisca-pisca aconteça simultaneamente
                if ( (int)(GetTime() * 2) % 2 == 0 ) {
                    
                    Rectangle fontePressStart = { 552, 1992, 144, 8 };
                    float escalaPressStart = 2.0f; // Mantém a proporção da tela
                    
                    // Centraliza horizontalmente e posiciona perto do rodapé da arte principal
                    Rectangle destPressStart = {
                        (gameWindow->width / 2.0f) - ((144 * escalaPressStart) / 2.0f),
                        dest.y + dest.height - 72.0f, // 48 pixels acima da base da imagem principal
                        144 * escalaPressStart, 
                        8 * escalaPressStart
                    };

                    // Desenha a sprite do texto por cima da arte principal em tempo real
                    DrawTexturePro( 
                        rm.texturaTelaTitulo, 
                        fontePressStart, 
                        destPressStart, 
                        (Vector2){0}, 
                        0.0f, 
                        WHITE 
                    );
                }

                EndDrawing();

            } else {
                
                // 4. Se não estiver no título, processa a GameWorld normalmente
                updateGameWorld( gameWindow->gw, delta );
                drawGameWorld( gameWindow->gw );
                
            }
        }

        if ( gameWindow->loadResources ) {
            unloadResourcesResourceManager();
        }

        bool initAudio = gameWindow->initAudio;

        destroyGameWindow( gameWindow );

        if ( initAudio ) {
            CloseAudioDevice();
        }

        CloseWindow();
    }
}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWindow( GameWindow *gameWindow ) {
    if ( gameWindow != NULL ) {
        // Limpa as structs de animação criadas
        destruirQuadrosAnimacao( &gameWindow->animacaoTelaTitulo );
        
        destroyGameWorld( gameWindow->gw );
        free( gameWindow );
    }
}