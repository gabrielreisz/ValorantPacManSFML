#include "Player.hpp"
#include "Placeholders.hpp"
#include <cmath>

namespace
{
    // Recorte da spritesheet para cada direcao em que a Jett esta olhando.
    sf::IntRect frameForFacing(Direction facing)
    {
        switch (facing)
        {
            case Direction::Left:  return sf::IntRect({671, 524}, {279, 435});
            case Direction::Up:    return sf::IntRect({368, 519}, {271, 441});
            case Direction::Down:  return sf::IntRect({36, 24}, {327, 495});
            case Direction::Right:
            default:               return sf::IntRect({653, 77}, {268, 423});
        }
    }
}

void Player::setFrames(const sf::Texture& f0, const sf::Texture& f1, const sf::Texture& f2)
{
    m_frames = {&f0, &f1, &f2};
}

void Player::setPlaceholder(bool on, sf::Color color)
{
    m_placeholder = on;
    m_color = color;
}

void Player::reset()
{
    m_row = config::PLAYER_START_ROW;
    m_col = config::PLAYER_START_COL;
    m_pixel  = sf::Vector2f(m_col * config::CELL_SIZE, m_row * config::CELL_SIZE);
    m_target = m_pixel;
    m_moving = false;
    m_teleporting = false;
    m_intention = Direction::None;
    m_movement  = Direction::None;
    m_facing    = Direction::Right;
}

void Player::setIntention(Direction dir)
{
    m_intention = dir;
}

Map::Consumed Player::update(Map& map, float dt)
{
    Map::Consumed eaten = Map::Consumed::None;

    if (m_moving)
    {
        sf::Vector2f toTarget = m_target - m_pixel;
        float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

        float speed = m_teleporting
                          ? config::PLAYER_SPEED * config::TELEPORT_SPEED_MUL
                          : config::PLAYER_SPEED;
        float step = speed * dt;

        if (distance > step)
        {
            m_pixel += (toTarget / distance) * step;
        }
        else
        {
            // Chegou exatamente na celula de destino.
            m_pixel = m_target;
            bool wasTeleporting = m_teleporting;
            m_teleporting = false;

            eaten = map.consume(m_row, m_col);

            int exitRow, exitCol;
            if (map.isTunnel(m_row, m_col) && !wasTeleporting &&
                map.tunnelExit(m_row, m_col, exitRow, exitCol))
            {
                m_teleporting = true;
                m_row = exitRow;
                m_col = exitCol;
                m_target = sf::Vector2f(m_col * config::CELL_SIZE, m_row * config::CELL_SIZE);
            }
            else
            {
                m_moving = false;
            }
        }
    }
    else
    {
        int ir, ic;
        directionDelta(m_intention, ir, ic);
        if (m_intention != Direction::None && !map.isWall(m_row + ir, m_col + ic))
            m_movement = m_intention;

        int mr, mc;
        directionDelta(m_movement, mr, mc);
        if (m_movement != Direction::None && !map.isWall(m_row + mr, m_col + mc))
        {
            m_moving = true;
            m_facing = m_movement;
            m_row += mr;
            m_col += mc;
            m_target = sf::Vector2f(m_col * config::CELL_SIZE, m_row * config::CELL_SIZE);
        }
    }

    return eaten;
}

void Player::draw(sf::RenderWindow& window, int animFrame)
{
    if (m_placeholder)
    {
        placeholders::drawPacman(window, m_pixel, config::CELL_SIZE, m_color, m_facing);
        return;
    }

    const sf::Texture* tex = m_frames[animFrame % 3];
    if (!tex)
        return;

    sf::Sprite sprite(*tex);
    sprite.setTextureRect(frameForFacing(m_facing));
    sf::FloatRect bounds = sprite.getLocalBounds();
    if (bounds.size.x > 0 && bounds.size.y > 0)
        sprite.setScale({config::CELL_SIZE / bounds.size.x,
                         config::CELL_SIZE / bounds.size.y});
    sprite.setPosition(m_pixel);
    window.draw(sprite);
}
