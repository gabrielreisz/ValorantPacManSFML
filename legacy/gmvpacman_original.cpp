#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cctype>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <SFML/Window.hpp>
#include <random>

using namespace std;
// Código base para jogo do Pac-Man usando SFML
// Mapa desenhado:          André Gustavo     15/06/23
// Movimentos Pac-Man:      André Gustavo     15/06/23
// Movimento contínuo:      André Gustavo     16/06/23
// Colisão com paredes:     Gabriel Reis      10/06/25
// Intenção de movimento:   Gabriel Reis      10/06/25

const int SIZE = 39; // Tamanho de cada célula do mapa

// struct para ficar melhor organiado
// se tiverem com duvida em struct me avisa que ajudo (gabriel)
// to pensando em passar mais coisa pra struct pra ficar mais "clean code"
struct Coordenada
{
    int x;
    int y;
};

const int largura = 27;
const int altura = 26;
const int tamanhoMaxFila = largura * altura;

char mapa[altura][largura + 1] = {
    "111111111111111111111111111",
    "1xxxxxxxxxxxxxxxxxxxxxxxxx1",
    "1x1x1111111111111111111x1x1",
    "1xxxxxxxxxxxxxxxxxxxxxxxxx1",
    "1x1x1x1111x11x11x1111x1x1x1",
    "1x1x1xxxx1x11x11x1xxxx1x1x1",
    "1x1x1111x1x11x11x1x1111x1x1",
    "1x1xxxxxxxx11x11xxxxxxxx1x1",
    "1x1x111x11111x11111x111x1x1",
    "1x1x111xxxxxxxxxxxxx111x1x1",
    "1x1x111x11111111111x111x1x1",
    "1xxx111x11111111111x111xxx1",
    "1x1x111x11111111111x111x1x1",
    "1x1x211x11111111111x112x1x1",
    "1x1x111x11111111111x111x1x1",
    "1xxx111x11111111111x111xxx1",
    "1x1x111xxxxxxXxxxxxx111x1x1",
    "1x1x111x11111x11111x111x1x1",
    "1x1xxxxxxxx11x11xxxxxxxx1x1",
    "1x1x1111x1x11x11x1x1111x1x1",
    "1x1x1xxxx1x11x11x1xxxx1x1x1",
    "1x1x1x1111x11x11x1111x1x1x1",
    "1xxxxxxxxxxxxxxxxxxxxxxxxx1",
    "1x1x1111111111111111111x1x1",
    "1xxxxxxxxxxxxxxxxxxxxxxxxx1",
    "111111111111111111111111111"};

char mapa_original[altura][largura + 1] = {
    "111111111111111111111111111",
    "1xxxxxxxxxxxxxxxxxxxxxxxxx1",
    "1x1x1111111111111111111x1x1",
    "1xxxxxxxxxxxxxxxxxxxxxxxxx1",
    "1x1x1x1111x11x11x1111x1x1x1",
    "1x1x1xxxx1x11x11x1xxxx1x1x1",
    "1x1x1111x1x11x11x1x1111x1x1",
    "1x1xxxxxxxx11x11xxxxxxxx1x1",
    "1x1x111x11111x11111x111x1x1",
    "1x1x111xxxxxxxxxxxxx111x1x1",
    "1x1x111x11111111111x111x1x1",
    "1xxx111x11111111111x111xxx1",
    "1x1x111x11111111111x111x1x1",
    "1x1x211x11111111111x112x1x1",
    "1x1x111x11111111111x111x1x1",
    "1xxx111x11111111111x111xxx1",
    "1x1x111xxxxxxXxxxxxx111x1x1",
    "1x1x111x11111x11111x111x1x1",
    "1x1xxxxxxxx11x11xxxxxxxx1x1",
    "1x1x1111x1x11x11x1x1111x1x1",
    "1x1x1xxxx1x11x11x1xxxx1x1x1",
    "1x1x1x1111x11x11x1111x1x1x1",
    "1xxxxxxxxxxxxxxxxxxxxxxxxx1",
    "1x1x1111111111111111111x1x1",
    "1xxxxxxxxxxxxxxxxxxxxxxxxx1",
    "111111111111111111111111111"};

int posx = 13; // posicao do PacMan
int posy = 16;

enum EstadoJogo
{
    MENU_PRINCIPAL,
    JOGANDO,
    VITORIA,
    GAME_OVER,
    CREDITOS,
    PRONTO
};

// direcao de movimento ATUAL do PacMan
bool cima = false, baixo = false, esq = false, dir = true;

// vetor para mudanca de texturas
bool textura[3] = {true, false, false};

// variaveis inteção de movimento
int intencao_x = 0, intencao_y = 0;
int mov_x = 0, mov_y = 0;

int posfx = 1; // posição inicial do fantasma 1
int posfy = 1;

int posfx2 = 25; // posição inicial do fantasma 2
int posfy2 = 24;

int posfx3 = 25; // posição inicial do fantasma 3
int posfy3 = 1;

int posfx4 = 1; // posição inicial do fantasma 4
int posfy4 = 24;

// clocks do fantasmas
sf::Clock clockFantasmas;
sf::Clock clockFantasmas2;
sf::Clock clockFantasmas3;
sf::Clock clockFantasmas4;

enum Direcao { CIMA, BAIXO, ESQUERDA, DIREITA, NENHUMA };

// clock para mudanca de testuras
sf::Clock text;

// posicao atual do pacman
sf::Vector2f pacmanPosicaoatual(posx *SIZE, posy *SIZE);

sf::Vector2f fantasma1PosicaoPixel;
sf::Vector2f fantasma2PosicaoPixel;
sf::Vector2f fantasma3PosicaoPixel;
sf::Vector2f fantasma4PosicaoPixel;

// posicao que esta indo
sf::Vector2f pacmanDestino;

// verificar se esta em movimento
bool estaSeMovendo = false;

const float VELOCIDADE_PACMAN = 250.0f;
const float VELOCIDADE_FANTASMA = 220.0f;

// (testando) Variaveis para transporte na corda1
bool emTransporte = false;
sf::Vector2f posPixelAtual;
sf::Vector2f posPixelDestino;
// NOTA: A velocidade 40.0 é muito lenta, aumentei para 400.0f para um efeito visível. Ajuste se necessário.
const float VELOCIDADE_TRANSPORTE = 800.0f;
// verificar se ja estava na corda1 (pra nn ficar no loop de ir e voltar depois que pega a corda1)
bool podeTeleportar = true;

// -> (gabriel): tentando fazer o "pacman" (Jett) ficar olhando pro lado que ela ta andando
// o springsheet é uma png como as imagens da jett para cada lado
// IntRect é como se fosse uma função (na verdade é uma classe do SFML)
// Os valores (x, y, z, w) é respectivamente : esquerda, topo, largura e altura
// Apenas por conhecimento tem sf::Rect e sf::floatRect porem como os pixels sao sempre interios, melhor usar int
sf::IntRect GetPacManFrame(bool cima, bool baixo, bool esq, bool dir)
{
    if (dir)
        return sf::IntRect(653, 77, 268, 423); // Jett virada para direita
    if (esq)
        return sf::IntRect(671, 524, 279, 435); // Jett virada para esquerda
    if (cima)
        return sf::IntRect(368, 519, 271, 441); // Jett virada para cima
    if (baixo)
        return sf::IntRect(36, 24, 327, 495); // Jett virada para baixo
    return sf::IntRect(653, 77, 268, 423);    // o default (como nasce)
}

// Pega o frame correto dos fantasmas
Direcao direcaoFantasma1 = NENHUMA;
Direcao direcaoFantasma2 = NENHUMA;
Direcao direcaoFantasma3 = BAIXO;
Direcao direcaoFantasma4 = CIMA;

sf::IntRect GetGhostFrame(int direcaoFantasma)
{

    if (direcaoFantasma == DIREITA)
        return sf::IntRect(72, 360, 240, 310); // Fantasma virado para direita
    if (direcaoFantasma == ESQUERDA)
        return sf::IntRect(352, 361, 272, 310); // Fantasma virado para esquerda
    if (direcaoFantasma == CIMA)
        return sf::IntRect(61, 29, 242, 316); // Fantasma virado para cima
    if (direcaoFantasma == BAIXO)
        return sf::IntRect(358, 35, 259, 316); // Fantasma virado para baixo
    return sf::IntRect(358, 35, 259, 316);
}

int pontos = 0; // contador de pontos;
bool vitoria = false;
int vida = 3;

sf::Clock relogioPacman;

void resetarPosicoes()
{
    posx = 13;
    posy = 16;
    pacmanPosicaoatual = sf::Vector2f(posx * SIZE, posy * SIZE);
    estaSeMovendo = false;
    intencao_x = 0;
    intencao_y = 0;
    mov_x = 0;
    mov_y = 0;
    dir = true;
    esq = cima = baixo = false;

    posfx = 1;
    posfy = 1;
    posfx2 = 25;
    posfy2 = 24;
    posfx3 = 25;
    posfy3 = 1;
    posfx4 = 1;
    posfy4 = 24;

    fantasma1PosicaoPixel = sf::Vector2f(posfx * SIZE, posfy * SIZE);
    fantasma2PosicaoPixel = sf::Vector2f(posfx2 * SIZE, posfy2 * SIZE);
    fantasma3PosicaoPixel = sf::Vector2f(posfx3 * SIZE, posfy3 * SIZE);
    fantasma4PosicaoPixel = sf::Vector2f(posfx4 * SIZE, posfy4 * SIZE);
}

void resetarJogo()
{
    for (int i = 0; i < altura; ++i)
    {
        for (int j = 0; j < largura + 1; ++j)
        {
            mapa[i][j] = mapa_original[i][j];
        }
    }

    resetarPosicoes();

    textura[0] = true;
    textura[1] = false;
    textura[2] = false;
    emTransporte = false;
    podeTeleportar = true;
    pontos = 0;
    vida = 3;
    relogioPacman.restart();
    text.restart();
    clockFantasmas.restart();
    clockFantasmas2.restart();
    clockFantasmas3.restart();
    clockFantasmas4.restart();
}

bool checarVitoria()
{
    for (int i = 0; i < altura; ++i)
    {
        for (int j = 0; j < largura; ++j)
        {
            if (mapa[i][j] >= 'a' && mapa[i][j] <= 'z')
            {
                return false; // Encontrou uma pílula, o jogo não acabou
            }
        }
    }
    return true;
    
}

bool checarDerrota()
{
    if (vida <= 0)
        return true;
    else
        return false;
}

int main()
{
    std::random_device rd;
    std::mt19937 gen(rd());

    sf::RenderWindow window(sf::VideoMode(SIZE * 27, SIZE * 26), "Orb Rush");
    window.setFramerateLimit(60); // Limita o FPS para não sobrecarregar a CPU

    EstadoJogo estadoAtual = MENU_PRINCIPAL;

    fantasma1PosicaoPixel = sf::Vector2f(posfx * SIZE, posfy * SIZE);
    fantasma2PosicaoPixel = sf::Vector2f(posfx2 * SIZE, posfy2 * SIZE);
    fantasma3PosicaoPixel = sf::Vector2f(posfx3 * SIZE, posfy3 * SIZE);
    fantasma4PosicaoPixel = sf::Vector2f(posfx4 * SIZE, posfy4 * SIZE);

    //---------------------------------- TEXTURAS ABAIXO ------------------------------
    sf::Texture pacmanSpriteSheet; // sprite pacman
    if (!pacmanSpriteSheet.loadFromFile("spritesheet.png"))
    {
        cout << "Erro: spritesheet.png\n";
        return 1;
    }
    sf::Sprite pacmanSprite(pacmanSpriteSheet);

    sf::Texture textfantasmazul2; // sprite fantasma
    if (!textfantasmazul2.loadFromFile("roboazul2.png"))
    {
        cout << "Erro: roboazul2.png\n";
        return 1;
    }
    sf::Sprite fantasmazul2(textfantasmazul2);

    sf::Texture textfantasmazul1; // sprite fantasma
    if (!textfantasmazul1.loadFromFile("roboazul1.png"))
    {
        cout << "Erro: roboazul1.png\n";
        return 1;
    }
    sf::Sprite fantasmazul1(textfantasmazul1);

    sf::Texture textfantasmazul3; // sprite fantasma
    if (!textfantasmazul3.loadFromFile("roboazul3.png"))
    {
        cout << "Erro: roboazul3.png\n";
        return 1;
    }
    sf::Sprite fantasmazul3(textfantasmazul3);

    sf::Texture textfantasmavermelho1; // sprite fantasma
    if (!textfantasmavermelho1.loadFromFile("robovermelho1.png"))
    {
        cout << "Erro: robovermelho1.png\n";
        return 1;
    }
    sf::Sprite fantasmavermelho1(textfantasmavermelho1);

    sf::Texture textfantasmavermelho2; // sprite fantasma
    if (!textfantasmavermelho2.loadFromFile("robovermelho2.png"))
    {
        cout << "Erro: robovermelho2.png\n";
        return 1;
    }
    sf::Sprite fantasmavermelho2(textfantasmavermelho2);

    sf::Texture textfantasmavermelho3; // sprite fantasma
    if (!textfantasmavermelho3.loadFromFile("robovermelho3.png"))
    {
        cout << "Erro: robovermelho3.png\n";
        return 1;
    }
    sf::Sprite fantasmavermelho3(textfantasmavermelho3);

    sf::Texture textfantasmaverde1; // sprite fantasma
    if (!textfantasmaverde1.loadFromFile("roboverde1.png"))
    {
        cout << "Erro: roboverde1.png\n";
        return 1;
    }
    sf::Sprite fantasmaverde1(textfantasmaverde1);

    sf::Texture textfantasmaverde2; // sprite fantasma
    if (!textfantasmaverde2.loadFromFile("roboverde2.png"))
    {
        cout << "Erro: roboverde2.png\n";
        return 1;
    }
    sf::Sprite fantasmaverde2(textfantasmaverde2);

    sf::Texture textfantasmaverde3; // sprite fantasma
    if (!textfantasmaverde3.loadFromFile("roboverde3.png"))
    {
        cout << "Erro: roboverde3.png\n";
        return 1;
    }
    sf::Sprite fantasmaverde3(textfantasmaverde3);

    sf::Texture textfantasmamarelo1; // sprite fantasma
    if (!textfantasmamarelo1.loadFromFile("roboamarelo1.png"))
    {
        cout << "Erro: roboamarelo1.png\n";
        return 1;
    }
    sf::Sprite fantasmamarelo1(textfantasmamarelo1);

    sf::Texture textfantasmamarelo2; // sprite fantasma
    if (!textfantasmamarelo2.loadFromFile("roboamarelo2.png"))
    {
        cout << "Erro: roboamarelo2.png\n";
        return 1;
    }
    sf::Sprite fantasmamarelo2(textfantasmamarelo2);

    sf::Texture textfantasmamarelo3; // sprite fantasma
    if (!textfantasmamarelo3.loadFromFile("roboamarelo3.png"))
    {
        cout << "Erro: roboamarelo3.png\n";
        return 1;
    }
    sf::Sprite fantasmamarelo3(textfantasmamarelo3);

    sf::Texture pacmanSpriteSheet2; // sprite pacman 2
    if (!pacmanSpriteSheet2.loadFromFile("spritesheet2.png"))
    {
        cout << "Erro: spritesheet2.png\n";
        return 1;
    }
    sf::Sprite pacmanSprite2(pacmanSpriteSheet2);

    sf::Texture pacmanSpriteSheet3; // sprite pacman 3
    if (!pacmanSpriteSheet3.loadFromFile("spritesheet3.png"))
    {
        cout << "Erro: spritesheet3.png\n";
        return 1;
    }
    sf::Sprite pacmanSprite3(pacmanSpriteSheet3);

    sf::Texture back; // sprite background
    if (!back.loadFromFile("fundo3.png"))
    {
        cout << "Erro: fundo3.png\n";
        return 1;
    }
    sf::Sprite spriteFundo(back);

    sf::Texture menuFundoTexture; // sprite background do menu
    if (!menuFundoTexture.loadFromFile("menu.png"))
    {
        cout << "Erro: menu.png\n";
        return 1;
    }
    sf::Sprite spriteMenuFundo(menuFundoTexture);

    sf::Texture back2; // sprite das copas das arvores
    if (!back2.loadFromFile("copas2.png"))
    {
        cout << "Erro: copas.png\n";
        return 1;
    }
    sf::Sprite spriteCopas(back2);
    // carrega uma fonte
    sf::Font font;
    if (!font.loadFromFile("Valorant_Font.ttf"))
    {
        std::cout << "Erro lendo fonte\n";
        return 0;
    }

    sf::Texture heartTexture;
    if (!heartTexture.loadFromFile("heart.png"))
    {
        cout << "Erro: heart.png\n";
        return 1;
    }
    sf::Sprite heartSprite(heartTexture);
    heartSprite.setScale(SIZE / (float)heartTexture.getSize().x, SIZE / (float)heartTexture.getSize().y);

    // textura dos caminhos

    sf::Texture caminhoh; // sprite do caminho horizontal
    if (!caminhoh.loadFromFile("horizontalcheio.png"))
    {
        cout << "Erro: horizontal.png\n";
        return 1;
    }
    sf::Sprite chorizontal(caminhoh);

    sf::Texture caminhov; // sprite do caminho vertical
    if (!caminhov.loadFromFile("verticalcheio.png"))
    {
        cout << "Erro: vertical.jpg\n";
        return 1;
    }
    sf::Sprite cvertical(caminhov);

    sf::Texture canto1; // sprite do canto 1
    if (!canto1.loadFromFile("canto1.jpg"))
    {
        cout << "Erro: canto1.jpg\n";
        return 1;
    }
    sf::Sprite canto1s(canto1);

    sf::Texture canto2; // sprite do canto 2
    if (!canto2.loadFromFile("canto2.jpg"))
    {
        cout << "Erro: canto2.png\n";
        return 1;
    }
    sf::Sprite canto2s(canto2);

    sf::Texture canto3; // sprite do canto 3
    if (!canto3.loadFromFile("canto3.jpg"))
    {
        cout << "Erro: canto3.jpg\n";
        return 1;
    }
    sf::Sprite canto3s(canto3);

    sf::Texture canto4; // sprite do canto 4
    if (!canto4.loadFromFile("canto4.jpg"))
    {
        cout << "Erro: canto4.jpg\n";
        return 1;
    }
    sf::Sprite canto4s(canto4);

    sf::Texture t1; // sprite do t1
    if (!t1.loadFromFile("t1.jpg"))
    {
        cout << "Erro: t1.jpg\n";
        return 1;
    }
    sf::Sprite t1s(t1);

    sf::Texture t2; // sprite do t2
    if (!t2.loadFromFile("t2.jpg"))
    {
        cout << "Erro: t2.jpg\n";
        return 1;
    }
    sf::Sprite t2s(t2);

    sf::Texture t3; // sprite do t3
    if (!t3.loadFromFile("t3.jpg"))
    {
        cout << "Erro: t3.jpg\n";
        return 1;
    }
    sf::Sprite t3s(t3);

    sf::Texture t4; // sprite do t1
    if (!t4.loadFromFile("t4.jpg"))
    {
        cout << "Erro: t4.jpg\n";
        return 1;
    }
    sf::Sprite t4s(t4);

    sf::Texture cruzamento; // sprite do cruzamento
    if (!cruzamento.loadFromFile("cruzamento.jpg"))
    {
        cout << "Erro: cruzamento.jpg\n";
        return 1;
    }
    sf::Sprite cruzamentos(cruzamento);

    sf::Texture cordasprite; // sprite da corda
    if (!cordasprite.loadFromFile("corda.png"))
    {
        std::cout << "Erro: corda.png\n";
        return 0;
    }
    sf::Sprite corda(cordasprite);
    corda.setTexture(cordasprite);
    corda.setScale(
        SIZE / (float)cordasprite.getSize().x,
        SIZE / (float)cordasprite.getSize().y);

    sf::Texture pilulaTexture;
    if (!pilulaTexture.loadFromFile("orb.png"))
    {
        cout << "Erro: orb.png\n";
        return 1;
    }
    sf::Sprite pilulaSprite(pilulaTexture);
    pilulaSprite.setOrigin(pilulaSprite.getLocalBounds().width / 2.0f, pilulaSprite.getLocalBounds().height / 2.0f);
    float escalaX = (float)(SIZE / 1.5) / pilulaSprite.getLocalBounds().width;
    float escalaY = (float)(SIZE / 1.5) / pilulaSprite.getLocalBounds().height;
    pilulaSprite.setScale(escalaX, escalaY);

    // sf::CircleShape fantasma(SIZE / 2.0f); // sprite fantasma (ta um circulo precisamos adicionar uma png bonitinho)
    // fantasma.setFillColor(sf::Color::Green);
    // fantasma.setOrigin(fantasma.getRadius(), fantasma.getRadius());

    sf::CircleShape fantasma2(SIZE / 2.0f); // sprite fantasma 2(ta um circulo precisamos adicionar uma png bonitinho)
    fantasma2.setFillColor(sf::Color::Blue);
    fantasma2.setOrigin(fantasma2.getRadius(), fantasma2.getRadius());

    sf::CircleShape fantasma3(SIZE / 2.0f); // sprite fantasma 3(ta um circulo precisamos adicionar uma png bonitinho)
    fantasma3.setFillColor(sf::Color(80, 0, 128));
    fantasma3.setOrigin(fantasma3.getRadius(), fantasma3.getRadius());

    sf::CircleShape fantasma4(SIZE / 2.0f); // sprite fantasma 4(ta um circulo precisamos adicionar uma png bonitinho)
    fantasma4.setFillColor(sf::Color::Yellow);
    fantasma4.setOrigin(fantasma4.getRadius(), fantasma4.getRadius());

    // Adicione esta parte após carregar a fonte
    sf::Text textoPontuacao;
    textoPontuacao.setFont(font);
    textoPontuacao.setCharacterSize(40); // Tamanho em pixels
    textoPontuacao.setFillColor(sf::Color::Red);
    textoPontuacao.setPosition(60, -6); // Posição no canto superior esquerdo

    sf::Text textoVitoria;
    textoVitoria.setFont(font);
    textoVitoria.setString("VICTORY!");
    textoVitoria.setFillColor(sf::Color(122, 216, 189));

    sf::Text textoPronto;
    textoPronto.setFont(font);
    textoPronto.setString("Pressione qualquer\ntecla para continuar");
    textoPronto.setCharacterSize(33);
    textoPronto.setFillColor(sf::Color::Yellow);

    sf::Text textoDerrota;
    textoDerrota.setFont(font);
    textoDerrota.setString("DEFEAT");
    textoDerrota.setFillColor(sf::Color::Red);
    textoDerrota.setCharacterSize(80);

    sf::Clock relogioVitoria;
    float tamanhoAtual = 0;
    bool animacaoVitoriaCompleta = false;

    sf::Music musicaMenu;
    if (!musicaMenu.openFromFile("menu.mp3"))
    {
        cout << "Erro ao carregar menu.mp3" << endl;
        return 1;
    }
    musicaMenu.setLoop(true);
    musicaMenu.setVolume(50.0f);
    sf::Music musicaInicio;
    if (!musicaInicio.openFromFile("inicio.mp3"))
    {
        cout << "Erro ao carregar inicio.mp3" << endl;
        return 1;
    }

    // Carrega a música de fundo do jogo
    sf::Music musicaJogo;
    if (!musicaJogo.openFromFile("jogo.mp3"))
    {
        cout << "Erro ao carregar jogo.mp3" << endl;
        return 1;
    }
    musicaJogo.setLoop(true); // Faz a música de fundo repetir

    // Carrega o som de vitória sem perder vidas
    sf::SoundBuffer flawlessBuffer;
    if (!flawlessBuffer.loadFromFile("flawless.mp3"))
    {
        cout << "Erro ao carregar flawless.mp3" << endl;
        return 1;
    }
    sf::Sound flawlessSound;
    flawlessSound.setBuffer(flawlessBuffer);

    // Carrega o som de vitória normal
    sf::SoundBuffer victoryBuffer;
    if (!victoryBuffer.loadFromFile("victory.mp3"))
    {
        cout << "Erro ao carregar victory.mp3" << endl;
        return 1;
    }
    sf::Sound victorySound;
    victorySound.setBuffer(victoryBuffer);

    sf::SoundBuffer defeatBuffer;
    if (!defeatBuffer.loadFromFile("defeat.mp3"))
    {
        cout << "Erro ao carregar defeat.mp3" << endl;
        return 1;
    }
    sf::Sound defeatSound;
    defeatSound.setBuffer(defeatBuffer);

    sf::Text textoMenuTitulo, textoJogar, textoCreditos, textoSair;
    textoMenuTitulo.setFont(font);
    textoMenuTitulo.setString("ORB RUSH");
    textoMenuTitulo.setCharacterSize(80);
    textoMenuTitulo.setFillColor(sf::Color(122, 216, 189));

    textoJogar.setFont(font);
    textoJogar.setString("Jogar");
    textoJogar.setCharacterSize(50);
    textoJogar.setFillColor(sf::Color::White);

    textoCreditos.setFont(font);
    textoCreditos.setString("Creditos");
    textoCreditos.setCharacterSize(50);
    textoCreditos.setFillColor(sf::Color::White);

    textoSair.setFont(font);
    textoSair.setString("Sair");
    textoSair.setCharacterSize(50);
    textoSair.setFillColor(sf::Color::White);

    sf::Text textoCreditosConteudo, textoVoltar;
    textoCreditosConteudo.setFont(font);
    textoCreditosConteudo.setString("Desenvolvido por:\n\n\n->Gabriel Costa Reis - 120549\n->Marcos Vinicius Mariano Dias - 120560\n->Victor Alexandre S Ribeiro - 120557\n\nProf. Andre Gustavo dos Santos");
    textoCreditosConteudo.setCharacterSize(40);
    textoCreditosConteudo.setFillColor(sf::Color::White);

    textoVoltar.setFont(font);
    textoVoltar.setString("Voltar");
    textoVoltar.setCharacterSize(50);
    textoVoltar.setFillColor(sf::Color::White);

    sf::Text textoJogarNovamente, textoIrMenu, textoSairGameOver;
    textoJogarNovamente.setFont(font);
    textoJogarNovamente.setString("Jogar Novamente");
    textoJogarNovamente.setCharacterSize(50);
    textoJogarNovamente.setFillColor(sf::Color::White);

    textoIrMenu.setFont(font);
    textoIrMenu.setString("Ir para o Menu");
    textoIrMenu.setCharacterSize(50);
    textoIrMenu.setFillColor(sf::Color::White);

    textoSairGameOver.setFont(font);
    textoSairGameOver.setString("Sair");
    textoSairGameOver.setCharacterSize(50);
    textoSairGameOver.setFillColor(sf::Color::White);

    // Toca os sons iniciais
    // musicaInicio.play(); // movido para o estado de jogo
    // musicaInicio.setVolume(50.0f);

    // Array com os nomes dos sprites

    sf::Sprite sprites[16]; // criação

    sprites[3] = chorizontal;
    sprites[5] = canto1s;
    sprites[6] = canto2s;
    sprites[7] = t1s;
    sprites[9] = canto3s;
    sprites[10] = canto4s;
    sprites[11] = t2s;
    sprites[12] = cvertical;
    sprites[13] = t3s;
    sprites[14] = t4s;
    sprites[15] = cruzamentos;

    // aqui to usando o (novo) struct criado para verificar a posição da corda1
    Coordenada corda1 = {-1, -1}, corda2 = {-1, -1}; // cria as variaveis corda1 e corda2
                                                     // recebe valor negativo pq valores negativos não fazem parte do mapa
    for (int i = 0; i < 26; ++i)
    {
        // como a corda1 na matriz char é um local onde tem '2', quando encontra 2 recebe o local da corda1
        for (int j = 0; j < 27; ++j)
        {
            if (mapa[i][j] == '2')
            {
                if (corda1.x == -1)
                {
                    corda1.x = j;
                    corda1.y = i;
                }
                else
                {
                    corda2.x = j;
                    corda2.y = i;
                }
                /*
                explicando o if/else:
                quando encontra um 2 no mapa, armazena o corda1 com seus valores de x e y
                como ele é iniciado com -1, pra saber se é o primeiro ou segundo (lado) da corda1
                é so verificar se corda1 tem -1, se a corda1 nn tiver salvo -1, significa que ela ja recebeu um valor
                logo é a corda1 2 que achamos
                */
            }
        }
    }
    bool musicaJogoIniciada = false;

    int selectedMenuIndex = 0;
    const int totalMenuItems = 3;

    int selectedGameOverIndex = 0;
    const int totalGameOverItems = 3;

    int selectedVitoriaIndex = 0;
    const int totalVitoriaItems = 2;

    // executa o programa enquanto a janela está aberta
    while (window.isOpen())
    {
        if ((estadoAtual == MENU_PRINCIPAL || estadoAtual == CREDITOS) && musicaMenu.getStatus() != sf::Music::Playing)
        {
            musicaJogo.stop();
            musicaInicio.stop();
            musicaMenu.play();
        }
        else if (estadoAtual != MENU_PRINCIPAL && estadoAtual != CREDITOS && musicaMenu.getStatus() == sf::Music::Playing)
        {
            musicaMenu.stop();
        }

        // Lógica de eventos e atualização de estados
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            switch (estadoAtual)
            {
            case MENU_PRINCIPAL:
                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Up)
                    {
                        selectedMenuIndex = (selectedMenuIndex - 1 + totalMenuItems) % totalMenuItems;
                    }
                    else if (event.key.code == sf::Keyboard::Down)
                    {
                        selectedMenuIndex = (selectedMenuIndex + 1) % totalMenuItems;
                    }
                    else if (event.key.code == sf::Keyboard::Return)
                    {
                        if (selectedMenuIndex == 0)
                        { // Jogar
                            resetarJogo();
                            estadoAtual = PRONTO;
                            musicaInicio.play();
                            musicaInicio.setVolume(50.0f);
                            musicaJogoIniciada = false;
                        }
                        else if (selectedMenuIndex == 1)
                        { // Creditos
                            estadoAtual = CREDITOS;
                        }
                        else if (selectedMenuIndex == 2)
                        { // Sair
                            window.close();
                        }
                    }
                }
                else if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        if (textoJogar.getGlobalBounds().contains(mousePos.x, mousePos.y))
                        {
                            resetarJogo();
                            estadoAtual = PRONTO;
                            musicaInicio.play();
                            musicaInicio.setVolume(50.0f);
                            musicaJogoIniciada = false;
                        }
                        else if (textoCreditos.getGlobalBounds().contains(mousePos.x, mousePos.y))
                        {
                            estadoAtual = CREDITOS;
                        }
                        else if (textoSair.getGlobalBounds().contains(mousePos.x, mousePos.y))
                        {
                            window.close();
                        }
                    }
                }
                break;
            case GAME_OVER:
                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Up)
                    {
                        selectedGameOverIndex = (selectedGameOverIndex - 1 + totalGameOverItems) % totalGameOverItems;
                    }
                    else if (event.key.code == sf::Keyboard::Down)
                    {
                        selectedGameOverIndex = (selectedGameOverIndex + 1) % totalGameOverItems;
                    }
                    else if (event.key.code == sf::Keyboard::Return)
                    {
                        if (selectedGameOverIndex == 0)
                        { // Jogar Novamente
                            resetarJogo();
                            estadoAtual = PRONTO;
                            musicaInicio.play();
                            musicaInicio.setVolume(50.0f);
                            musicaJogoIniciada = false;
                        }
                        else if (selectedGameOverIndex == 1)
                        { // Ir para o Menu
                            estadoAtual = MENU_PRINCIPAL;
                        }
                        else if (selectedGameOverIndex == 2)
                        { // Sair
                            window.close();
                        }
                    }
                }
                else if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        if (textoJogarNovamente.getGlobalBounds().contains(mousePos.x, mousePos.y))
                        {
                            resetarJogo();
                            estadoAtual = PRONTO;
                            musicaInicio.play();
                            musicaInicio.setVolume(50.0f);
                            musicaJogoIniciada = false;
                        }
                        else if (textoIrMenu.getGlobalBounds().contains(mousePos.x, mousePos.y))
                        {
                            estadoAtual = MENU_PRINCIPAL;
                        }
                        else if (textoSairGameOver.getGlobalBounds().contains(mousePos.x, mousePos.y))
                        {
                            window.close();
                        }
                    }
                }
                break;
            case CREDITOS:
                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Escape)
                    {
                        estadoAtual = MENU_PRINCIPAL;
                    }
                }
                else if (event.type == sf::Event::MouseButtonPressed)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (textoVoltar.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        estadoAtual = MENU_PRINCIPAL;
                    }
                }
                break;
            case PRONTO:
                if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
                {
                    estadoAtual = JOGANDO;
                    relogioPacman.restart();
                }
                break;
            case JOGANDO:
                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Left)
                    {
                        intencao_x = -1;
                        intencao_y = 0;
                    }
                    else if (event.key.code == sf::Keyboard::Right)
                    {
                        intencao_x = 1;
                        intencao_y = 0;
                    }
                    else if (event.key.code == sf::Keyboard::Up)
                    {
                        intencao_y = -1;
                        intencao_x = 0;
                    }
                    else if (event.key.code == sf::Keyboard::Down)
                    {
                        intencao_y = 1;
                        intencao_x = 0;
                    }
                }
                break;
            case VITORIA:
                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Up)
                    {
                        selectedVitoriaIndex = (selectedVitoriaIndex - 1 + totalVitoriaItems) % totalVitoriaItems;
                    }
                    else if (event.key.code == sf::Keyboard::Down)
                    {
                        selectedVitoriaIndex = (selectedVitoriaIndex + 1) % totalVitoriaItems;
                    }
                    else if (event.key.code == sf::Keyboard::Return)
                    {
                        if (selectedVitoriaIndex == 0)
                        { // Jogar Novamente
                            resetarJogo();
                            estadoAtual = PRONTO;
                            musicaInicio.play();
                            musicaInicio.setVolume(50.0f);
                            musicaJogoIniciada = false;
                        }
                        else if (selectedVitoriaIndex == 1)
                        { // Ir para o Menu
                            estadoAtual = MENU_PRINCIPAL;
                        }
                    }
                }
                else if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        if (textoJogarNovamente.getGlobalBounds().contains(mousePos.x, mousePos.y))
                        {
                            resetarJogo();
                            estadoAtual = PRONTO;
                            musicaInicio.play();
                            musicaInicio.setVolume(50.0f);
                            musicaJogoIniciada = false;
                        }
                        else if (textoIrMenu.getGlobalBounds().contains(mousePos.x, mousePos.y))
                        {
                            estadoAtual = MENU_PRINCIPAL;
                        }
                    }
                }
                break;
            default:
                break;
            }
        }

        // Lógica de atualização de Jogo
        switch (estadoAtual)
        {
        case JOGANDO:
        {
            if (musicaInicio.getStatus() == sf::Sound::Stopped && !musicaJogoIniciada)
            {
                musicaJogo.play();
                musicaJogo.setVolume(50.0f);
                musicaJogoIniciada = true; // Marca que a música já começou
            }

            float reinicioRelogio = relogioPacman.restart().asSeconds();

            if (estaSeMovendo)
            {
                // calculando o vetor de direcao
                sf::Vector2f vetorDirecao = pacmanDestino - pacmanPosicaoatual;

                // distancia entre dois pontos
                float distancia = std::sqrt(vetorDirecao.x * vetorDirecao.x + vetorDirecao.y * vetorDirecao.y);

                // calculo do movimento usando o clock
                float velocidadeAtual = emTransporte ? VELOCIDADE_PACMAN * 2.5f : VELOCIDADE_PACMAN;
                float movimentoNesteFrame = velocidadeAtual * reinicioRelogio;

                // se a distancia > que o movimento que deseja realizar
                if (distancia > movimentoNesteFrame)
                {
                    // "concerta" o movimento
                    pacmanPosicaoatual += (vetorDirecao / distancia) * movimentoNesteFrame;
                }
                else
                // chegada onde deseja
                {
                    pacmanPosicaoatual = pacmanDestino;
                    bool estavaEmTransporte = emTransporte;

                    if (estavaEmTransporte)
                    {
                        emTransporte = false;
                    }

                    if (mapa[posy][posx] >= 'a' && mapa[posy][posx] <= 'z')
                    {
                        mapa[posy][posx] = toupper(mapa[posy][posx]);
                        pontos++;
                        if (checarVitoria())
                        {
                            musicaJogo.stop();
                            if (vida == 3)
                            { // Se não perdeu vida, toca o som "flawless"
                                flawlessSound.play();
                                textoVitoria.setString("Flawless!");
                            }
                            else
                            {
                                victorySound.play();
                                textoVitoria.setString("VICTORY!");
                            }
                            estadoAtual = VITORIA;
                        }
                    }

                    if (mapa[posy][posx] == '2' && !estavaEmTransporte)
                    {
                        emTransporte = true;

                        if (posx == corda1.x && posy == corda1.y)
                        {
                            posx = corda2.x;
                            posy = corda2.y;
                        }
                        else
                        {
                            posx = corda1.x;
                            posy = corda1.y;
                        }
                        pacmanDestino = sf::Vector2f(posx * SIZE, posy * SIZE);
                    }
                    else
                    {
                        estaSeMovendo = false;
                    }
                }
            }
            else // se nao esta se movendo
            {
                if ((intencao_x != 0 || intencao_y != 0) && mapa[posy + intencao_y][posx + intencao_x] != '1')
                {
                    mov_x = intencao_x;
                    mov_y = intencao_y;
                }

                if ((mov_x != 0 || mov_y != 0) && mapa[posy + mov_y][posx + mov_x] != '1')
                {
                    estaSeMovendo = true;

                    dir = (mov_x == 1);
                    esq = (mov_x == -1);
                    baixo = (mov_y == 1);
                    cima = (mov_y == -1);

                    posx += mov_x;
                    posy += mov_y;

                    pacmanDestino = sf::Vector2f(posx * SIZE, posy * SIZE);
                }
            }
            
            sf::Vector2f destinoFantasma1 = sf::Vector2f(posfx * SIZE, posfy * SIZE);
            sf::Vector2f vetorDirecaoFantasma1 = destinoFantasma1 - fantasma1PosicaoPixel;
            float distanciaFantasma1 = std::sqrt(vetorDirecaoFantasma1.x * vetorDirecaoFantasma1.x + vetorDirecaoFantasma1.y * vetorDirecaoFantasma1.y);
            float movimentoFantasmaFrame1 = (VELOCIDADE_FANTASMA + 30.0f) * reinicioRelogio;

            if (distanciaFantasma1 > movimentoFantasmaFrame1) {
                fantasma1PosicaoPixel += (vetorDirecaoFantasma1 / distanciaFantasma1) * movimentoFantasmaFrame1;
            } else {
                fantasma1PosicaoPixel = destinoFantasma1;
            }

            sf::Vector2f destinoFantasma2 = sf::Vector2f(posfx2 * SIZE, posfy2 * SIZE);
            sf::Vector2f vetorDirecaoFantasma2 = destinoFantasma2 - fantasma2PosicaoPixel;
            float distanciaFantasma2 = std::sqrt(vetorDirecaoFantasma2.x * vetorDirecaoFantasma2.x + vetorDirecaoFantasma2.y * vetorDirecaoFantasma2.y);
            float movimentoFantasmaFrame2 = VELOCIDADE_FANTASMA * reinicioRelogio;

            if (distanciaFantasma2 > movimentoFantasmaFrame2) {
                fantasma2PosicaoPixel += (vetorDirecaoFantasma2 / distanciaFantasma2) * movimentoFantasmaFrame2;
            } else {
                fantasma2PosicaoPixel = destinoFantasma2;
            }

            sf::Vector2f destinoFantasma3 = sf::Vector2f(posfx3 * SIZE, posfy3 * SIZE);
            sf::Vector2f vetorDirecaoFantasma3 = destinoFantasma3 - fantasma3PosicaoPixel;
            float distanciaFantasma3 = std::sqrt(vetorDirecaoFantasma3.x * vetorDirecaoFantasma3.x + vetorDirecaoFantasma3.y * vetorDirecaoFantasma3.y);
            float movimentoFantasmaFrame3 = VELOCIDADE_FANTASMA * reinicioRelogio;

            if (distanciaFantasma3 > movimentoFantasmaFrame3) {
                fantasma3PosicaoPixel += (vetorDirecaoFantasma3 / distanciaFantasma3) * movimentoFantasmaFrame3;
            } else {
                fantasma3PosicaoPixel = destinoFantasma3;
            }

            sf::Vector2f destinoFantasma4 = sf::Vector2f(posfx4 * SIZE, posfy4 * SIZE);
            sf::Vector2f vetorDirecaoFantasma4 = destinoFantasma4 - fantasma4PosicaoPixel;
            float distanciaFantasma4 = std::sqrt(vetorDirecaoFantasma4.x * vetorDirecaoFantasma4.x + vetorDirecaoFantasma4.y * vetorDirecaoFantasma4.y);
            float movimentoFantasmaFrame4 = VELOCIDADE_FANTASMA * reinicioRelogio;

            if (distanciaFantasma4 > movimentoFantasmaFrame4) {
                fantasma4PosicaoPixel += (vetorDirecaoFantasma4 / distanciaFantasma4) * movimentoFantasmaFrame4;
            } else {
                fantasma4PosicaoPixel = destinoFantasma4;
            }

            // movimentação do fantasma 1:verde -> persegue mais rapido

            if (clockFantasmas.getElapsedTime() > sf::seconds(0.25f))
            {
                bool encontrou = false;
                Coordenada pai[altura][largura];

                // Arrays para fila manual
                Coordenada fila[tamanhoMaxFila];
                int inicio = 0, fim = 0;

                bool visitado[altura][largura] = {false};

                // Insere a posição inicial do fantasma na fila
                fila[fim++] = {posfy, posfx};
                visitado[posfy][posfx] = true;

                while (inicio < fim && !encontrou)
                {
                    Coordenada atual = fila[inicio++];

                    // Direções: cima, baixo, esquerda, direita
                    int dx[4] = {0, 0, -1, 1};
                    int dy[4] = {-1, 1, 0, 0};

                    for (int d = 0; d < 4; d++)
                    {
                        int nx = atual.x + dx[d];
                        int ny = atual.y + dy[d];

                        // Verifica se está dentro dos limites
                        if (nx >= 0 && nx < altura && ny >= 0 && ny < largura)
                        {
                            // Verifica se não é parede e não foi visitado
                            if (mapa[nx][ny] != '1' && !visitado[nx][ny])
                            {
                                visitado[nx][ny] = true;
                                pai[nx][ny] = atual;
                                fila[fim++] = {nx, ny};

                                // Verifica se achou o Pac-Man
                                if (nx == posy && ny == posx)
                                {
                                    encontrou = true;
                                    break;
                                }
                            }
                        }
                    }
                }

                // Se encontrou, reconstrói o caminho
                if (encontrou)
                {
                    Coordenada atual = {posy, posx};
                    Coordenada anterior;

                    while (true)
                    {
                        anterior = pai[atual.x][atual.y];
                        if (anterior.x == posfy && anterior.y == posfx)
                        {
                            // Calcula diferenças
                            int deltaX = atual.y - anterior.y; // coluna (x)
                            int deltaY = atual.x - anterior.x; // linha (y)

                            // Determina direção baseada no movimento
                            if (deltaX == 1)
                            {
                                direcaoFantasma1 = DIREITA;
                            }
                            else if (deltaX == -1)
                            {
                                direcaoFantasma1 = ESQUERDA;
                            }
                            else if (deltaY == 1)
                            {
                                direcaoFantasma1 = BAIXO;
                            }
                            else if (deltaY == -1)
                            {
                                direcaoFantasma1 = CIMA;
                            }

                            // Atualiza posição do fantasma
                            posfx = atual.y;
                            posfy = atual.x;
                            break;
                        }
                        atual = anterior;
                    }
                }
                clockFantasmas.restart();
            }

            // movimentação do fantasma 2:azul -> persegue
            if (clockFantasmas2.getElapsedTime() > sf::seconds(0.3f))
            {
                // fantasma 2
                bool encontrou2 = false;
                Coordenada pai2[altura][largura];

                // Arrays para fila manual
                Coordenada fila2[tamanhoMaxFila];
                int inicio2 = 0, fim2 = 0;

                bool visitado2[altura][largura] = {false};

                // Insere a posição inicial do fantasma na fila
                fila2[fim2++] = {posfy2, posfx2};
                visitado2[posfy2][posfx2] = true;

                while (inicio2 < fim2 && !encontrou2)
                {
                    Coordenada atual2 = fila2[inicio2++];

                    // Direções: cima, baixo, esquerda, direita
                    int dx2[4] = {0, 0, -1, 1};
                    int dy2[4] = {-1, 1, 0, 0};

                    for (int d = 0; d < 4; d++)
                    {
                        int nx2 = atual2.x + dx2[d];
                        int ny2 = atual2.y + dy2[d];

                        // Verifica se está dentro dos limites
                        if (nx2 >= 0 && nx2 < altura && ny2 >= 0 && ny2 < largura)
                        {
                            // Verifica se não é parede e não foi visitado
                            if (mapa[nx2][ny2] != '1' && !visitado2[nx2][ny2])
                            {
                                visitado2[nx2][ny2] = true;
                                pai2[nx2][ny2] = atual2;
                                fila2[fim2++] = {nx2, ny2};

                                // Verifica se achou o Pac-Man
                                if (nx2 == posy && ny2 == posx)
                                {
                                    encontrou2 = true;
                                    break;
                                }
                            }
                        }
                    }
                }

                // Se encontrou, reconstrói o caminho
                if (encontrou2)
                {
                    Coordenada atual = {posy, posx};
                    Coordenada anterior;

                    while (true)
                    {
                        anterior = pai2[atual.x][atual.y];
                        // CORREÇÃO 1: Verificar usando a posição do fantasma2 (posfy2, posfx2)
                        if (anterior.x == posfy2 && anterior.y == posfx2)
                        {
                            // Calcula diferenças
                            int deltaX = atual.y - anterior.y; // coluna (x)
                            int deltaY = atual.x - anterior.x; // linha (y)

                            // Determina direção baseada no movimento
                            if (deltaX == 1)
                            {
                                // CORREÇÃO 2: Atualizar direcaoFantasma2
                                direcaoFantasma2 = DIREITA;
                            }
                            else if (deltaX == -1)
                            {
                                direcaoFantasma2 = ESQUERDA;
                            }
                            else if (deltaY == 1)
                            {
                                direcaoFantasma2 = BAIXO;
                            }
                            else if (deltaY == -1)
                            {
                                direcaoFantasma2 = CIMA;
                            }

                            // CORREÇÃO 3: Atualizar a posição do fantasma2 (posfx2, posfy2)
                            posfx2 = atual.y;
                            posfy2 = atual.x;
                            break;
                        }
                        atual = anterior;
                    }
                }
                clockFantasmas2.restart();
            }

            // Movimentação do fantasma 3:roxo -> aleatório

            if (clockFantasmas3.getElapsedTime() > sf::seconds(0.3f))
            {
                // Variáveis para direções possíveis
                Direcao direcoesPossiveis[4]; // Agora com 4 posições para incluir todas possibilidades
                int numDirecoes = 0;
                Direcao direcaoOposta = NENHUMA;

                // Determina a direção oposta à atual
                switch (direcaoFantasma3)
                {
                case CIMA:
                    direcaoOposta = BAIXO;
                    break;
                case BAIXO:
                    direcaoOposta = CIMA;
                    break;
                case ESQUERDA:
                    direcaoOposta = DIREITA;
                    break;
                case DIREITA:
                    direcaoOposta = ESQUERDA;
                    break;
                }

                // Verifica todas as direções possíveis (incluindo a oposta se necessário)
                if (posfy3 > 0 && mapa[posfy3 - 1][posfx3] != '1')
                {
                    direcoesPossiveis[numDirecoes++] = CIMA;
                }
                if (posfy3 < altura - 1 && mapa[posfy3 + 1][posfx3] != '1')
                {
                    direcoesPossiveis[numDirecoes++] = BAIXO;
                }
                if (posfx3 > 0 && mapa[posfy3][posfx3 - 1] != '1')
                {
                    direcoesPossiveis[numDirecoes++] = ESQUERDA;
                }
                if (posfx3 < largura - 1 && mapa[posfy3][posfx3 + 1] != '1')
                {
                    direcoesPossiveis[numDirecoes++] = DIREITA;
                }

                // Remove a direção oposta se houver outras opções
                if (numDirecoes > 1)
                {
                    for (int i = 0; i < numDirecoes; i++)
                    {
                        if (direcoesPossiveis[i] == direcaoOposta)
                        {
                            // Troca com o último elemento e decrementa o contador
                            direcoesPossiveis[i] = direcoesPossiveis[numDirecoes - 1];
                            numDirecoes--;
                            break;
                        }
                    }
                }

                // Escolhe aleatoriamente entre as direções possíveis
                if (numDirecoes > 0)
                {
                    std::uniform_int_distribution<> distrib(0, numDirecoes - 1);
                    int escolha = distrib(gen);
                    direcaoFantasma3 = direcoesPossiveis[escolha];

                    // Executa o movimento com verificação final
                    switch (direcaoFantasma3)
                    {
                    case CIMA:
                        if (mapa[posfy3 - 1][posfx3] != '1')
                            posfy3--;
                        break;
                    case BAIXO:
                        if (mapa[posfy3 + 1][posfx3] != '1')
                            posfy3++;
                        break;
                    case ESQUERDA:
                        if (mapa[posfy3][posfx3 - 1] != '1')
                            posfx3--;
                        break;
                    case DIREITA:
                        if (mapa[posfy3][posfx3 + 1] != '1')
                            posfx3++;
                        break;
                    }
                }
                clockFantasmas3.restart();
            }

            // Movimentação do fantasma 4:amarelo -> aleatório

            if (clockFantasmas4.getElapsedTime() > sf::seconds(0.3f))
            {
                // Variáveis para direções possíveis
                Direcao direcoesPossiveis[4];
                int numDirecoes = 0;
                Direcao direcaoOposta = NENHUMA;

                // Determina a direção oposta à atual
                switch (direcaoFantasma4)
                {
                case CIMA:
                    direcaoOposta = BAIXO;
                    break;
                case BAIXO:
                    direcaoOposta = CIMA;
                    break;
                case ESQUERDA:
                    direcaoOposta = DIREITA;
                    break;
                case DIREITA:
                    direcaoOposta = ESQUERDA;
                    break;
                }

                // Verifica todas as direções possíveis
                if (posfy4 > 0 && mapa[posfy4 - 1][posfx4] != '1')
                {
                    direcoesPossiveis[numDirecoes++] = CIMA;
                }
                if (posfy4 < altura - 1 && mapa[posfy4 + 1][posfx4] != '1')
                {
                    direcoesPossiveis[numDirecoes++] = BAIXO;
                }
                if (posfx4 > 0 && mapa[posfy4][posfx4 - 1] != '1')
                {
                    direcoesPossiveis[numDirecoes++] = ESQUERDA;
                }
                if (posfx4 < largura - 1 && mapa[posfy4][posfx4 + 1] != '1')
                {
                    direcoesPossiveis[numDirecoes++] = DIREITA;
                }

                // Remove a direção oposta se houver outras opções
                if (numDirecoes > 1)
                {
                    for (int i = 0; i < numDirecoes; i++)
                    {
                        if (direcoesPossiveis[i] == direcaoOposta)
                        {
                            direcoesPossiveis[i] = direcoesPossiveis[numDirecoes - 1];
                            numDirecoes--;
                            break;
                        }
                    }
                }

                // Escolhe aleatoriamente entre as direções possíveis
                if (numDirecoes > 0)
                {
                    std::uniform_int_distribution<> distrib(0, numDirecoes - 1);
                    int escolha = distrib(gen);
                    direcaoFantasma4 = direcoesPossiveis[escolha];

                    // Executa o movimento com verificação final
                    switch (direcaoFantasma4)
                    {
                    case CIMA:
                        if (mapa[posfy4 - 1][posfx4] != '1')
                            posfy4--;
                        break;
                    case BAIXO:
                        if (mapa[posfy4 + 1][posfx4] != '1')
                            posfy4++;
                        break;
                    case ESQUERDA:
                        if (mapa[posfy4][posfx4 - 1] != '1')
                            posfx4--;
                        break;
                    case DIREITA:
                        if (mapa[posfy4][posfx4 + 1] != '1')
                            posfx4++;
                        break;
                    }
                }
                clockFantasmas4.restart();
            }

            if (posfx == posx && posfy == posy)
            {
                vida--;
                if (checarDerrota())
                {
                    musicaJogo.stop();
                    defeatSound.play();
                    estadoAtual = GAME_OVER;
                }
                else
                {
                    resetarPosicoes();
                    estadoAtual = PRONTO;
                }
            }
            if (posfx2 == posx && posfy2 == posy)
            {
                vida--;
                if (checarDerrota())
                {
                    musicaJogo.stop();
                    defeatSound.play();
                    estadoAtual = GAME_OVER;
                }
                else
                {
                    resetarPosicoes();
                    estadoAtual = PRONTO;
                }
            }
            if (posfx3 == posx && posfy3 == posy)
            {
                vida--;
                if (checarDerrota())
                {
                    musicaJogo.stop();
                    defeatSound.play();
                    estadoAtual = GAME_OVER;
                }
                else
                {
                    resetarPosicoes();
                    estadoAtual = PRONTO;
                }
            }
            if (posfx4 == posx && posfy4 == posy)
            {
                vida--;
                if (checarDerrota())
                {
                    musicaJogo.stop();
                    defeatSound.play();
                    estadoAtual = GAME_OVER;
                }
                else
                {
                    resetarPosicoes();
                    estadoAtual = PRONTO;
                }
            }

            break;
        } // Fim do case JOGANDO
        default:
            break;
        }

        // Seção de Desenho
        window.clear();

        if (estadoAtual == MENU_PRINCIPAL)
        {
            spriteMenuFundo.setScale((float)window.getSize().x / spriteMenuFundo.getTexture()->getSize().x, (float)window.getSize().y / spriteMenuFundo.getTexture()->getSize().y);
            window.draw(spriteMenuFundo);

            textoMenuTitulo.setPosition(window.getSize().x / 2.0f - textoMenuTitulo.getGlobalBounds().width / 2.0f, 150);
            textoJogar.setPosition(window.getSize().x / 2.0f - textoJogar.getGlobalBounds().width / 2.0f, 350);
            textoCreditos.setPosition(window.getSize().x / 2.0f - textoCreditos.getGlobalBounds().width / 2.0f, 450);
            textoSair.setPosition(window.getSize().x / 2.0f - textoSair.getGlobalBounds().width / 2.0f, 550);

            textoJogar.setFillColor(selectedMenuIndex == 0 ? sf::Color::Yellow : sf::Color::White);
            textoCreditos.setFillColor(selectedMenuIndex == 1 ? sf::Color::Yellow : sf::Color::White);
            textoSair.setFillColor(selectedMenuIndex == 2 ? sf::Color::Yellow : sf::Color::White);

            window.draw(textoMenuTitulo);
            window.draw(textoJogar);
            window.draw(textoCreditos);
            window.draw(textoSair);
        }
        else if (estadoAtual == CREDITOS)
        {
            spriteMenuFundo.setScale((float)window.getSize().x / spriteMenuFundo.getTexture()->getSize().x, (float)window.getSize().y / spriteMenuFundo.getTexture()->getSize().y);
            window.draw(spriteMenuFundo);

            textoCreditosConteudo.setPosition(window.getSize().x / 2.0f - textoCreditosConteudo.getGlobalBounds().width / 2.0f, 100);
            textoVoltar.setPosition(window.getSize().x / 2.0f - textoVoltar.getGlobalBounds().width / 2.0f, 600);

            textoVoltar.setFillColor(sf::Color::Yellow);

            window.draw(textoCreditosConteudo);
            window.draw(textoVoltar);
        }
        else if (estadoAtual == GAME_OVER)
        {
            spriteMenuFundo.setScale((float)window.getSize().x / spriteMenuFundo.getTexture()->getSize().x, (float)window.getSize().y / spriteMenuFundo.getTexture()->getSize().y);
            window.draw(spriteMenuFundo);

            textoDerrota.setPosition(window.getSize().x / 2.0f - textoDerrota.getGlobalBounds().width / 2.0f, 150);
            textoJogarNovamente.setPosition(window.getSize().x / 2.0f - textoJogarNovamente.getGlobalBounds().width / 2.0f, 350);
            textoIrMenu.setPosition(window.getSize().x / 2.0f - textoIrMenu.getGlobalBounds().width / 2.0f, 450);
            textoSairGameOver.setPosition(window.getSize().x / 2.0f - textoSairGameOver.getGlobalBounds().width / 2.0f, 550);

            textoJogarNovamente.setFillColor(selectedGameOverIndex == 0 ? sf::Color::Yellow : sf::Color::White);
            textoIrMenu.setFillColor(selectedGameOverIndex == 1 ? sf::Color::Yellow : sf::Color::White);
            textoSairGameOver.setFillColor(selectedGameOverIndex == 2 ? sf::Color::Yellow : sf::Color::White);

            window.draw(textoDerrota);
            window.draw(textoJogarNovamente);
            window.draw(textoIrMenu);
            window.draw(textoSairGameOver);
        }
        else if (estadoAtual == JOGANDO || estadoAtual == PRONTO || estadoAtual == VITORIA)
        {
            spriteFundo.setScale((float)window.getSize().x / spriteFundo.getTexture()->getSize().x, (float)window.getSize().y / spriteFundo.getTexture()->getSize().y);
            window.draw(spriteFundo);

            for (int i = 0; i < 26; i++)
            {
                for (int j = 0; j < 27; j++)
                {
                    char celula = mapa[i][j];
                    int soma = 0;
                    if ((mapa[i][j - 1] >= 'A' && mapa[i][j - 1] <= 'z') && celula != '1')
                        soma += 1;
                    if ((mapa[i][j + 1] >= 'A' && mapa[i][j + 1] <= 'z') && celula != '1')
                        soma += 2;
                    if ((mapa[i - 1][j] >= 'A' && mapa[i - 1][j] <= 'z') && celula != '1')
                        soma += 4;
                    if ((mapa[i + 1][j] >= 'A' && mapa[i + 1][j] <= 'z') && celula != '1')
                        soma += 8;

                    sf::Sprite &spriteusado = sprites[soma];
                    spriteusado.setScale(SIZE / (float)spriteusado.getTextureRect().width, SIZE / (float)spriteusado.getTextureRect().height);
                    spriteusado.setPosition(j * SIZE, i * SIZE);
                    window.draw(spriteusado);

                    if (celula >= 'a' && celula <= 'z')
                    {
                        pilulaSprite.setPosition(j * SIZE + SIZE / 2.0f, i * SIZE + SIZE / 2.0f);
                        window.draw(pilulaSprite);
                    }
                }
            }

            pacmanSprite.setTextureRect(GetPacManFrame(cima, baixo, esq, dir));
            sf::FloatRect bounds = pacmanSprite.getLocalBounds();
            pacmanSprite.setScale(SIZE / bounds.width, SIZE / bounds.height);

            pacmanSprite2.setTextureRect(GetPacManFrame(cima, baixo, esq, dir));
            sf::FloatRect bounds2 = pacmanSprite2.getLocalBounds();
            pacmanSprite2.setScale(SIZE / bounds.width, SIZE / bounds.height);

            pacmanSprite3.setTextureRect(GetPacManFrame(cima, baixo, esq, dir));
            sf::FloatRect bounds3 = pacmanSprite3.getLocalBounds();
            pacmanSprite3.setScale(SIZE / bounds.width, SIZE / bounds.height);

            // frame 1

            fantasmazul1.setTextureRect(GetGhostFrame(direcaoFantasma1));
            sf::FloatRect boundsfantasma = fantasmazul1.getLocalBounds();
            fantasmazul1.setScale(SIZE / boundsfantasma.width, SIZE / boundsfantasma.height);
            fantasmazul1.setOrigin(boundsfantasma.left + boundsfantasma.width / 2.0f, boundsfantasma.top + boundsfantasma.height / 2.0f);

            fantasmavermelho1.setTextureRect(GetGhostFrame(direcaoFantasma2));
            sf::FloatRect boundsfantasma2 = fantasmavermelho1.getLocalBounds();
            fantasmavermelho1.setScale(SIZE / boundsfantasma2.width, SIZE / boundsfantasma2.height);
            fantasmavermelho1.setOrigin(boundsfantasma2.left + boundsfantasma2.width / 2.0f, boundsfantasma2.top + boundsfantasma2.height / 2.0f);

            fantasmamarelo1.setTextureRect(GetGhostFrame(direcaoFantasma3));
            sf::FloatRect boundsfantasma3 = fantasmamarelo1.getLocalBounds();
            fantasmamarelo1.setScale(SIZE / boundsfantasma3.width, SIZE / boundsfantasma3.height);
            fantasmamarelo1.setOrigin(boundsfantasma3.left + boundsfantasma3.width / 2.0f, boundsfantasma3.top + boundsfantasma3.height / 2.0f);

            fantasmaverde1.setTextureRect(GetGhostFrame(direcaoFantasma4));
            sf::FloatRect boundsfantasma4 = fantasmaverde1.getLocalBounds();
            fantasmaverde1.setScale(SIZE / boundsfantasma4.width, SIZE / boundsfantasma4.height);
            fantasmaverde1.setOrigin(boundsfantasma4.left + boundsfantasma4.width / 2.0f, boundsfantasma4.top + boundsfantasma4.height / 2.0f);

            // frame 2

            fantasmazul2.setTextureRect(GetGhostFrame(direcaoFantasma1));
            sf::FloatRect boundsfantasma5 = fantasmazul2.getLocalBounds();
            fantasmazul2.setScale(SIZE / boundsfantasma5.width, SIZE / boundsfantasma5.height);
            fantasmazul2.setOrigin(boundsfantasma5.left + boundsfantasma5.width / 2.0f, boundsfantasma5.top + boundsfantasma5.height / 2.0f);

            fantasmavermelho2.setTextureRect(GetGhostFrame(direcaoFantasma2));
            sf::FloatRect boundsfantasma6 = fantasmavermelho2.getLocalBounds();
            fantasmavermelho2.setScale(SIZE / boundsfantasma6.width, SIZE / boundsfantasma6.height);
            fantasmavermelho2.setOrigin(boundsfantasma6.left + boundsfantasma6.width / 2.0f, boundsfantasma6.top + boundsfantasma6.height / 2.0f);

            fantasmamarelo2.setTextureRect(GetGhostFrame(direcaoFantasma3));
            sf::FloatRect boundsfantasma7 = fantasmamarelo2.getLocalBounds();
            fantasmamarelo2.setScale(SIZE / boundsfantasma7.width, SIZE / boundsfantasma7.height);
            fantasmamarelo2.setOrigin(boundsfantasma7.left + boundsfantasma7.width / 2.0f, boundsfantasma7.top + boundsfantasma7.height / 2.0f);

            fantasmaverde2.setTextureRect(GetGhostFrame(direcaoFantasma4));
            sf::FloatRect boundsfantasma8 = fantasmaverde2.getLocalBounds();
            fantasmaverde2.setScale(SIZE / boundsfantasma8.width, SIZE / boundsfantasma8.height);
            fantasmaverde2.setOrigin(boundsfantasma8.left + boundsfantasma8.width / 2.0f, boundsfantasma8.top + boundsfantasma8.height / 2.0f);

            // frame 3

            fantasmazul3.setTextureRect(GetGhostFrame(direcaoFantasma1));
            sf::FloatRect boundsfantasma9 = fantasmazul3.getLocalBounds();
            fantasmazul3.setScale(SIZE / boundsfantasma9.width, SIZE / boundsfantasma9.height);
            fantasmazul3.setOrigin(boundsfantasma9.left + boundsfantasma9.width / 2.0f, boundsfantasma9.top + boundsfantasma9.height / 2.0f);

            fantasmavermelho3.setTextureRect(GetGhostFrame(direcaoFantasma2));
            sf::FloatRect boundsfantasma10 = fantasmavermelho3.getLocalBounds();
            fantasmavermelho3.setScale(SIZE / boundsfantasma10.width, SIZE / boundsfantasma10.height);
            fantasmavermelho3.setOrigin(boundsfantasma10.left + boundsfantasma10.width / 2.0f, boundsfantasma10.top + boundsfantasma10.height / 2.0f);

            fantasmamarelo3.setTextureRect(GetGhostFrame(direcaoFantasma3));
            sf::FloatRect boundsfantasma11 = fantasmamarelo3.getLocalBounds();
            fantasmamarelo3.setScale(SIZE / boundsfantasma11.width, SIZE / boundsfantasma11.height);
            fantasmamarelo3.setOrigin(boundsfantasma11.left + boundsfantasma11.width / 2.0f, boundsfantasma11.top + boundsfantasma11.height / 2.0f);

            fantasmaverde3.setTextureRect(GetGhostFrame(direcaoFantasma4));
            sf::FloatRect boundsfantasma12 = fantasmaverde3.getLocalBounds();
            fantasmaverde3.setScale(SIZE / boundsfantasma12.width, SIZE / boundsfantasma12.height);
            fantasmaverde3.setOrigin(boundsfantasma12.left + boundsfantasma12.width / 2.0f, boundsfantasma12.top + boundsfantasma12.height / 2.0f);

            for (int j = 4; j < 23; j++)
            {
                corda.setPosition(j * SIZE, 13 * SIZE);
                window.draw(corda);
            }

            pacmanSprite.setPosition(pacmanPosicaoatual);
            pacmanSprite2.setPosition(pacmanPosicaoatual);
            pacmanSprite3.setPosition(pacmanPosicaoatual);

            if (estadoAtual == JOGANDO && text.getElapsedTime().asSeconds() >= 0.1f)
            {
                bool aux, aux2;
                aux = textura[0];
                textura[0] = textura[2];
                aux2 = textura[1];
                textura[1] = aux;
                textura[2] = aux2;
                text.restart();
            }

            if (textura[0])
                window.draw(pacmanSprite);
            if (textura[1])
                window.draw(pacmanSprite2);
            if (textura[2])
                window.draw(pacmanSprite3);

            if (textura[0])
            {
                fantasmazul1.setPosition(fantasma1PosicaoPixel.x + SIZE / 2.0f, fantasma1PosicaoPixel.y + SIZE / 2.0f);
                window.draw(fantasmazul1);
                fantasmavermelho1.setPosition(fantasma2PosicaoPixel.x + SIZE / 2.0f, fantasma2PosicaoPixel.y + SIZE / 2.0f);
                window.draw(fantasmavermelho1);
                fantasmamarelo1.setPosition(fantasma3PosicaoPixel.x + SIZE / 2.0f, fantasma3PosicaoPixel.y + SIZE / 2.0f);
                window.draw(fantasmamarelo1);
                fantasmaverde1.setPosition(fantasma4PosicaoPixel.x + SIZE / 2.0f, fantasma4PosicaoPixel.y + SIZE / 2.0f);
                window.draw(fantasmaverde1);
            }

            if (textura[1])
            {
                fantasmazul2.setPosition(fantasma1PosicaoPixel.x + SIZE / 2.0f, fantasma1PosicaoPixel.y + SIZE / 2.0f);
                window.draw(fantasmazul2);
                fantasmavermelho2.setPosition(fantasma2PosicaoPixel.x + SIZE / 2.0f, fantasma2PosicaoPixel.y + SIZE / 2.0f);
                window.draw(fantasmavermelho2);
                fantasmamarelo2.setPosition(fantasma3PosicaoPixel.x + SIZE / 2.0f, fantasma3PosicaoPixel.y + SIZE / 2.0f);
                window.draw(fantasmamarelo2);
                fantasmaverde2.setPosition(fantasma4PosicaoPixel.x + SIZE / 2.0f, fantasma4PosicaoPixel.y + SIZE / 2.0f);
                window.draw(fantasmaverde2);
            }

            if (textura[2])
            {
                fantasmazul3.setPosition(fantasma1PosicaoPixel.x + SIZE / 2.0f, fantasma1PosicaoPixel.y + SIZE / 2.0f);
                window.draw(fantasmazul3);
                fantasmavermelho3.setPosition(fantasma2PosicaoPixel.x + SIZE / 2.0f, fantasma2PosicaoPixel.y + SIZE / 2.0f);
                window.draw(fantasmavermelho3);
                fantasmamarelo3.setPosition(fantasma3PosicaoPixel.x + SIZE / 2.0f, fantasma3PosicaoPixel.y + SIZE / 2.0f);
                window.draw(fantasmamarelo3);
                fantasmaverde3.setPosition(fantasma4PosicaoPixel.x + SIZE / 2.0f, fantasma4PosicaoPixel.y + SIZE / 2.0f);
                window.draw(fantasmaverde3);
            }

            spriteCopas.setScale((float)window.getSize().x / spriteCopas.getTexture()->getSize().x, (float)window.getSize().y / spriteCopas.getTexture()->getSize().y);
            window.draw(spriteCopas);

            textoPontuacao.setString("Pontos: " + std::to_string(pontos*50));
            window.draw(textoPontuacao);

            for (int i = 0; i < vida; ++i)
            {
                heartSprite.setPosition(window.getSize().x - ((i + 2.2) * (SIZE + 5)), 1);
                window.draw(heartSprite);
            }

            if (estadoAtual == PRONTO)
            {
                sf::FloatRect boundsPronto = textoPronto.getLocalBounds();
                textoPronto.setOrigin(boundsPronto.left + boundsPronto.width / 2.0f, boundsPronto.top + boundsPronto.height / 2.0f);
                textoPronto.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
                window.draw(textoPronto);
            }

            if (estadoAtual == VITORIA)
            {
                if (!animacaoVitoriaCompleta && relogioVitoria.getElapsedTime().asMilliseconds() > 10)
                {
                    tamanhoAtual += 1.5f;
                    if (tamanhoAtual >= 200.0f)
                    {
                        tamanhoAtual = 200.0f;
                        animacaoVitoriaCompleta = true;
                    }
                    textoVitoria.setCharacterSize(tamanhoAtual);
                    relogioVitoria.restart();
                }
                sf::FloatRect boundsVitoria = textoVitoria.getLocalBounds();
                textoVitoria.setOrigin(boundsVitoria.left + boundsVitoria.width / 2.0f, boundsVitoria.top + boundsVitoria.height / 2.0f);
                textoVitoria.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
                window.draw(textoVitoria);

                textoJogarNovamente.setPosition(window.getSize().x / 2.0f - textoJogarNovamente.getGlobalBounds().width / 2.0f, 450);
                textoIrMenu.setPosition(window.getSize().x / 2.0f - textoIrMenu.getGlobalBounds().width / 2.0f, 550);

                textoJogarNovamente.setFillColor(selectedVitoriaIndex == 0 ? sf::Color::Yellow : sf::Color::White);
                textoIrMenu.setFillColor(selectedVitoriaIndex == 1 ? sf::Color::Yellow : sf::Color::White);

                window.draw(textoJogarNovamente);
                window.draw(textoIrMenu);
            }
        }

        // termina
        window.display();
    }

    return 0;
}