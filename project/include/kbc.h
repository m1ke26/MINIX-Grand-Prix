#ifndef _KBC_H_
#define _KBC_H_

#include <lcom/lcf.h>

#define ESC_BREAK 0x81

void (kbc_ih)();

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();

uint8_t (kbc_get_scancode)();
bool (kbc_scancode_ready)();
bool (kbc_has_error)();

#endif
