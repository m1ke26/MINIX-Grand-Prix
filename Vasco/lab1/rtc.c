#include "rtc.h"
#include <stdbool.h>
#include <minix/syslib.h>
#include <stdio.h>
#include <minix/sysutil.h>


#define TODO return -1

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_REG_DAY 0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09
#define RTC_UIP_MSK (1 << 7)
#define RTC_DM_MSK (1 << 2)

int util_sys_inb(int port, uint8_t *value) {
  uint32_t a;

  if (sys_inb(port, &a) != 0)
    return 1;

  *value = (uint8_t) a;
  return 0;
}

static int bcd_to_bin(uint8_t bcd) {
  int mul = 1;
  int result = 0;
  while(bcd > 0){
    result += (uint8_t)(bcd & 0xF) * mul;
    bcd >>= 4;
    mul *= 10;
  }
  return result;
}


int rtc_read_date(rtc_date *date) { 
  uint8_t regA , regB;

  while(true){
    if(sys_outb(RTC_ADDR_REG, RTC_REG_A)) return -1;
    tickdelay(micros_to_ticks(500));
    if(util_sys_inb(RTC_DATA_REG, &regA)) return -1;
    if(!(regA & RTC_UIP_MSK)){
      break;
    }
  }

  uint8_t day,month,year;

  if (sys_outb(RTC_ADDR_REG, RTC_REG_DAY)) return 1;
  if (util_sys_inb(RTC_DATA_REG, &day)) return 1;

  if (sys_outb(RTC_ADDR_REG, RTC_REG_MONTH)) return 1;
  if (util_sys_inb(RTC_DATA_REG, &month)) return 1;

  if (sys_outb(RTC_ADDR_REG, RTC_REG_YEAR)) return 1;
  if (util_sys_inb(RTC_DATA_REG, &year)) return 1;


  if (sys_outb(RTC_ADDR_REG, RTC_REG_B)) return 1;
  if (util_sys_inb(RTC_DATA_REG, &regB)) return 1;

  if (!(regB & RTC_DM_MSK)) {
    day = bcd_to_bin(day);
    month = bcd_to_bin(month);
    year = bcd_to_bin(year);
  }

  date->day = day;
  date->month = month;
  date->year = year;

  return 0;
}
  
