#include "bitwise.h"
#include <stdarg.h>

// Põe o bit na posição pos a 0: cria uma máscara com 1 na posição pos,
// inverte-a com ~ (fica 0 nessa posição), e faz AND com msk
uint8_t clear(uint8_t msk, int pos) { return msk & ~(1 << pos); }

// Põe o bit na posição pos a 1: cria uma máscara com 1 na posição pos
// e faz OR com msk
uint8_t set(uint8_t msk, int pos) { return msk | (1 << pos); }

// Verifica se o bit na posição pos é 1: desloca msk para a direita pos vezes
// e verifica se o bit mais à direita é 1
bool is_set(uint8_t msk, int pos) { return (msk >> pos) & 1; }

// Devolve os 8 bits da direita: faz AND com 0xFF (11111111)
// para apagar os 8 bits da esquerda
uint8_t lsb(uint16_t wide_msk) { return wide_msk & 0xFF; }

// Devolve os 8 bits da esquerda: desloca 8 bits para a direita
// para trazer os bits da esquerda para a direita
uint8_t msb(uint16_t wide_msk) { return wide_msk >> 8; }

// Constrói uma bitmask com bits a 1 nas posições dadas
// usa va_list para ler os argumentos variáveis até encontrar MSK_END
uint8_t mask(int pos, ...) {
    uint8_t result = 0;
    va_list args;
    va_start(args, pos);
    while (pos != MSK_END) {
        result = set(result, pos);
        pos = va_arg(args, int);
    }
    va_end(args);
    return result;
}