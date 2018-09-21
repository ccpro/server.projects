#ifndef __AUTH_H__
#define __AUTH_H__

#include "config.h"

#define USER_CAN_READ 0x00000001

typedef struct {
   char			user[STRLEN+1];
   char			cgi_name[STRLEN+1];
   char			db_server[STRLEN+1];
   unsigned long 	access;
   unsigned short	in_production;
} t_user;

void init_user(const char *cgi_name);

#endif // __AUTH_H__
