// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "mouse.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  uint8_t bit_no;

  if(mouse_enable_data_reporting() != 0) {
    return 1;
  }

  if (mouse_subscribe_int(&bit_no) != 0) {
    return 1;
  }
  uint32_t irq_set = BIT(bit_no);

  int ipc_status;
  message msg;
  int r;

  uint32_t packet_count = 0;
  extern int mouse_byte_count; 



  while (packet_count < cnt) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();
            // If we have 3 bytes, we have a full packet
            if (mouse_byte_count == 3) { 
              struct packet pp;
              mouse_sync_and_parse(&pp);
              mouse_print_packet(&pp);
              packet_count++;
              mouse_byte_count = 0; 
            }
          }
          break;
        default: break;
      }
    }
  }

  if (mouse_unsubscribe_int() != 0) return 1;
  if (mouse_disable_data_reporting() != 0) return 1;

  return 0;
  
}


int (mouse_test_async)(uint8_t idle_time) {
    uint8_t m_bit_no, t_bit_no;

  if (mouse_enable_data_reporting() != 0) return 1;
  if (mouse_subscribe_int(&m_bit_no) != 0) return 1;
  if (timer_subscribe_int(&t_bit_no) != 0) return 1;

  uint32_t m_irq_set = BIT(m_bit_no);
  uint32_t t_irq_set = BIT(t_bit_no);
  
  int ipc_status, r;
  message msg;
  int timer_counter = 0;
  extern int mouse_byte_count; 


  while (timer_counter < idle_time * 60) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & t_irq_set) {
            timer_counter++;
          }
          if (msg.m_notify.interrupts & m_irq_set) {
            mouse_ih();
            if (mouse_byte_count == 3) {
              struct packet pp;
              mouse_sync_and_parse(&pp);
              mouse_print_packet(&pp);
              timer_counter = 0;
              mouse_byte_count = 0; 
            }
          }
          break;
        default: break;
      }
    }
  }

  if (timer_unsubscribe_int() != 0) return 1;
  if (mouse_unsubscribe_int() != 0) return 1;
  if (mouse_disable_data_reporting() != 0) return 1;

  return 0;
}
