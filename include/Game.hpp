#pragma once

#include "Config.hpp"
#include "Ghost.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "ResourceManager.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <optional>
#include <random>
#include <string>
#include <vector>

// ============================================================================
//  Game
//  Orquestra tudo: janela, recursos, mapa, jogador, fantasmas, audio, HUD e a
//  maquina de estados. Substitui o main() de ~1900 linhas por um laco limpo
//  run() -> processEvents() / update() / render().
// ============================================================================
class Game
{
public:
    Game();
    void run();

private:
    // --- Ciclo principal ----------------------------------------------------
    void processEvents();
    void update();
    void render();

    // --- Setup --------------------------------------------------------------
    void loadResources();
    void setupEntities();
    void buildTileset();
    void applyLetterboxView(sf::Vector2u windowSize); // mantem a proporcao do mapa

    // --- Fluxo de jogo ------------------------------------------------------
    void startNewGame();
    void loseLife();
    void checkCollisions();
    void updateAudio();
    void onPelletEaten(Map::Consumed eaten);

    // --- Eventos por estado -------------------------------------------------
    void handleMenuKey(sf::Keyboard::Key key);
    void handleGameOverKey(sf::Keyboard::Key key);
    void handleVictoryKey(sf::Keyboard::Key key);
    void handlePlayingKey(sf::Keyboard::Key key);
    void handlePausedKey(sf::Keyboard::Key key);

    // --- Render por estado --------------------------------------------------
    void drawFullscreen(const sf::Texture* tex);
    void drawMaze();
    void drawHUD();
    void drawWorld();
    void drawVerticalMenu(const std::string& title, sf::Color titleColor,
                          const std::vector<std::string>& options, int selected);

    // --- Persistencia -------------------------------------------------------
    void loadHighScore();
    void saveHighScore();

    // --- Helpers ------------------------------------------------------------
    sf::Text makeText(const std::string& str, unsigned size, sf::Color color);

    // ------------------------------------------------------------------------
    sf::RenderWindow m_window;
    sf::View         m_view;   // view de resolucao logica fixa (1053x1014)
    ResourceManager  m_resources;
    std::mt19937     m_rng;

    GameState m_state = GameState::MainMenu;

    Map    m_map;
    Player m_player;
    std::array<Ghost, 4> m_ghosts;

    // Regras / pontuacao
    int  m_score = 0;
    int  m_lives = config::STARTING_LIVES;
    int  m_highScore = 0;
    bool m_frightened = false;
    sf::Clock m_frightenedClock;

    // Tempo / animacao
    sf::Clock m_deltaClock;
    sf::Clock m_animClock;
    int m_animFrame = 0;

    // Selecao de menus
    int m_menuIndex = 0;
    int m_gameOverIndex = 0;
    int m_victoryIndex = 0;

    // Hitboxes clicaveis do menu ativo (preenchidas no render, lidas nos eventos)
    std::array<sf::FloatRect, 4> m_clickBounds{};

    // Animacao da tela de vitoria
    sf::Clock m_victoryClock;
    float m_victorySize = 0.0f;
    std::string m_victoryText = "VICTORY!";

    // Recursos (ponteiros estaveis vindos do ResourceManager)
    sf::Font* m_font = nullptr;
    const sf::Texture* m_texGameBg   = nullptr;
    const sf::Texture* m_texMenuBg   = nullptr;
    const sf::Texture* m_texTreetops = nullptr;
    const sf::Texture* m_texHeart    = nullptr;
    const sf::Texture* m_texRope     = nullptr;
    const sf::Texture* m_texPellet   = nullptr;
    std::array<const sf::Texture*, 16> m_tileTex{};

    // Audio
    sf::Music m_musicMenu;
    sf::Music m_musicIntro;
    sf::Music m_musicGame;
    std::optional<sf::Sound> m_soundFlawless;
    std::optional<sf::Sound> m_soundVictory;
    std::optional<sf::Sound> m_soundDefeat;
    bool m_gameMusicStarted = false;
};
