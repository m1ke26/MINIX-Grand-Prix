// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include "video-card.h"
#include "kbc.h"

// Any header files included below this line should have been created by you


int main(int argc, char *argv[]) {
    // sets the language of LCF messages (can be either EN-US or PT-PT)
    lcf_set_language("EN-US");

    // enables to log function invocations that are being "wrapped" by LCF
    // [comment this out if you don't want/need it]
    lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

    // enables to save the output of printf function calls on a file
    // [comment this out if you don't want/need it]
    lcf_log_output("/home/lcom/labs/lab5/output.txt");

    // handles control over to LCF
    // [LCF handles command line arguments and invokes the right function]
    if (lcf_start(argc, argv))
        return 1;

    // LCF clean up tasks
    // [must be the last statement before return]
    lcf_cleanup();

    return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
    
    if (set_video_mode(mode) != 0) return 1;

    tickdelay(micros_to_ticks(delay * 1000000)); 

    if (vg_exit() != 0) return 1;

    return 0;
}

int (video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
   
    if(map_video_memory(mode) != 0) return 1;

    if(vg_draw_rect(x, y, width, height, color) != 0) return 1;

    uint8_t bit_no;
  uint32_t irq_set;
  if (kbd_subscribe_int(&bit_no)) return 1;
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

  if (kbd_unsubscribe_int()) return 1;
  if (vg_exit()) return 1;
  return 0;
    
    return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if (map_video_memory(0x105)) return 1;
  if (vg_draw_xpm(xpm, x, y)) return 1;

  uint8_t bit_no;
  uint32_t irq_set;
  if (kbd_subscribe_int(&bit_no)) return 1;
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

  if (kbd_unsubscribe_int()) return 1;
  if (vg_exit()) return 1;
  return 0;
}
