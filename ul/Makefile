CC=     gcc
CP=     cp
CFLAGS= -s -Wall -O2 -I/usr/local/include
LDFLAGS= -L/usr/local/lib
LIBS=   -lpq -lm
#COMMLIBS= -I /usr/include/openssl/ -lcrypto
#SSLLIBS= -I /usr/include/openssl/ -lssl
#DHCPLIBS= -ldhcpctl -lomapi -ldst
#FTPLIBS= -lftp
OBJS=   cgi.o html.o auth.o pio.o base64.o

BINS= upload.cgi
all: ${BINS}
${OBJS}: config.h upload.h auth.h
#base64.o: base64.h
#	${CC} ${CFLAGS} -o base64.o base64.c
upload.cgi: Makefile upload.c ${OBJS}
	${CC} ${CFLAGS} -o upload.cgi ${OBJS} upload.c ${LDFLAGS} ${LIBS} 
#${DHCPLIBS} ${FTPLIBS}
install: upload.cgi
	install upload.cgi /usr/local/www/bin/ul
clean:
	rm *~ *o *core ${BINS}