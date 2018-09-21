#ifndef __COOKIE_H__
#define __COOKIE_H__

#include <user.h>

typedef struct
{
	char    name[256];
	char    value[1024];
	char    domain[256];
	char	path[256];
	char    expires[64];
	int     secure;
} HTTP_COOKIE;

char *get_cookie_value(const char *name);
int parse_cookie();
int send_cookie(HTTP_COOKIE *scookie);
int add_cookie(HTTP_COOKIE *scookie);

#endif // __COOKIE_H__
