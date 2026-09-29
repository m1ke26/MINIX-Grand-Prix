#pragma once
#include <lcom/lcf.h>
#include <stdint.h>

#define KBC_ST_REG    0x64
#define KBC_CMD_REG   0x64
#define KBC_OUT_BUF   0x60
#define KBC_IN_BUF    0x60
#define KBC_IRQ       1

#define KBC_OBF       BIT(0)
#define KBC_IBF       BIT(1)
#define KBC_AUX       BIT(5)
#define KBC_PARITY    BIT(7)
#define KBC_TIMEOUT   BIT(6)

#define KBC_READ_CMD  0x20
#define KBC_WRITE_CMD 0x60
#define KBC_INT       BIT(0)

#define ESC_BREAK     0x81
#define TWO_BYTE_CODE 0xE0

#define DELAY_US      20000
#define KBC_MAX_TRIES 10

int kbc_subscribe_int(uint8_t *bit_no);
int kbc_unsubscribe_int();
void (kbc_ih)();
int kbc_read_status(uint8_t *st);
int kbc_read_output(uint8_t *data);
int kbc_issue_cmd(uint8_t cmd);
int kbc_write_arg(uint8_t arg);
int kbc_enable_int();

uint8_t kbc_get_scancode();
bool kbc_scancode_ready();
bool kbc_has_error();
