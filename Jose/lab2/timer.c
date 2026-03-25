#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int counter = 0;
int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t st;
  if(timer_get_conf(timer, &st) != 0) {
    return 1;
  }
  //Get the operating mode and couting base from the status byte and force TIMER_LSB_MSB since we will be writing 2 bytes
  //Then construct the control word combining it with the timr selectiion bits
  uint8_t control_word = 0x00;
  if(timer == 0){
    control_word |= TIMER_SEL0 | (TIMER_LSB_MSB) | (st & BIT(0)) | (st & (BIT(3) | BIT(2) | BIT(1))); 

  }
  else if(timer == 1){
    control_word |= TIMER_SEL1 | (TIMER_LSB_MSB) | (st & BIT(0)) | (st & (BIT(3) | BIT(2) | BIT(1))); 

  }
  else if(timer == 2){
    control_word |= TIMER_SEL2 | (TIMER_LSB_MSB) | (st & BIT(0)) | (st & (BIT(3) | BIT(2) | BIT(1))); 

  }
  else{
    return 1; 
  }
  if(sys_outb(TIMER_CTRL, control_word)) // Write the new control word
    return 1;

  uint16_t divisor = TIMER_FREQ / freq;
  uint8_t lsb, msb;

  //Since sys_outb only accepts 8 bit values 
  //Need to split the 16 bit divisor into two 8 bit values and write them separately
  
  util_get_LSB(divisor, &lsb);
  util_get_MSB(divisor, &msb);

  if(sys_outb(TIMER_0 + timer, lsb))
    return 1;
  if(sys_outb(TIMER_0 + timer, msb))
    return 1;

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  int hook_id = 2; 
  *bit_no = (uint8_t)hook_id;
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0) {
    return 1;
  }

  return 0;
}

int (timer_unsubscribe_int)() {
  int hook_id = 2;
  if (sys_irqrmpolicy(&hook_id) != 0) {
    return 1;
  }
  return 0;
}

void (timer_int_handler)() {
  counter++;

}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
   // Read back command (TIMMER_RB_COUNT_ pois quando o seu valor é 1 este nao é selecionado e o o bit do status fica a 0 sendo selecionado)
    //Logica invertida para selecionar o bit do status e nao o do contador
   if(sys_outb(TIMER_CTRL, cmd))
    return 1;
  if(util_sys_inb(TIMER_0 + timer, st))
    return 1;
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  union timer_status_field_val conf;

  switch (field) {
    case tsf_all:
      conf.byte = st;
      break;
    case tsf_initial:
      conf.in_mode = (st & (BIT(5) | BIT(4))) >> 4;
      break;
    case tsf_mode:
      conf.count_mode = (st & (BIT(3) | BIT(2) | BIT(1))) >> 1;
      break;
    case tsf_base:
      conf.bcd = st & BIT(0);
      break;
    default:
      return 1; // Invalid field
  }

  return timer_print_config(timer, field, conf);
}
