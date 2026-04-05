#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <lcom/lcf.h>


int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();  


uint8_t kbc_get_scancode();
bool kbc_get_ih_error();

#endif

