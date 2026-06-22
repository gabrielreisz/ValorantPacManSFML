#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

// ============================================================================
//  ResourceManager
//  Cache de texturas / fontes / buffers de som. Carrega cada arquivo uma unica
//  vez e devolve uma referencia estavel (os sprites guardam ponteiros para a
//  textura, entao ela precisa viver enquanto o jogo rodar).
//
//  Tolerante a falta de assets (a arte original deste trabalho nao foi toda
//  versionada): se um arquivo nao existe, gera um PLACEHOLDER procedural por
//  categoria, e a fonte cai para uma do sistema. O jogo roda mesmo assim.
// ============================================================================
class ResourceManager
{
public:
    sf::Texture& texture(const std::string& path)
    {
        auto it = m_textures.find(path);
        if (it != m_textures.end())
            return it->second;

        sf::Texture& tex = m_textures[path];
        if (tex.loadFromFile(path))
        {
            tex.setSmooth(true);
        }
        else
        {
            std::cerr << "[aviso] textura ausente, usando placeholder: " << path << '\n';
            sf::Image img = placeholderImage(path);
            (void)tex.loadFromImage(img);
            m_placeholders.insert(path);
        }
        return tex;
    }

    // true se a textura desse caminho foi substituida por um placeholder.
    bool isPlaceholder(const std::string& path) const
    {
        return m_placeholders.count(path) > 0;
    }

    sf::Font& font(const std::string& path)
    {
        auto it = m_fonts.find(path);
        if (it != m_fonts.end())
            return it->second;

        sf::Font& f = m_fonts[path];
        if (f.openFromFile(path))
            return f;

        // Fallback: tenta fontes do sistema (macOS / Linux).
        static const std::array<const char*, 6> fallbacks = {
            "/System/Library/Fonts/Supplemental/Arial.ttf",
            "/Library/Fonts/Arial.ttf",
            "/System/Library/Fonts/Helvetica.ttc",
            "/System/Library/Fonts/Supplemental/Verdana.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf"};
        for (const char* fb : fallbacks)
            if (f.openFromFile(fb))
            {
                std::cerr << "[aviso] fonte ausente (" << path
                          << "), usando do sistema: " << fb << '\n';
                return f;
            }

        throw std::runtime_error("Falha ao carregar fonte e nenhum fallback do "
                                 "sistema disponivel: " + path);
    }

    sf::SoundBuffer& soundBuffer(const std::string& path)
    {
        auto it = m_buffers.find(path);
        if (it != m_buffers.end())
            return it->second;

        sf::SoundBuffer& b = m_buffers[path];
        if (!b.loadFromFile(path))
            throw std::runtime_error("Falha ao carregar som: " + path);
        return b;
    }

private:
    // Gera uma textura-placeholder coerente com o tipo de asset esperado,
    // deduzido pelo nome do arquivo.
    static sf::Image placeholderImage(const std::string& path)
    {
        std::string n = path;
        std::transform(n.begin(), n.end(), n.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        auto has = [&](const char* s) { return n.find(s) != std::string::npos; };

        // Sprites de personagens precisam ser grandes (o codigo recorta regioes).
        if (has("spritesheet"))            return solid({1024, 1024}, {80, 220, 230});  // Jett
        if (has("roboazul"))               return solid({1024, 1024}, {60, 110, 255});
        if (has("robovermelho"))           return solid({1024, 1024}, {235, 70, 70});
        if (has("roboamarelo"))            return solid({1024, 1024}, {240, 220, 70});
        if (has("roboverde"))              return solid({1024, 1024}, {80, 210, 110});
        if (has("robo"))                   return solid({1024, 1024}, {220, 80, 220});

        // Overlay desenhado por cima de tudo -> precisa ser transparente.
        if (has("copas") || has("treetop")) return solid({64, 64}, {0, 0, 0, 0});

        if (has("fundo"))                  return solid({64, 64}, {18, 18, 28});
        if (has("menu"))                   return solid({64, 64}, {14, 16, 26});
        if (has("heart"))                  return solid({64, 64}, {255, 70, 80});
        if (has("corda"))                  return solid({64, 64}, {150, 95, 45});
        if (has("orb"))                    return disc({64, 64}, {235, 240, 255});

        // Restante (tiles do caminho do labirinto).
        return solid({64, 64}, {40, 62, 96});
    }

    static sf::Image solid(sf::Vector2u size, sf::Color color)
    {
        return sf::Image(size, color);
    }

    static sf::Image disc(sf::Vector2u size, sf::Color color)
    {
        sf::Image img(size, sf::Color(0, 0, 0, 0));
        float cx = size.x / 2.0f, cy = size.y / 2.0f;
        float r = std::min(cx, cy) - 1.0f;
        for (unsigned y = 0; y < size.y; ++y)
            for (unsigned x = 0; x < size.x; ++x)
            {
                float dx = x + 0.5f - cx, dy = y + 0.5f - cy;
                if (dx * dx + dy * dy <= r * r)
                    img.setPixel({x, y}, color);
            }
        return img;
    }

    std::unordered_map<std::string, sf::Texture>     m_textures;
    std::unordered_map<std::string, sf::Font>        m_fonts;
    std::unordered_map<std::string, sf::SoundBuffer> m_buffers;
    std::unordered_set<std::string>                  m_placeholders;
};
