#include "bitwise.h"

#define TODO return 255

uint8_t clear(uint8_t msk, int pos) { 
  uint8_t nmsk;
  nmsk = msk & ~BIT(pos);
  return nmsk;
}

uint8_t set(uint8_t msk, int pos) { 
  uint8_t nmsk;
  nmsk = msk | BIT(pos);
  return nmsk; }

bool is_set(uint8_t msk, int pos) { 
  return (BIT(pos) & msk) != 0; 
}

uint8_t lsb(uint16_t wide_msk) { 
  uint16_t nmsk;
  nmsk = wide_msk & 0xFF;
  return nmsk;
 }

uint8_t msb(uint16_t wide_msk) { 
  uint16_t nmsk = wide_msk >> 8;
  return nmsk;
}

uint8_t mask(int pos, ...) {  
  va_list args;
  va_start(args , pos);
  uint8_t msk = 0;

  while(pos != MSK_END){
    msk = msk | BIT(pos);
    pos = va_arg(args,int);
  }
  va_end(args);
  return msk;
}
