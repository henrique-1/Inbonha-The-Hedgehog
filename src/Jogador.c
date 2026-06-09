/**
 * @file Jogador.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Jogador.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoMotobug.h"
#include "InimigoSpikes.h"
#include "InimigoBuzz.h"
#include "InimigoCrabmeat.h"
#include "Item.h"
#include "ItemAnel.h"
#include "ItemAnelAzul.h"
#include "ItemEstrelinha.h"
#include "ItemEscudo.h"
#include "Macros.h"
#include "Jogador.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacaoJogador( Jogador *j, QuadroAnimacao *qa, Color tonalidade );
static QuadroAnimacao *getQuadroAnimacaoAtualJogador( Jogador *j );
static Animacao *getAnimacaoAtualJogador( Jogador *j );

static void resolverColisaoJogadorObstaculosMapaX( Jogador *j, Mapa *mapa );
static void resolverColisaoJogadorObstaculosMapaY( Jogador *j, Mapa *mapa );

static void resolverColisaoJogadorItensMapa( Jogador *j, Mapa *mapa );
static void resolverColisaoJogadorInimigosMapa( Jogador *j, Mapa *mapa );

static void configurarAnimacaoJogador( Animacao *animacao, int qtdQuadros, int duracao, int inicioX, int inicioY, Rectangle retColisao );

static const bool MOSTRAR_RETANGULOS = false;

/**
 * @brief Cria uma instância alocada dinamicamente da struct Jogador.
 */
Jogador *criarJogador( float x, float y, float w, float h ) {

    Jogador *novoJogador = (Jogador*) malloc( sizeof( Jogador ) );

    novoJogador->ret.x = x;
    novoJogador->ret.y = y;
    novoJogador->ret.width = w;
    novoJogador->ret.height = h;
    novoJogador->vel = (Vector2) { 0 };

    novoJogador->cor = BLUE;

    novoJogador->velAndando = 200;
    novoJogador->velAndandoRapido = 400;
    novoJogador->velCorrendo = 800;
    novoJogador->velPulo = -550;
    novoJogador->velMaxQueda = 600;

    novoJogador->aceleracao = 200;
    novoJogador->desaceleracao = 400;
    novoJogador->frenagem = 1800;

    novoJogador->quantidadePulos = 0;
    novoJogador->quantidadeMaxPulos = 1;

    novoJogador->quantidadeAneis = 0;
    novoJogador->quantidadeVidas = 3;

    // Inicialização do controle de tédio
    novoJogador->contadorTempoTedio = 0.0f;
    novoJogador->tempoParaTedio = 4.0f; // Tempo em segundos parado para ativar o tédio

    novoJogador->pontuacao = 0;
    novoJogador->invulneravel = false;
    novoJogador->tempoInvulnerabilidade = 3.0f;
    novoJogador->contadorTempoInvulnerabilidade = 0.0f;

    novoJogador->piscaPisca = false;
    novoJogador->tempoPiscaPisca = 0.05f;
    novoJogador->contadorTempoPiscaPisca = 0.0f;

    novoJogador->freando = false;

    novoJogador->estado = ESTADO_JOGADOR_PARADO;
    novoJogador->olhandoParaDireita = true;

    novoJogador->temEscudo = false;
    novoJogador->temEstrela = false;

    int quantidadeAnimacoes = 0;

    configurarAnimacaoJogador( &novoJogador->animacaoParado, 1, 1000, 24, 251, (Rectangle){ 32, 20, 42, 76 } );
    configurarAnimacaoJogador( &novoJogador->animacaoAndando, 8, 80, 664, 324, (Rectangle){ 32, 20, 42, 76 } );
    configurarAnimacaoJogador( &novoJogador->animacaoAndandoRapido, 8, 40, 664, 324, (Rectangle){ 32, 20, 42, 76 } );
    configurarAnimacaoJogador( &novoJogador->animacaoCorrendo, 4, 20, 24, 397, (Rectangle){ 32, 20, 42, 76 } );
    configurarAnimacaoJogador( &novoJogador->animacaoPulando, 4, 40, 248, 397, (Rectangle){ 32, 46, 42, 50 } );
    configurarAnimacaoJogador( &novoJogador->animacaoMorto, 1, 1000, 740, 750, (Rectangle){ 32, 20, 42, 76 } );
    configurarAnimacaoJogador( &novoJogador->animacaoTedio, 9, 120, 24, 251, (Rectangle){ 32, 20, 42, 76 } );
    configurarAnimacaoJogador( &novoJogador->animacaoAcordando, 1, 150, 440, 251, (Rectangle){ 32, 20, 42, 76 } );
    novoJogador->animacaoAcordando.executarUmaVez = true;
    configurarAnimacaoJogador( &novoJogador->animacaoOlhandoCima, 2, 120, 708, 251, (Rectangle){ 32, 20, 42, 76 } );
    novoJogador->animacaoOlhandoCima.pararNoUltimoQuadro = true;
    configurarAnimacaoJogador( &novoJogador->animacaoAgachado, 2, 120, 826, 251, (Rectangle){ 32, 20, 42, 76 } );
    novoJogador->animacaoAgachado.pararNoUltimoQuadro = true;

    novoJogador->animacaoPulandoRapido.quantidadeQuadros = 4;
    novoJogador->animacaoPulandoRapido.quadroAtual = 0;
    novoJogador->animacaoPulandoRapido.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoPulandoRapido.pararNoUltimoQuadro = false;
    novoJogador->animacaoPulandoRapido.executarUmaVez = false;
    novoJogador->animacaoPulandoRapido.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoPulandoRapido, novoJogador->animacaoPulandoRapido.quantidadeQuadros );
    inicializarQuadrosAnimacao(
        novoJogador->animacaoPulandoRapido.quadros,
        novoJogador->animacaoPulandoRapido.quantidadeQuadros,
        25,              // duração padrão para cada quadro
        248, 397,        // início
        48, 48,          // dimensões
        4,               // separação
        false,           // de trás para frente
        (Rectangle) {    // retângulo de colisão padrão para cada quadro
            32, 46, 42, 50
            //18, 36, 60, 60
        }
    );

    novoJogador->animacaoPulandoCorrendo.quantidadeQuadros = 4;
    novoJogador->animacaoPulandoCorrendo.quadroAtual = 0;
    novoJogador->animacaoPulandoCorrendo.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoPulandoCorrendo.pararNoUltimoQuadro = false;
    novoJogador->animacaoPulandoCorrendo.executarUmaVez = false;
    novoJogador->animacaoPulandoCorrendo.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoPulandoCorrendo, novoJogador->animacaoPulandoCorrendo.quantidadeQuadros );
    inicializarQuadrosAnimacao(
        novoJogador->animacaoPulandoCorrendo.quadros,
        novoJogador->animacaoPulandoCorrendo.quantidadeQuadros,
        15,              // duração padrão para cada quadro
        248, 397,        // início
        48, 48,          // dimensões
        4,               // separação
        false,           // de trás para frente
        (Rectangle) {    // retângulo de colisão padrão para cada quadro
            32, 46, 42, 50
            //18, 36, 60, 60
        }
    );

    novoJogador->animacaoEscudo.quantidadeQuadros = 3; // Ex: 4 quadros de animação
    novoJogador->animacaoEscudo.quadroAtual = 0;
    novoJogador->animacaoEscudo.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoEscudo.pararNoUltimoQuadro = false;
    novoJogador->animacaoEscudo.executarUmaVez = false;
    novoJogador->animacaoEscudo.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoEscudo, novoJogador->animacaoEscudo.quantidadeQuadros );
    inicializarQuadrosAnimacao( novoJogador->animacaoEscudo.quadros, novoJogador->animacaoEscudo.quantidadeQuadros, 100, 
        1, 1, 48, 48, 1, false, (Rectangle){0} ); // <-- AJUSTE A LARGURA (48) E ALTURA (48) DO SEU ESCUDO

    novoJogador->animacaoEstrela.quantidadeQuadros = 4;
    novoJogador->animacaoEstrela.quadroAtual = 0;
    novoJogador->animacaoEstrela.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoEstrela.pararNoUltimoQuadro = false;
    novoJogador->animacaoEstrela.executarUmaVez = false;
    novoJogador->animacaoEstrela.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoEstrela, novoJogador->animacaoEstrela.quantidadeQuadros );
    inicializarQuadrosAnimacao( novoJogador->animacaoEstrela.quadros, novoJogador->animacaoEstrela.quantidadeQuadros, 100, 
        1, 1, 48, 48, 1, false, (Rectangle){0} ); // <-- AJUSTE A LARGURA (48) E ALTURA (48) DAS FAÍSCAS

    novoJogador->animacoes[ESTADO_JOGADOR_PARADO] = &novoJogador->animacaoParado; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_ANDANDO] = &novoJogador->animacaoAndando; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_ANDANDO_RAPIDO] = &novoJogador->animacaoAndandoRapido; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_CORRENDO] = &novoJogador->animacaoCorrendo; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_PULANDO] = &novoJogador->animacaoPulando; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_PULANDO_RAPIDO] = &novoJogador->animacaoPulandoRapido; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_PULANDO_CORRENDO] = &novoJogador->animacaoPulandoCorrendo; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_MORTO] = &novoJogador->animacaoMorto; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_TEDIO] = &novoJogador->animacaoTedio; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_ACORDANDO] = &novoJogador->animacaoAcordando; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_OLHANDO_CIMA] = &novoJogador->animacaoOlhandoCima; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_AGACHADO] = &novoJogador->animacaoAgachado; quantidadeAnimacoes++;
    novoJogador->quantidadeAnimacoes = quantidadeAnimacoes;

    return novoJogador;

}

/**
 * @brief Destrói um objeto Jogador e libera seus recursos.
 */
void destruirJogador( Jogador *j ) {
    if ( j != NULL ) {
        for ( int i = 0; i < j->quantidadeAnimacoes; i++ ) {
            destruirQuadrosAnimacao( j->animacoes[i] );
        }
        destruirQuadrosAnimacao( &j->animacaoEscudo ); // <-- ADICIONADO
        destruirQuadrosAnimacao( &j->animacaoEstrela ); // <-- ADICIONADO
        free( j );
    }
}

/**
 * @brief Lê a entrada do usuário e atualiza as velocidades do jogador.
 */
void entradaJogador( Jogador *j, float delta ) {

    if ( j->estado == ESTADO_JOGADOR_MORTO ) {
        return; 
    }

    EstadoJogador estadoAnterior = j->estado;

    bool direitaDown  = IsKeyDown( KEY_RIGHT )     || ( IsGamepadAvailable( 0 ) && IsGamepadButtonDown( 0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT ) );
    bool esquerdaDown = IsKeyDown( KEY_LEFT )      || ( IsGamepadAvailable( 0 ) && IsGamepadButtonDown( 0, GAMEPAD_BUTTON_LEFT_FACE_LEFT ) );
    bool puloPressed  = IsKeyPressed( KEY_SPACE )  || ( IsGamepadAvailable( 0 ) && IsGamepadButtonDown( 0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN ) );
    bool cimaDown     = IsKeyDown( KEY_UP )        || ( IsGamepadAvailable( 0 ) && IsGamepadButtonDown( 0, GAMEPAD_BUTTON_LEFT_FACE_UP ) );
    bool baixoDown    = IsKeyDown( KEY_DOWN )      || ( IsGamepadAvailable( 0 ) && IsGamepadButtonDown( 0, GAMEPAD_BUTTON_LEFT_FACE_DOWN ) );

    // 1. Intercepta a saída do tédio
    if ( j->estado == ESTADO_JOGADOR_TEDIO ) {
        if ( direitaDown || esquerdaDown || puloPressed || cimaDown || baixoDown ) {
            j->estado = ESTADO_JOGADOR_ACORDANDO;
            reiniciarAnimacao( &j->animacaoAcordando );
            
            // Já vira o personagem visualmente para o lado que o jogador quer ir
            if ( direitaDown ) j->olhandoParaDireita = true;
            if ( esquerdaDown ) j->olhandoParaDireita = false;
            
            return; // Bloqueia a movimentação neste exato frame
        }
    }

    // 2. Aguarda a animação de transição finalizar
    if ( j->estado == ESTADO_JOGADOR_ACORDANDO ) {
        return; 
    }

    // 3. Se não estiver em tédio ou acordando, reseta o timer normalmente
    if ( direitaDown || esquerdaDown || puloPressed || cimaDown || baixoDown ) {
        j->contadorTempoTedio = 0.0f;
    }

    if ( direitaDown ) {
        if ( j->vel.x < 0 ) {
            j->vel.x += j->frenagem * delta;
            if ( !j->freando && j->estado == ESTADO_JOGADOR_CORRENDO ) {
                PlaySound( rm.somFrenagem );
                j->freando = true;
            }
            if ( j->vel.x > 0 ) {
                j->vel.x = 0;
                j->freando = false;
            }
        } else {
            j->vel.x += j->aceleracao * delta;
            if ( j->vel.x > j->velCorrendo ) {
                j->vel.x = j->velCorrendo;
            }
        }
        j->olhandoParaDireita = true;
    } else if ( esquerdaDown ) {
        if ( j->vel.x > 0 ) {
            j->vel.x -= j->frenagem * delta;
            if ( !j->freando && j->estado == ESTADO_JOGADOR_CORRENDO ) {
                PlaySound( rm.somFrenagem );
                j->freando = true;
            }
            if ( j->vel.x < 0 ) {
                j->vel.x = 0;
                j->freando = false;
            }
        } else {
            j->vel.x -= j->aceleracao * delta;
            if ( j->vel.x < -j->velCorrendo ) {
                j->vel.x = -j->velCorrendo;
            }
        }
        j->olhandoParaDireita = false;
    } else {
        if ( j->vel.x > 0 ) {
            j->vel.x -= j->desaceleracao * delta;
            if ( j->vel.x < 0 ) {
                j->vel.x = 0;
            }
        } else if ( j->vel.x < 0 ) {
            j->vel.x += j->desaceleracao * delta;
            if ( j->vel.x > 0 ) {
                j->vel.x = 0;
            }
        }
    }

    float absVelX = fabsf( j->vel.x );
    if ( j->quantidadePulos > 0 ) {
        if ( absVelX <= j->velAndando ) {
            j->estado = ESTADO_JOGADOR_PULANDO;
        } else if ( absVelX <= j->velAndandoRapido ) {
            j->estado = ESTADO_JOGADOR_PULANDO_RAPIDO;
        } else {
            j->estado = ESTADO_JOGADOR_PULANDO_CORRENDO;
        }
    } else if ( absVelX < 1.0f && !direitaDown && !esquerdaDown ) {
        // Personagem está no chão e não está tentando andar para os lados
        if ( cimaDown ) {
            if ( j->estado != ESTADO_JOGADOR_OLHANDO_CIMA ) {
                j->estado = ESTADO_JOGADOR_OLHANDO_CIMA;
                reiniciarAnimacao( &j->animacaoOlhandoCima );
            }
        } else if ( baixoDown ) {
            if ( j->estado != ESTADO_JOGADOR_AGACHADO ) {
                j->estado = ESTADO_JOGADOR_AGACHADO;
                reiniciarAnimacao( &j->animacaoAgachado );
            }
        } else {
            // Se nenhuma tecla está pressionada e a velocidade é zero, volta a ficar parado
            if ( j->estado != ESTADO_JOGADOR_TEDIO ) {
                j->estado = ESTADO_JOGADOR_PARADO;
            }
        }
    } else if ( absVelX <= j->velAndando ) {
        j->estado = ESTADO_JOGADOR_ANDANDO;
    } else if ( absVelX <= j->velAndandoRapido ) {
        j->estado = ESTADO_JOGADOR_ANDANDO_RAPIDO;
    } else {
        j->estado = ESTADO_JOGADOR_CORRENDO;
    }

    if ( puloPressed && j->quantidadePulos < j->quantidadeMaxPulos ) {
        j->vel.y = j->velPulo;
        j->quantidadePulos++;
        PlaySound( rm.somPulo );
    }

    // sincronização de animações andando e andando rápido
    if ( estadoAnterior == ESTADO_JOGADOR_ANDANDO && j->estado == ESTADO_JOGADOR_ANDANDO_RAPIDO ) {
        sincronizarAnimacao( &j->animacaoAndandoRapido, &j->animacaoAndando );
    } else if ( estadoAnterior == ESTADO_JOGADOR_ANDANDO_RAPIDO && j->estado == ESTADO_JOGADOR_ANDANDO ) {
        sincronizarAnimacao( &j->animacaoAndando, &j->animacaoAndandoRapido );
    }

}

/**
 * @brief Aplica física e resolve colisões do jogador com o mundo.
 */
void atualizarJogador( Jogador *j, GameWorld *gw, float delta ) {

    Animacao *animacaoAtual = getAnimacaoAtualJogador( j );
    atualizarAnimacao( animacaoAtual, delta );

    if ( j->estado == ESTADO_JOGADOR_MORTO ) {
        j->vel.y += gw->gravidade * delta;
        if ( j->vel.y > j->velMaxQueda ) {
            j->vel.y = j->velMaxQueda;
        }
        j->ret.y += j->vel.y * delta;
        return; // Retorna cedo para pular todas as resoluções de colisão abaixo
    }

    // Verifica se a animação de susto/despertar terminou
    if ( j->estado == ESTADO_JOGADOR_ACORDANDO ) {
        if ( j->animacaoAcordando.finalizada ) {
            j->estado = ESTADO_JOGADOR_PARADO;
            j->contadorTempoTedio = 0.0f;
        }
    }

    // 1. Controle do temporizador de inatividade
    if ( j->estado == ESTADO_JOGADOR_PARADO ) {
        j->contadorTempoTedio += delta;
        if ( j->contadorTempoTedio >= j->tempoParaTedio ) {
            j->estado = ESTADO_JOGADOR_TEDIO;
            reiniciarAnimacao( &j->animacaoTedio );
        }
    }

    // 2. Regra de Loop customizada para o Tédio
    if ( j->estado == ESTADO_JOGADOR_TEDIO ) {
        // A 8ª imagem é o índice 7. Se avançar para o índice 8,
        // significa que o tempo da 8ª imagem expirou, retornando para a 4ª imagem (índice 3).
        if ( j->animacaoTedio.quadroAtual == 8 ) {
            j->animacaoTedio.quadroAtual = 3;
            j->animacaoTedio.contadorTempoQuadro = 0;
        }
    }

    if ( j->invulneravel ) {

        j->contadorTempoPiscaPisca += delta;
        if ( j->contadorTempoPiscaPisca >= j->tempoPiscaPisca ) {
            j->contadorTempoPiscaPisca = 0.0f;
            j->piscaPisca = !j->piscaPisca;
        }

        j->contadorTempoInvulnerabilidade += delta;
        if ( j->contadorTempoInvulnerabilidade >= j->tempoInvulnerabilidade ) {
            j->contadorTempoInvulnerabilidade = 0.0f;
            j->invulneravel = false;
            j->temEstrela = false; // <-- DESLIGA A ANIMAÇÃO DA ESTRELA
            j->contadorTempoPiscaPisca = 0.0f;
            j->piscaPisca = false;
        }

    }

    if ( j->temEscudo ) atualizarAnimacao( &j->animacaoEscudo, delta );
    if ( j->temEstrela ) atualizarAnimacao( &j->animacaoEstrela, delta );

    // fase X: move horizontalmente e resolve colisões laterais
    j->ret.x += j->vel.x * delta;
    resolverColisaoJogadorObstaculosMapaX( j, gw->mapa );

    // fase Y: aplica gravidade, move verticalmente e resolve colisões verticais
    j->vel.y += gw->gravidade * delta;
    if ( j->vel.y > j->velMaxQueda ) {
        j->vel.y = j->velMaxQueda;
    }
    j->ret.y += j->vel.y * delta;
    resolverColisaoJogadorObstaculosMapaY( j, gw->mapa );

    resolverColisaoJogadorItensMapa( j, gw->mapa );
    resolverColisaoJogadorInimigosMapa( j, gw->mapa );

}

/**
 * @brief Desenha o jogador.
 */
void desenharJogador( Jogador *j ) {

    if ( !j->piscaPisca ) {
        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );
        desenharQuadroAnimacaoJogador( j, qa, WHITE );
    }

    if ( j->temEscudo && j->estado != ESTADO_JOGADOR_MORTO ) {
        QuadroAnimacao *qaEscudo = getQuadroAtualAnimacao( &j->animacaoEscudo );
        DrawTexturePro( 
            rm.texturaShield, qaEscudo->fonte,
            (Rectangle) { 
                j->ret.x + j->ret.width / 2 - qaEscudo->fonte.width / 2, // Centraliza X
                j->ret.y + j->ret.height / 2 - qaEscudo->fonte.height / 2, // Centraliza Y
                qaEscudo->fonte.width, qaEscudo->fonte.height 
            },
            (Vector2) { 0 }, 0.0f, WHITE 
        );
    }

    if ( j->temEstrela && j->estado != ESTADO_JOGADOR_MORTO ) {
        QuadroAnimacao *qaEstrela = getQuadroAtualAnimacao( &j->animacaoEstrela );
        DrawTexturePro( 
            rm.texturaSparkles, qaEstrela->fonte,
            (Rectangle) { 
                j->ret.x + j->ret.width / 2 - qaEstrela->fonte.width / 2,
                j->ret.y + j->ret.height / 2 - qaEstrela->fonte.height / 2,
                qaEstrela->fonte.width, qaEstrela->fonte.height 
            },
            (Vector2) { 0 }, 0.0f, WHITE 
        );
    }

    if ( MOSTRAR_RETANGULOS ) {
        DrawRectangleRec( j->ret, Fade( j->cor, 0.5f ) );
        DrawRectangleLines( j->ret.x, j->ret.y, j->ret.width, j->ret.height, BLACK );
    }

}

void matarJogador( Jogador *j ) {
    if ( j->estado != ESTADO_JOGADOR_MORTO ) {
        j->estado = ESTADO_JOGADOR_MORTO;
        j->vel.x = 0;                  // Zera inércia horizontal
        j->vel.y = j->velPulo * 1.2f;  // Salto dramático para cima (levemente mais alto que o normal)
        j->quantidadeVidas--;
        PlaySound( rm.somMorte );
    }
}

static void desenharQuadroAnimacaoJogador( Jogador *j, QuadroAnimacao *qa, Color tonalidade ) {

    if ( qa != NULL ) {

        DrawTexturePro(
            rm.texturaJogador,
            (Rectangle) {
                qa->fonte.x,
                qa->fonte.y,
                j->olhandoParaDireita ? qa->fonte.width : -qa->fonte.width,
                qa->fonte.height
            },
            j->ret,
            (Vector2) { 0 },
            0.0f,
            tonalidade
        );

        if ( MOSTRAR_RETANGULOS ) {
            float xDesenho = j->olhandoParaDireita
                ? j->ret.x + qa->retColisao.x
                : j->ret.x + j->ret.width - qa->retColisao.x - qa->retColisao.width;
            float yDesenho = j->ret.y + qa->retColisao.y;
            DrawRectangle( xDesenho, yDesenho, qa->retColisao.width, qa->retColisao.height, Fade( GREEN, 0.5f ) );
        }

    }

}

static QuadroAnimacao *getQuadroAnimacaoAtualJogador( Jogador *j ) {
    return getQuadroAtualAnimacao( getAnimacaoAtualJogador( j ) );
}

static Animacao *getAnimacaoAtualJogador( Jogador *j ) {
    return j->animacoes[j->estado];
}

/**
 * @brief Resolve colisões do jogador com o mapa no eixo X.
 */
static void resolverColisaoJogadorObstaculosMapaX( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );

        float deslocamentoX = j->olhandoParaDireita
            ? qa->retColisao.x
            : j->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };

        Obstaculo *o = (Obstaculo*) el->objeto;

        if ( CheckCollisionRecs( retColCalculado, o->ret ) ) {
            if ( retColCalculado.x + retColCalculado.width / 2 < o->ret.x + o->ret.width / 2 ) {
                j->ret.x = o->ret.x - qa->retColisao.width - deslocamentoX;
            } else {
                j->ret.x = o->ret.x + o->ret.width - deslocamentoX;
            }
            j->vel.x = 0;
        }

        el = el->proximo;

    }

}

/**
 * @brief Resolve colisões do jogador com o mapa no eixo Y.
 */
static void resolverColisaoJogadorObstaculosMapaY( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );

        float deslocamentoX = j->olhandoParaDireita
            ? qa->retColisao.x
            : j->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };

        Obstaculo *o = (Obstaculo*) el->objeto;

        if ( CheckCollisionRecs( retColCalculado, o->ret ) ) {
            if ( retColCalculado.y + retColCalculado.height / 2 < o->ret.y + o->ret.height / 2 ) {
                j->ret.y = o->ret.y - qa->retColisao.height - deslocamentoY;
                j->quantidadePulos = 0;
            } else {
                j->ret.y = o->ret.y + o->ret.height - deslocamentoY;
            }
            j->vel.y = 0;
        }

        el = el->proximo;

    }

}

static void resolverColisaoJogadorItensMapa( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->itens;

    while ( el != NULL ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );

        float deslocamentoX = j->olhandoParaDireita
            ? qa->retColisao.x
            : j->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };

        Item *item = (Item*) el->objeto;

        if ( item->tipo == TIPO_ITEM_ANEL ) {

            ItemAnel *itemAnel = (ItemAnel*) item->objeto;

            if ( !itemAnel->ativo || itemAnel->estado == ESTADO_ITEM_ANEL_COLETADO ) {
                el = el->proximo;
                continue;
            }

            QuadroAnimacao *qaItem = getQuadroAnimacaoAtualItemAnel( itemAnel );
            
            Rectangle retColItemCalculado = {
                itemAnel->ret.x + qaItem->retColisao.x,
                itemAnel->ret.y + qaItem->retColisao.y,
                qaItem->retColisao.width,
                qaItem->retColisao.height
            };

            if ( CheckCollisionRecs( retColCalculado, retColItemCalculado ) ) {
                itemAnel->estado = ESTADO_ITEM_ANEL_COLETADO;
                j->quantidadeAneis++;
                PlaySound( rm.somAnel );
            }

        } else if ( item->tipo == TIPO_ITEM_ANEL_AZUL ) {

            ItemAnelAzul *itemAnelAzul = (ItemAnelAzul*) item->objeto;

            if ( !itemAnelAzul->ativo || itemAnelAzul->estado == ESTADO_ITEM_ANEL_AZUL_COLETADO ) {
                el = el->proximo;
                continue;
            }

            QuadroAnimacao *qaItem = getQuadroAnimacaoAtualItemAnelAzul( itemAnelAzul );
            
            Rectangle retColItemCalculado = {
                itemAnelAzul->ret.x + qaItem->retColisao.x,
                itemAnelAzul->ret.y + qaItem->retColisao.y,
                qaItem->retColisao.width,
                qaItem->retColisao.height
            };

            if ( CheckCollisionRecs( retColCalculado, retColItemCalculado ) ) {
                itemAnelAzul->estado = ESTADO_ITEM_ANEL_AZUL_COLETADO;
                j->quantidadeAneis--;
                PlaySound( rm.somAnel );
            }

        } else if ( item->tipo == TIPO_ITEM_ESTRELINHA ) {

            ItemEstrelinha *itemEstrela = (ItemEstrelinha*) item->objeto;

            if ( !itemEstrela->ativo || itemEstrela->estado == ESTADO_ITEM_ESTRELINHA_COLETADO ) {
                el = el->proximo;
                continue;
            }

            QuadroAnimacao *qaItem = getQuadroAnimacaoAtualItemEstrelinha( itemEstrela );
            Rectangle retColItemCalculado = {
                itemEstrela->ret.x + qaItem->retColisao.x,
                itemEstrela->ret.y + qaItem->retColisao.y,
                qaItem->retColisao.width, qaItem->retColisao.height
            };

            if ( CheckCollisionRecs( retColCalculado, retColItemCalculado ) ) {
                itemEstrela->estado = ESTADO_ITEM_ESTRELINHA_COLETADO;
                j->invulneravel = true;
                j->temEstrela = true; // <-- ATIVA O EFEITO VISUAL
                j->tempoInvulnerabilidade = 15.0f; 
                j->contadorTempoInvulnerabilidade = 0.0f; 
                j->piscaPisca = false; 
                PlaySound( rm.somAnel ); 
            }

        } else if ( item->tipo == TIPO_ITEM_ESCUDO ) {

            ItemEscudo *itemEscudo = (ItemEscudo*) item->objeto;

            if ( !itemEscudo->ativo || itemEscudo->estado == ESTADO_ITEM_ESCUDO_COLETADO ) {
                el = el->proximo;
                continue;
            }

            QuadroAnimacao *qaItem = getQuadroAnimacaoAtualItemEscudo( itemEscudo );
            Rectangle retColItemCalculado = {
                itemEscudo->ret.x + qaItem->retColisao.x,
                itemEscudo->ret.y + qaItem->retColisao.y,
                qaItem->retColisao.width, qaItem->retColisao.height
            };

            if ( CheckCollisionRecs( retColCalculado, retColItemCalculado ) ) {
                itemEscudo->estado = ESTADO_ITEM_ESCUDO_COLETADO;
                j->temEscudo = true;
                PlaySound( rm.somAnel ); // Substitua pelo som de escudo
            }

        }

        el = el->proximo;

    }

}

static void resolverColisaoJogadorInimigosMapa( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->inimigos;

    while ( el != NULL ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );

        float deslocamentoX = j->olhandoParaDireita
            ? qa->retColisao.x
            : j->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };

        Inimigo *inimigo = (Inimigo*) el->objeto;

        QuadroAnimacao *qaInimigo = NULL;
        bool *olhandoParaDireita = NULL;
        Rectangle *ret = NULL;

        if ( inimigo->tipo == TIPO_INIMIGO_MOTOBUG ) {

            InimigoMotobug *motobug = (InimigoMotobug*) inimigo->objeto;

            if ( !motobug->ativo || motobug->estado == ESTADO_INIMIGO_MOTOBUG_MORRENDO ) {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoMotobug( motobug );
            olhandoParaDireita = &motobug->olhandoParaDireita;
            ret = &motobug->ret;

            float deslocamentoX = *olhandoParaDireita
                ? ret->width - qaInimigo->retColisao.x - qaInimigo->retColisao.width
                : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX,
                ret->y + deslocamentoY,
                qaInimigo->retColisao.width,
                qaInimigo->retColisao.height
            };

            if ( CheckCollisionRecs( retColCalculado, retColInimigoCalculado ) ) {

                if ( j->estado >= ESTADO_JOGADOR_PULANDO && j->estado <= ESTADO_JOGADOR_PULANDO_CORRENDO ) {
                    j->vel.y = j->velPulo;
                    motobug->estado = ESTADO_INIMIGO_MOTOBUG_MORRENDO;
                    PlaySound( rm.somHitInimigo );
                    j->pontuacao += 100;
                } else if ( !j->invulneravel ) {
                    if ( j->temEscudo ) {
                        j->temEscudo = false;
                        j->invulneravel = true;
                        j->tempoInvulnerabilidade = 3.0f; // Tempo de recuo/invulnerabilidade padrão
                        j->contadorTempoInvulnerabilidade = 0.0f;
                        PlaySound( rm.somHitInimigo ); // Som de escudo quebrando
                    } else if ( j->quantidadeAneis > 0 ) {
                        j->quantidadeAneis = 0;
                        PlaySound( rm.somHitComAnel );
                        j->invulneravel = true;
                        j->tempoInvulnerabilidade = 3.0f; 
                        j->contadorTempoInvulnerabilidade = 0.0f;
                    } else {
                        matarJogador( j );
                    }
                }

                return; // um inimigo de cada vez!

            }

        } else if ( inimigo->tipo == TIPO_INIMIGO_SPIKES ) {

            InimigoSpikes *spikes = (InimigoSpikes*) inimigo->objeto;

            if ( !spikes->ativo || spikes->estado == ESTADO_INIMIGO_SPIKES_MORRENDO ) {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoSpikes( spikes );
            olhandoParaDireita = &spikes->olhandoParaDireita;
            ret = &spikes->ret;

            float deslocamentoX = *olhandoParaDireita
                ? ret->width - qaInimigo->retColisao.x - qaInimigo->retColisao.width
                : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX,
                ret->y + deslocamentoY,
                qaInimigo->retColisao.width,
                qaInimigo->retColisao.height
            };

            if ( CheckCollisionRecs( retColCalculado, retColInimigoCalculado ) ) {

                if ( j->estado >= ESTADO_JOGADOR_PULANDO && j->estado <= ESTADO_JOGADOR_PULANDO_CORRENDO ) {
                    j->vel.y = j->velPulo;
                    spikes->estado = ESTADO_INIMIGO_SPIKES_MORRENDO;
                    PlaySound( rm.somHitInimigo );
                    j->pontuacao += 100;
                } else if ( !j->invulneravel ) {
                    if ( j->quantidadeAneis > 0 ) {
                        j->quantidadeAneis = 0;
                        PlaySound( rm.somHitComAnel );
                        j->invulneravel = true; // Só ganha invulnerabilidade se sobreviveu
                    } else {
                        matarJogador( j );
                    }
                }

                return; // um inimigo de cada vez!

            }

        } else if ( inimigo->tipo == TIPO_INIMIGO_BUZZ ) {

            InimigoBuzz *buzz = (InimigoBuzz*) inimigo->objeto;

            if ( !buzz->ativo || buzz->estado == ESTADO_INIMIGO_BUZZ_MORRENDO ) {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoBuzz( buzz );
            olhandoParaDireita = &buzz->olhandoParaDireita;
            ret = &buzz->ret;

            float deslocamentoX = *olhandoParaDireita
                ? ret->width - qaInimigo->retColisao.x - qaInimigo->retColisao.width
                : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX,
                ret->y + deslocamentoY,
                qaInimigo->retColisao.width,
                qaInimigo->retColisao.height
            };

            if ( CheckCollisionRecs( retColCalculado, retColInimigoCalculado ) ) {

                if ( j->estado >= ESTADO_JOGADOR_PULANDO && j->estado <= ESTADO_JOGADOR_PULANDO_CORRENDO ) {
                    j->vel.y = j->velPulo;
                    buzz->estado = ESTADO_INIMIGO_BUZZ_MORRENDO;
                    PlaySound( rm.somHitInimigo );
                    j->pontuacao += 100;
                } else if ( !j->invulneravel ) {
                    if ( j->quantidadeAneis > 0 ) {
                        j->quantidadeAneis = 0;
                        PlaySound( rm.somHitComAnel );
                        j->invulneravel = true; // Só ganha invulnerabilidade se sobreviveu
                    } else {
                        matarJogador( j );
                    }
                }

                return; // um inimigo de cada vez!

            }

        } else if ( inimigo->tipo == TIPO_INIMIGO_CRABMEAT ) {

            InimigoCrabmeat *crabmeat = (InimigoCrabmeat*) inimigo->objeto;

            if ( !crabmeat->ativo || crabmeat->estado == ESTADO_INIMIGO_CRABMEAT_MORRENDO) {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoCrabmeat( crabmeat );
            olhandoParaDireita = &crabmeat->olhandoParaDireita;
            ret = &crabmeat->ret;

            float deslocamentoX = *olhandoParaDireita
                ? ret->width - qaInimigo->retColisao.x - qaInimigo->retColisao.width
                : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX,
                ret->y + deslocamentoY,
                qaInimigo->retColisao.width,
                qaInimigo->retColisao.height
            };

            if ( CheckCollisionRecs( retColCalculado, retColInimigoCalculado ) ) {

                if ( j->estado >= ESTADO_JOGADOR_PULANDO && j->estado <= ESTADO_JOGADOR_PULANDO_CORRENDO ) {
                    j->vel.y = j->velPulo;
                    crabmeat->estado = ESTADO_INIMIGO_CRABMEAT_MORRENDO;
                    PlaySound( rm.somHitInimigo );
                    j->pontuacao += 100;
                } else if ( !j->invulneravel ) {
                    if ( j->quantidadeAneis > 0 ) {
                        j->quantidadeAneis = 0;
                        PlaySound( rm.somHitComAnel );
                        j->invulneravel = true; // Só ganha invulnerabilidade se sobreviveu
                    } else {
                        matarJogador( j );
                    }
                }

                return; // um inimigo de cada vez!

            }

        }

        el = el->proximo;

    }

}

static void configurarAnimacaoJogador( Animacao *animacao, int qtdQuadros, int duracao, int inicioX, int inicioY, Rectangle retColisao ) {
    animacao->quantidadeQuadros = qtdQuadros;
    animacao->quadroAtual = 0;
    animacao->contadorTempoQuadro = 0.0f;
    animacao->pararNoUltimoQuadro = false;
    animacao->executarUmaVez = false;
    animacao->finalizada = false;
    
    criarQuadrosAnimacao( animacao, animacao->quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        animacao->quadros, 
        animacao->quantidadeQuadros, 
        duracao, 
        inicioX, inicioY,        // início X e Y
        48, 48,                  // dimensões fixas
        4,                       // separação
        false,                   // de trás para frente
        retColisao               // retângulo de colisão
    );
}