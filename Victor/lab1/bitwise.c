#include "bitwise.h"
#include <stdarg.h>

uint8_t clear(uint8_t msk, int pos) {
    return msk & ~(1 << pos);
}

uint8_t set(uint8_t msk, int pos) {
    return msk | (1 << pos);
}

bool is_set(uint8_t msk, int pos) {
    return (msk & (1 << pos)) != 0;
}

uint8_t lsb(uint16_t wide_msk) {
    return wide_msk & 0xFF;
}

uint8_t msb(uint16_t wide_msk) {
    return (wide_msk >> 8) & 0xFF;
}

uint8_t mask(int pos, ...) {
    va_list args;
    va_start(args, pos);

    uint8_t result = 0;

    while (pos != MSK_END) {
        result |= (1 << pos);
        pos = va_arg(args, int);
    }

    va_end(args);

    return result;
}