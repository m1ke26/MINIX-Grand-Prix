#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>

/**
 * @brief Returns the least significant byte of a 16-bit value
 * 
 * @param val 16-bit value
 * @param lsb Pointer to the 8-bit variable to store the LSB
 * @return 0 on success, non-zero otherwise
 */
int (util_get_LSB)(uint16_t val, uint8_t *lsb);

/**
 * @brief Returns the most significant byte of a 16-bit value
 * 
 * @param val 16-bit value
 * @param msb Pointer to the 8-bit variable to store the MSB
 * @return 0 on success, non-zero otherwise
 */
int (util_get_MSB)(uint16_t val, uint8_t *msb);

/**
 * @brief Reads a byte from a port using sys_inb, avoiding the 32-bit width issue
 * 
 * @param port I/O port to read from
 * @param buff Pointer to the 8-bit variable to store the value read
 * @return 0 on success, non-zero otherwise
 */
int (util_sys_inb)(int port, uint8_t *buff);

#endif /* _UTILS_H_ */
