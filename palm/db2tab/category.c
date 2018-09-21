#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "functions.h"
#include "category.h"

int read_categories(int handle, uint category_count, CategoryEntry **ce)
{
	uint i;
	CategoryEntry *t;
	CategoryEntry *pce;
	
	printf ("handle %d, category_count %d, ce 0x%X, *ce 0x%X\n", handle, category_count, ce, *ce);
	if (handle < 0 || category_count <= 0 || ce == NULL)
		return 0;

	pce = *ce = (CategoryEntry*)malloc(sizeof(CategoryEntry) * category_count);

	for(i=0; i < category_count; i++)
	{
		t = &pce[i];

		memset((void*)t, 0, sizeof(CategoryEntry));

		read(handle, (void*)&t->index, sizeof(t->index));
		read(handle, (void*)&t->id, sizeof(t->id));
		read(handle, (void*)&t->dirty_flag, sizeof(t->dirty_flag));

		read_string(handle, t->long_name, 0);
		read_string(handle, t->short_name, 0);
	}

	return i;
}

void destroy_categories (CategoryEntry **ce)
{
	if (ce != NULL && *ce != NULL)
	{
		free(*ce);
		*ce = NULL;
	}
}

void print_categories (CategoryEntry *ce, uint category_count)
{
	uint i;
	CategoryEntry *t;

	for(i=0; i < category_count; i++)
	{
		t = &ce[i];

		printf("index %d, id %d, dirty_flag %d\n", t->index, t->id, t->dirty_flag);

		printf("%s:%s\n", t->long_name, t->short_name);
	}
}
