PROG=lab3

SRCS = lab3.c kbc.c

CFLAGS += -pedantic -DLAB3
CFLAGS += -I../lab2

DPADD += ${LIBLCF} ${LIB_NAME}
LDADD += -llcf -L. -L../lab2
LDADD += -lkbc -ltimer

LIB_NAME = libkbc.a
LIB_SRCS = kbc.c
LIB_OBJS = ${LIB_SRCS:.c=.o}
CLEANFILES += ${LIB_NAME} ${LIB_OBJS}

all: lib
lib: ${LIB_OBJS}
	ar rcs ${LIB_NAME} ${LIB_OBJS}

.include <minix.lcom.mk>