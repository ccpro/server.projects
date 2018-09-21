CC=     g++
CP=     cp
CFLAGS= -g -Wall -O2
LDFLAGS=
LIBS=   -lm
#COMMLIBS= -I /usr/include/openssl/ -lcrypto
#SSLLIBS= -I /usr/include/openssl/ -lssl
#DHCPLIBS= -ldhcpctl -lomapi -ldst
#FTPLIBS= -lftp
OBJS=   cpalmheader.o cpalmcategory.o main.o utilites.o cpalmaddress.o

BINS= dat_parser
all: ${BINS}
${OBJS}: cpalmheader.hpp common.h
#base64.o: base64.h
#	${CC} ${CFLAGS} -o base64.o base64.c
dat_parser: Makefile ${OBJS}
	${CC} ${CFLAGS} -o dat_parser ${OBJS} ${LDFLAGS} ${LIBS} 
#${DHCPLIBS} ${FTPLIBS}
#install: books.cgi
#	cp books.cgi /usr/local/www/bin/
clean:
	rm *~ *o *core ${BINS}