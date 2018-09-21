#include "stdafx.h"

//#include <string.h>
//#include <io.h>
//#include <stdio.h>
//#include <stdlib.h>

#include "StdAfx.h"
#include "category.h"

CCategoryDat::CCategoryDat(int cnt) : pce(NULL)
{
	if (cnt > 0)
		count = cnt;
	else
		count = 0;
}

CCategoryDat::~CCategoryDat()
{
	if (pce != NULL)
		free(pce);
}

int CCategoryDat::read(ifstream &ifs)
{
	int i;
	CategoryEntry *t;
	
	if (!ifs.is_open())
		return 0;

	if (pce)
		free (pce);

	pce = (CategoryEntry*)calloc(count, sizeof(CategoryEntry));

	for(i=0; i < count; i++)
	{
		t = &pce[i];

		ifs.read((char*)(void*)&t->index, sizeof(t->index));
		ifs.read((char*)(void*)&t->id, sizeof(t->id));
		ifs.read((char*)(void*)&t->dirty_flag, sizeof(t->dirty_flag));

		read_string(ifs, t->long_name, 0);
		read_string(ifs, t->short_name, 0);
	}

	return i;
}

int read_categories(ifstream &ifs, uint category_count, CategoryEntry **ce)
{
	uint i;
	CategoryEntry *t;
	CategoryEntry *pce;
	
	if (!ifs.is_open() || category_count <= 0 || ce == NULL)
		return 0;

	pce = *ce = (CategoryEntry*)calloc(category_count, sizeof(CategoryEntry));

	for(i=0; i < category_count; i++)
	{
		t = &pce[i];

		ifs.read((char*)(void*)&t->index, sizeof(t->index));
		ifs.read((char*)(void*)&t->id, sizeof(t->id));
		ifs.read((char*)(void*)&t->dirty_flag, sizeof(t->dirty_flag));

		read_string(ifs, t->long_name, 0);
		read_string(ifs, t->short_name, 0);
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
