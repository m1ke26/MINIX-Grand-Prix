#ifndef _KBC_H_
#define _KBC_H_

#include <lcom/lcf.h>

/* --- Scancodes --- */

/* Utilitary Keys */
#define ESC_BREAK  0x81
#define ESC_MAKE   0x01
#define ENTER_MAKE 0x1C
#define ENTER_BREAK (ENTER_MAKE | 0x80)

/* Movement make codes */
#define W_MAKE     0x11
#define S_MAKE     0x1F
#define A_MAKE     0x1E
#define D_MAKE     0x20
#define SPACE_MAKE 0x39
#define SHIFT_MAKE 0x2A

/* Movement break codes (make | 0x80) */
#define W_BREAK    (W_MAKE | 0x80)
#define S_BREAK    (S_MAKE | 0x80)
#define A_BREAK    (A_MAKE | 0x80)
#define D_BREAK    (D_MAKE | 0x80)
#define SPACE_BREAK (SPACE_MAKE | 0x80)
#define SHIFT_BREAK (SHIFT_MAKE | 0x80)

void (kbc_ih)();

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();

uint8_t (kbc_get_scancode)();
bool (kbc_scancode_ready)();
bool (kbc_has_error)();

char kbd_scancode_to_char(uint8_t scancode);

#endif
