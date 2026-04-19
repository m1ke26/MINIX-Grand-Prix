# name of the program (Minix service)
PROG = lab3

SRCS = lab3.c timer.c utils.c kbc.c

.PATH: ../lab2
CFLAGS += -pedantic
CFLAGS += -DLAB3
CFLAGS += -I../lab2
CFLAGS += -I.

# -----------------------------------------------------------------------
# libtimer.a – recompiled with -DLAB3
# -----------------------------------------------------------------------
TIMER_LIB  = libtimer.a
TIMER_OBJS = timer.o utils.o

# -----------------------------------------------------------------------
# libkbc.a – built here, reused by lab4+
# -----------------------------------------------------------------------
KBC_LIB  = libkbc.a
KBC_OBJS = kbc.o

DPADD += ${LIBLCF} ${KBC_LIB} ${TIMER_LIB}
LDADD += -L. -lkbc -ltimer -llcf

CLEANFILES += ${TIMER_LIB} ${KBC_LIB}

${TIMER_LIB}: ${TIMER_OBJS}
	ar rcs ${TIMER_LIB} ${TIMER_OBJS}

${KBC_LIB}: ${KBC_OBJS}
	ar rcs ${KBC_LIB} ${KBC_OBJS}

.include <minix.lcom.mk>	