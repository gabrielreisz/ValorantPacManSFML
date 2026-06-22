#pragma once

#include "Config.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
//  Placeholders procedurais
//  Quando a arte original esta ausente, desenhamos formas reconheciveis no
//  lugar de quadrados: a Jett vira um "Pac-Man", os robos viram fantasmas
//  classicos e as vidas viram coracoes. Tudo com sf::Shape, sem assets.
// ============================================================================
namespace placeholders
{
    // Pac-Man com a boca apontando para a direcao do movimento.
    inline void drawPacman(sf::RenderTarget& target, sf::Vector2f topLeft,
                           float cell, sf::Color color, Direction facing)
    {
        sf::Vector2f c = topLeft + sf::Vector2f(cell / 2.f, cell / 2.f);
        float r = cell * 0.46f;

        float base = 0.f;
        switch (facing)
        {
            case Direction::Left: base = static_cast<float>(M_PI);        break;
            case Direction::Up:   base = -static_cast<float>(M_PI) / 2.f; break;
            case Direction::Down: base =  static_cast<float>(M_PI) / 2.f; break;
            default:              base = 0.f;                              break;
        }

        const float mouth = 0.62f; // meia-abertura da boca (radianos)
        const int seg = 28;
        sf::ConvexShape pac;
        pac.setPointCount(seg + 2);
        pac.setPoint(0, c);
        for (int i = 0; i <= seg; ++i)
        {
            float a = base + mouth + (2.f * static_cast<float>(M_PI) - 2.f * mouth) * i / seg;
            pac.setPoint(i + 1, c + sf::Vector2f(std::cos(a) * r, std::sin(a) * r));
        }
        pac.setFillColor(color);
        target.draw(pac);

        // Olho
        float er = cell * 0.06f;
        sf::Vector2f off;
        switch (facing)
        {
            case Direction::Left:  off = {-r * 0.10f, -r * 0.45f}; break;
            case Direction::Up:    off = { r * 0.45f, -r * 0.10f}; break;
            case Direction::Down:  off = { r * 0.45f,  r * 0.10f}; break;
            default:               off = { r * 0.10f, -r * 0.45f}; break;
        }
        sf::CircleShape eye(er);
        eye.setOrigin({er, er});
        eye.setPosition(c + off);
        eye.setFillColor(sf::Color(20, 20, 30));
        target.draw(eye);
    }

    // Fantasma classico (cupula + corpo + 3 pes arredondados + olhos).
    inline void drawGhost(sf::RenderTarget& target, sf::Vector2f center,
                          float cell, sf::Color body, Direction facing, bool frightened)
    {
        sf::Color col = frightened ? sf::Color(60, 80, 220) : body;
        float w = cell * 0.86f;
        float h = cell * 0.86f;
        float r = w / 2.f;
        float domeCy = center.y - h * 0.12f;

        sf::RectangleShape rect({w, h * 0.5f});
        rect.setOrigin({w / 2.f, 0.f});
        rect.setPosition({center.x, domeCy});
        rect.setFillColor(col);
        target.draw(rect);

        sf::CircleShape dome(r);
        dome.setOrigin({r, r});
        dome.setPosition({center.x, domeCy});
        dome.setFillColor(col);
        target.draw(dome);

        float footR = w / 6.f;
        float footY = domeCy + h * 0.5f;
        for (int i = 0; i < 3; ++i)
        {
            sf::CircleShape foot(footR);
            foot.setOrigin({footR, footR});
            foot.setPosition({center.x - w / 2.f + footR + i * 2.f * footR, footY});
            foot.setFillColor(col);
            target.draw(foot);
        }

        // Olhos com pupila apontando para a direcao do movimento.
        float eyeR = w * 0.17f;
        float pupR = eyeR * 0.5f;
        float eyeY = domeCy - h * 0.06f;
        int dr, dc;
        directionDelta(facing, dr, dc);
        for (int s = -1; s <= 1; s += 2)
        {
            sf::Vector2f ec(center.x + s * w * 0.20f, eyeY);
            sf::CircleShape eye(eyeR);
            eye.setOrigin({eyeR, eyeR});
            eye.setPosition(ec);
            eye.setFillColor(frightened ? sf::Color(200, 200, 255) : sf::Color::White);
            target.draw(eye);

            sf::CircleShape pupil(pupR);
            pupil.setOrigin({pupR, pupR});
            pupil.setPosition({ec.x + dc * eyeR * 0.4f, ec.y + dr * eyeR * 0.4f});
            pupil.setFillColor(frightened ? sf::Color(40, 40, 120) : sf::Color(30, 30, 60));
            target.draw(pupil);
        }
    }

    // Coracao (duas circunferencias + triangulo).
    inline void drawHeart(sf::RenderTarget& target, sf::Vector2f topLeft,
                          float cell, sf::Color color)
    {
        sf::Vector2f c = topLeft + sf::Vector2f(cell / 2.f, cell / 2.f);
        float r = cell * 0.22f;

        sf::ConvexShape tri;
        tri.setPointCount(3);
        tri.setPoint(0, {c.x - r * 1.7f, c.y - r * 0.1f});
        tri.setPoint(1, {c.x + r * 1.7f, c.y - r * 0.1f});
        tri.setPoint(2, {c.x, c.y + r * 1.7f});
        tri.setFillColor(color);
        target.draw(tri);

        for (int s = -1; s <= 1; s += 2)
        {
            sf::CircleShape lobe(r);
            lobe.setOrigin({r, r});
            lobe.setPosition({c.x + s * r * 0.85f, c.y - r * 0.5f});
            lobe.setFillColor(color);
            target.draw(lobe);
        }
    }
}
