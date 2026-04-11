# name of the program (Minix service)
PROG=lab3 

.PATH: ../lab2

# source code files to be compiled
SRCS = lab3.c kbc.c utils.c timer.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic
CFLAGS += -DLAB3
CFLAGS += -I../lab2

# dependencies and libraries
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
