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

/**
 * @brief Subscribes to mouse interrupts.
 * @param bit_no Pointer to the bit number for the interrupt.
 * @return 0 on success, -1 on failure.
 */
int  mouse_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes from mouse interrupts.
 * @return 0 on success, -1 on failure.
 */
int  mouse_unsubscribe_int(void);

/**
 * @brief Mouse interrupt handler.
 */
void (mouse_ih)(void);

/**
 * @brief Writes a command to the mouse and polls for a response.
 * @param cmd The command to write.
 * @return 0 on success, -1 on failure.
 */
int  mouse_write_cmd_poll(uint8_t cmd);
/**
 * @brief Disables mouse data reporting.
 * @return 0 on success, -1 on failure.
 */
int  mouse_disable_data_reporting(void);
/**
 * @brief Synchronizes and parses the mouse packet.
 * @param pp Pointer to the packet structure to populate.
 */
void mouse_sync_and_parse(struct packet *pp);
/**
 * @brief Gets the number of bytes received from the mouse.
 * @return The number of bytes received.
 */
int  mouse_get_byte_count(void);
/**
 * @brief Sets the number of bytes received from the mouse.
 * @param count The number of bytes to set.
 */
void mouse_set_byte_count(int count);
/**
 * @brief Increments the byte count of received mouse bytes by one.
 */
void mouse_increment_byte_count(void);

#endif /* _LCOM_MOUSE_H_ */
