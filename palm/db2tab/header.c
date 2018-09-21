#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "functions.h"
#include "header.h"

int  read_palm_header(int handle, PalmHeader_DAT **ph)
{
	PalmHeader_DAT *pph;
	uint i;
	int rc;

	if (handle <0 || ph == NULL)
		return 0;

	pph = *ph = (PalmHeader_DAT *)malloc(sizeof(PalmHeader_DAT));
	memset((void*)pph, 0, sizeof(PalmHeader_DAT));

	read(handle, (void*)&pph->version_tag, sizeof(pph->version_tag));
	read_string(handle, pph->fname, 0);
	read_string(handle, pph->table, 0);

	read(handle, (void*)&pph->next_free, sizeof(pph->next_free));
	read(handle, (void*)&pph->category_count, sizeof(pph->category_count));

	rc = read_categories(handle, pph->category_count, &pph->category);
	if (rc < 0)
		return rc;

	read(handle, (void*)&pph->resource_id, sizeof(pph->resource_id));
	read(handle, (void*)&pph->fields_per_row, sizeof(pph->fields_per_row));

	read(handle, (void*)&pph->rec_id_pos, sizeof(pph->rec_id_pos));
	read(handle, (void*)&pph->rec_status_pos, sizeof(pph->rec_status_pos));

	read(handle, (void*)&pph->placement_pos, sizeof(pph->placement_pos));
	read(handle, (void*)&pph->field_count, sizeof(pph->field_count));

	for(i=0; i < pph->field_count; i++)
		read(handle, (void*)&pph->field_entry[i], sizeof(pph->field_entry[i]));

	read(handle, (void*)&pph->num_entries, sizeof(pph->num_entries));

	return 0;
}

void destroy_palm_header(PalmHeader_DAT **ph)
{
	if (ph && *ph)
	{
		PalmHeader_DAT *pph = *ph;

		if (pph->category)
			destroy_categories(&pph->category);
		
		free(*ph);
		*ph = NULL;
	}
}

void print_palm_header(PalmHeader_DAT *ph)
{
	int i;
	if (ph == NULL)
		return;

	printf("file_name '%s', table '%s'\n", ph->fname, ph->table);

	printf("next_free %d, category_free %d\n", ph->next_free, ph->category_count);

	print_categories(ph->category, ph->category_count);

	printf("resource %d, fields_per_row %d\n", ph->resource_id, ph->fields_per_row);

	printf("rec_id_pos %d, rec_status_pos %d\n", ph->rec_id_pos, ph->rec_status_pos);

	printf("placement_pos %d, field_count %d\n", ph->placement_pos, ph->field_count);

	for(i=0; i < ph->field_count; i++)
		printf("%d ", ph->field_entry[i]);
	puts("");

	printf("num_entries %d\n", ph->num_entries);
}
