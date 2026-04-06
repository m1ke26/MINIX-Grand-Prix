#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}



int(kbd_test_scan)() {

  int hook_id = 0;
  uint8_t bit_no = hook_id;
  uint32_t irq_set = BIT(bit_no);

  // Match the type declared in kbc.c
  extern uint8_t scancode;

  if (sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != 0) {
    return 1;
  }


  int ipc_status;
  message msg;
  int r;

  bool running = true;

  uint8_t bytes[2];
  int size = 0;

  while (running) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();
            
            bytes[size] = scancode;
            size++;

            if (scancode == 0xE0) {
              // Wait for next interrupt for the second byte
              continue; 
            }

            kbd_print_scancode(!(scancode & BIT(7)), size, bytes);
              // Exit on ESC break code (single-byte: 0x81)
              
            if (scancode == 0x81) {
              running = false;
              }
            size = 0;
            }
          
          break;
        default:
          break;
      }
    }
  }

  if (sys_irqrmpolicy(&hook_id) != 0) return 1;
  return 0;
}

int(kbd_test_poll)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
