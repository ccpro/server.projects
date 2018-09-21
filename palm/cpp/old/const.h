#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#define ulong	unsigned long
#define uint	unsigned int
#define ushort	unsigned short
#define byte	unsigned char

#define _ADD		0x01
#define _UPDATE	0x02
#define _DELETE	0x04
#define _PENDING 0x08
#define _ARCHIVE	0x80

char *read_string(int , char *, int);
char *is_error();
void reset_error();
void set_error( const char *fmt, ... );
int skip_new_line(char *str);

#endif // __FUNCTIONS_H__