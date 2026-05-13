# name of the program (Minix service)
PROG = lab5

# source files
SRCS = lab5.c

# include paths
CFLAGS += -pedantic -DLAB5 -I. -I../lab3

# video library (built here)
VIDEO_LIB = libvideo.a
VIDEO_OBJS = video-card.o

# keyboard and timer libraries (from lab3)
KBC_LIB   = ../lab3/libkbc.a
TIMER_LIB = ../lab3/libtimer.a

# libraries to link
LDADD += -L. -lvideo -L../lab3 -lkbc -ltimer -llcf
DPADD += ${LIBLCF} ${VIDEO_LIB} ${KBC_LIB} ${TIMER_LIB}

CLEANFILES += ${VIDEO_LIB}

# rule to create the video library
${VIDEO_LIB}: ${VIDEO_OBJS}
	ar rcs ${VIDEO_LIB} ${VIDEO_OBJS}

.include <minix.lcom.mk>