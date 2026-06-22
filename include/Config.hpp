#pragma once

#include <string>

// ============================================================================
//  Config.hpp
//  Constantes globais, tipos e helpers de caminho de assets.
//  Centraliza todos os "numeros magicos" que antes ficavam espalhados.
// ============================================================================

namespace config
{
    // ---- Grade / janela -----------------------------------------------------
    inline constexpr int CELL_SIZE = 39;  // tamanho em pixels de cada celula
    inline constexpr int GRID_COLS = 27;  // colunas do mapa (antigo "largura")
    inline constexpr int GRID_ROWS = 26;  // linhas do mapa  (antigo "altura")

    inline constexpr int WINDOW_WIDTH  = CELL_SIZE * GRID_COLS;
    inline constexpr int WINDOW_HEIGHT = CELL_SIZE * GRID_ROWS;
    inline constexpr int FPS_LIMIT     = 60;

    // ---- Jogador (Jett) -----------------------------------------------------
    inline constexpr int   PLAYER_START_COL   = 13;
    inline constexpr int   PLAYER_START_ROW   = 16;
    inline constexpr float PLAYER_SPEED       = 250.0f;  // pixels/segundo
    inline constexpr float TELEPORT_SPEED_MUL = 2.5f;    // boost ao usar a corda

    // ---- Fantasmas ----------------------------------------------------------
    inline constexpr float GHOST_SPEED         = 220.0f;
    inline constexpr float GHOST_CHASER_BONUS  = 30.0f;  // o cacador rapido
    inline constexpr float ANIMATION_INTERVAL  = 0.10f;  // troca de frame (s)

    // ---- Regras -------------------------------------------------------------
    inline constexpr int  STARTING_LIVES   = 3;
    inline constexpr int  SCORE_PER_PELLET = 50;
    inline constexpr int  SCORE_PER_POWER  = 100;
    inline constexpr int  SCORE_PER_GHOST  = 200;
    inline constexpr float FRIGHTENED_TIME = 7.0f;       // duracao do modo cacavel

    // ---- Caminhos de assets -------------------------------------------------
    inline const std::string ASSETS_DIR = "assets/";

    inline std::string img(const std::string& name)   { return ASSETS_DIR + "images/" + name; }
    inline std::string audio(const std::string& name) { return ASSETS_DIR + "audio/"  + name; }
    inline std::string font(const std::string& name)  { return ASSETS_DIR + "fonts/"  + name; }

    inline const std::string HIGHSCORE_FILE = "highscore.txt";
}

// ---- Estados da maquina de jogo --------------------------------------------
enum class GameState
{
    MainMenu,
    Ready,      // tela "pressione qualquer tecla"
    Playing,
    Paused,
    Victory,
    GameOver,
    Credits
};

// ---- Direcoes cardeais ------------------------------------------------------
enum class Direction
{
    None,
    Up,
    Down,
    Left,
    Right
};

// Deslocamento (linha, coluna) de uma direcao.
inline void directionDelta(Direction dir, int& dRow, int& dCol)
{
    switch (dir)
    {
        case Direction::Up:    dRow = -1; dCol =  0; break;
        case Direction::Down:  dRow =  1; dCol =  0; break;
        case Direction::Left:  dRow =  0; dCol = -1; break;
        case Direction::Right: dRow =  0; dCol =  1; break;
        default:               dRow =  0; dCol =  0; break;
    }
}

inline Direction oppositeDirection(Direction dir)
{
    switch (dir)
    {
        case Direction::Up:    return Direction::Down;
        case Direction::Down:  return Direction::Up;
        case Direction::Left:  return Direction::Right;
        case Direction::Right: return Direction::Left;
        default:               return Direction::None;
    }
}
