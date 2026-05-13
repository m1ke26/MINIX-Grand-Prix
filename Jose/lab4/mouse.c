#include <lcom/lcf.h>
#include "mouse.h"

static int hook_id = 4;
uint8_t mouse_bytes[3]; 
int mouse_byte_count = 0;

int mouse_subscribe_int(uint8_t *bit_no) {
    *bit_no = hook_id; 
    return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

int mouse_unsubscribe_int() {
    return sys_irqrmpolicy(&hook_id);
}

void (mouse_ih)(void) {
    uint8_t status, data;
    
    if (util_sys_inb(KBC_STAT_REG, &status) != 0) return;

    if (status & KBC_OBF) {
        if (util_sys_inb(KBC_OUT_BUF, &data) != 0) return;

        if (status & (KBC_PARITY | KBC_TIMEOUT)) return;

        if (!(status & KBC_AUX)) return;

        // Sync: Byte 0 must have BIT(3) set.
        if (mouse_byte_count == 0 && !(data & MOUSE_SYNC)) return;

        mouse_bytes[mouse_byte_count++] = data;
    }
}

static int kbc_write_cmd(uint8_t cmd) {
    int retries = 3;
    while (retries--) {
        uint8_t status;
        if (util_sys_inb(KBC_STAT_REG, &status) != 0) return 1;
        if (!(status & KBC_IBF)) {
            if (sys_outb(KBC_CMD_REG, cmd) == 0) return 0;
        }
        tickdelay(micros_to_ticks(DELAY_US));
    }
    return 1;
}

static int kbc_write_data(uint8_t data) {
    int retries = 3;
    while (retries--) {
        uint8_t status;
        if (util_sys_inb(KBC_STAT_REG, &status) != 0) return 1;
        if (!(status & KBC_IBF)) {
            if (sys_outb(KBC_IN_BUF, data) == 0) return 0;
        }
        tickdelay(micros_to_ticks(DELAY_US));
    }
    return 1;
}

static int kbc_read_data(uint8_t *data) {
    int retries = 3;
    while (retries--) {
        uint8_t status;
        if (util_sys_inb(KBC_STAT_REG, &status) != 0) return 1;
        if (status & KBC_OBF) {
            if (!(status & (KBC_TIMEOUT | KBC_PARITY))) {
                if (util_sys_inb(KBC_OUT_BUF, data) == 0) return 0;
            } else {
                uint8_t trash;
                util_sys_inb(KBC_OUT_BUF, &trash);
            }
        }
        tickdelay(micros_to_ticks(DELAY_US));
    }
    return 1;
}

int mouse_write_cmd_poll(uint8_t cmd) {
    uint8_t ack;
    for (int i = 0; i < 3; i++) {
        if (kbc_write_cmd(WRITE_MOUSE) != 0) continue;
        if (kbc_write_data(cmd) != 0) continue;
        if (kbc_read_data(&ack) != 0) continue;
        if (ack == MOUSE_ACK) return 0;
        if (ack == 0xFC) return 1; // Error
        tickdelay(micros_to_ticks(DELAY_US));
    }
    return 1;
}

int mouse_disable_data_reporting() {
    return mouse_write_cmd_poll(MOUSE_DISABLE_DR);
}

void mouse_sync_and_parse(struct packet *pp) {
    for (int i = 0; i < 3; i++) pp->bytes[i] = mouse_bytes[i];

    pp->lb = mouse_bytes[0] & LB;
    pp->rb = mouse_bytes[0] & RB;
    pp->mb = mouse_bytes[0] & MB;

    pp->delta_x = (mouse_bytes[0] & X_SIGN) ? (0xFF00 | mouse_bytes[1]) : mouse_bytes[1];
    pp->delta_y = (mouse_bytes[0] & Y_SIGN) ? (0xFF00 | mouse_bytes[2]) : mouse_bytes[2];
    
    pp->x_ov = mouse_bytes[0] & X_OVF;
    pp->y_ov = mouse_bytes[0] & Y_OVF;
}

