PROG=lab5

SRCS = lab5.c video_gr.c kbc.c ../lab2/utils.c

CFLAGS += -pedantic
CFLAGS += -I../lab2

DPADD += ${LIBLCF} ${LIB_NAME}
LDADD += -llcf -L.
LDADD += -lvideo

LIB_NAME = libvideo.a
LIB_SRCS = video_gr.c kbc.c
LIB_OBJS = ${LIB_SRCS:.c=.o}
CLEANFILES += ${LIB_NAME} ${LIB_OBJS}

all: lib
lib: ${LIB_OBJS}
	ar rcs ${LIB_NAME} ${LIB_OBJS}

.include <minix.lcom.mk>