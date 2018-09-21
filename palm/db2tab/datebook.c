#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include "functions.h"
#include "datebook.h"

int  read_datebook_dat(DateBook_DAT *db, const char *fname)
{
	int handle;
	int rc;

	if (fname == NULL || strlen(fname) == 0 || db == NULL)
		return 1;

	memset((void*)db, 0, sizeof(DateBook_DAT));

	handle = open(fname, O_RDONLY);
	if (handle == -1)
	{
		fprintf(stderr, "can not open file %s (%s)\n", fname, strerror(errno));
		return 2;
	}

	rc = read_palm_header(handle, &db->header);
	if (rc < 0)
	{
		fprintf(stderr, "error reading %s error code %d\n", rc == -1 ? "header":"category", rc);
		close (handle);
		return rc;
	}

	rc = read_datebook(handle, db);
	if (rc < 0)
	{
		fprintf(stderr, "error reading datebook field type %d\n", abs(rc));
		close (handle);
		return rc;
	}

	return 0;
}

int read_datebook(int handle, DateBook_DAT *db)
{
	uint i;
	DateBookEntry *t;
	uint num_entries;

	if (handle < 0 || db == NULL || db->header == NULL || db->header->num_entries <= 0)
		return 0;

	num_entries = db->header->num_entries / db->header->field_count;

	db->datebook = (DateBookEntry *)malloc(sizeof(DateBookEntry) * num_entries);
	for(i=0; i< num_entries; i++)
	{
		t = &db->datebook[i];

		memset((void*)t, 0, sizeof(DateBookEntry));
		read(handle, (void*)&t->ftype1, sizeof(ulong));
		if (t->ftype1 != db->header->field_entry[0])
			return -1;

		read(handle, (void*)&t->record_id, sizeof(ulong));

		read(handle, (void*)&t->ftype2, sizeof(ulong));
		if (t->ftype2 != db->header->field_entry[1])
			return -2;

		read(handle, (void*)&t->status_field, sizeof(ulong));

		read(handle, (void*)&t->ftype3, sizeof(ulong));
		if (t->ftype3 != db->header->field_entry[2])
			return -3;

		read(handle, (void*)&t->position, sizeof(ulong));

		read(handle, (void*)&t->ftype4, sizeof(ulong));
		if (t->ftype4 != db->header->field_entry[3])
			return -4;

		read(handle, (void*)&t->start_time, sizeof(ulong));

		read(handle, (void*)&t->ftype5, sizeof(ulong));
		if (t->ftype5 != db->header->field_entry[4])
			return -5;

		read(handle, (void*)&t->end_time, sizeof(ulong));

		read(handle, (void*)&t->ftype6, sizeof(ulong));
		if (t->ftype6 != db->header->field_entry[5])
			return -6;

		read(handle, (void*)&t->padding6, sizeof(ulong));
		t->description = read_string(handle, t->description, 1);

		read(handle, (void*)&t->ftype7, sizeof(ulong));
		if (t->ftype7 != db->header->field_entry[6])
			return -7;

		read(handle, (void*)&t->duration, sizeof(ulong));

		read(handle, (void*)&t->ftype8, sizeof(ulong));
		if (t->ftype8 != db->header->field_entry[7])
			return -8;

		read(handle, (void*)&t->padding8, sizeof(ulong));
		t->note = read_string(handle, t->note, 1);

		read(handle, (void*)&t->ftype9, sizeof(ulong));
		if (t->ftype9 != db->header->field_entry[8])
			return -9;

		read(handle, (void*)&t->unlimited, sizeof(ulong));

		read(handle, (void*)&t->ftype10, sizeof(ulong));
		if (t->ftype10 != db->header->field_entry[9])
			return -10;

		read(handle, (void*)&t->is_private, sizeof(ulong));

		read(handle, (void*)&t->ftype11, sizeof(ulong));
		if (t->ftype11 != db->header->field_entry[10])
			return -11;

		read(handle, (void*)&t->category, sizeof(ulong));

		read(handle, (void*)&t->ftype12, sizeof(ulong));
		if (t->ftype12 != db->header->field_entry[11])
			return -12;

		read(handle, (void*)&t->alarm_set, sizeof(ulong));

		read(handle, (void*)&t->ftype13, sizeof(ulong));
		if (t->ftype13 != db->header->field_entry[12])
			return -13;

		read(handle, (void*)&t->alarm_advanced_units, sizeof(ulong));

		read(handle, (void*)&t->ftype14, sizeof(ulong));
		if (t->ftype1 != db->header->field_entry[0])
			return -1;

		read(handle, (void*)&t->alarm_advanced_type, sizeof(ulong));

		read(handle, (void*)&t->ftype15, sizeof(ulong));
		if (t->ftype15 != db->header->field_entry[14])
			return -15;

		read_repeatevent(handle, t);
	}

	return 0;
}

int  read_repeatevent(int handle, DateBookEntry *dbe)
{
	RepeatEventEntry *t;
	ushort i;

	if (handle < 0 || dbe == NULL)
		return 0;

	dbe->repeat_event = NULL;

	t = dbe->repeat_event = (RepeatEventEntry*)malloc(sizeof(RepeatEventEntry));
	memset((void*)t, 0, sizeof(RepeatEventEntry));

	read(handle, (void*)&t->date_exeptions, sizeof(ushort));

	for(i = 0; i < t->date_exeptions; i++)
		read(handle, (void*)&t->exeption_entry[i], sizeof(ulong));

	read(handle, (void*)&t->repeat_event_flag, sizeof(ushort));

	/* 	IMPORTANT 0xFFFF means that a class name record follows.
	 *	0x0000 that nothing further follows, you should stop reading 
	 *	the record. Any other value will be the brand of the repeat 
	 *	event or'd with 0x8000
	 */
	if (t->repeat_event_flag == 0)
		return 1;
	if (t->repeat_event_flag == 0xFFFF)
	{
		/*	Class Entry is present ONLY if the repeat event flag is 0xFFFF */
		t->classe = (ClassEntry*)malloc(sizeof(ClassEntry));
		memset((void*)t->classe, 0, sizeof(ClassEntry));

		read(handle, (void*)&t->classe->constant, sizeof(ushort));
		read(handle, (void*)&t->classe->length, sizeof(ushort));
		read(handle, (void*)&t->classe->class_name, t->classe->length);
	}

	// All next field are present only if repeat_event_flag WAS NOT 0x0000
	/*	The Brand of the repeat event. Possible values are:
		1 = Daily
		2 = Weekly
		3 = MonthlybyDay
		4 = MonthlybyDate
		5 = YearlybyDate
		6 = YearlybyDay
	*/
	read(handle, (void*)&t->brand, sizeof(ulong));
	read(handle, (void*)&t->interval, sizeof(ulong));
	read(handle, (void*)&t->end_date, sizeof(ulong));
	read(handle, (void*)&t->first_day_of_week, sizeof(ulong));

	t->brand_data = (BrandDataEntry*)malloc(sizeof(BrandDataEntry));
	memset((void*)t->brand_data, 0, sizeof(BrandDataEntry));

	if (t->brand == 1 || t->brand == 2 || t->brand == 3)
		read(handle, (void*)&t->brand_data->day_index, sizeof(ulong));
	if (t->brand == 2)
		read(handle, (void*)&t->brand_data->days_mask, sizeof(byte));
	if (t->brand == 3)
		read(handle, (void*)&t->brand_data->week_index, sizeof(ulong));
	if (t->brand == 4 || t->brand == 5)
		read(handle, (void*)&t->brand_data->day_number, sizeof(ulong));
	if (t->brand == 5)
		read(handle, (void*)&t->brand_data->month_index, sizeof(ulong));

	return 2;
}

void destroy_datebook_dat(DateBook_DAT *db)
{
	if (db == NULL)
		return;

	if (db->datebook != NULL && db->header)
	{
		uint i;
		DateBookEntry *t;
		uint num_entries;

		num_entries = db->header->num_entries / db->header->field_count;

		for(i=0; i< num_entries; i++)
		{
			t = &db->datebook[i];

			if (t->description)
				free(t->description);
			if (t->note)
				free(t->note);
		}

		if (db->datebook->repeat_event->classe)
			free (db->datebook->repeat_event->classe);

		if (db->datebook->repeat_event->brand_data)
			free (db->datebook->repeat_event->brand_data);

		if (db->datebook->repeat_event)
			free (db->datebook->repeat_event);
	}

	if (db->header)
		destroy_palm_header(&db->header);
}

void print_datebook(DateBook_DAT *db)
{
 uint i;
 DateBookEntry *t;
 uint num_entries;

   if (db == NULL || db->header == NULL || db->header->num_entries <= 0) {
      return;
   }

   num_entries = db->header->num_entries / db->header->field_count;

   for (i=0; i< num_entries; i++) {
      t = &db->datebook[i];

      printf("------------------------------------------\n");

      printf("ftype1 %d, record_id %d\n", t->ftype1, t->record_id);

      printf("ftype2 %d, status_field %d\n", t->ftype2, t->status_field);

      printf("ftype3 %d, position %d\n", t->ftype3, t->position);

		printf("ftype4 %d, start_time %d\n", t->ftype4, t->start_time);
		printf("start_time %s\n", ctime((const time_t *)&t->start_time));

		printf("ftype5 %d, end_time %d\n", t->ftype5, t->end_time);
		printf("end_time %s\n", ctime((const time_t *)&t->end_time));

		printf("ftype6 %d, padding6 %d description %s\n", t->ftype6, t->padding6, t->description);

		printf("ftype %d, duration %d\n", t->ftype7, t->duration);

		printf("ftype8 %d, padding8 %d note %s\n", t->ftype8, t->padding8, t->note);

		printf("ftype9 %d, unlimited %d\n", t->ftype9, t->unlimited);

		printf("ftype10 %d, private %d\n", t->ftype10, t->is_private);

		printf("ftype11 %d, category %d\n", t->ftype11, t->category);

		printf("ftype12 %d, alarm_set %d\n", t->ftype12, t->alarm_set);

		printf("ftype13 %d, alarm_advanced_units %d\n", t->ftype13, t->alarm_advanced_units);

		printf("ftype14 %d, alarm_advanced_type %d\n", t->ftype14, t->alarm_advanced_type);

		printf("ftype15 %d\n", t->ftype15);

      print_repeatevent(t);
   }
}

void  print_repeatevent(DateBookEntry *dbe)
{
 RepeatEventEntry *t;
 ushort i;

   if (dbe == NULL || dbe->repeat_event == NULL) {
      return;
   }

   t = dbe->repeat_event;
   printf("date_exeption %d\n", t->date_exeptions);

   for (i = 0; i < t->date_exeptions; i++) {
      printf("[%d] exeption_entry %ld\n", i, t->exeption_entry);
   }

   printf("repeat_event_flag %d\n", t->repeat_event_flag);

	/* 	IMPORTANT 0xFFFF means that a class name record follows.
	 *	0x0000 that nothing further follows, you should stop reading 
	 *	the record. Any other value will be the brand of the repeat 
	 *	event or'd with 0x8000
	 */
   if (t->repeat_event_flag == 0) {
      return;
   }

   if (t->repeat_event_flag == 0xFFFF && t->classe) {
      printf("constant %d, length %d, class_name %s\n", 
             t->classe->constant, t->classe->length, t->classe->class_name);
   }

	// All next field are present only if repeat_event_flag WAS NOT 0x0000
	/*	The Brand of the repeat event. Possible values are:
		1 = Daily
		2 = Weekly
		3 = MonthlybyDay
		4 = MonthlybyDate
		5 = YearlybyDate
		6 = YearlybyDay
	*/
   printf("brand %d, interval %d\n", t->brand, t->interval);
   printf("end_date %d, first_day_of_week %d\n", t->end_date, t->first_day_of_week);

   if (t->brand_data) {
		if (t->brand == 1 || t->brand == 2 || t->brand == 3)
			printf("day_index %d\n", t->brand_data->day_index);
		if (t->brand == 2)
			printf("day_mask %d\n", t->brand_data->days_mask);
		if (t->brand == 3)
			printf("week_index %d\n", t->brand_data->week_index);
		if (t->brand == 4 || t->brand == 5)
			printf("day_number %d\n", t->brand_data->day_number);
		if (t->brand == 5)
			printf("month_index %d\n", t->brand_data->month_index);
   }
}
