// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include "video.h"
#include "../lab3/kbc.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
    // sets the language of LCF messages (can be either EN-US or PT-PT)
    lcf_set_language("EN-US");

    // enables to log function invocations that are being "wrapped" by LCF
    // [comment this out if you don't want/need it]
    lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

    // enables to save the output of printf function calls on a file
    // [comment this out if you don't want/need it]
    lcf_log_output("/home/lcom/labs/lab5/output.txt");

    // handles control over to LCF
    // [LCF handles command line arguments and invokes the right function]
    if (lcf_start(argc, argv))
        return 1;

    // LCF clean up tasks
    // [must be the last statement before return]
    lcf_cleanup();

    return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
    reg86_t reg;
    memset(&reg, 0, sizeof(reg)); // set reg86_t reg to zero

    reg.intno = 0x10;             // Use INT 0x10
    reg.ah = 0x4F;                // set AH=0x4F
    reg.al = 0x02;                // specify function in AL
    reg.bx = mode | BIT(14);      // mode + linear framebuffer (bit 14 set)

    if (sys_int86(&reg) != OK) {
      printf("sys_int86() failed \n");
      return 1;
    }

    // Check VBE return status
    if (reg.ah != 0x00 || reg.al != 0x4F) return 1;

    tickdelay(micros_to_ticks(delay * 1000000));  
    if (vg_exit() != 0) return 1;
    return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
    /* Map VRAM  */
    if (video_init(mode) != 0) {
        printf("video_test_rectangle: vg_init failed\n");
        return 1;
    }
    /* Step 2: Switch to graphics mode */
    reg86_t reg;
    memset(&reg, 0, sizeof(reg));
    reg.intno = 0x10;
    reg.ah    = 0x4F;
    reg.al    = 0x02;
    reg.bx    = mode | BIT(14);

    if (sys_int86(&reg) != 0 || reg.ah != 0x00 || reg.al != 0x4F) {
        printf("failed to set video mode\n");
        return 1;
    }
    /* Step 3: Draw the rectangle */
    if (vg_draw_rectangle(x, y, width, height, color) != 0) {
        printf("video_test_rectangle: draw failed\n");
        return 1;
    }

    uint8_t kbd_fd;

    if (kbd_subscribe_int(&kbd_fd) != 0) {
        printf("video_test_rectangle: keyboard subscribe failed\n");
        vg_exit();
        return 1;
    }

    /* Step 4: Wait for ESC breakcode (0x81) */
    int ipc_status;
    message msg;

    while (1) {
        if (driver_receive(ANY, &msg, &ipc_status) != 0) {
            printf("video_test_rectangle: driver_receive failed\n");
            return 1;
        }

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & BIT(kbd_fd)) {
                        uint8_t scancode = kbc_get_scancode();
                        kbc_ih(); /* call KBC interrupt handler */
                        if (scancode == 0x81){ break;} /* ESC breakcode */
                    }
                default:
                    break;
            }
        }
    }
    kbd_unsubscribe_int();
    vg_exit();

    return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    /* To be completed */
    printf("%s(%8p, %u, %u): under construction\n", __func__, xpm, x, y);

    return 1;
}

