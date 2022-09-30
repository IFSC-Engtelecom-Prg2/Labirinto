#include <iostream>
#include <string>
#include "Labirinto.h"

// 250 milissegundos
const uint32_t PASSO=250;

int main(int c, char **v)
{
    // Cria um labirinto com 8 colunas e 16 linhas
    Labirinto maze(16, 8);

    // Espera 250 ms entre cada movimentação dentro do
    // do labirinto.
    maze.mostra();
    while (maze.avanca(Direcao::Direita) != Status::NaoAvancou) {
        maze.mostra();
        espera(PASSO);
    }

//    while (maze.avanca(Direcao::Abaixo) != Status::NaoAvancou) {
//        maze.mostra();
//        espera(PASSO);
//    }

//
//    while (maze.avanca(Direcao::Esquerda) != Status::NaoAvancou) {
//        maze.mostra();
//        espera(PASSO);
//    }
//    while (maze.avanca(Direcao::Acima) != Status::NaoAvancou) {
//        maze.mostra();
//        espera(PASSO);
//    }

    std::cout << "Não pode mais avançar. Tecle ENTER para terminar !" << std::endl;
    std::string algo;
    getline(std::cin, algo);

    auto p = maze.obtem_posicao();
    std::cout << "Parou em: x=" << p.x << ",  y=" << p.y << std::endl;

    return 0;
}
