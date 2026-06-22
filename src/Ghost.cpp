#include "Ghost.hpp"
#include "Map.hpp"
#include "Pathfinding.hpp"
#include "Placeholders.hpp"
#include <cmath>

namespace
{
    sf::IntRect frameForDirection(Direction dir)
    {
        switch (dir)
        {
            case Direction::Right: return sf::IntRect({72, 360}, {240, 310});
            case Direction::Left:  return sf::IntRect({352, 361}, {272, 310});
            case Direction::Up:    return sf::IntRect({61, 29}, {242, 316});
            case Direction::Down:
            default:               return sf::IntRect({358, 35}, {259, 316});
        }
    }
}

void Ghost::configure(Behavior behavior, float speed, float interval,
                      int startRow, int startCol, Direction startDir)
{
    m_behavior = behavior;
    m_speed    = speed;
    m_interval = interval;
    m_startRow = startRow;
    m_startCol = startCol;
    m_startDir = startDir;
    reset();
}

void Ghost::setFrames(const sf::Texture& f0, const sf::Texture& f1, const sf::Texture& f2)
{
    m_frames = {&f0, &f1, &f2};
}

void Ghost::setPlaceholder(bool on, sf::Color color)
{
    m_placeholder = on;
    m_color = color;
}

void Ghost::reset()
{
    m_row = m_startRow;
    m_col = m_startCol;
    m_dir = m_startDir;
    m_pixel = sf::Vector2f(m_col * config::CELL_SIZE, m_row * config::CELL_SIZE);
    m_clock.restart();
}

void Ghost::applyMove(const Map& map, Direction dir)
{
    if (dir == Direction::None)
        return;
    int dr, dc;
    directionDelta(dir, dr, dc);
    if (!map.isWall(m_row + dr, m_col + dc))
    {
        m_dir = dir;
        m_row += dr;
        m_col += dc;
    }
}

void Ghost::stepChase(const Map& map, int playerRow, int playerCol)
{
    Direction next = pathfinding::nextStep(map, m_row, m_col, playerRow, playerCol);
    applyMove(map, next);
}

void Ghost::stepRandom(const Map& map, std::mt19937& rng)
{
    const std::array<Direction, 4> all = {
        Direction::Up, Direction::Down, Direction::Left, Direction::Right};

    std::array<Direction, 4> options{};
    int count = 0;
    for (Direction d : all)
    {
        int dr, dc;
        directionDelta(d, dr, dc);
        if (!map.isWall(m_row + dr, m_col + dc))
            options[count++] = d;
    }
    if (count == 0)
        return;

    // Evita dar meia-volta se houver outra opcao (movimento mais natural).
    Direction reverse = oppositeDirection(m_dir);
    if (count > 1)
    {
        for (int i = 0; i < count; ++i)
        {
            if (options[i] == reverse)
            {
                options[i] = options[count - 1];
                --count;
                break;
            }
        }
    }

    std::uniform_int_distribution<int> pick(0, count - 1);
    applyMove(map, options[pick(rng)]);
}

void Ghost::update(const Map& map, int playerRow, int playerCol,
                   float dt, std::mt19937& rng, bool frightened)
{
    // Interpolacao suave em pixels ate a celula-alvo (a cada frame).
    sf::Vector2f target(m_col * config::CELL_SIZE, m_row * config::CELL_SIZE);
    sf::Vector2f toTarget = target - m_pixel;
    float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
    float step = m_speed * dt;
    if (distance > step)
        m_pixel += (toTarget / distance) * step;
    else
        m_pixel = target;

    // Decisao de rota em intervalos fixos.
    if (m_clock.getElapsedTime().asSeconds() > m_interval)
    {
        // Quando cacavel, todos fogem de forma erratica (anda aleatorio).
        if (frightened || m_behavior == Behavior::Random)
            stepRandom(map, rng);
        else
            stepChase(map, playerRow, playerCol);

        m_clock.restart();
    }
}

void Ghost::draw(sf::RenderWindow& window, int animFrame, bool frightened)
{
    if (m_placeholder)
    {
        sf::Vector2f center(m_pixel.x + config::CELL_SIZE / 2.0f,
                            m_pixel.y + config::CELL_SIZE / 2.0f);
        placeholders::drawGhost(window, center, config::CELL_SIZE, m_color, m_dir, frightened);
        return;
    }

    const sf::Texture* tex = m_frames[animFrame % 3];
    if (!tex)
        return;

    sf::Sprite sprite(*tex);
    sprite.setTextureRect(frameForDirection(m_dir));
    sf::FloatRect bounds = sprite.getLocalBounds();
    if (bounds.size.x > 0 && bounds.size.y > 0)
        sprite.setScale({config::CELL_SIZE / bounds.size.x,
                         config::CELL_SIZE / bounds.size.y});
    sprite.setOrigin({bounds.position.x + bounds.size.x / 2.0f,
                      bounds.position.y + bounds.size.y / 2.0f});
    sprite.setColor(frightened ? sf::Color(80, 120, 255) : sf::Color::White);
    sprite.setPosition({m_pixel.x + config::CELL_SIZE / 2.0f,
                        m_pixel.y + config::CELL_SIZE / 2.0f});
    window.draw(sprite);
}
