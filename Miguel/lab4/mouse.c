#include <lcom/lcf.h>
#include "mouse.h"
#include "kbc.h"
#include <stdint.h>
#include <stdbool.h>

static int mouse_hook_id = 2;
static uint8_t mouse_byte = 0;
static bool byte_ready = false;
static bool byte_error = false;

int mouse_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = mouse_hook_id;
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id);
}

int mouse_unsubscribe_int() {
  return sys_irqrmpolicy(&mouse_hook_id);
}

void (mouse_ih)() {
  byte_ready = false;
  byte_error = false;
  uint8_t st;
  if (kbc_read_status(&st)) return;
  if (st & (KBC_PARITY | KBC_TIMEOUT)) {
    util_sys_inb(KBC_OUT_BUF, &mouse_byte);
    byte_error = true;
    return;
  }
  if (st & KBC_OBF) {
    util_sys_inb(KBC_OUT_BUF, &mouse_byte);
    byte_ready = true;
  }
}

int mouse_write_cmd(uint8_t cmd) {
  int tries = 0;
  while (tries < KBC_MAX_TRIES) {
    if (kbc_issue_cmd(MOUSE_WRITE_CMD)) return 1;
    if (kbc_write_arg(cmd)) return 1;
    uint8_t ack;
    if (kbc_read_output(&ack)) return 1;
    if (ack == MOUSE_ACK) return 0;
    tries++;
  }
  return 1;
}

int mouse_disable_data_reporting() {
  return mouse_write_cmd(MOUSE_DISABLE);
}

bool mouse_byte_ready() {
  return byte_ready;
}

bool mouse_has_error() {
  return byte_error;
}

uint8_t mouse_get_byte() {
  return mouse_byte;
}

int mouse_parse_packet(uint8_t *bytes, struct packet *pp) {
  if (pp == NULL) return 1;

  pp->bytes[0] = bytes[0];
  pp->bytes[1] = bytes[1];
  pp->bytes[2] = bytes[2];

  pp->lb = bytes[0] & BIT(0);
  pp->rb = bytes[0] & BIT(1);
  pp->mb = bytes[0] & BIT(2);

  pp->x_ov = bytes[0] & BIT(6);
  pp->y_ov = bytes[0] & BIT(7);

  pp->delta_x = bytes[1];
  if (bytes[0] & BIT(4)) pp->delta_x |= 0xFF00;

  pp->delta_y = bytes[2];
  if (bytes[0] & BIT(5)) pp->delta_y |= 0xFF00;

  return 0;
}
