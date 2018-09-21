#ifndef __CATEGORY_H__
#define __CATEGORY_H__

#include "structures.h"

typedef struct
{
	ulong	index;
	ulong	id;
	ulong	dirty_flag;
	char	long_name[256];
	char	short_name[256];
}CategoryEntry;

int  read_categories(int , uint , CategoryEntry **);
void destroy_categories (CategoryEntry **);
void print_categories (CategoryEntry *, uint);

#endif // __CATEGORY_H__
