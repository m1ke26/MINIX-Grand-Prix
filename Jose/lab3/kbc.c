#include <lcom/lcf.h>
#include <stdint.h>
#include "kbc.h"

uint8_t scancode;
static int hook_id = 3;

void (kbc_ih)() {
  uint8_t status;

  // Read the status register
  if (util_sys_inb(0x64, &status) != 0) {
    return;
  }

  // Check if Output Buffer is Full (Bit 0) - must be set before reading
  if (!(status & BIT(0))) {
    return;
  }

  // Check for parity or timeout errors (Bits 7 and 6)
  if ((status & (BIT(7) | BIT(6))) != 0) {
    // Read 0x60 to clear the output buffer, but discard the invalid data
    uint8_t trash;
    util_sys_inb(0x60, &trash);
    return;
  }

  // Read the scancode from the output buffer
  if (util_sys_inb(0x60, &scancode) != 0) {
    return;
  }
}

int (kbd_subscribe_int)(uint8_t *bit_no) {
  *bit_no = BIT(hook_id);
  if (sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != 0) {
    return 1;
  }
  return 0;
}

int (kbd_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != 0) {
    return 1;
  }
  return 0;
}

