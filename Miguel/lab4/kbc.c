#include <lcom/lcf.h>
#include "kbc.h"
#include <stdint.h>
#include <stdbool.h>
#include <minix/sysutil.h>

static int kbc_hook_id = 1;
static uint8_t scancode_byte = 0;
static bool scancode_ready = false;
static bool scancode_error = false;

int kbc_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = kbc_hook_id;
  return sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbc_hook_id);
}

int kbc_unsubscribe_int() {
  return sys_irqrmpolicy(&kbc_hook_id);
}

int kbc_read_status(uint8_t *st) {
  return util_sys_inb(KBC_ST_REG, st);
}

int kbc_read_output(uint8_t *data) {
  uint8_t st;
  int tries = 0;
  while (tries < KBC_MAX_TRIES) {
    if (kbc_read_status(&st)) return 1;
    if (st & KBC_OBF) {
      if (util_sys_inb(KBC_OUT_BUF, data)) return 1;
      if (st & (KBC_PARITY | KBC_TIMEOUT)) return 1;
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US));
    tries++;
  }
  return 1;
}

int kbc_issue_cmd(uint8_t cmd) {
  uint8_t st;
  int tries = 0;
  while (tries < KBC_MAX_TRIES) {
    if (kbc_read_status(&st)) return 1;
    if (!(st & KBC_IBF)) {
      return sys_outb(KBC_CMD_REG, cmd);
    }
    tickdelay(micros_to_ticks(DELAY_US));
    tries++;
  }
  return 1;
}

int kbc_write_arg(uint8_t arg) {
  uint8_t st;
  int tries = 0;
  while (tries < KBC_MAX_TRIES) {
    if (kbc_read_status(&st)) return 1;
    if (!(st & KBC_IBF)) {
      return sys_outb(KBC_IN_BUF, arg);
    }
    tickdelay(micros_to_ticks(DELAY_US));
    tries++;
  }
  return 1;
}

int kbc_enable_int() {
  // Lê o Command Byte atual
  if (kbc_issue_cmd(KBC_READ_CMD)) return 1;
  uint8_t cmd_byte;
  if (kbc_read_output(&cmd_byte)) return 1;
  // Ativa o bit INT (bit 0)
  cmd_byte |= KBC_INT;
  // Escreve o novo Command Byte
  if (kbc_issue_cmd(KBC_WRITE_CMD)) return 1;
  if (kbc_write_arg(cmd_byte)) return 1;
  return 0;
}

void (kbc_ih)() {
  uint8_t st;
  scancode_ready = false;
  scancode_error = false;
  if (kbc_read_status(&st)) return;
  if (st & (KBC_PARITY | KBC_TIMEOUT)) {
    // lê mas descarta
    util_sys_inb(KBC_OUT_BUF, &scancode_byte);
    scancode_error = true;
    return;
  }
  if (st & KBC_OBF) {
    util_sys_inb(KBC_OUT_BUF, &scancode_byte);
    scancode_ready = true;
  }
}

uint8_t kbc_get_scancode() {
  return scancode_byte;
}

bool kbc_scancode_ready() {
  return scancode_ready;
}

bool kbc_has_error() {
  return scancode_error;
}

