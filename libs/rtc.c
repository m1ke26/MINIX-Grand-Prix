#include <stdbool.h>
#include <minix/syslib.h>
#include <stdio.h>
#include <minix/sysutil.h>
#include "rtc.h"
#include "utils.h"

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

  //Check if 0x0B bit 2 is set to determine if the RTC is in BCD mode

  if(sys_outb(0x70, RTC_REG_B)) return -1;
  uint8_t reg_b;
  if(util_sys_inb(0x71, &reg_b)) return -1;
  bool bcd_mode = !(reg_b & RTC_DM_MSK);

  //Wait until the Update In Progress (UIP) bit is clear

  if(sys_outb(0x70, RTC_REG_A)) return -1;
  uint8_t reg_a;
  while(true){
    tickdelay(micros_to_ticks(500)); // Wait for 500 microseconds before checking again
    if(util_sys_inb(0x71, &reg_a)) return -1;
    if(!(reg_a & RTC_UIP_MSK)){
      break;
    }
  }

  //Read the date values from the RTC registers
  //Day
  if(sys_outb(0x70, RTC_REG_DAY)) return -1;
  uint8_t day;
  if(util_sys_inb(0x71, &day)) return -1;
  date->day = bcd_mode ? bcd_to_bin(day) : day;

  //Month
  if(sys_outb(0x70, RTC_REG_MONTH)) return -1;
  uint8_t month;
  if(util_sys_inb(0x71, &month)) return -1;
  date->month = bcd_mode ? bcd_to_bin(month) : month;

  //Year
  if(sys_outb(0x70, RTC_REG_YEAR)) return -1;
  uint8_t year;
  if(util_sys_inb(0x71, &year)) return -1;
  date->year = bcd_mode ? bcd_to_bin(year) : year;

  return 0;
}