#include "functions.h"
#include "structures.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static char palm_error[1024] = "";
char *is_error()
{
	if (strlen(palm_error) == 0)
		return NULL;

	return palm_error;
}

void reset_error()
{
	palm_error[0] = '\0';
}

void set_error( const char *fmt, ... )
{
	va_list vlist ;
	va_start( vlist, fmt ) ;

	vsprintf( palm_error, fmt, vlist ) ;

	va_end( vlist ) ;
}

char *read_string(int handle, char *str, int allocate_memory)
{
	byte b_size;
	ushort w_size;

	if (handle <= 2 || (!allocate_memory && str == NULL))
		return 0;

	read(handle, (void*)&b_size, sizeof(b_size));
	if (b_size == 0xFF)
	{
		read(handle, (void*)&w_size, sizeof(w_size));

		if (allocate_memory)
			str = (char*)malloc((size_t)w_size+1);

		read(handle, str, w_size);
		str[w_size] = '\0';
	}
	else
	{
		if (allocate_memory)
			str = (char*)malloc((size_t)b_size+1);

		read(handle, str, b_size);
		str[b_size] = '\0';
	}

	return str;
}
