#include "Game.hpp"
#include "Placeholders.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

using config::CELL_SIZE;
using config::GRID_COLS;
using config::GRID_ROWS;

namespace
{
    const sf::Color kTeal(122, 216, 189);

    // Resolucao logica do jogo (coordenadas do mundo, independentes da janela).
    constexpr float LOGICAL_W = static_cast<float>(config::WINDOW_WIDTH);
    constexpr float LOGICAL_H = static_cast<float>(config::WINDOW_HEIGHT);

    // Centraliza horizontalmente um texto no espaco logico.
    float centerX(const sf::Text& text)
    {
        return LOGICAL_W / 2.0f - text.getGlobalBounds().size.x / 2.0f;
    }
}

// ============================================================================
//  Construcao e setup
// ============================================================================
Game::Game()
    : m_window(sf::VideoMode({static_cast<unsigned>(config::WINDOW_WIDTH),
                              static_cast<unsigned>(config::WINDOW_HEIGHT)}),
               "Orb Rush"),
      m_rng(std::random_device{}())
{
    m_window.setFramerateLimit(config::FPS_LIMIT);

    // A janela cheia (1053x1014) costuma ser maior que a tela. Reduz o tamanho
    // inicial para caber no desktop, mantendo a proporcao via view com letterbox.
    sf::Vector2u desktop = sf::VideoMode::getDesktopMode().size;
    float fit = std::min({desktop.x * 0.9f / LOGICAL_W,
                          desktop.y * 0.9f / LOGICAL_H, 1.0f});
    m_window.setSize({static_cast<unsigned>(LOGICAL_W * fit),
                      static_cast<unsigned>(LOGICAL_H * fit)});
    applyLetterboxView(m_window.getSize());

    loadResources();
    setupEntities();
    loadHighScore();
}

void Game::applyLetterboxView(sf::Vector2u windowSize)
{
    // Encaixa o mundo logico na janela preservando a proporcao (barras pretas
    // nas laterais ou em cima/baixo quando a proporcao difere).
    float worldRatio = LOGICAL_W / LOGICAL_H;
    float winRatio   = static_cast<float>(windowSize.x) / windowSize.y;

    float sx = 1.0f, sy = 1.0f, px = 0.0f, py = 0.0f;
    if (winRatio > worldRatio) { sx = worldRatio / winRatio; px = (1.0f - sx) / 2.0f; }
    else                       { sy = winRatio / worldRatio; py = (1.0f - sy) / 2.0f; }

    m_view.setSize({LOGICAL_W, LOGICAL_H});
    m_view.setCenter({LOGICAL_W / 2.0f, LOGICAL_H / 2.0f});
    m_view.setViewport(sf::FloatRect({px, py}, {sx, sy}));
    m_window.setView(m_view);
}

void Game::buildTileset()
{
    // O indice e uma mascara de bits dos vizinhos que sao caminho:
    //   1=esq  2=dir  4=cima  8=baixo. Cada combinacao tem seu desenho.
    auto setTile = [&](int index, const std::string& file)
    {
        m_tileTex[index] = &m_resources.texture(config::img(file));
    };
    setTile(3,  "horizontalcheio.png"); // esq+dir
    setTile(12, "verticalcheio.png");   // cima+baixo
    setTile(5,  "canto1.jpg");          // esq+cima
    setTile(6,  "canto2.jpg");          // dir+cima
    setTile(9,  "canto3.jpg");          // esq+baixo
    setTile(10, "canto4.jpg");          // dir+baixo
    setTile(7,  "t1.jpg");              // esq+dir+cima
    setTile(11, "t2.jpg");              // esq+dir+baixo
    setTile(13, "t3.jpg");              // esq+cima+baixo
    setTile(14, "t4.jpg");              // dir+cima+baixo
    setTile(15, "cruzamento.jpg");      // cruzamento
}

void Game::loadResources()
{
    m_font = &m_resources.font(config::font("Valorant_Font.ttf"));

    m_texGameBg   = &m_resources.texture(config::img("fundo3.png"));
    m_texMenuBg   = &m_resources.texture(config::img("menu.png"));
    m_texTreetops = &m_resources.texture(config::img("copas2.png"));
    m_texHeart    = &m_resources.texture(config::img("heart.png"));
    m_texRope     = &m_resources.texture(config::img("corda.png"));
    m_texPellet   = &m_resources.texture(config::img("orb.png"));

    buildTileset();

    // Musicas (streamadas)
    if (m_musicMenu.openFromFile(config::audio("menu.mp3")))
    {
        m_musicMenu.setLooping(true);
        m_musicMenu.setVolume(50.0f);
    }
    if (m_musicIntro.openFromFile(config::audio("inicio.mp3")))
        m_musicIntro.setVolume(50.0f);
    if (m_musicGame.openFromFile(config::audio("jogo.mp3")))
    {
        m_musicGame.setLooping(true);
        m_musicGame.setVolume(50.0f);
    }

    // Efeitos sonoros (precisam do buffer no construtor na SFML 3).
    // Sao opcionais: se o arquivo faltar, o jogo apenas roda sem aquele som.
    auto tryLoadSound = [&](std::optional<sf::Sound>& slot, const std::string& file) {
        try { slot.emplace(m_resources.soundBuffer(config::audio(file))); }
        catch (const std::exception& e) { std::cerr << "[aviso] " << e.what() << '\n'; }
    };
    tryLoadSound(m_soundFlawless, "flawless.mp3");
    tryLoadSound(m_soundVictory,  "victory.mp3");
    tryLoadSound(m_soundDefeat,   "defeat.mp3");
}

void Game::setupEntities()
{
    // Helper: liga o modo placeholder se a 1a textura da entidade for gerada.
    auto missing = [&](const std::string& file) {
        return m_resources.isPlaceholder(config::img(file));
    };

    m_player.setFrames(m_resources.texture(config::img("spritesheet.png")),
                       m_resources.texture(config::img("spritesheet2.png")),
                       m_resources.texture(config::img("spritesheet3.png")));
    m_player.setPlaceholder(missing("spritesheet.png"), sf::Color(95, 225, 235));

    // 0: azul  -> cacador rapido
    m_ghosts[0].configure(Ghost::Behavior::Chase,
                          config::GHOST_SPEED + config::GHOST_CHASER_BONUS, 0.25f,
                          1, 1, Direction::None);
    m_ghosts[0].setFrames(m_resources.texture(config::img("roboazul1.png")),
                          m_resources.texture(config::img("roboazul2.png")),
                          m_resources.texture(config::img("roboazul3.png")));
    m_ghosts[0].setPlaceholder(missing("roboazul1.png"), sf::Color(70, 120, 255));

    // 1: vermelho -> cacador
    m_ghosts[1].configure(Ghost::Behavior::Chase, config::GHOST_SPEED, 0.30f,
                          24, 25, Direction::None);
    m_ghosts[1].setFrames(m_resources.texture(config::img("robovermelho1.png")),
                          m_resources.texture(config::img("robovermelho2.png")),
                          m_resources.texture(config::img("robovermelho3.png")));
    m_ghosts[1].setPlaceholder(missing("robovermelho1.png"), sf::Color(235, 70, 70));

    // 2: amarelo -> aleatorio
    m_ghosts[2].configure(Ghost::Behavior::Random, config::GHOST_SPEED, 0.30f,
                          1, 25, Direction::Down);
    m_ghosts[2].setFrames(m_resources.texture(config::img("roboamarelo1.png")),
                          m_resources.texture(config::img("roboamarelo2.png")),
                          m_resources.texture(config::img("roboamarelo3.png")));
    m_ghosts[2].setPlaceholder(missing("roboamarelo1.png"), sf::Color(240, 220, 70));

    // 3: verde -> aleatorio
    m_ghosts[3].configure(Ghost::Behavior::Random, config::GHOST_SPEED, 0.30f,
                          24, 1, Direction::Up);
    m_ghosts[3].setFrames(m_resources.texture(config::img("roboverde1.png")),
                          m_resources.texture(config::img("roboverde2.png")),
                          m_resources.texture(config::img("roboverde3.png")));
    m_ghosts[3].setPlaceholder(missing("roboverde1.png"), sf::Color(80, 210, 110));
}

sf::Text Game::makeText(const std::string& str, unsigned size, sf::Color color)
{
    sf::Text text(*m_font, str, size);
    text.setFillColor(color);
    return text;
}

// ============================================================================
//  Ciclo principal
// ============================================================================
void Game::run()
{
    while (m_window.isOpen())
    {
        processEvents();
        update();
        render();
    }
}

void Game::update()
{
    updateAudio();

    if (m_state != GameState::Playing)
        return;

    float dt = std::min(m_deltaClock.restart().asSeconds(), 0.1f);

    if (m_frightened && m_frightenedClock.getElapsedTime().asSeconds() > config::FRIGHTENED_TIME)
        m_frightened = false;

    Map::Consumed eaten = m_player.update(m_map, dt);
    onPelletEaten(eaten);
    if (m_state != GameState::Playing) // virou vitoria
        return;

    for (Ghost& ghost : m_ghosts)
        ghost.update(m_map, m_player.row(), m_player.col(), dt, m_rng, m_frightened);

    checkCollisions();
}

void Game::updateAudio()
{
    using Status = sf::SoundSource::Status;
    bool inMenuArea = (m_state == GameState::MainMenu || m_state == GameState::Credits);

    if (inMenuArea && m_musicMenu.getStatus() != Status::Playing)
    {
        m_musicGame.stop();
        m_musicIntro.stop();
        m_musicMenu.play();
    }
    else if (!inMenuArea && m_musicMenu.getStatus() == Status::Playing)
    {
        m_musicMenu.stop();
    }

    if (m_state == GameState::Playing &&
        m_musicIntro.getStatus() == Status::Stopped && !m_gameMusicStarted)
    {
        m_musicGame.play();
        m_gameMusicStarted = true;
    }
}

// ============================================================================
//  Fluxo de jogo
// ============================================================================
void Game::startNewGame()
{
    m_map.reset();
    m_player.reset();
    for (Ghost& ghost : m_ghosts)
        ghost.reset();

    m_score = 0;
    m_lives = config::STARTING_LIVES;
    m_frightened = false;
    m_animFrame = 0;

    m_state = GameState::Ready;
    m_gameMusicStarted = false;
    m_musicIntro.play();
}

void Game::onPelletEaten(Map::Consumed eaten)
{
    if (eaten == Map::Consumed::Pellet)
    {
        m_score += config::SCORE_PER_PELLET;
    }
    else if (eaten == Map::Consumed::Power)
    {
        m_score += config::SCORE_PER_POWER;
        m_frightened = true;
        m_frightenedClock.restart();
    }

    if (eaten != Map::Consumed::None && m_map.allPelletsEaten())
    {
        m_musicGame.stop();
        if (m_lives == config::STARTING_LIVES)
        {
            m_victoryText = "Flawless!";
            if (m_soundFlawless) m_soundFlawless->play();
        }
        else
        {
            m_victoryText = "VICTORY!";
            if (m_soundVictory) m_soundVictory->play();
        }
        m_victorySize = 0.0f;
        m_victoryClock.restart();
        m_state = GameState::Victory;
        if (m_score > m_highScore) { m_highScore = m_score; saveHighScore(); }
    }
}

void Game::checkCollisions()
{
    for (Ghost& ghost : m_ghosts)
    {
        if (ghost.row() == m_player.row() && ghost.col() == m_player.col())
        {
            if (m_frightened)
            {
                ghost.reset();
                m_score += config::SCORE_PER_GHOST;
            }
            else
            {
                loseLife();
                return; // posicoes ja foram resetadas
            }
        }
    }
}

void Game::loseLife()
{
    --m_lives;
    if (m_lives <= 0)
    {
        m_musicGame.stop();
        if (m_soundDefeat) m_soundDefeat->play();
        m_state = GameState::GameOver;
        if (m_score > m_highScore) { m_highScore = m_score; saveHighScore(); }
    }
    else
    {
        m_player.reset();
        for (Ghost& ghost : m_ghosts)
            ghost.reset();
        m_frightened = false;
        m_state = GameState::Ready;
    }
}

// ============================================================================
//  Eventos
// ============================================================================
void Game::processEvents()
{
    while (const std::optional event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
            return;
        }

        if (const auto* resized = event->getIf<sf::Event::Resized>())
        {
            applyLetterboxView(resized->size);
            continue;
        }

        const auto* keyEvent   = event->getIf<sf::Event::KeyPressed>();
        const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();

        auto clickedIndex = [&](int count) -> int {
            if (!mouseEvent) return -1;
            // Converte pixel da janela -> coordenada do mundo logico.
            sf::Vector2f p = m_window.mapPixelToCoords(mouseEvent->position, m_view);
            for (int i = 0; i < count; ++i)
                if (m_clickBounds[i].contains(p))
                    return i;
            return -1;
        };

        switch (m_state)
        {
            case GameState::MainMenu:
                if (keyEvent) handleMenuKey(keyEvent->code);
                else if (int i = clickedIndex(3); i >= 0)
                {
                    if (i == 0) startNewGame();
                    else if (i == 1) m_state = GameState::Credits;
                    else m_window.close();
                }
                break;

            case GameState::Credits:
                if (keyEvent && (keyEvent->code == sf::Keyboard::Key::Enter ||
                                 keyEvent->code == sf::Keyboard::Key::Escape))
                    m_state = GameState::MainMenu;
                else if (clickedIndex(1) == 0)
                    m_state = GameState::MainMenu;
                break;

            case GameState::Ready:
                if (keyEvent || mouseEvent)
                {
                    m_state = GameState::Playing;
                    m_deltaClock.restart();
                }
                break;

            case GameState::Playing:
                if (keyEvent) handlePlayingKey(keyEvent->code);
                break;

            case GameState::Paused:
                if (keyEvent) handlePausedKey(keyEvent->code);
                break;

            case GameState::GameOver:
                if (keyEvent) handleGameOverKey(keyEvent->code);
                else if (int i = clickedIndex(3); i >= 0)
                {
                    if (i == 0) startNewGame();
                    else if (i == 1) m_state = GameState::MainMenu;
                    else m_window.close();
                }
                break;

            case GameState::Victory:
                if (keyEvent) handleVictoryKey(keyEvent->code);
                else if (int i = clickedIndex(2); i >= 0)
                {
                    if (i == 0) startNewGame();
                    else m_state = GameState::MainMenu;
                }
                break;
        }
    }
}

void Game::handleMenuKey(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Key::Up)
        m_menuIndex = (m_menuIndex + 2) % 3;
    else if (key == sf::Keyboard::Key::Down)
        m_menuIndex = (m_menuIndex + 1) % 3;
    else if (key == sf::Keyboard::Key::Enter)
    {
        if (m_menuIndex == 0)      startNewGame();
        else if (m_menuIndex == 1) m_state = GameState::Credits;
        else                       m_window.close();
    }
}

void Game::handleGameOverKey(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Key::Up)
        m_gameOverIndex = (m_gameOverIndex + 2) % 3;
    else if (key == sf::Keyboard::Key::Down)
        m_gameOverIndex = (m_gameOverIndex + 1) % 3;
    else if (key == sf::Keyboard::Key::Enter)
    {
        if (m_gameOverIndex == 0)      startNewGame();
        else if (m_gameOverIndex == 1) m_state = GameState::MainMenu;
        else                           m_window.close();
    }
}

void Game::handleVictoryKey(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::Down)
        m_victoryIndex = (m_victoryIndex + 1) % 2;
    else if (key == sf::Keyboard::Key::Enter)
    {
        if (m_victoryIndex == 0) startNewGame();
        else                     m_state = GameState::MainMenu;
    }
}

void Game::handlePlayingKey(sf::Keyboard::Key key)
{
    switch (key)
    {
        case sf::Keyboard::Key::Left:  m_player.setIntention(Direction::Left);  break;
        case sf::Keyboard::Key::Right: m_player.setIntention(Direction::Right); break;
        case sf::Keyboard::Key::Up:    m_player.setIntention(Direction::Up);    break;
        case sf::Keyboard::Key::Down:  m_player.setIntention(Direction::Down);  break;
        case sf::Keyboard::Key::P:
        case sf::Keyboard::Key::Escape:
            m_state = GameState::Paused;
            m_musicGame.pause();
            break;
        default: break;
    }
}

void Game::handlePausedKey(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Key::P || key == sf::Keyboard::Key::Enter)
    {
        m_state = GameState::Playing;
        m_deltaClock.restart();
        if (m_gameMusicStarted)
            m_musicGame.play();
    }
    else if (key == sf::Keyboard::Key::Escape)
    {
        m_musicGame.stop();
        m_state = GameState::MainMenu;
    }
}

// ============================================================================
//  Render
// ============================================================================
void Game::drawFullscreen(const sf::Texture* tex)
{
    if (!tex)
        return;
    sf::Sprite sprite(*tex);
    sprite.setScale({LOGICAL_W / tex->getSize().x, LOGICAL_H / tex->getSize().y});
    m_window.draw(sprite);
}

void Game::drawVerticalMenu(const std::string& title, sf::Color titleColor,
                            const std::vector<std::string>& options, int selected)
{
    sf::Text titleText = makeText(title, 80, titleColor);
    titleText.setPosition({centerX(titleText), 150.f});
    m_window.draw(titleText);

    for (std::size_t i = 0; i < options.size(); ++i)
    {
        bool active = (static_cast<int>(i) == selected);
        sf::Text item = makeText(options[i], 50,
                                 active ? sf::Color::Yellow : sf::Color::White);
        item.setPosition({centerX(item), 350.f + 100.f * i});
        m_window.draw(item);
        m_clickBounds[i] = item.getGlobalBounds();
    }
}

void Game::drawMaze()
{
    auto isPath = [](char ch) { return ch >= 'A' && ch <= 'z'; };

    for (int i = 0; i < GRID_ROWS; ++i)
    {
        for (int j = 0; j < GRID_COLS; ++j)
        {
            char cell = m_map.at(i, j);
            if (cell == '1')
                continue;

            int mask = 0;
            if (isPath(m_map.at(i, j - 1))) mask += 1;
            if (isPath(m_map.at(i, j + 1))) mask += 2;
            if (isPath(m_map.at(i - 1, j))) mask += 4;
            if (isPath(m_map.at(i + 1, j))) mask += 8;

            if (const sf::Texture* tex = m_tileTex[mask])
            {
                sf::Sprite tile(*tex);
                sf::Vector2u sz = tex->getSize();
                if (sz.x > 0 && sz.y > 0)
                {
                    tile.setScale({CELL_SIZE / static_cast<float>(sz.x),
                                   CELL_SIZE / static_cast<float>(sz.y)});
                    tile.setPosition({static_cast<float>(j * CELL_SIZE),
                                      static_cast<float>(i * CELL_SIZE)});
                    m_window.draw(tile);
                }
            }

            if (cell >= 'a' && cell <= 'z' && m_texPellet)
            {
                sf::Sprite pellet(*m_texPellet);
                sf::FloatRect pb = pellet.getLocalBounds();
                pellet.setOrigin({pb.size.x / 2.0f, pb.size.y / 2.0f});
                float base = (CELL_SIZE / 1.5f) / pb.size.x;
                if (cell == 'o')
                {
                    static sf::Clock pulse;
                    float s = base * (1.6f + 0.2f * std::sin(pulse.getElapsedTime().asSeconds() * 6.0f));
                    pellet.setScale({s, s});
                }
                else
                {
                    pellet.setScale({base, base});
                }
                pellet.setPosition({j * CELL_SIZE + CELL_SIZE / 2.0f,
                                    i * CELL_SIZE + CELL_SIZE / 2.0f});
                m_window.draw(pellet);
            }
        }
    }
}

void Game::drawHUD()
{
    sf::Text score = makeText("Pontos: " + std::to_string(m_score), 40, sf::Color::Red);
    score.setPosition({60.f, -6.f});
    m_window.draw(score);

    sf::Text high = makeText("Recorde: " + std::to_string(m_highScore), 28, kTeal);
    high.setPosition({centerX(high), 4.f});
    m_window.draw(high);

    bool heartMissing = m_resources.isPlaceholder(config::img("heart.png"));
    for (int i = 0; i < m_lives; ++i)
    {
        sf::Vector2f pos(LOGICAL_W - (i + 2.2f) * (CELL_SIZE + 5), 1.f);
        if (heartMissing || !m_texHeart)
        {
            placeholders::drawHeart(m_window, pos, CELL_SIZE, sf::Color(255, 70, 80));
        }
        else
        {
            sf::Sprite heart(*m_texHeart);
            sf::Vector2u sz = m_texHeart->getSize();
            heart.setScale({CELL_SIZE / static_cast<float>(sz.x),
                            CELL_SIZE / static_cast<float>(sz.y)});
            heart.setPosition(pos);
            m_window.draw(heart);
        }
    }
}

void Game::drawWorld()
{
    drawFullscreen(m_texGameBg);
    drawMaze();

    // Corda (tunel) na linha central
    if (m_texRope)
    {
        sf::Vector2u sz = m_texRope->getSize();
        for (int j = 4; j < 23; ++j)
        {
            sf::Sprite rope(*m_texRope);
            rope.setScale({CELL_SIZE / static_cast<float>(sz.x),
                           CELL_SIZE / static_cast<float>(sz.y)});
            rope.setPosition({static_cast<float>(j * CELL_SIZE),
                              static_cast<float>(13 * CELL_SIZE)});
            m_window.draw(rope);
        }
    }

    // Avanca animacao apenas enquanto joga
    if (m_state == GameState::Playing &&
        m_animClock.getElapsedTime().asSeconds() >= config::ANIMATION_INTERVAL)
    {
        m_animFrame = (m_animFrame + 1) % 3;
        m_animClock.restart();
    }

    m_player.draw(m_window, m_animFrame);
    for (Ghost& ghost : m_ghosts)
        ghost.draw(m_window, m_animFrame, m_frightened);

    drawFullscreen(m_texTreetops);
    drawHUD();

    if (m_state == GameState::Ready)
    {
        sf::Text ready = makeText("Pressione qualquer\ntecla para continuar", 33, sf::Color::Yellow);
        sf::FloatRect b = ready.getLocalBounds();
        ready.setOrigin({b.position.x + b.size.x / 2.0f, b.position.y + b.size.y / 2.0f});
        ready.setPosition({LOGICAL_W / 2.0f, LOGICAL_H / 2.0f});
        m_window.draw(ready);
    }

    if (m_state == GameState::Victory)
    {
        if (m_victorySize < 200.0f && m_victoryClock.getElapsedTime().asMilliseconds() > 10)
        {
            m_victorySize = std::min(m_victorySize + 1.5f, 200.0f);
            m_victoryClock.restart();
        }
        sf::Text victory = makeText(m_victoryText, static_cast<unsigned>(m_victorySize), kTeal);
        sf::FloatRect b = victory.getLocalBounds();
        victory.setOrigin({b.position.x + b.size.x / 2.0f, b.position.y + b.size.y / 2.0f});
        victory.setPosition({LOGICAL_W / 2.0f, LOGICAL_H / 2.0f});
        m_window.draw(victory);

        const std::array<std::string, 2> opts = {"Jogar Novamente", "Ir para o Menu"};
        for (std::size_t i = 0; i < opts.size(); ++i)
        {
            bool active = (static_cast<int>(i) == m_victoryIndex);
            sf::Text item = makeText(opts[i], 50, active ? sf::Color::Yellow : sf::Color::White);
            item.setPosition({centerX(item), 450.f + 100.f * i});
            m_window.draw(item);
            m_clickBounds[i] = item.getGlobalBounds();
        }
    }
}

void Game::render()
{
    m_window.clear();

    switch (m_state)
    {
        case GameState::MainMenu:
            drawFullscreen(m_texMenuBg);
            drawVerticalMenu("ORB RUSH", kTeal, {"Jogar", "Creditos", "Sair"}, m_menuIndex);
            break;

        case GameState::Credits:
        {
            drawFullscreen(m_texMenuBg);
            sf::Text body = makeText(
                "Desenvolvido por:\n\n\n"
                "->Gabriel Costa Reis - 120549\n"
                "->Marcos Vinicius Mariano Dias - 120560\n"
                "->Victor Alexandre S Ribeiro - 120557\n\n"
                "Prof. Andre Gustavo dos Santos",
                40, sf::Color::White);
            body.setPosition({centerX(body), 100.f});
            m_window.draw(body);

            sf::Text back = makeText("Voltar", 50, sf::Color::Yellow);
            back.setPosition({centerX(back), 600.f});
            m_window.draw(back);
            m_clickBounds[0] = back.getGlobalBounds();
            break;
        }

        case GameState::GameOver:
            drawFullscreen(m_texMenuBg);
            drawVerticalMenu("DEFEAT", sf::Color::Red,
                             {"Jogar Novamente", "Ir para o Menu", "Sair"}, m_gameOverIndex);
            break;

        default: // Playing / Ready / Victory / Paused
            drawWorld();
            if (m_state == GameState::Paused)
            {
                sf::Text pause = makeText("PAUSA\n\nEnter: continuar\nEsc: menu", 50, sf::Color::Yellow);
                sf::FloatRect b = pause.getLocalBounds();
                pause.setOrigin({b.position.x + b.size.x / 2.0f, b.position.y + b.size.y / 2.0f});
                pause.setPosition({LOGICAL_W / 2.0f, LOGICAL_H / 2.0f});
                m_window.draw(pause);
            }
            break;
    }

    m_window.display();
}

// ============================================================================
//  Persistencia do recorde
// ============================================================================
void Game::loadHighScore()
{
    std::ifstream file(config::HIGHSCORE_FILE);
    if (!(file >> m_highScore))
        m_highScore = 0;
}

void Game::saveHighScore()
{
    std::ofstream file(config::HIGHSCORE_FILE);
    file << m_highScore;
}
