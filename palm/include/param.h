#ifndef __PARAM_H__
#define __PARAM_H__

#include <llist.h>

typedef struct
{
	char	name[256];
	char	data[1024];
	int	file;
} HTTP_PARAM;
			
int parse_cgi_params();
char *get_param(const char *name);

#endif // __PARAM_H__
