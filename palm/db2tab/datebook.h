#ifndef __DATEBOOK_H__
#define __DATEBOOK_H__

#include "header.h"

typedef struct
{
	ushort	constant;
	ushort	length;
	char	class_name[256];
}ClassEntry;

typedef struct
{
	ulong	day_index;		/* Present ONLY for Brand = 1 or Brand = 2 or Brand = 3 */
	byte	days_mask;		/* Present ONLY for Brand = 2 */
	ulong	week_index;		/* Present ONLY for Brand = 3 */
	ulong	day_number;		/* Present ONLY for Brand = 4 or Brand = 5 */
	ulong	month_index;	/* Present ONLY for Brand = 5 */
}BrandDataEntry;

#define NOREPEAT		0
#define DAILY			1
#define WEEKLY			2
#define MONTHLYBYDAY	3
#define MONTHLYBYDATE	4
#define YEARLYBYDATE	5

typedef struct
{
	ushort	date_exeptions;		/* Count of any date exception entries Corrected Nov 16, 1999 */
	ulong	exeption_entry[256];/*	date exception entry, occurs with Date Exceptions. If date 
									exceptions is zero, there will be no entries here. */
	ushort	repeat_event_flag;	/* 	IMPORTANT 0xFFFF means that a class name record follows.
									0x0000 that nothing further follows, you should stop reading 
									the record. Any other value will be the brand of the repeat 
									event or'd with 0x8000
								*/
	ClassEntry *classe;			/*	This field is present ONLY if the repeat event flag is 0xFFFF */
	ulong	brand;				/*	The Brand of the repeat event. Possible values are:
									1 = Daily
									2 = Weekly
									3 = MonthlybyDay
									4 = MonthlybyDate
									5 = YearlybyDate
									6 = YearlybyDay
									Note that this field is present ONLY if the repeat event flag was 
									NOT 0x0000 */
	ulong	interval;			/*	Note that this field is present ONLY if the repeat event flag was 
									NOT 0x0000 */
	ulong	end_date;			/* --""-- */
	ulong	first_day_of_week;	/* --""-- */
	BrandDataEntry *brand_data;	/* --""-- */
}RepeatEventEntry;

typedef struct
{
	ulong	ftype1;
	ulong	record_id;
	ulong	ftype2;
	ulong	status_field;
	ulong	ftype3;
	ulong	position;
	ulong	ftype4;
	ulong	start_time;
	ulong	ftype5;
	ulong	end_time;
	ulong	ftype6;
	ulong	padding6;
	char	*description;
	ulong	ftype7;
	ulong	duration;
	ulong	ftype8;
	ulong	padding8;
	char	*note;
	ulong	ftype9;
	ulong	unlimited;
	ulong	ftype10;
	ulong	is_private;
	ulong	ftype11;
	ulong	category;
	ulong	ftype12;
	ulong	alarm_set;
	ulong	ftype13;
	ulong	alarm_advanced_units;
	ulong	ftype14;
	ulong	alarm_advanced_type;
	ulong	ftype15;
	RepeatEventEntry *repeat_event;
} DateBookEntry;

typedef struct
{
	PalmHeader_DAT *header;
	DateBookEntry *datebook;
}DateBook_DAT;

int  read_datebook_dat(DateBook_DAT*, const char *);
void destroy_datebook_dat(DateBook_DAT*);
int  read_datebook(int , DateBook_DAT*);
int  read_repeatevent(int , DateBookEntry *);

void print_repeatevent(DateBookEntry *);
void print_datebook(DateBook_DAT *);

#endif //__DATEBOOK_H__
