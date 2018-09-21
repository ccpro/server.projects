// PalmHeader.cpp: implementation of the CPalmHeader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PalmHeader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPalmHeader::CPalmHeader()
{
	memset((void*)&header, 0, sizeof(header));
}

CPalmHeader::~CPalmHeader()
{
	if (category)
		delete category;
}

int CPalmHeader::read(ifstream &ifs)
{
	uint i;
	PalmHeader_DAT *pph = &header;

	if (!ifs.is_open())
		return 0;

	ifs.read((char*)(void*)&pph->version_tag, sizeof(pph->version_tag));
	read_string(ifs, pph->fname, 0);
	skip_new_line(pph->table);
	read_string(ifs, pph->table, 0);

	ifs.read((char*)(void*)&pph->next_free, sizeof(pph->next_free));
	ifs.read((char*)(void*)&pph->category_count, sizeof(pph->category_count));

	category = new CCategoryDat(pph->category_count);
	category->read(ifs);

	ifs.read((char*)(void*)&pph->resource_id, sizeof(pph->resource_id));
	ifs.read((char*)(void*)&pph->fields_per_row, sizeof(pph->fields_per_row));

	ifs.read((char*)(void*)&pph->rec_id_pos, sizeof(pph->rec_id_pos));
	ifs.read((char*)(void*)&pph->rec_status_pos, sizeof(pph->rec_status_pos));

	ifs.read((char*)(void*)&pph->placement_pos, sizeof(pph->placement_pos));
	ifs.read((char*)(void*)&pph->field_count, sizeof(pph->field_count));

	for(i=0; i < pph->field_count; i++)
		ifs.read((char*)(void*)&pph->field_entry[i], sizeof(pph->field_entry[i]));

	ifs.read((char*)(void*)&pph->num_entries, sizeof(pph->num_entries));

	return 0;
}
