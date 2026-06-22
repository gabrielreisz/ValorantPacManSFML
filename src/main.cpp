#include "Game.hpp"
#include <iostream>

// ============================================================================
//  Ponto de entrada. Toda a logica vive na classe Game.
//  Se algum recurso faltar, o ResourceManager lanca excecao com mensagem clara.
// ============================================================================
int main()
{
    try
    {
        Game game;
        game.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Orb Rush] Erro fatal: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
