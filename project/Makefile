# LCOM Project Makefile
# Builds project with all lab libraries

PROG = project

# Tell make where to find source files
VPATH = src

# Source files (without directory prefix)
SRCS = main.c

# Include directories
CFLAGS += -I../include
CFLAGS += -pedantic

# Libraries to link
DPADD += ${LIBLCF} lib/librtc.a lib/libtimer.a lib/libkbc.a lib/libmouse.a lib/libvideo.a
LDADD += -L./lib -lrtc -ltimer -lkbc -lmouse -lvideo -llcf

.include <minix.lcom.mk>
