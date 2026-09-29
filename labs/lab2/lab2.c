#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");
  lcf_log_output("/home/lcom/labs/lab2/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  lcf_cleanup();
  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field)
{
  uint8_t st;
  if (timer_get_conf(timer, &st))
    return 1;
  if (timer_display_conf(timer, st, field))
    return 1;
  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq)
{
  if (timer_set_frequency(timer, freq))
    return 1;
  return 0;
}

int(timer_test_int)(uint8_t time)
{
  uint8_t bit_no;
  uint32_t irq_set;

  if (timer_subscribe_int(&bit_no))
    return 1;
  irq_set = BIT(bit_no);

  uint32_t counter = 0;
  // TIMER_FREQ / (divisor padrão 0 = 65536) ≈ 18.2 Hz, mas o LCF usa 60 Hz
  int ipc_status;
  message msg;

  while (counter < (uint32_t)(time * 60))
  {
    if (driver_receive(ANY, &msg, &ipc_status))
      continue;
    if (is_ipc_notify(ipc_status))
    {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE)
      {
        if (msg.m_notify.interrupts & irq_set)
        {
          timer_int_handler();
          counter++;
          if (counter % 60 == 0)
            timer_print_elapsed_time();
        }
      }
    }
  }

  if (timer_unsubscribe_int())
    return 1;
  return 0;
}

