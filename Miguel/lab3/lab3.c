#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>
#include "kbc.h"

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");
  lcf_log_output("/home/lcom/labs/lab3/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  lcf_cleanup();
  return 0;
}

int(kbd_test_scan)() {
  uint8_t bit_no;
  uint32_t irq_set;
  if (kbc_subscribe_int(&bit_no)) return 1;
  irq_set = BIT(bit_no);

  bool done = false;
  bool two_byte = false;
  uint8_t bytes[2];
  uint8_t size = 0;

  int ipc_status;
  message msg;

  while (!done) {
    if (driver_receive(ANY, &msg, &ipc_status)) continue;
    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & irq_set) {
          kbc_ih();
          if (!kbc_scancode_ready() || kbc_has_error()) continue;

          uint8_t byte = kbc_get_scancode();

          if (byte == TWO_BYTE_CODE) {
            two_byte = true;
            bytes[0] = byte;
            size = 1;
          } else {
            if (two_byte) {
              bytes[1] = byte;
              size = 2;
              two_byte = false;
            } else {
              bytes[0] = byte;
              size = 1;
            }
            bool make = !(bytes[size - 1] & BIT(7));
            kbd_print_scancode(make, size, bytes);
            if (size == 1 && bytes[0] == ESC_BREAK) done = true;
          }
        }
      }
    }
  }

  if (kbc_unsubscribe_int()) return 1;
  return 0;
}

int(kbd_test_poll)() {
  bool done = false;
  bool two_byte = false;
  uint8_t bytes[2];
  uint8_t size = 0;

#ifdef LAB3
  uint32_t sys_inb_count = 0;
#endif

  while (!done) {
    uint8_t st;
    if (kbc_read_status(&st)) continue;
#ifdef LAB3
    sys_inb_count++;
#endif

    if (!(st & KBC_OBF)) {
      tickdelay(micros_to_ticks(DELAY_US));
      continue;
    }
    if (st & (KBC_PARITY | KBC_TIMEOUT)) {
      uint8_t discard;
      util_sys_inb(KBC_OUT_BUF, &discard);
#ifdef LAB3
      sys_inb_count++;
#endif
      continue;
    }

    uint8_t byte;
    if (util_sys_inb(KBC_OUT_BUF, &byte)) continue;
#ifdef LAB3
    sys_inb_count++;
#endif

    if (byte == TWO_BYTE_CODE) {
      two_byte = true;
      bytes[0] = byte;
      size = 1;
    } else {
      if (two_byte) {
        bytes[1] = byte;
        size = 2;
        two_byte = false;
      } else {
        bytes[0] = byte;
        size = 1;
      }
      bool make = !(bytes[size - 1] & BIT(7));
      kbd_print_scancode(make, size, bytes);
      if (size == 1 && bytes[0] == ESC_BREAK) done = true;
    }
  }

  kbc_enable_int();

#ifdef LAB3
  kbd_print_no_sysinb(sys_inb_count);
#endif

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t kbc_bit, timer_bit;
  uint32_t irq_set_kbc, irq_set_timer;

  if (kbc_subscribe_int(&kbc_bit)) return 1;
  irq_set_kbc = BIT(kbc_bit);

  if (timer_subscribe_int(&timer_bit)) return 1;
  irq_set_timer = BIT(timer_bit);

  bool done = false;
  bool two_byte = false;
  uint8_t bytes[2];
  uint8_t size = 0;
  uint32_t idle_count = 0;

  int ipc_status;
  message msg;

  while (!done) {
    if (driver_receive(ANY, &msg, &ipc_status)) continue;
    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & irq_set_timer) {
          timer_int_handler();
          idle_count++;
          if (idle_count >= (uint32_t)(n * 60)) done = true;
        }
        if (msg.m_notify.interrupts & irq_set_kbc) {
          kbc_ih();
          if (!kbc_scancode_ready() || kbc_has_error()) continue;

          uint8_t byte = kbc_get_scancode();
          idle_count = 0;

          if (byte == TWO_BYTE_CODE) {
            two_byte = true;
            bytes[0] = byte;
            size = 1;
          } else {
            if (two_byte) {
              bytes[1] = byte;
              size = 2;
              two_byte = false;
            } else {
              bytes[0] = byte;
              size = 1;
            }
            bool make = !(bytes[size - 1] & BIT(7));
            kbd_print_scancode(make, size, bytes);
            if (size == 1 && bytes[0] == ESC_BREAK) done = true;
          }
        }
      }
    }
  }

  if (kbc_unsubscribe_int()) return 1;
  if (timer_unsubscribe_int()) return 1;
  return 0;
}
