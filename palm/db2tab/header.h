#ifndef __HEADER_H__
#define __HEADER_H__

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

int  read_palm_header(int , PalmHeader_DAT **);
void destroy_palm_header(PalmHeader_DAT **);
void print_palm_header(PalmHeader_DAT *);

#endif // __HEADER_H__
