#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int counter = 0;
int hook_id = 2;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if(timer > 2 ) return 1;
  uint8_t config;
  if(timer_get_conf(timer,&config)!=0) return 1;

  uint16_t div = TIMER_FREQ / freq;

  uint8_t cmd = (timer << 6) | TIMER_LSB_MSB | (config & 0x0F);
  if (sys_outb(TIMER_CTRL , cmd) != 0) return 1;

  uint8_t lsb, msb;
  if (util_get_LSB(div, &lsb) != 0) return 1;
  if (util_get_MSB(div, &msb) != 0) return 1;

  if(sys_outb(TIMER_0 + timer, lsb)) return 1;
  if(sys_outb(TIMER_0 + timer, msb)) return 1;

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) { 
  *bit_no = hook_id;
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0) return 1;
  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != 0) return 1;
  return 0;
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (timer > 2 || st ==  NULL) return 1;

  uint8_t read_back = TIMER_RB_CMD | TIMER_RB_COUNT_| TIMER_RB_SEL(timer);

  if (sys_outb(TIMER_CTRL, read_back) != 0) return 1; // Write to control register

  uint8_t port;
  switch (timer){
    case 0: 
      port = TIMER_0;
      break;
    case 1: 
      port = TIMER_1;
      break;
    case 2: 
      port = TIMER_2;
      break;
    default: return 1;
  }

  if (util_sys_inb(port,st) != 0) return 1; // Read st byte
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  union timer_status_field_val val;

  switch(field) {

    case tsf_all:
      val.byte = st;
      break;

    case tsf_initial:
      val.in_mode = (st >> 4) & 0x03;
      break;

    case tsf_mode:
      val.count_mode = (st >> 1) & 0x07;
      if (val.count_mode == 6) val.count_mode = 2;
      if (val.count_mode == 7) val.count_mode = 3;
      break;

    case tsf_base:
      val.bcd = st & 0x01;
      break;

    default:
      return 1;
  }
  return timer_print_config(timer, field, val);
}

