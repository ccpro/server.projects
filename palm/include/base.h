#ifndef __BASE64_H__
#define __BASE64_H__

/* Function definitions for the base 64 encoder and decoder */

int encode64( char *, int , char * ) ;
int decode64( char *, char * ) ;

        /* returns a pointer to a string that has been encoded */
char * encode64_num( int ) ;

#endif // __BASE64_H__