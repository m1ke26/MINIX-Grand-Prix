#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

#include "kbc.h"

#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_IN_BUF 0x60
#define KBC_OUT_BUF 0x60

#define KBC_ST_IBF BIT(1)  // Input buffer full
#define KBC_ST_OBF BIT(0)  // Output buffer full
#define KBC_CMD_READ_CMD 0x20
#define KBC_CMD_WRITE_CMD 0x60
#define KBC_PARITY_ERR BIT(7)
#define KBC_TIMEOUT_ERR BIT(6)

#define DELAY_US 20000

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

  uint8_t bit_no;

  if (kbd_subscribe_int(&bit_no) != 0) {
    return 1;
  }
  uint32_t irq_set = BIT(bit_no);

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

  if (kbd_unsubscribe_int() != 0) return 1;
  return 0;
}

int(kbd_test_poll)() {

    /*sys_outb(0x64, 0x20); // Read Command Byte (0x20 means we will be reading command_byte from port 0x60)
  uint8_t command_byte;
  util_sys_inb(0x60, &command_byte); // Actual reading 

  bool interrupts = command_byte & BIT(0); // Bit 0 of command byte tells if interrupts for keyboard are enabled

  if(interrupts){
    command_byte &= ~BIT(0); // Set bit 0 to 0 
    sys_outb(0x64, 0x60); // Tell the keyboard we will be writing a new command byte
    sys_outb(0x60, command_byte);
  }
  */ // Already done by lcf_start();


  //Read initial command byte
  uint8_t status;
 

  uint8_t bytes[2];
  int size = 0;
  bool running = true;

  while (running) {
   
    if (util_sys_inb(KBC_ST_REG, &status) != 0) continue;

    if (status & KBC_ST_OBF) {
      kbc_ih();

      bytes[size++] = scancode;

      if (size == 1 && scancode == 0xE0) continue;

      kbd_print_scancode(!(scancode & BIT(7)), size, bytes);

      if (scancode == 0x81) {
        running = false;
      }

      size = 0;
    }

    tickdelay(micros_to_ticks(DELAY_US)); // Small delay to avoid busy waiting too fast
  }
   //Read initial command byte
  sys_outb(0x64, 0x20); // Read Command Byte (0x20 means we will be reading command_byte from port 0x60)
  uint8_t command_byte; 
  while( 1 ) {
    util_sys_inb(KBC_ST_REG, &status);
     /* assuming it returns OK */
    /* loop while 8042 output buffer is empty */
    if(status & KBC_ST_OBF ) {
        util_sys_inb(0x60, &command_byte);
        if ( (status &(KBC_PARITY_ERR | KBC_TIMEOUT_ERR)) == 0 )
            break; /* success, no errors */
        else
            return -1;
    }
    tickdelay(micros_to_ticks(DELAY_US));
}
  //Activate interrupts
  command_byte |= BIT(0);
  sys_outb(0x64, 0x60);
  while ( 1 ) {
    util_sys_inb(KBC_ST_REG, &status); /* assuming it returns OK */
    /* loop while 8042 input buffer is not empty */
    if( (status & KBC_ST_IBF) == 0 ) {
          sys_outb(0x60, command_byte); /* no args command */
        return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US));
 
}
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t bit_no_timer;
  if (timer_subscribe_int(&bit_no_timer) != 0) return 1;
  uint32_t irq_set_timer = BIT(bit_no_timer);

  uint8_t bit_no;
  uint32_t irq_set;

  if (kbd_subscribe_int(&bit_no) != 0) {
    return 1;
  }
  irq_set = bit_no;

  int ipc_status;
  message msg;
  int r;

  bool running = true;
  uint8_t bytes[2];
  int size = 0;
  int timer_ticks = 0;

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
            
            // Reset the idle timer counter since a key was pressed
            timer_ticks = 0; 
            
            bytes[size] = scancode;
            size++;

            if (scancode == 0xE0) {
              continue; 
            }

            kbd_print_scancode(!(scancode & BIT(7)), size, bytes);
              
            if (scancode == 0x81) {
              running = false;
            }
            size = 0;
          }
          
          if (msg.m_notify.interrupts & irq_set_timer) {
            timer_ticks++;
      
            if (timer_ticks >= n * 60) {
              running = false;
            }
          }
          break;
        default:
          break;
      }
    }
  }

  if (timer_unsubscribe_int() != 0) return 1;
  if (kbd_unsubscribe_int() != 0) return 1;
  
  return 0;
}
