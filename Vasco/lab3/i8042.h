#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KBC_ST_REG_TIMEOUT_ERR BIT(6)
#define KBC_ST_REG_PARITY_ERR BIT(7)

#define PORT_0 0x60
#define PORT_1 0x64

#define READ_CMD 0x20
#define WRITE_CMD 0x60

#endif

