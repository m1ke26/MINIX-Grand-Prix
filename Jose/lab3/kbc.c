#include <lcom/lcf.h>
#include <stdint.h>

uint8_t scancode; 

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
