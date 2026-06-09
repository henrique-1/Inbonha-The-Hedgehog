/**
 * @file ResourceManager.c
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager implementation.
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "ResourceManager.h"
#include "Utils.h"

ResourceManager rm = { 0 };

void loadResourcesResourceManager( void ) {

    rm.texturaJogador = carregarTexturaAlterandoCores( 
        "resources/imagens/sprites/sonic.png",
        (Color[]) {
            { 37, 102, 26, 255 },
            { 13, 72, 7, 255 },
        },
        (Color[]) {
            BLANK,
            BLANK,
        },
        2
    );

    rm.texturaBadniks = carregarTexturaAlterandoCores( 
        "resources/imagens/sprites/badniks.png",
        (Color[]) {
            { 13, 72, 7, 255 },
        },
        (Color[]) {
            BLANK,
        },
        1
    );

    rm.texturaItens = carregarTexturaAlterandoCores( 
        "resources/imagens/itens/itens.png",
        (Color[]) {
            { 16, 112, 132, 255 },
            { 13, 72, 7, 255 },
            { 37, 102, 26, 255 },
        },
        (Color[]) {
            BLANK,
            BLANK,
            BLANK,
        },
        3
    );

    rm.texturaHudScore = carregarTexturaAlterandoCores(
        "resources/imagens/hud/score_text.png",
        (Color[]) {
            {16, 112, 132, 255}
        }, 
        (Color[]) {
            BLANK,
        }, 
        1
    );

    rm.texturaHudTime = carregarTexturaAlterandoCores(
         "resources/imagens/hud/time_text.png",
        (Color[]) {
            {16, 112, 132, 255}
        }, 
        (Color[]) {
            BLANK,
        }, 
        1 
    );

    rm.texturaHudTimeRed = carregarTexturaAlterandoCores(
         "resources/imagens/hud/time_text_red.png",
        (Color[]) {
            {16, 112, 132, 255}
        }, 
        (Color[]) {
            BLANK,
        }, 
        1 
    );

    rm.texturaHudTimeColon = carregarTexturaAlterandoCores(
         "resources/imagens/hud/time_text_colon.png",
        (Color[]) {
            {16, 112, 132, 255}
        }, 
        (Color[]) {
            BLANK,
        }, 
        1 
    );

    rm.texturaHudRings = carregarTexturaAlterandoCores(
         "resources/imagens/hud/rings_text.png",
        (Color[]) {
            {16, 112, 132, 255}
        }, 
        (Color[]) {
            BLANK,
        }, 
        1 
    );

    rm.texturaHudRingsRed = carregarTexturaAlterandoCores(
         "resources/imagens/hud/rings_text_red.png",
        (Color[]) {
            {16, 112, 132, 255}
        }, 
        (Color[]) {
            BLANK,
        }, 
        1 
    );

    rm.texturaHudNumeros = carregarTexturaAlterandoCores(
         "resources/imagens/hud/socre_number.png",
        (Color[]) {
            {16, 112, 132, 255}
        }, 
        (Color[]) {
            BLANK,
        }, 
        1 
    );

    rm.texturaHudNumerosPontos = carregarTexturaAlterandoCores(
         "resources/imagens/hud/sonic_points_number.png",
        (Color[]) {
            {16, 112, 132, 255}
        }, 
        (Color[]) {
            BLANK,
        }, 
        1 
    );

    rm.texturaHudVidas = carregarTexturaAlterandoCores(
         "resources/imagens/hud/sonic_scoreboard.png",
        (Color[]) {
            {25, 214, 247, 255},
            {0, 128, 248, 255},
        }, 
        (Color[]) {
            BLANK,
            BLANK,
        }, 
        2 
    );

    rm.texturaShield = carregarTexturaAlterandoCores( "resources/imagens/sprites/player_shield.png",
        (Color[]) {
            {13, 72, 7, 255}
        }, 
        (Color[]) {
            BLANK,
        }, 
        1 
    );

    rm.texturaSparkles = carregarTexturaAlterandoCores( "resources/imagens/sprites/player_invincibility_sparkles.png",
        (Color[]) {
            {13, 72, 7, 255}
        }, 
        (Color[]) {
            BLANK,
        }, 
        1 
    );
    

    rm.texturaTerreno = LoadTexture( "resources/imagens/tiles/terreno.png" );
    rm.texturaFundo = LoadTexture( "resources/imagens/fundo/fundo.png" );

    // Filtro de textura nearest-neighbor (ponto) para todas as texturas do jogo.
    // Evita interpolação bilinear nas bordas dos tiles e sprites, que causaria
    // bleeding de cor entre pixels adjacentes — efeito indesejável em pixel art.
    SetTextureFilter( rm.texturaJogador, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaBadniks, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaItens, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaTerreno, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaFundo, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaHudScore, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaHudTime, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaHudRings, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaHudNumeros, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaHudNumerosPontos, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaHudVidas, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaShield, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaSparkles, TEXTURE_FILTER_POINT );

    rm.somAnel = LoadSound( "resources/sons/efeitos/anel.wav" );
    rm.somFrenagem = LoadSound( "resources/sons/efeitos/frenagem.wav" );
    rm.somHitComAnel = LoadSound( "resources/sons/efeitos/hit-com-anel.wav" );
    rm.somHitInimigo = LoadSound( "resources/sons/efeitos/hit-inimigo.wav" );
    rm.somMorte = LoadSound( "resources/sons/efeitos/morte.wav" );
    rm.somPulo = LoadSound( "resources/sons/efeitos/pulo.wav" );

    rm.musicaFase01 = LoadMusicStream( "resources/sons/musicas/green-hill-zone.mp3" );

}

void unloadResourcesResourceManager( void ) {

    UnloadTexture( rm.texturaJogador );
    UnloadTexture( rm.texturaBadniks );
    UnloadTexture( rm.texturaItens );
    UnloadTexture( rm.texturaTerreno );
    UnloadTexture( rm.texturaFundo );
    UnloadTexture( rm.texturaHudScore );
    UnloadTexture( rm.texturaHudTime );
    UnloadTexture( rm.texturaHudRings );
    UnloadTexture( rm.texturaHudNumeros );
    UnloadTexture( rm.texturaHudNumerosPontos );
    UnloadTexture( rm.texturaHudVidas );
    UnloadTexture( rm.texturaHudTimeRed );
    UnloadTexture( rm.texturaHudTimeColon );
    UnloadTexture( rm.texturaHudRingsRed );
    UnloadTexture( rm.texturaShield );
    UnloadTexture( rm.texturaSparkles );

    UnloadSound( rm.somAnel );
    UnloadSound( rm.somFrenagem );
    UnloadSound( rm.somHitComAnel );
    UnloadSound( rm.somHitInimigo );
    UnloadSound( rm.somMorte );
    UnloadSound( rm.somPulo );

    UnloadMusicStream( rm.musicaFase01 );

}