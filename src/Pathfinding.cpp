#include "Pathfinding.hpp"
#include "Map.hpp"
#include <array>
#include <queue>
#include <utility>

namespace pathfinding
{
    Direction nextStep(const Map& map,
                       int startRow, int startCol,
                       int targetRow, int targetCol)
    {
        if (startRow == targetRow && startCol == targetCol)
            return Direction::None;

        constexpr int ROWS = config::GRID_ROWS;
        constexpr int COLS = config::GRID_COLS;

        // Para cada celula visitada guardamos de onde viemos, para reconstruir
        // o caminho do alvo de volta ate a origem.
        std::array<std::array<bool, COLS>, ROWS> visited{};
        std::array<std::array<std::pair<int, int>, COLS>, ROWS> parent{};

        const std::array<Direction, 4> dirs = {
            Direction::Up, Direction::Down, Direction::Left, Direction::Right};

        std::queue<std::pair<int, int>> frontier;
        frontier.push({startRow, startCol});
        visited[startRow][startCol] = true;

        bool found = false;
        while (!frontier.empty() && !found)
        {
            auto [row, col] = frontier.front();
            frontier.pop();

            for (Direction d : dirs)
            {
                int dr, dc;
                directionDelta(d, dr, dc);
                int nr = row + dr;
                int nc = col + dc;

                if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                    continue;
                if (map.isWall(nr, nc) || visited[nr][nc])
                    continue;

                visited[nr][nc] = true;
                parent[nr][nc] = {row, col};
                frontier.push({nr, nc});

                if (nr == targetRow && nc == targetCol)
                {
                    found = true;
                    break;
                }
            }
        }

        if (!found)
            return Direction::None;

        // Reconstroi do alvo ate o vizinho da origem para descobrir o 1o passo.
        int curRow = targetRow;
        int curCol = targetCol;
        while (true)
        {
            auto [pr, pc] = parent[curRow][curCol];
            if (pr == startRow && pc == startCol)
            {
                int dRow = curRow - startRow;
                int dCol = curCol - startCol;
                if (dRow == -1) return Direction::Up;
                if (dRow == 1)  return Direction::Down;
                if (dCol == -1) return Direction::Left;
                if (dCol == 1)  return Direction::Right;
                return Direction::None;
            }
            curRow = pr;
            curCol = pc;
        }
    }
}
