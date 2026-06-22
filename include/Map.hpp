#pragma once

#include "Config.hpp"
#include <array>
#include <string>

// ============================================================================
//  Map
//  Dono da grade do labirinto. Encapsula paredes, pilulas, power pellets,
//  tuneis (cordas) e a condicao de vitoria. Substitui as matrizes globais
//  `mapa` / `mapa_original` e o loop solto que procurava as cordas.
//
//  Convencao de caracteres:
//    '1'        -> parede
//    'x'..'z'   -> pilula (minuscula = nao comida)
//    'o'        -> power pellet (deixa os fantasmas cacaveis)
//    'X'..'Z'/'O' -> caminho ja "comido" (maiuscula)
//    '2'        -> corda (tunel de teletransporte)
// ============================================================================
class Map
{
public:
    enum class Consumed { None, Pellet, Power };

    Map();

    void reset();                                  // volta ao layout original

    bool isWall(int row, int col) const;
    bool isTunnel(int row, int col) const;
    char at(int row, int col) const;

    // Tenta comer o conteudo da celula; devolve o que foi comido.
    Consumed consume(int row, int col);

    bool allPelletsEaten() const;

    // Dada uma ponta da corda, devolve a outra ponta. true se existir.
    bool tunnelExit(int row, int col, int& outRow, int& outCol) const;

private:
    static const std::array<std::string, config::GRID_ROWS> k_layout;

    std::array<std::string, config::GRID_ROWS> m_grid;

    struct Cell { int row = -1; int col = -1; };
    Cell m_tunnelA;
    Cell m_tunnelB;

    void locateTunnels();
};
