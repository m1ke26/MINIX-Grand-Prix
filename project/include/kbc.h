#ifndef _KBC_H_
#define _KBC_H_

#include <lcom/lcf.h>

/* --- Scancodes --- */
/* Break codes */
#define ESC_BREAK  0x81

/* ESC */
#define ESC_MAKE   0x01

/* WASD make codes */
#define W_MAKE     0x11
#define S_MAKE     0x1F
#define A_MAKE     0x1E
#define D_MAKE     0x20
#define SPACE_MAKE 0x39

/* WASD break codes (make | 0x80) */
#define W_BREAK    (W_MAKE | 0x80)
#define S_BREAK    (S_MAKE | 0x80)
#define A_BREAK    (A_MAKE | 0x80)
#define D_BREAK    (D_MAKE | 0x80)
#define SPACE_BREAK (SPACE_MAKE | 0x80)

void (kbc_ih)();

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();

uint8_t (kbc_get_scancode)();
bool (kbc_scancode_ready)();
bool (kbc_has_error)();

#endif
