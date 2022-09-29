//
// Created by msobral on 29/09/2022.
//

#ifndef LABIRINTO_V1_LABIRINTO_H
#define LABIRINTO_V1_LABIRINTO_H

#include <cstdint>
#include <memory>

#define DOUBLE_SPACE 0

#if DOUBLE_SPACE
#	define SPC "　"
#else
#	define SPC " "
#endif

struct Posicao {
    int x,y;
};

// resultado de um movimento no labirinto
enum class Status:uint8_t {
    Chegou,
    Avancou,
    NaoAvancou
};

// as possíveis direções de movimento no labirinto
enum class Direcao:uint8_t {
    Acima,
    Abaixo,
    Esquerda,
    Direita
};

// Representa um labirinto
class Labirinto {
public:
    // cria um labirinto com esta largura e eltura
    Labirinto(int largura, int altura);

    // desenha o labirinto na tela, incluindo as posições ocupadas
    void mostra() const;

    // testa se se chegou à saída do labirinto
    bool chegou() const;

    // obtém a posição da saída do labirinto
    constexpr Posicao obtem_alvo() const;

    // obtém a posição atual no labirinto
    Posicao obtem_posicao() const;

    // avança a posição em alguma direção
    // dir: a direção para o movimento (ver enum Direcao)
    // volta: se true, limpa a casa para onde se avançou. Isso pode ser usado para retroceder no labirinto.
    // Resultado: um valor do tipo enum Status (ver acima), indicando se:
    //    Status::Avancou: avançou-se na direção solicitada
    //    Status::Chegou: avançou-se na direção solicitada e chegou-as à saída do labirinto
    //    Status::NaoAvancou: não foi possível avançar na direção solicitada (tem uma parede)
    Status avanca(Direcao dir, bool volta=false);

    // reinicia o labirinto, apagando as movimentações e retornando para a posição inicial
    void reinicia();
private:
    void walk(const Posicao & pos);

    uint8_t **cell;
    std::unique_ptr<uint8_t[]> mem;
    int w, h, avail;
    int x=1, y=2;
};

// faz uma pausa
void espera(uint32_t milissegundos);

#endif //LABIRINTO_V1_LABIRINTO_H
