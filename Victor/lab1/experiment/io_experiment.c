#include <minix/syslib.h>
#include <minix/sef.h>
#include <stdio.h>

int main() {

    sef_startup();                         // Register as a service
    freopen("/dev/console", "w", stdout);  // Redirect output to console

    int ret = sys_outb(0x70, 0xA);          // Write 0xA to RTC address register
    printf("sys_outb returned: %d\n", ret);

    return 0;
}