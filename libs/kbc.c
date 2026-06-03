#include <lcom/lcf.h>
#include <stdint.h>
#include "kbc.h"

uint8_t scancode;
static int hook_id = 3;
static bool error = false;
static bool ready = false;

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
    error = true;
    return;
  }
  ready = true;
  error = false;
}

int (kbd_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;
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

uint8_t (kbc_get_scancode)() {
  ready = false;
  return scancode;
}

bool (kbc_scancode_ready)() {
  return ready;
}

bool (kbc_has_error)() {
  return error;
}

char kbd_scancode_to_char(uint8_t scancode) {
  switch (scancode) {
    // Números
    case 0x02: return '1'; case 0x03: return '2'; case 0x04: return '3';
    case 0x05: return '4'; case 0x06: return '5'; case 0x07: return '6';
    case 0x08: return '7'; case 0x09: return '8'; case 0x0A: return '9';
    case 0x0B: return '0';
    // Linha de cima
    case 0x10: return 'Q'; case 0x11: return 'W'; case 0x12: return 'E';
    case 0x13: return 'R'; case 0x14: return 'T'; case 0x15: return 'Y';
    case 0x16: return 'U'; case 0x17: return 'I'; case 0x18: return 'O';
    case 0x19: return 'P';
    // Linha do meio
    case 0x1E: return 'A'; case 0x1F: return 'S'; case 0x20: return 'D';
    case 0x21: return 'F'; case 0x22: return 'G'; case 0x23: return 'H';
    case 0x24: return 'J'; case 0x25: return 'K'; case 0x26: return 'L';
    // Linha de baixo
    case 0x2C: return 'Z'; case 0x2D: return 'X'; case 0x2E: return 'C';
    case 0x2F: return 'V'; case 0x30: return 'B'; case 0x31: return 'N';
    case 0x32: return 'M';
    // Backspace
    case 0x0E: return '\b';
    // Qualquer outro (break codes, setas, etc.) — ignora
    default:   return 0;
  }
}

