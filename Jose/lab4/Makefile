# name of the program (Minix service)
PROG = lab4

SRCS = lab4.c timer.c utils.c kbc.c mouse.c

.PATH: ../lab2 ../lab3
CFLAGS += -pedantic
CFLAGS += -DLAB4
CFLAGS += -I../lab2
CFLAGS += -I../lab3
CFLAGS += -I.

# -----------------------------------------------------------------------
# libtimer.a – recompiled with -DLAB4
# -----------------------------------------------------------------------
TIMER_LIB  = libtimer.a
TIMER_OBJS = timer.o utils.o

# -----------------------------------------------------------------------
# libkbc.a – recompiled with -DLAB4
# -----------------------------------------------------------------------
KBC_LIB  = libkbc.a
KBC_OBJS = kbc.o

# -----------------------------------------------------------------------
# libmouse.a – built here, reused by lab5+
# -----------------------------------------------------------------------
MOUSE_LIB  = libmouse.a
MOUSE_OBJS = mouse.o

DPADD += ${LIBLCF} ${MOUSE_LIB} ${KBC_LIB} ${TIMER_LIB}
LDADD += -L. -lmouse -lkbc -ltimer -llcf

CLEANFILES += ${TIMER_LIB} ${KBC_LIB} ${MOUSE_LIB}

${TIMER_LIB}: ${TIMER_OBJS}
	ar rcs ${TIMER_LIB} ${TIMER_OBJS}

${KBC_LIB}: ${KBC_OBJS}
	ar rcs ${KBC_LIB} ${KBC_OBJS}

${MOUSE_LIB}: ${MOUSE_OBJS}
	ar rcs ${MOUSE_LIB} ${MOUSE_OBJS}

.include <minix.lcom.mk>