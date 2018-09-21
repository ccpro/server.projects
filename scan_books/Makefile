CC=     c++
CFLAGS= -g -Wall -O2 -I /usr/local/include
LIBS=   -L/usr/local/lib -lpq++ -lm -lboost_regex
#COMMLIBS= -I /usr/include/openssl/ -lcrypto
#SSLLIBS= -I /usr/include/openssl/ -lssl
#DHCPLIBS= -ldhcpctl -lomapi -ldst
#FTPLIBS= -lftp
OBJS= data_path.o uniq_value.o sha1.o book.o sql.o config.o news.o

BINS= scan_books
all: ${BINS}
${OBJS}: 
#base64.o: base64.h
#	${CC} ${CFLAGS} -o base64.o base64.c
scan_books: Makefile scan_books.cpp ${OBJS}
	${CC} ${CFLAGS} -o scan_books ${OBJS} scan_books.cpp ${LIBS} 
#${DHCPLIBS} ${FTPLIBS}
install: scan_books
	install scan_books ~ccpro/ubin/
clean:
	rm *~ *o *core ${BINS}