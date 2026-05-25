#include <lcom/lcf.h>
#include <lcom/timer.h>

#include "kbc.h"
#include "mouse.h"
#include "video-card.h"
#include "start_menu.h"
#include "font.h"
#include "button.h"
#include "pixmaps.h"
#include "state.h"
#include "track.h"

bool running = true;
int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");

  if (lcf_start(argc, argv))
    return 1;

  lcf_cleanup();
  return 0;
}

int (proj_main_loop)(int argc, char *argv[]) {
  uint8_t bit_no_timer, bit_no_kbd, bit_no_mouse;
  int cursor_x = 400;
  int cursor_y = 300;

  if (timer_subscribe_int(&bit_no_timer)) return 1;
  if (kbd_subscribe_int(&bit_no_kbd)) return 1;
  if (mouse_subscribe_int(&bit_no_mouse)) return 1;

  mouse_write_cmd_poll(0xF4);

  if (map_video_memory(0x115)) return 1;
  if (vg_init_double_buffer(800, 600, 3)) return 1;

  uint32_t irq_set_timer = BIT(bit_no_timer);
  uint32_t irq_set_kbd   = BIT(bit_no_kbd);
  uint32_t irq_set_mouse = BIT(bit_no_mouse);

  font_t *font = font_create();
  if (font == NULL) return 1;

  state_t *state = init_state();
  if (state == NULL) {font_destroy(font); return 1; }

  state->data.start.menu = start_menu_create(font, (xpm_map_t) button_normal_xpm, (xpm_map_t) button_hover_xpm);
  if (state->data.start.menu == NULL) { font_destroy(font); destroy_state(state); return 1;}


  int ipc_status;
  message msg;
  struct packet pp;

  while (running) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (!is_ipc_notify(ipc_status)) continue;

    switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & irq_set_timer) {
          timer_int_handler();
          update_state(state);
          draw_state(state);
          vg_buf_swap();
        }

        if (msg.m_notify.interrupts & irq_set_kbd) {
          kbc_ih();
          if (kbc_scancode_ready()) {
            uint8_t scancode = kbc_get_scancode();
            if (scancode == ESC_BREAK) running = false;
            handle_kbd_event(state, scancode);
          }
        }

        if (msg.m_notify.interrupts & irq_set_mouse) {
          mouse_ih();
          mouse_sync_and_parse(&pp);
          if (mouse_get_byte_count() == 3) {
            mouse_set_byte_count(0);
            cursor_x += pp.delta_x;
            cursor_y -= pp.delta_y;
            if (cursor_x < 0)   cursor_x = 0;
            if (cursor_x > 799) cursor_x = 799;
            if (cursor_y < 0)   cursor_y = 0;
            if (cursor_y > 599) cursor_y = 599;

            handle_mouse_event(state, &pp, cursor_x, cursor_y);
          }
        }
        break;

      default:
        break;
    }
  }

  if (state->tag == STATE_START) {
    start_menu_destroy(state->data.start.menu);
  }
  track_free();
  destroy_state(state);
  font_destroy(font);
  vg_free_double_buffer();
  vg_exit();
  mouse_disable_data_reporting();
  mouse_unsubscribe_int();
  kbd_unsubscribe_int();
  timer_unsubscribe_int();

  return 0;
}
