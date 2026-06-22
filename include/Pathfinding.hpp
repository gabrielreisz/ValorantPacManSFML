#pragma once

#include "Config.hpp"

class Map;

// ============================================================================
//  Pathfinding
//  Busca em Largura (BFS) sobre a grade. Devolve a PRIMEIRA direcao que o
//  fantasma deve tomar para alcancar o alvo pelo menor caminho.
//
//  Antes: o BFS era copiado-e-colado dentro do main() para cada fantasma
//  cacador, com indices (x/y) trocados e bugs sutis. Agora e uma funcao unica.
//  Devolve Direction::None se nao houver caminho.
// ============================================================================
namespace pathfinding
{
    Direction nextStep(const Map& map,
                       int startRow, int startCol,
                       int targetRow, int targetCol);
}
