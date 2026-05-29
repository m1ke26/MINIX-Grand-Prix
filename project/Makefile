# LCOM Project Makefile
# Builds project with all lab libraries

PROG = proj

# Tell make where to find source files
VPATH = src

# Source files (without directory prefix)
SRCS = proj.c sprite.c font.c button.c start_menu.c double_buffer.c state.c car.c track.c camera.c pause_menu.c

# Include directories
CFLAGS += -Iinclude
CFLAGS += -pedantic
CFLAGS += -D__LCOM_OPTIMIZED__

# Libraries to link
DPADD += ${LIBLCF} lib/librtc.a lib/libtimer.a lib/libkbc.a lib/libmouse.a lib/libvideo.a lib/libutils.a
LDADD += -L./lib -lrtc -ltimer -lkbc -lmouse -lvideo -llcf -lutils


.include <minix.lcom.mk>
