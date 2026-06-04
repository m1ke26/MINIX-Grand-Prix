#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"

static int timer_hook_id = 0;
static uint32_t timer_counter = 0;

int(timer_set_frequency)(uint8_t timer, uint32_t freq)
{
  // Frequência tem de ser válida
  if (freq < 19 || freq > TIMER_FREQ)
    return 1;

  // Lê configuração atual com read-back command para preservar modo e BCD
  uint8_t st;
  if (timer_get_conf(timer, &st))
    return 1;

  // Calcula o divisor
  uint32_t div = TIMER_FREQ / freq;

  // Constrói o control word
  // Bits 7-6: seleciona timer, Bits 5-4: LSB+MSB, Bits 3-1: modo, Bit 0: BCD/bin
  uint8_t timer_sel;
  uint8_t timer_port;
  switch (timer)
  {
  case 0:
    timer_sel = TIMER_SEL0;
    timer_port = TIMER_0;
    break;
  case 1:
    timer_sel = TIMER_SEL1;
    timer_port = TIMER_1;
    break;
  case 2:
    timer_sel = TIMER_SEL2;
    timer_port = TIMER_2;
    break;
  default:
    return 1;
  }

  // Preserva os 4 LSBs do status (modo e BCD) e define LSB+MSB
  uint8_t ctrl = timer_sel | TIMER_LSB_MSB | (st & 0x0F);
  if (sys_outb(TIMER_CTRL, ctrl))
    return 1;

  // Escreve LSB e MSB do divisor
  uint8_t lsb, msb;
  if (util_get_LSB(div, &lsb))
    return 1;
  if (util_get_MSB(div, &msb))
    return 1;
  if (sys_outb(timer_port, lsb))
    return 1;
  if (sys_outb(timer_port, msb))
    return 1;

  return 0;
}

int(timer_subscribe_int)(uint8_t *bit_no)
{
  if (bit_no == NULL)
    return 1;
  *bit_no = timer_hook_id;
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id);
}

int(timer_unsubscribe_int)()
{
  return sys_irqrmpolicy(&timer_hook_id);
}

void(timer_int_handler)()
{
  timer_counter++;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st)
{
  if (st == NULL)
    return 1;

  // Read-back command: lê status do timer pedido
  uint8_t rb_cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  if (sys_outb(TIMER_CTRL, rb_cmd))
    return 1;

  // Lê o status do porto do timer
  uint8_t timer_port;
  switch (timer)
  {
  case 0:
    timer_port = TIMER_0;
    break;
  case 1:
    timer_port = TIMER_1;
    break;
  case 2:
    timer_port = TIMER_2;
    break;
  default:
    return 1;
  }

  return util_sys_inb(timer_port, st);
}

int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field)
{
  union timer_status_field_val val;

  switch (field)
  {
  case tsf_all:
    val.byte = st;
    break;
  case tsf_initial:
    // Bits 5-4 do status byte
    switch ((st >> 4) & 0x03)
    {
    case 1:
      val.in_mode = LSB_only;
      break;
    case 2:
      val.in_mode = MSB_only;
      break;
    case 3:
      val.in_mode = MSB_after_LSB;
      break;
    default:
      val.in_mode = INVAL_val;
      break;
    }
    break;
  case tsf_mode:
    // Bits 3-1 do status byte
    val.count_mode = (st >> 1) & 0x07;
    // Modo 6 e 7 são alias de 2 e 3
    if (val.count_mode == 6)
      val.count_mode = 2;
    if (val.count_mode == 7)
      val.count_mode = 3;
    break;
  case tsf_base:
    // Bit 0 do status byte
    val.bcd = st & 0x01;
    break;
  default:
    return 1;
  }

  return timer_print_config(timer, field, val);
}
