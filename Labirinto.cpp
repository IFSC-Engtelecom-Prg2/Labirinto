//
// Created by msobral on 29/09/2022.
//

#include "Labirinto.h"
#include <stdlib.h>
#include <sys/time.h>
#include <locale.h>
#include <cstdio>
#include <algorithm>
#include <time.h>

constexpr static wchar_t glyph[] = L"" SPC "│││─┘┐┤─└┌├─┴┬┼" SPC "┆┆┆┄╯╮ ┄╰╭ ┄";
constexpr static int dirs[4][2] = {{-2, 0}, {0, 2}, {2, 0}, {0, -2}};

enum { N = 1, S = 2, W = 4, E = 8, V = 16 };
#define each(i, x, y) for (i = x; i <= y; i++)

using byte = uint8_t;

void espera(uint32_t milissegundos) {
    timespec ts{milissegundos/1000, 1000000*(milissegundos % 1000)};

//    ts.tv_sec = milissegundos/1000;
//    ts.tv_nsec = 1000000*(milissegundos % 1000);
    nanosleep(&ts, NULL);
}

int irand(int n)
{
    int r, rmax = n * (RAND_MAX / n);
    while ((r = rand()) >= rmax);
    return r / (RAND_MAX/n);
}

void initrand() {
    struct timeval tv;

    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);
}

Labirinto::Labirinto(int largura, int altura): w(largura), h(altura) {
    setlocale(LC_ALL, "");
    reinicia();
}

void Labirinto::mostra() const {
    int i, j, c;
    //each(c,0,31) {
    //  printf("%d: %lc\n", c, glyph[c]);
    //}
    //return;
    each(i, 0, 2 * h) {
        each(j, 0, 2 * w) {
            c = cell[i][j];
            if (c == 127) {
                printf("\033[31m");
                printf("x");
                //printf("%02X ", c);
                printf("\033[m");
            } else  printf("%lc", glyph[c]);

/*			if (c > V) printf("\033[31m");
			printf("%lc", glyph[c]);
			//printf("%02X ", c);
			if (c > V) printf("\033[m");
*/
        }
        putchar('\n');
    }
}

bool Labirinto::chegou() const {
    auto pos = obtem_alvo();

    return (x == pos.x) && (y == pos.y);
}

Posicao Labirinto::obtem_alvo() const {
    return Posicao{2*w-1,2*h-1};
}

Posicao Labirinto::obtem_posicao() const {
    return Posicao{x,y};
}

Status Labirinto::avanca(Direcao dir, bool volta) {
    Status status = Status::NaoAvancou;
    constexpr int casa = 127;

    //if (volta) casa = 0;

    //printf("dir=%d, x=%d, y=%d\n", direcao, x, y);
    switch (dir) {
        case Direcao::Acima: // acima
            if ((y > 1) && ((cell[y-1][x] == 0) || (cell[y-1][x] == 127))) {
                if (volta) cell[y][x] = 0;
                y-=1;
                cell[y][x] = casa;
                status = chegou()?Status::Chegou:Status::Avancou;
            }
            break;
        case Direcao::Abaixo: // abaixo
            if ((y < 2*h) && ((cell[y+1][x] == 0) || (cell[y+1][x] == 127))) {
                if (volta) cell[y][x] = 0;
                y+=1;
                cell[y][x] = casa;
                status = chegou()?Status::Chegou:Status::Avancou;
            }
            break;
        case Direcao::Esquerda: // esquerda
            if ((x > 1) && ((cell[y][x-1] == 0) || (cell[y][x-1] == 127))) {
                if (volta) cell[y][x] = 0;
                x-=1;
                cell[y][x] = casa;
                status = chegou()?Status::Chegou:Status::Avancou;
            }
            break;
        case Direcao::Direita: // direita
//            printf("x=%d, y=%d, cell[y][x+1]=%d\n", x, y, cell[y][x+1]);
            if ((x < 2*w) && ((cell[y][x+1] == 0) || (cell[y][x+1] == 127))) {
                if (volta) cell[y][x] = 0;
                x+=1;
                cell[y][x] = casa;
                status = chegou()?Status::Chegou:Status::Avancou;
            }
            break;
    }
    return status;
}

void Labirinto::reinicia() {
    int i, j;
    int h2 = 2 * h + 2, w2 = 2 * w + 2;
    byte **p;

    initrand();

    mem.reset(new byte[sizeof(byte*) * (h2 + 2) + w2 * h2 + 1]);
    p = (byte**)mem.get();

    p[1] = (byte*)(p + h2 + 2) + 1;
    each(i, 2, h2) p[i] = p[i-1] + w2;
    p[0] = p[h2];
    cell = &p[1];

    each(i, -1, 2 * h + 1) cell[i][-1] = cell[i][w2 - 1] = V;
    each(j, 0, 2 * w) cell[-1][j] = cell[h2 - 1][j] = V;
    each(i, 0, h) each(j, 0, 2 * w) cell[2*i][j] |= E|W;
    each(i, 0, 2 * h) each(j, 0, w) cell[i][2*j] |= N|S;
    each(j, 0, 2 * w) cell[0][j] &= ~N, cell[2*h][j] &= ~S;
    each(i, 0, 2 * h) cell[i][0] &= ~W, cell[i][2*w] &= ~E;

    avail = w * h;
    walk(Posicao{irand(2) * 2 + 1, irand(h) * 2 + 1});

    /* reset visited marker (it's also used by path finder) */
    each(i, 0, 2 * h) each(j, 0, 2 * w) cell[i][j] &= ~V;
    //solve(1, 1, 2 * w - 1, 2 * h - 1);
    y = 1;
    x = 1;
    cell[y][x] = 127;

    //show();
}

void Labirinto::walk(const Posicao &pos) {
    int i, t, x1, y1, d[4] = { 0, 1, 2, 3 };

    cell[pos.y][pos.x] |= V;
    avail--;

    for (x1 = 3; x1; x1--)
        if (x1 != (y1 = irand(x1 + 1)))
            i = d[x1], d[x1] = d[y1], d[y1] = i;

    for (i = 0; avail && i < 4; i++) {
        x1 = pos.x + dirs[ d[i] ][0], y1 = pos.y + dirs[ d[i] ][1];

        if (cell[y1][x1] & V) continue;

        /* break walls */
        if (x1 == pos.x) {
            t = (pos.y + y1) / 2;
            cell[t][pos.x+1] &= ~W, cell[t][pos.x] &= ~(E|W), cell[t][pos.x-1] &= ~E;
        } else if (y1 == pos.y) {
            t = (pos.x + x1)/2;
            cell[pos.y-1][t] &= ~S, cell[pos.y][t] &= ~(N|S), cell[pos.y+1][t] &= ~N;
        }
        walk(Posicao{x1, y1});
    }

}