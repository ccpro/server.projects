#include "stdafx.h"

//#include <string.h>
//#include <io.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <fcntl.h>

#include "address.h"

CPalmAddressDat::CPalmAddressDat() : CPalmHeader()
{
	memset((void*)&address, 0, sizeof(address));
}

CPalmAddressDat::~CPalmAddressDat()
{
}

int CPalmAddressDat::read(ifstream &ifs)
{
	uint i;
	AddressEntry *t, *pd;
	uint num_entries;

	pd = &address;

	if (!ifs.is_open())
		return 0;

	CPalmHeader::read(ifs);

	t = (AddressEntry *)&address;

	num_entries = getHeader()->num_entries / getHeader()->field_count;
	for(i=0; i < num_entries; i++)
	{

		memset((void*)t, 0, sizeof(AddressEntry));
		ifs.read((char*)(void*)&t->ftype1, sizeof(t->ftype1));
		if (t->ftype1 != getHeader()->field_entry[0])
			return -1;

		ifs.read((char*)(void*)&t->record_id, sizeof(t->record_id));

		ifs.read((char*)(void*)&t->ftype2, sizeof(t->ftype2));
		if (t->ftype1 != getHeader()->field_entry[1])
			return -2;

		ifs.read((char*)(void*)&t->status_field, sizeof(t->status_field));

		ifs.read((char*)(void*)&t->ftype3, sizeof(t->ftype3));
		if (t->ftype3 != getHeader()->field_entry[2])
			return -3;

		ifs.read((char*)(void*)&t->position, sizeof(t->position));

		ifs.read((char*)(void*)&t->ftype4, sizeof(t->ftype4));
		if (t->ftype4 != getHeader()->field_entry[3])
			return -4;

		ifs.read((char*)(void*)&t->padding4, sizeof(t->padding4));
		read_string(ifs, t->name, 0);
printf("%s\t", t->name);

		ifs.read((char*)(void*)&t->ftype5, sizeof(t->ftype5));
		if (t->ftype5 != getHeader()->field_entry[4])
			return -5;

		ifs.read((char*)(void*)&t->padding5, sizeof(t->padding5));
		read_string(ifs, t->last, 0);
printf("%s\t", t->last);

		ifs.read((char*)(void*)&t->ftype6, sizeof(t->ftype6));
		if (t->ftype6 != getHeader()->field_entry[5])
			return -6;

		ifs.read((char*)(void*)&t->padding6, sizeof(t->padding6));
		read_string(ifs, t->title, 0);
printf("%s\t", t->title);

		ifs.read((char*)(void*)&t->ftype7, sizeof(t->ftype7));
		if (t->ftype7 != getHeader()->field_entry[6])
			return -7;

		ifs.read((char*)(void*)&t->padding7, sizeof(t->padding7));
		read_string(ifs, t->company, 0);
printf("%s\t", t->company);

		ifs.read((char*)(void*)&t->ftype8, sizeof(t->ftype8));
		if (t->ftype8 != getHeader()->field_entry[7])
			return -8;

		ifs.read((char*)(void*)&t->phone_id1, sizeof(t->phone_id1));

		ifs.read((char*)(void*)&t->ftype9, sizeof(t->ftype9));
		if (t->ftype9 != getHeader()->field_entry[8])
			return -9;

		ifs.read((char*)(void*)&t->padding9, sizeof(t->padding9));
		read_string(ifs, t->phone1, 0);
printf("%d:%s\t", t->phone_id1, t->phone1);

		ifs.read((char*)(void*)&t->ftype10, sizeof(t->ftype10));
		if (t->ftype10 != getHeader()->field_entry[9])
			return -10;

		ifs.read((char*)(void*)&t->phone_id2, sizeof(t->phone_id2));

		ifs.read((char*)(void*)&t->ftype11, sizeof(t->ftype11));
		if (t->ftype11 != getHeader()->field_entry[10])
			return -11;

		ifs.read((char*)(void*)&t->padding11, sizeof(t->padding11));
		read_string(ifs, t->phone2, 0);
printf("%d:%s\t", t->phone_id2, t->phone2);

		ifs.read((char*)(void*)&t->ftype12, sizeof(t->ftype12));
		if (t->ftype12 != getHeader()->field_entry[11])
			return -12;
		ifs.read((char*)(void*)&t->phone_id3, sizeof(t->phone_id3));

		ifs.read((char*)(void*)&t->ftype13, sizeof(t->ftype13));
		if (t->ftype13 != getHeader()->field_entry[12])
			return -13;

		ifs.read((char*)(void*)&t->padding13, sizeof(t->padding13));
		read_string(ifs, t->phone3, 0);
printf("%d:%s\t", t->phone_id3, t->phone3);

		ifs.read((char*)(void*)&t->ftype14, sizeof(t->ftype14));
		if (t->ftype14 != getHeader()->field_entry[13])
			return -14;

		ifs.read((char*)(void*)&t->phone_id4, sizeof(t->phone_id4));

		ifs.read((char*)(void*)&t->ftype15, sizeof(t->ftype15));
		if (t->ftype15 != getHeader()->field_entry[14])
			return -15;

		ifs.read((char*)(void*)&t->padding15, sizeof(t->padding15));
		read_string(ifs, t->phone4, 0);
printf("%d:%s\t", t->phone_id4, t->phone4);

		ifs.read((char*)(void*)&t->ftype16, sizeof(t->ftype16));
		if (t->ftype16 != getHeader()->field_entry[15])
			return -16;

		ifs.read((char*)(void*)&t->phone_id5, sizeof(t->phone_id5));

		ifs.read((char*)(void*)&t->ftype17, sizeof(t->ftype17));
		if (t->ftype17 != getHeader()->field_entry[16])
			return -17;

		ifs.read((char*)(void*)&t->padding17, sizeof(t->padding17));
		read_string(ifs, t->phone5, 0);
printf("%d:%s\t", t->phone_id5, t->phone5);

		ifs.read((char*)(void*)&t->ftype18, sizeof(t->ftype18));
		if (t->ftype18 != getHeader()->field_entry[17])
			return -18;

		ifs.read((char*)(void*)&t->padding18, sizeof(t->padding18));
		read_string(ifs, t->address, 0);
printf("%s\t", t->address);

		ifs.read((char*)(void*)&t->ftype19, sizeof(t->ftype19));
		if (t->ftype19 != getHeader()->field_entry[18])
			return -19;
		ifs.read((char*)(void*)&t->padding19, sizeof(t->padding19));
		read_string(ifs, t->city, 0);
printf("%s\t", t->city);

		ifs.read((char*)(void*)&t->ftype20, sizeof(t->ftype20));
		if (t->ftype20 != getHeader()->field_entry[19])
			return -20;
		ifs.read((char*)(void*)&t->padding20, sizeof(t->padding20));
		read_string(ifs, t->state, 0);
printf("%s\t", t->state);

		ifs.read((char*)(void*)&t->ftype21, sizeof(t->ftype21));
		if (t->ftype21 != getHeader()->field_entry[20])
			return -21;

		ifs.read((char*)(void*)&t->padding21, sizeof(t->padding21));
		read_string(ifs, t->zip, 0);
printf("%s\t", t->zip);

		ifs.read((char*)(void*)&t->ftype22, sizeof(t->ftype22));
		if (t->ftype22 != getHeader()->field_entry[21])
			return -22;

		ifs.read((char*)(void*)&t->padding22, sizeof(t->padding22));
		read_string(ifs, t->country, 0);
printf("%s\t", t->country);

		ifs.read((char*)(void*)&t->ftype23, sizeof(t->ftype23));
		if (t->ftype23 != getHeader()->field_entry[22])
			return -1;

		ifs.read((char*)(void*)&t->padding23, sizeof(t->padding23));
		read_string(ifs, t->note, 0);
printf("'%s'\t", t->note);

		ifs.read((char*)(void*)&t->ftype24, sizeof(t->ftype24));
		if (t->ftype24 != getHeader()->field_entry[23])
			return -24;

		ifs.read((char*)(void*)&t->is_private, sizeof(t->is_private));
printf("%d\t", t->is_private);

		ifs.read((char*)(void*)&t->ftype25, sizeof(t->ftype25));
		if (t->ftype25 != getHeader()->field_entry[24])
			return -25;

		ifs.read((char*)(void*)&t->category, sizeof(t->category));
printf("%d\t", t->category);

		ifs.read((char*)(void*)&t->ftype26, sizeof(t->ftype26));
		if (t->ftype26 != getHeader()->field_entry[25])
			return -26;

		ifs.read((char*)(void*)&t->padding26, sizeof(t->padding26));
		read_string(ifs, t->custom1, 0);
printf("'%s'\t", t->custom1);

		ifs.read((char*)(void*)&t->ftype27, sizeof(t->ftype27));
		if (t->ftype27 != getHeader()->field_entry[26])
			return -27;

		ifs.read((char*)(void*)&t->padding27, sizeof(t->padding27));
		read_string(ifs, t->custom2, 0);
printf("'%s'\t", t->custom2);

		ifs.read((char*)(void*)&t->ftype28, sizeof(t->ftype28));
		if (t->ftype28 != getHeader()->field_entry[27])
			return -28;

		ifs.read((char*)(void*)&t->padding28, sizeof(t->padding28));
		read_string(ifs, t->custom3, 0);
printf("'%s'\t", t->custom3);

		ifs.read((char*)(void*)&t->ftype29, sizeof(t->ftype29));
		if (t->ftype29 != getHeader()->field_entry[28])
			return -29;

		ifs.read((char*)(void*)&t->padding29, sizeof(t->padding29));
		read_string(ifs, t->custom4, 0);
printf("'%s'\n", t->custom4);

		ifs.read((char*)(void*)&t->ftype30, sizeof(t->ftype30));
		if (t->ftype30 != getHeader()->field_entry[29])
			return -30;

		ifs.read((char*)(void*)&t->display_phone, sizeof(t->display_phone));	
    }

	return i;
}

int CPalmAddressDat::print(FILE *fp)
{
	uint i;
	AddressEntry *t, *pd;
	uint num_entries;

	pd = &address;

	if (fp == NULL)
		fp = stdout;

	num_entries = getHeader()->num_entries / getHeader()->field_count;

	fprintf(fp, "<Addresses>\n");

//	CPalmHeader::print(fp);

	for(i=0; i< num_entries; i++)
	{
		t = (AddressEntry *)&pd->address[i];

		fprintf(fp,"\t<Address record_id='%ld' status_field='%ld' position='%ld' is_private='%d' category='%ld' display_phone='%ld'>\n", 
			t->record_id, 
			t->status_field, 
			t->position,
			t->is_private, 
			t->category,
			t->display_phone);

//		skip_new_line(t->name);
//		skip_new_line(t->last);
		fprintf(fp, "\t\t<Name first=\"%s\" last=\"%s\" />\n", t->name, t->last);
		skip_new_line(t->title);
		fprintf(fp, "\t\t<Title name=\"%s\" />\n", t->title);
		skip_new_line(t->company);
		fprintf(fp, "\t\t<Company name=\"%s\" />\n", t->company);
		fprintf(fp, "\t\t\t<Phones>\n");
		skip_new_line(t->phone1);
		skip_new_line(t->phone2);
		skip_new_line(t->phone3);
		skip_new_line(t->phone4);
		skip_new_line(t->phone5);
		fprintf(fp, "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", t->phone_id1,  t->phone1);
		fprintf(fp, "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", t->phone_id2,  t->phone2);
		fprintf(fp, "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", t->phone_id3,  t->phone3);
		fprintf(fp, "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", t->phone_id4,  t->phone4);
		fprintf(fp, "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", t->phone_id5,  t->phone5);
		fprintf(fp, "\t\t\t</Phones>\n");

		skip_new_line(t->address);
		skip_new_line(t->city);
		skip_new_line(t->state);
		skip_new_line(t->zip);
		skip_new_line(t->country);
		fprintf(fp, "\t\t\t<Address address=\"%s\" city=\"%s\" state=\"%s\" zip=\"%s\" country=\"%s\" />\n",
			t->address,
			t->city,
			t->state,
			t->zip,
			t->country);

		skip_new_line(t->note);
		fprintf(fp,"\t\t\t<Note value=\"%s\" />\n", t->note);

		skip_new_line(t->custom1);
		skip_new_line(t->custom2);
		skip_new_line(t->custom3);
		skip_new_line(t->custom4);
		fprintf(fp, "\t\t\t<Customs>\n");
		fprintf(fp, "\t\t\t\t<Custom id='1'>%s</Custom>\n", t->custom1);
		fprintf(fp, "\t\t\t\t<Custom id='2'>%s</Custom>\n", t->custom2);
		fprintf(fp, "\t\t\t\t<Custom id='3'>%s</Custom>\n", t->custom3);
		fprintf(fp, "\t\t\t\t<Custom id='4'>%s</Custom>\n", t->custom4);
		fprintf(fp, "\t\t\t</Customs>\n");
		fprintf(fp,"\t</Address>");
	}

	fprintf(fp, "</Addresses>\n");

	return i;
}

int	print_address_dat(Addresses_DAT *pd, FILE *fp = NULL)
{
	uint i;
	AddressEntry *t;
	uint num_entries;

	if (pd == NULL)
		return -1;

	if (fp == NULL)
		fp = stdout;

	num_entries = pd->header->num_entries / pd->header->field_count;

	fprintf(fp, "<Addresses>\n");

//	print_palm_header(pd->header, fp);

	for(i=0; i< num_entries; i++)
	{
		t = &pd->address[i];

		fprintf(fp,"\t<Address record_id='%ld' status_field='%ld' position='%ld' is_private='%d' category='%ld' display_phone='%ld'>\n", 
			t->record_id, 
			t->status_field, 
			t->position,
			t->is_private, 
			t->category,
			t->display_phone);

//		skip_new_line(t->name);
//		skip_new_line(t->last);
		fprintf(fp, "\t\t<Name first=\"%s\" last=\"%s\" />\n", t->name, t->last);
		skip_new_line(t->title);
		fprintf(fp, "\t\t<Title name=\"%s\" />\n", t->title);
		skip_new_line(t->company);
		fprintf(fp, "\t\t<Company name=\"%s\" />\n", t->company);
		fprintf(fp, "\t\t\t<Phones>\n");
		skip_new_line(t->phone1);
		skip_new_line(t->phone2);
		skip_new_line(t->phone3);
		skip_new_line(t->phone4);
		skip_new_line(t->phone5);
		fprintf(fp, "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", t->phone_id1,  t->phone1);
		fprintf(fp, "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", t->phone_id2,  t->phone2);
		fprintf(fp, "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", t->phone_id3,  t->phone3);
		fprintf(fp, "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", t->phone_id4,  t->phone4);
		fprintf(fp, "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", t->phone_id5,  t->phone5);
		fprintf(fp, "\t\t\t</Phones>\n");

		skip_new_line(t->address);
		skip_new_line(t->city);
		skip_new_line(t->state);
		skip_new_line(t->zip);
		skip_new_line(t->country);
		fprintf(fp, "\t\t\t<Address address=\"%s\" city=\"%s\" state=\"%s\" zip=\"%s\" country=\"%s\" />\n",
			t->address,
			t->city,
			t->state,
			t->zip,
			t->country);

		skip_new_line(t->note);
		fprintf(fp,"\t\t\t<Note value=\"%s\" />\n", t->note);

		skip_new_line(t->custom1);
		skip_new_line(t->custom2);
		skip_new_line(t->custom3);
		skip_new_line(t->custom4);
		fprintf(fp, "\t\t\t<Customs>\n");
		fprintf(fp, "\t\t\t\t<Custom id='1'>%s</Custom>\n", t->custom1);
		fprintf(fp, "\t\t\t\t<Custom id='2'>%s</Custom>\n", t->custom2);
		fprintf(fp, "\t\t\t\t<Custom id='3'>%s</Custom>\n", t->custom3);
		fprintf(fp, "\t\t\t\t<Custom id='4'>%s</Custom>\n", t->custom4);
		fprintf(fp, "\t\t\t</Customs>\n");
		fprintf(fp,"\t</Address>");
	}

	fprintf(fp, "</Addresses>\n");

	return i;
}

/*
int read_address_dat(Addresses_DAT *pd, const char *fname)
{
	int handle;
	int rc;

	if (fname == NULL || strlen(fname) == 0 || pd == NULL)
		return 1;

	reset_error();

	memset((void*)pd, 0, sizeof(Addresses_DAT));

	handle = open(fname, O_RDONLY|O_BINARY);
	if (handle == -1)
	{
		set_error("can not open file %s (%s)\n", fname, strerror(NULL));
		return 2;
	}

	rc = read_palm_header(handle, &pd->header);
	if (rc < 0)
	{
		set_error("error reading %s error code %d\n", rc == -1 ? "header":"category", rc);
		close (handle);
		return rc;
	}

	rc = read_addresses(handle, pd);
	if (rc < 0)
	{
		set_error("error reading address field type %d\n", abs(rc));
		close (handle);
		return rc;
	}

	close (handle);

	return 0;
}

void destroy_address_dat(Addresses_DAT *pd)
{
	if (pd == NULL)
		return;

	if (pd->header)
		destroy_palm_header(&pd->header);

	if (pd->address)
		free(pd->address);
}

int read_addresses(int handle, Addresses_DAT *pd)
{
	uint i;
	AddressEntry *t;
	uint num_entries;

	if (handle < 0 || pd->header == NULL || pd->header->num_entries <= 0)
		return 0;

	num_entries = pd->header->num_entries / pd->header->field_count;

	pd->address = (AddressEntry *)malloc(sizeof(AddressEntry) * num_entries);
	for(i=0; i< num_entries; i++)
	{
		t = &pd->address[i];

		memset((void*)t, 0, sizeof(AddressEntry));
		read(handle, (void*)&t->ftype1, sizeof(t->ftype1));
		if (t->ftype1 != pd->header->field_entry[0])
			return -1;

		read(handle, (void*)&t->record_id, sizeof(t->record_id));

		read(handle, (void*)&t->ftype2, sizeof(t->ftype2));
		if (t->ftype1 != pd->header->field_entry[1])
			return -2;

		read(handle, (void*)&t->status_field, sizeof(t->status_field));

		read(handle, (void*)&t->ftype3, sizeof(t->ftype3));
		if (t->ftype3 != pd->header->field_entry[2])
			return -3;

		read(handle, (void*)&t->position, sizeof(t->position));

		read(handle, (void*)&t->ftype4, sizeof(t->ftype4));
		if (t->ftype4 != pd->header->field_entry[3])
			return -4;

		read(handle, (void*)&t->padding4, sizeof(t->padding4));
		read_string(handle, t->name, 0);

		read(handle, (void*)&t->ftype5, sizeof(t->ftype5));
		if (t->ftype5 != pd->header->field_entry[4])
			return -5;

		read(handle, (void*)&t->padding5, sizeof(t->padding5));
		read_string(handle, t->last, 0);

		read(handle, (void*)&t->ftype6, sizeof(t->ftype6));
		if (t->ftype6 != pd->header->field_entry[5])
			return -6;

		read(handle, (void*)&t->padding6, sizeof(t->padding6));
		read_string(handle, t->title, 0);

		read(handle, (void*)&t->ftype7, sizeof(t->ftype7));
		if (t->ftype7 != pd->header->field_entry[6])
			return -7;

		read(handle, (void*)&t->padding7, sizeof(t->padding7));
		read_string(handle, t->company, 0);

		read(handle, (void*)&t->ftype8, sizeof(t->ftype8));
		if (t->ftype8 != pd->header->field_entry[7])
			return -8;

		read(handle, (void*)&t->phone_id1, sizeof(t->phone_id1));

		read(handle, (void*)&t->ftype9, sizeof(t->ftype9));
		if (t->ftype9 != pd->header->field_entry[8])
			return -9;

		read(handle, (void*)&t->padding9, sizeof(t->padding9));
		read_string(handle, t->phone1, 0);

		read(handle, (void*)&t->ftype10, sizeof(t->ftype10));
		if (t->ftype10 != pd->header->field_entry[9])
			return -10;

		read(handle, (void*)&t->phone_id2, sizeof(t->phone_id2));

		read(handle, (void*)&t->ftype11, sizeof(t->ftype11));
		if (t->ftype11 != pd->header->field_entry[10])
			return -11;

		read(handle, (void*)&t->padding11, sizeof(t->padding11));
		read_string(handle, t->phone2, 0);

		read(handle, (void*)&t->ftype12, sizeof(t->ftype12));
		if (t->ftype12 != pd->header->field_entry[11])
			return -12;
		read(handle, (void*)&t->phone_id3, sizeof(t->phone_id3));

		read(handle, (void*)&t->ftype13, sizeof(t->ftype13));
		if (t->ftype13 != pd->header->field_entry[12])
			return -13;

		read(handle, (void*)&t->padding13, sizeof(t->padding13));
		read_string(handle, t->phone3, 0);

		read(handle, (void*)&t->ftype14, sizeof(t->ftype14));
		if (t->ftype14 != pd->header->field_entry[13])
			return -14;

		read(handle, (void*)&t->phone_id4, sizeof(t->phone_id4));

		read(handle, (void*)&t->ftype15, sizeof(t->ftype15));
		if (t->ftype15 != pd->header->field_entry[14])
			return -15;

		read(handle, (void*)&t->padding15, sizeof(t->padding15));
		read_string(handle, t->phone4, 0);

		read(handle, (void*)&t->ftype16, sizeof(t->ftype16));
		if (t->ftype16 != pd->header->field_entry[15])
			return -16;

		read(handle, (void*)&t->phone_id5, sizeof(t->phone_id5));

		read(handle, (void*)&t->ftype17, sizeof(t->ftype17));
		if (t->ftype17 != pd->header->field_entry[16])
			return -17;

		read(handle, (void*)&t->padding17, sizeof(t->padding17));
		read_string(handle, t->phone5, 0);

		read(handle, (void*)&t->ftype18, sizeof(t->ftype18));
		if (t->ftype18 != pd->header->field_entry[17])
			return -18;

		read(handle, (void*)&t->padding18, sizeof(t->padding18));
		read_string(handle, t->address, 0);

		read(handle, (void*)&t->ftype19, sizeof(t->ftype19));
		if (t->ftype19 != pd->header->field_entry[18])
			return -19;
		read(handle, (void*)&t->padding19, sizeof(t->padding19));
		read_string(handle, t->city, 0);

		read(handle, (void*)&t->ftype20, sizeof(t->ftype20));
		if (t->ftype20 != pd->header->field_entry[19])
			return -20;
		read(handle, (void*)&t->padding20, sizeof(t->padding20));
		read_string(handle, t->state, 0);

		read(handle, (void*)&t->ftype21, sizeof(t->ftype21));
		if (t->ftype21 != pd->header->field_entry[20])
			return -21;

		read(handle, (void*)&t->padding21, sizeof(t->padding21));
		read_string(handle, t->zip, 0);

		read(handle, (void*)&t->ftype22, sizeof(t->ftype22));
		if (t->ftype22 != pd->header->field_entry[21])
			return -22;

		read(handle, (void*)&t->padding22, sizeof(t->padding22));
		read_string(handle, t->country, 0);

		read(handle, (void*)&t->ftype23, sizeof(t->ftype23));
		if (t->ftype23 != pd->header->field_entry[22])
			return -1;

		read(handle, (void*)&t->padding23, sizeof(t->padding23));
		read_string(handle, t->note, 0);

		read(handle, (void*)&t->ftype24, sizeof(t->ftype24));
		if (t->ftype24 != pd->header->field_entry[23])
			return -24;
		read(handle, (void*)&t->is_private, sizeof(t->is_private));

		read(handle, (void*)&t->ftype25, sizeof(t->ftype25));
		if (t->ftype25 != pd->header->field_entry[24])
			return -25;
		read(handle, (void*)&t->category, sizeof(t->category));

		read(handle, (void*)&t->ftype26, sizeof(t->ftype26));
		if (t->ftype26 != pd->header->field_entry[25])
			return -26;
		read(handle, (void*)&t->padding26, sizeof(t->padding26));
		read_string(handle, t->custom1, 0);

		read(handle, (void*)&t->ftype27, sizeof(t->ftype27));
		if (t->ftype27 != pd->header->field_entry[26])
			return -27;
		read(handle, (void*)&t->padding27, sizeof(t->padding27));
		read_string(handle, t->custom2, 0);

		read(handle, (void*)&t->ftype28, sizeof(t->ftype28));
		if (t->ftype28 != pd->header->field_entry[27])
			return -28;
		read(handle, (void*)&t->padding28, sizeof(t->padding28));
		read_string(handle, t->custom3, 0);

		read(handle, (void*)&t->ftype29, sizeof(t->ftype29));
		if (t->ftype29 != pd->header->field_entry[28])
			return -29;
		read(handle, (void*)&t->padding29, sizeof(t->padding29));
		read_string(handle, t->custom4, 0);

		read(handle, (void*)&t->ftype30, sizeof(t->ftype30));
		if (t->ftype30 != pd->header->field_entry[29])
			return -30;
		read(handle, (void*)&t->display_phone, sizeof(t->display_phone));
	}

	return i;
}
*/