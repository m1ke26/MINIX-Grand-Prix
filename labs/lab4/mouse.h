#pragma once
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdbool.h>

#define MOUSE_IRQ       12
#define MOUSE_WRITE_CMD 0xD4
#define MOUSE_ENABLE    0xF4
#define MOUSE_DISABLE   0xF5
#define MOUSE_ACK       0xFA
#define MOUSE_ERROR     0xFE
#define MOUSE_BIT3      BIT(3)

int mouse_subscribe_int(uint8_t *bit_no);
int mouse_unsubscribe_int();
void (mouse_ih)();
int mouse_write_cmd(uint8_t cmd);
int mouse_disable_data_reporting();
bool mouse_byte_ready();
bool mouse_has_error();
uint8_t mouse_get_byte();
int mouse_parse_packet(uint8_t *bytes, struct packet *pp);
