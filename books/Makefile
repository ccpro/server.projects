CC=     gcc
CP=     cp
CFLAGS= -s -Wall -O2 -I/usr/local/pgsql/include
LDFLAGS= -L/usr/local/pgsql/lib
LIBS=   -lpq -lm
#COMMLIBS= -I /usr/include/openssl/ -lcrypto
#SSLLIBS= -I /usr/include/openssl/ -lssl
#DHCPLIBS= -ldhcpctl -lomapi -ldst
#FTPLIBS= -lftp
OBJS=   cgi.o html.o list.o auth.o pio.o base64.o

BINS= books.cgi
all: ${BINS}
${OBJS}: config.h books.h auth.h
#base64.o: base64.h
#	${CC} ${CFLAGS} -o base64.o base64.c
books.cgi: Makefile books.c ${OBJS}
	${CC} ${CFLAGS} -o books.cgi ${OBJS} books.c ${LDFLAGS} ${LIBS} 
#${DHCPLIBS} ${FTPLIBS}
install: books.cgi
	cp books.cgi /usr/local/www/bin/
clean:
	rm *~ *o *core ${BINS}