#ifndef _LCOM_I8254_H_
#define _LCOM_I8254_H_

#include <lcom/lcf.h>

/** @defgroup i8254 i8254
 * @{
 *
 * Constants for programming the i8254 Timer. Needs to be completed.
 */

#define TIMER_FREQ 1193182 /**< @brief clock frequency for timer in PC and AT */
#define TIMER0_IRQ 0 /**< @brief Timer 0 IRQ line */

/* I/O port addresses */

#define TIMER_0    0x40 /**< @brief Timer 0 count register */
#define TIMER_1    0x41 /**< @brief Timer 1 count register */
#define TIMER_2    0x42 /**< @brief Timer 2 count register */
#define TIMER_CTRL 0x43 /**< @brief Control register */

#define SPEAKER_CTRL 0x61 /**< @brief Register for speaker control  */

/* Timer control */

/* Timer selection: bits 7 and 6 */

#define TIMER_SEL0   0x00              /**< @brief Control Word for Timer 0 */
#define TIMER_SEL1   BIT(6)            /**< @brief Control Word for Timer 1 */
#define TIMER_SEL2   BIT(7)            /**< @brief Control Word for Timer 2 */
#define TIMER_RB_CMD (BIT(7) | BIT(6)) /**< @brief Read Back Command */

/* Register selection: bits 5 and 4 */

#define TIMER_LSB     BIT(4)                  /**< @brief Initialize Counter LSB only */
#define TIMER_MSB     BIT(5)                  /**< @brief Initialize Counter MSB only */
#define TIMER_LSB_MSB (TIMER_LSB | TIMER_MSB) /**< @brief Initialize LSB first and MSB afterwards */

/* Operating mode: bits 3, 2 and 1 */

#define TIMER_SQR_WAVE (BIT(2) | BIT(1)) /**< @brief Mode 3: square wave generator */
#define TIMER_RATE_GEN BIT(2)            /**< @brief Mode 2: rate generator */

/* Counting mode: bit 0 */

#define TIMER_BCD 0x01 /**< @brief Count in BCD */
#define TIMER_BIN 0x00 /**< @brief Count in binary */

/* READ-BACK COMMAND FORMAT */

#define TIMER_RB_COUNT_  BIT(5)
#define TIMER_RB_STATUS_ BIT(4)
#define TIMER_RB_SEL(n)  BIT((n) + 1)

/* Timer Functions */

/**
 * @brief Set the frequency of a timer
 * 
 * @param timer Timer number (0, 1, or 2)
 * @param freq Desired frequency in Hz
 * @return 0 on success, 1 on error
 */
int timer_set_frequency(uint8_t timer, uint32_t freq);

/**
 * @brief Subscribe to timer interrupts
 * 
 * @param bit_no Pointer to store the bit number for this interrupt
 * @return 0 on success, 1 on error
 */
int timer_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribe from timer interrupts
 * 
 * @return 0 on success, 1 on error
 */
int timer_unsubscribe_int(void);

/**
 * @brief Timer interrupt handler
 */
void timer_int_handler(void);

/**
 * @brief Get the configuration status of a timer
 * 
 * @param timer Timer number (0, 1, or 2)
 * @param st Pointer to store the status byte
 * @return 0 on success, 1 on error
 */
int timer_get_conf(uint8_t timer, uint8_t *st);

/**
 * @brief Get the current timer counter value
 * 
 * @return Current counter value
 */
int timer_get_counter(void);

/**
 * @brief Reset the timer counter to 0
 */
void timer_reset_counter(void);

/**@}*/

#endif /* _LCOM_I8254_H */
