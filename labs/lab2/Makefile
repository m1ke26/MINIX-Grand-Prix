PROG=lab2

SRCS = lab2.c

CFLAGS += -pedantic

DPADD += ${LIBLCF} ${LIB_NAME}
LDADD += -llcf -L.
LDADD += -ltimer

LIB_NAME = libtimer.a
LIB_SRCS = timer.c utils.c
LIB_OBJS = ${LIB_SRCS:.c=.o}
CLEANFILES += ${LIB_NAME} ${LIB_OBJS}

all: lib
lib: ${LIB_OBJS}
	ar rcs ${LIB_NAME} ${LIB_OBJS}

.include <minix.lcom.mk>