#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <stdint.h>
#include "rtc.h"


#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B

#define RTC_DAY 0x07
#define RTC_MONTH 0x08
#define RTC_YEAR 0x09

#define UIP (1 << 7)
#define DM  (1 << 2)

static int rtc_read(uint8_t reg, uint8_t *data) {

    if (sys_outb(RTC_ADDR_REG, reg) != 0)
        return 1;

    uint32_t tmp;
    if (sys_inb(RTC_DATA_REG, &tmp) != 0)
        return 1;

    *data = (uint8_t) tmp;

    return 0;
}

static uint8_t bcd_to_binary(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

int rtc_read_date(rtc_date *date) {

    uint8_t regA;

    do {
        rtc_read(RTC_REG_A, &regA);
    } while (regA & UIP);

    uint8_t regB;
    rtc_read(RTC_REG_B, &regB);

    uint8_t day, month, year;
    rtc_read(RTC_DAY, &day);
    rtc_read(RTC_MONTH, &month);
    rtc_read(RTC_YEAR, &year);

    if (!(regB & DM)) {
        day = bcd_to_binary(day);
        month = bcd_to_binary(month);
        year = bcd_to_binary(year);
    }

    date->day = day;
    date->month = month;
    date->year = year;

    return 0;
}