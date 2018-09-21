// PalmHeader.h: interface for the CPalmHeader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PALMHEADER_H__354BF225_AE40_4E5C_8419_78BA19657C74__INCLUDED_)
#define AFX_PALMHEADER_H__354BF225_AE40_4E5C_8419_78BA19657C74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "category.h"

typedef struct
{
	ulong	version_tag;
	char	fname[256];
	char	table[256];
	ulong	next_free;
	ulong	category_count;
	CategoryEntry *category;
	ulong	resource_id;
	ulong	fields_per_row;
	ulong	rec_id_pos;
	ulong	rec_status_pos;
	ulong	placement_pos;
	ushort	field_count;
	ushort	field_entry[256];
	ulong	num_entries;
}PalmHeader_DAT;

class CPalmHeader  
{
private:
	PalmHeader_DAT header;
	CCategoryDat  *category;

public:
	CPalmHeader();
	virtual ~CPalmHeader();

	ulong getNumEntries(){return header.num_entries;};
	ulong getFieldCount(){return header.field_count;};

    read(ifstream &ifs);

	PalmHeader_DAT *getHeader(){return &header;}

};

#endif // !defined(AFX_PALMHEADER_H__354BF225_AE40_4E5C_8419_78BA19657C74__INCLUDED_)
