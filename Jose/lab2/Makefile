# name of the program (Minix service)
PROG = lab2

SRCS = lab2.c timer.c utils.c

CFLAGS += -pedantic
CFLAGS += -DLAB2
CFLAGS += -I.

# -----------------------------------------------------------------------
# libtimer.a – built from the same .o files minix.lcom.mk compiles
# -----------------------------------------------------------------------
TIMER_LIB  = libtimer.a
TIMER_OBJS = timer.o utils.o

DPADD += ${LIBLCF} ${TIMER_LIB}
LDADD += -L. -ltimer -llcf

CLEANFILES += ${TIMER_LIB}

# This rule teaches make HOW to build libtimer.a
# It runs after minix.lcom.mk has compiled the .o files
${TIMER_LIB}: ${TIMER_OBJS}
	ar rcs ${TIMER_LIB} ${TIMER_OBJS}

.include <minix.lcom.mk>