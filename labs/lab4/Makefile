PROG=lab4

SRCS = lab4.c mouse.c kbc.c ../lab2/timer.c ../lab2/utils.c

CFLAGS += -pedantic
CFLAGS += -I../lab2

DPADD += ${LIBLCF} ${LIB_NAME}
LDADD += -llcf -L.
LDADD += -lmouse

LIB_NAME = libmouse.a
LIB_SRCS = mouse.c kbc.c
LIB_OBJS = ${LIB_SRCS:.c=.o}
CLEANFILES += ${LIB_NAME} ${LIB_OBJS}

all: lib
lib: ${LIB_OBJS}
	ar rcs ${LIB_NAME} ${LIB_OBJS}

.include <minix.lcom.mk>