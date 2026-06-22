#pragma once

#include "Config.hpp"
#include "Map.hpp"
#include <SFML/Graphics.hpp>
#include <array>

// ============================================================================
//  Player (a agente Jett)
//  Encapsula posicao na grade, posicao em pixels, "intencao de movimento"
//  (controles responsivos), animacao de 3 frames e o teletransporte pela corda.
//  Toda a logica que antes vivia solta no main() agora mora aqui.
// ============================================================================
class Player
{
public:
    void setFrames(const sf::Texture& f0, const sf::Texture& f1, const sf::Texture& f2);
    void setPlaceholder(bool on, sf::Color color); // desenha um Pac-Man se faltar arte
    void reset();

    void setIntention(Direction dir);

    // Avanca o movimento; devolve o que foi comido ao chegar numa celula.
    Map::Consumed update(Map& map, float dt);

    void draw(sf::RenderWindow& window, int animFrame);

    int row() const { return m_row; }
    int col() const { return m_col; }

private:
    int m_row = config::PLAYER_START_ROW;
    int m_col = config::PLAYER_START_COL;

    sf::Vector2f m_pixel;
    sf::Vector2f m_target;
    bool m_moving = false;
    bool m_teleporting = false;

    Direction m_intention = Direction::None;
    Direction m_movement  = Direction::None;
    Direction m_facing    = Direction::Right;

    std::array<const sf::Texture*, 3> m_frames{nullptr, nullptr, nullptr};
    bool m_placeholder = false;
    sf::Color m_color = sf::Color::Yellow;
};
