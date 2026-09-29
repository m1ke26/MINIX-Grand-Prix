#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include "mouse.h"
#include "kbc.h"

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");
  lcf_log_output("/home/lcom/labs/lab4/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  lcf_cleanup();
  return 0;
}

int (mouse_test_packet)(uint32_t cnt) {
  uint8_t bit_no;
  uint32_t irq_set;

  if (mouse_enable_data_reporting()) return 1;
  if (mouse_subscribe_int(&bit_no)) return 1;
  irq_set = BIT(bit_no);

  uint32_t packets_received = 0;
  uint8_t packet_bytes[3];
  uint8_t byte_idx = 0;

  int ipc_status;
  message msg;

  while (packets_received < cnt) {
    if (driver_receive(ANY, &msg, &ipc_status)) continue;
    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & irq_set) {
          mouse_ih();
          if (!mouse_byte_ready() || mouse_has_error()) continue;

          uint8_t byte = mouse_get_byte();

          if (byte_idx == 0 && !(byte & MOUSE_BIT3)) continue;

          packet_bytes[byte_idx++] = byte;

          if (byte_idx == 3) {
            struct packet pp;
            mouse_parse_packet(packet_bytes, &pp);
            mouse_print_packet(&pp);
            packets_received++;
            byte_idx = 0;
          }
        }
      }
    }
  }

  if (mouse_unsubscribe_int()) return 1;
  if (mouse_disable_data_reporting()) return 1;
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  uint8_t mouse_bit, timer_bit;
  uint32_t irq_set_mouse, irq_set_timer;

  if (mouse_enable_data_reporting()) return 1;
  if (mouse_subscribe_int(&mouse_bit)) return 1;
  irq_set_mouse = BIT(mouse_bit);

  if (timer_subscribe_int(&timer_bit)) return 1;
  irq_set_timer = BIT(timer_bit);

  uint8_t packet_bytes[3];
  uint8_t byte_idx = 0;
  uint32_t idle_count = 0;
  bool done = false;

  int ipc_status;
  message msg;

  while (!done) {
    if (driver_receive(ANY, &msg, &ipc_status)) continue;
    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & irq_set_timer) {
          timer_int_handler();
          idle_count++;
          if (idle_count >= (uint32_t)(idle_time * 60)) {
            done = true;
            break;
          }
        }
        if (msg.m_notify.interrupts & irq_set_mouse) {
          mouse_ih();
          if (!mouse_byte_ready() || mouse_has_error()) continue;

          uint8_t byte = mouse_get_byte();

          if (byte_idx == 0 && !(byte & MOUSE_BIT3)) continue;

          packet_bytes[byte_idx++] = byte;
          idle_count = 0;

          if (byte_idx == 3) {
            struct packet pp;
            mouse_parse_packet(packet_bytes, &pp);
            mouse_print_packet(&pp);
            byte_idx = 0;
          }
        }
      }
    }
  }

  printf("unsubscribing mouse\n");
  if (mouse_unsubscribe_int()) return 1;
  printf("unsubscribing timer\n");
  if (timer_unsubscribe_int()) return 1;
  printf("disabling data reporting\n");
  if (mouse_disable_data_reporting()) return 1;
  printf("done\n");
  return 0;
}
