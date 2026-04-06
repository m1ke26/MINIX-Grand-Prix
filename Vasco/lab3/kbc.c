#include <lcom/lcf.h>
#include "kbc.h"

#include "i8042.h"

int hook_id = 3;
static uint8_t scancode;
static bool ih_error = false;

uint8_t kbc_get_scancode() { return scancode; }
bool kbc_get_ih_error()    { return ih_error; }

void (kbc_ih)(){
  uint8_t st;

  if(util_sys_inb(PORT_1,&st)) return ; // Read status byte

  if(!(st & BIT(0))) return ; // View if output buffer is full

  if (st & (KBC_ST_REG_TIMEOUT_ERR| KBC_ST_REG_PARITY_ERR)) { // Check hardware errors
    return ;
  }

  if (util_sys_inb(PORT_0, &scancode)) return;
}

int (kbd_subscribe_int)(uint8_t *bit_no){
  *bit_no = BIT(hook_id);
  if(sys_irqsetpolicy(KEYBOARD_IRQ,IRQ_REENABLE| IRQ_EXCLUSIVE,&hook_id) != 0 ) return 1;
  return 0;
}

int (kbd_unsubscribe_int)(){
  if (sys_irqrmpolicy(&hook_id) != 0) return 1;
  return 0;
}

