CC=     gcc
CP=     cp
CFLAGS= -g
#CFLAGS= -s -Wall -O2
LIBS=   -lm
OBJS=   category.o datebook.o functions.o header.o


BINS= db2tab

#datebook.o: datebook.c datebook.h
#	${CC} ${CFLAGS} -c -o datebook.o datebook.c

all:	${BINS}
${OBJS}: category.h datebook.h functions.h header.h structures.h
db2tab:	Makefile db2tab.c ${OBJS}
	${CC} ${CFLAGS} -o db2tab ${OBJS} db2tab.c ${LIBS} 
clean:
	rm *~ *o *core ${BINS}