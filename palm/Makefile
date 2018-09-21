CC=gcc

VERSIO_N=0.1

BINDIR=/usr/local/www/bin
BINNAME=test

POSTGRESQL=~pgsql/bin/espg -o.ic

CFLAGS=-march=i686 \
	-O2 \
	-g \
	-funsigned-bitfields \
	-DVERSION="\"$(VERSIO_N)\"" \
	-DDATE_COMPILE="\"`date`\"" \
	-DOS="\"`uname -s -m`\"" \
	-DDEV_DEBUG="Yes" 

INCLUDE=-I./include -I/usr/local/pgsql/include

LDFLAGS=-L/usr/local/pgsql/lib/ \
	-lpq 

C_SOURCES=utils/main.c \
cgi/cgi.c \
cgi/cookie.c \
cgi/param.c \
utils/upload.c \
utils/md5.c \
utils/llist.c \
utils/base64.c \
user/user.c \
dialogs/dialogs.c \
pgsql/pgsql.c 


SQL_C_OUT=$(SQL_SOURCES:.pgc=.c)

#making list of all object files
C_OBJ=$(C_SOURCES:.c=.o)
SQL_OBJ=$(SQL_SOURCES:.pgc=.o)

.SUFFIXES: .o .c .pgc .a

#.pgc.c:
#    $(POSTGRESQL)  -f$@ $<

.a	:
	cd libcgi-1.0 && gmake

.c.o    :
	$(CC) $(INCLUDE) $(CFLAGS) -o $@ -c $<

$(BINDIR)/$(BINNAME): $(C_OBJ)
	@cd $(PWD)
	@echo linking...
	@$(CC) -o $(BINDIR)/$(BINNAME) $(ORA_OBJ)$(SQL_OBJ) $(C_OBJ) $(EXTRALIBS) $(EXTRALIBS1) $(LDFLAGS)

binary  :
	$(BINDIR)/$(BINNAME) ;

cobj    :
	$(C_OBJ) ;

clean   :
	rm -f $(C_OBJ) ;
