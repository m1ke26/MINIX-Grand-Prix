#include "rtc.h"
#include <minix/syslib.h>
#include "bitwise.h"

#define TODO return -1

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_REG_A    0x0A
#define RTC_REG_B    0x0B
#define RTC_REG_DAY  0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09
#define RTC_UIP_MSK  (1 << 7)
#define RTC_DM_MSK   (1 << 2)

// Converte BCD para binário
// Ex: 0x25 (BCD) -> 25 (decimal)
// Os 4 bits da esquerda são as dezenas, os 4 da direita são as unidades
static int bcd_to_bin(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

int rtc_read_date(rtc_date *date) {
    uint32_t val;

    // 1. Espera que o UIP (Update In Progress) esteja a 0
    // Se UIP=1 o RTC está a atualizar e os dados podem estar inconsistentes
    do {
        sys_outb(RTC_ADDR_REG, RTC_REG_A);
        sys_inb(RTC_DATA_REG, &val);
    } while (val & RTC_UIP_MSK);

    // 2. Lê o registo B para saber se os dados estão em BCD ou binário
    // Se o bit DM (bit 2) for 0, os dados estão em BCD
    sys_outb(RTC_ADDR_REG, RTC_REG_B);
    sys_inb(RTC_DATA_REG, &val);
    bool is_binary = val & RTC_DM_MSK;

    // 3. Lê o dia
    sys_outb(RTC_ADDR_REG, RTC_REG_DAY);
    sys_inb(RTC_DATA_REG, &val);
    date->day = is_binary ? val : bcd_to_bin(val);

    // 4. Lê o mês
    sys_outb(RTC_ADDR_REG, RTC_REG_MONTH);
    sys_inb(RTC_DATA_REG, &val);
    date->month = is_binary ? val : bcd_to_bin(val);

    // 5. Lê o ano
    sys_outb(RTC_ADDR_REG, RTC_REG_YEAR);
    sys_inb(RTC_DATA_REG, &val);
    date->year = is_binary ? val : bcd_to_bin(val);

    return 0;
}