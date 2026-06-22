#pragma once

#include "Config.hpp"
#include <SFML/Graphics.hpp>
#include <array>
#include <random>

class Map;

// ============================================================================
//  Ghost (os robos inimigos)
//  Uma unica classe para os 4 inimigos. O comportamento (perseguir com BFS ou
//  andar aleatoriamente), a velocidade e a cadencia sao parametros — antes
//  cada fantasma tinha seu proprio bloco copiado de ~70 linhas dentro do main.
// ============================================================================
class Ghost
{
public:
    enum class Behavior { Chase, Random };

    void configure(Behavior behavior, float speed, float interval,
                   int startRow, int startCol, Direction startDir);
    void setFrames(const sf::Texture& f0, const sf::Texture& f1, const sf::Texture& f2);
    void setPlaceholder(bool on, sf::Color color); // desenha um fantasma se faltar arte

    void reset();

    void update(const Map& map, int playerRow, int playerCol,
                float dt, std::mt19937& rng, bool frightened);

    void draw(sf::RenderWindow& window, int animFrame, bool frightened);

    int row() const { return m_row; }
    int col() const { return m_col; }

private:
    void stepChase(const Map& map, int playerRow, int playerCol);
    void stepRandom(const Map& map, std::mt19937& rng);
    void applyMove(const Map& map, Direction dir);

    Behavior m_behavior = Behavior::Random;
    float m_speed    = config::GHOST_SPEED;
    float m_interval = 0.3f;

    int m_startRow = 1, m_startCol = 1;
    Direction m_startDir = Direction::None;

    int m_row = 1, m_col = 1;
    Direction m_dir = Direction::None;
    sf::Vector2f m_pixel;

    sf::Clock m_clock;

    std::array<const sf::Texture*, 3> m_frames{nullptr, nullptr, nullptr};
    bool m_placeholder = false;
    sf::Color m_color = sf::Color::White;
};
