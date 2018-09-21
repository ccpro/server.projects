#ifndef __CATEGORY_H__
#define __CATEGORY_H__

#include "StdAfx.h"

typedef struct
{
	ulong	index;
	ulong	id;
	ulong	dirty_flag;
	char	long_name[256];
	char	short_name[256];
}CategoryEntry;

class CCategoryDat
{
private:
	CategoryEntry *pce;
	int count;

public:
	CCategoryDat(int);
	~CCategoryDat();

	int  getCount(){return count;};

	int read(ifstream &ifs);
};

#endif // __CATEGORY_H__
