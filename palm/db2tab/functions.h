#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#define ulong	unsigned long
#define uint	unsigned int
#define ushort	unsigned short
#define byte	unsigned char

#define ADD		0x01
#define UPDATE	0x02
#define DELETE	0x04
#define PENDING 0x08
#define ARCHIVE	0x80

char *read_string(int , char *, int);
char *is_error();
void reset_error();
void set_error( const char *fmt, ... );

#endif // __FUNCTIONS_H__
