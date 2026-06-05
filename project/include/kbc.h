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

/**
 * @brief Keyboard interrupt handler. Reads the scancode and updates the internal state.
 */
void (kbc_ih)();

/**
 * @brief Subscribes to keyboard interrupts.
 * @param bit_no Pointer to the bit number for the interrupt.
 * @return 0 on success, -1 on failure.
 */
int (kbd_subscribe_int)(uint8_t *bit_no);
/**
 * @brief Unsubscribes from keyboard interrupts.
 * @return 0 on success, -1 on failure.
 */
int (kbd_unsubscribe_int)();

/**
 * @brief Gets the scancode from the keyboard.
 * @return The scancode.
 */
uint8_t (kbc_get_scancode)();
/**
 * @brief Checks if a scancode is ready to be read.
 * @return True if a scancode is ready, false otherwise.
 */

bool (kbc_scancode_ready)();

/**
 * @brief Checks if there was an error in the last scancode read.
 * @return True if there was an error, false otherwise.
 */
bool (kbc_has_error)();


/**
 * @brief Converts a scancode to a character.
 * @param scancode The scancode to convert.
 * @return The corresponding character, or 0 if the scancode does not correspond to a character.
 */
char kbd_scancode_to_char(uint8_t scancode);

#endif
