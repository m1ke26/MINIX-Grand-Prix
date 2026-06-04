#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#include <lcom/lcf.h>
#include <stdint.h>

/* PS/2 I/O ports */
#define KBC_STAT_REG  0x64
#define KBC_CMD_REG   0x64
#define KBC_OUT_BUF   0x60
#define KBC_IN_BUF    0x60

/* KBC status bits */
#define KBC_OBF     BIT(0)   /* Output Buffer Full  */
#define KBC_IBF     BIT(1)   /* Input Buffer Full   */
#define KBC_AUX     BIT(5)   /* Mouse data          */
#define KBC_TIMEOUT BIT(6)   /* Timeout error       */
#define KBC_PARITY  BIT(7)   /* Parity error        */

/* KBC/Mouse commands */
#define WRITE_MOUSE      0xD4
#define MOUSE_DISABLE_DR 0xF5
#define MOUSE_ACK        0xFA

/* Packet byte 0 bits */
#define LB         BIT(0)
#define RB         BIT(1)
#define MB         BIT(2)
#define MOUSE_SYNC BIT(3)
#define X_SIGN     BIT(4)
#define Y_SIGN     BIT(5)
#define X_OVF      BIT(6)
#define Y_OVF      BIT(7)

#define MOUSE_IRQ  12
#define DELAY_US   20000

int  mouse_subscribe_int(uint8_t *bit_no);
int  mouse_unsubscribe_int(void);
void (mouse_ih)(void);
int  mouse_write_cmd_poll(uint8_t cmd);
int  mouse_disable_data_reporting(void);
void mouse_sync_and_parse(struct packet *pp);
int  mouse_get_byte_count(void);
void mouse_set_byte_count(int count);
void mouse_increment_byte_count(void);

#endif /* _LCOM_MOUSE_H_ */
