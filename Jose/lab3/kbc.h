#ifndef _KBC_H_
#define _KBC_H_

#include <lcom/lcf.h>

extern uint8_t scancode;

void (kbc_ih)();

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();

#endif
