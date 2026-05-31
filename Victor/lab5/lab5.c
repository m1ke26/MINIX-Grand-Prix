#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <stdint.h>
#include <stdio.h>
#include "video_gr.h"
#include "kbc.h"

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");
  lcf_log_output("/home/lcom/labs/lab5/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  lcf_cleanup();
  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (video_init(mode)) return 1;
  tickdelay(micros_to_ticks(1000000 * delay));
  if (vg_exit()) return 1;
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  if (video_init(mode)) return 1;
  if (vg_draw_rectangle(x, y, width, height, color)) return 1;

  uint8_t bit_no;
  uint32_t irq_set;
  if (kbc_subscribe_int(&bit_no)) return 1;
  irq_set = BIT(bit_no);

  bool done = false;
  int ipc_status;
  message msg;

  while (!done) {
    if (driver_receive(ANY, &msg, &ipc_status)) continue;
    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & irq_set) {
          kbc_ih();
          if (!kbc_scancode_ready() || kbc_has_error()) continue;
          if (kbc_get_scancode() == ESC_BREAK) done = true;
        }
      }
    }
  }

  if (kbc_unsubscribe_int()) return 1;
  if (vg_exit()) return 1;
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if (video_init(0x105)) return 1;
  if (vg_draw_xpm(xpm, x, y)) return 1;

  uint8_t bit_no;
  uint32_t irq_set;
  if (kbc_subscribe_int(&bit_no)) return 1;
  irq_set = BIT(bit_no);

  bool done = false;
  int ipc_status;
  message msg;

  while (!done) {
    if (driver_receive(ANY, &msg, &ipc_status)) continue;
    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & irq_set) {
          kbc_ih();
          if (!kbc_scancode_ready() || kbc_has_error()) continue;
          if (kbc_get_scancode() == ESC_BREAK) done = true;
        }
      }
    }
  }

  if (kbc_unsubscribe_int()) return 1;
  if (vg_exit()) return 1;
  return 0;
}
