#include "Map.hpp"
#include <cctype>

// Layout original do labirinto desenhado por Andre Gustavo, agora com 4
// power pellets ('o') adicionados nos cantos como upgrade de jogabilidade.
const std::array<std::string, config::GRID_ROWS> Map::k_layout = {
    "111111111111111111111111111",
    "1oxxxxxxxxxxxxxxxxxxxxxxxo1",
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
    "1oxxxxxxxxxxxxxxxxxxxxxxxo1",
    "111111111111111111111111111"};

Map::Map()
{
    reset();
}

void Map::reset()
{
    m_grid = k_layout;
    locateTunnels();
}

void Map::locateTunnels()
{
    m_tunnelA = Cell{};
    m_tunnelB = Cell{};
    for (int r = 0; r < config::GRID_ROWS; ++r)
    {
        for (int c = 0; c < static_cast<int>(m_grid[r].size()); ++c)
        {
            if (m_grid[r][c] == '2')
            {
                if (m_tunnelA.row == -1)
                    m_tunnelA = Cell{r, c};
                else
                    m_tunnelB = Cell{r, c};
            }
        }
    }
}

char Map::at(int row, int col) const
{
    if (row < 0 || row >= config::GRID_ROWS ||
        col < 0 || col >= static_cast<int>(m_grid[row].size()))
        return '1'; // fora do mapa conta como parede
    return m_grid[row][col];
}

bool Map::isWall(int row, int col) const
{
    return at(row, col) == '1';
}

bool Map::isTunnel(int row, int col) const
{
    return at(row, col) == '2';
}

Map::Consumed Map::consume(int row, int col)
{
    char& cell = m_grid[row][col];
    if (cell == 'o')
    {
        cell = 'O';
        return Consumed::Power;
    }
    if (cell >= 'a' && cell <= 'z')
    {
        cell = static_cast<char>(std::toupper(cell));
        return Consumed::Pellet;
    }
    return Consumed::None;
}

bool Map::allPelletsEaten() const
{
    for (int r = 0; r < config::GRID_ROWS; ++r)
        for (char ch : m_grid[r])
            if (ch >= 'a' && ch <= 'z')
                return false;
    return true;
}

bool Map::tunnelExit(int row, int col, int& outRow, int& outCol) const
{
    if (row == m_tunnelA.row && col == m_tunnelA.col)
    {
        outRow = m_tunnelB.row;
        outCol = m_tunnelB.col;
        return true;
    }
    if (row == m_tunnelB.row && col == m_tunnelB.col)
    {
        outRow = m_tunnelA.row;
        outCol = m_tunnelA.col;
        return true;
    }
    return false;
}
