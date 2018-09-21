#ifndef __ADDRESS_H__
#define __ADDRESS_H__

#include "StdAfx.h"
#include "PalmHeader.h"

typedef struct
{
	ulong	ftype1;
	ulong	record_id;
	ulong	ftype2;
	ulong	status_field;
	ulong	ftype3;
	ulong	position;
	ulong	ftype4;
	ulong	padding4;
	char	name[256];
	ulong	ftype5;
	ulong	padding5;
	char	last[256];
	ulong	ftype6;
	ulong	padding6;
	char	title[256];
	ulong	ftype7;
	ulong	padding7;
	char	company[256];
	ulong	ftype8;
	ulong	phone_id1;
	ulong	ftype9;
	ulong	padding9;
	char	phone1[256];
	ulong	ftype10;
	ulong	phone_id2;
	ulong	ftype11;
	ulong	padding11;
	char	phone2[256];
	ulong	ftype12;
	ulong	phone_id3;
	ulong	ftype13;
	ulong	padding13;
	char	phone3[256];
	ulong	ftype14;
	ulong	phone_id4;
	ulong	ftype15;
	ulong	padding15;
	char	phone4[256];
	ulong	ftype16;
	ulong	phone_id5;
	ulong	ftype17;
	ulong	padding17;
	char	phone5[256];
	ulong	ftype18;
	ulong	padding18;
	char	address[256];
	ulong	ftype19;
	ulong	padding19;
	char	city[256];
	ulong	ftype20;
	ulong	padding20;
	char	state[256];
	ulong	ftype21;
	ulong	padding21;
	char	zip[256];
	ulong	ftype22;
	ulong	padding22;
	char	country[256];
	ulong	ftype23;
	ulong	padding23;
	char	note[256];
	ulong	ftype24;
	ulong	is_private;
	ulong	ftype25;
	ulong	category;
	ulong	ftype26;
	ulong	padding26;
	char	custom1[256];
	ulong	ftype27;
	ulong	padding27;
	char	custom2[256];
	ulong	ftype28;
	ulong	padding28;
	char	custom3[256];
	ulong	ftype29;
	ulong	padding29;
	char	custom4[256];
	ulong	ftype30;
	ulong	display_phone;
}AddressEntry;

typedef struct
{
	PalmHeader_DAT *header;
	AddressEntry *address;
}Addresses_DAT;

class CPalmAddressDat : public CPalmHeader
{
private:
	AddressEntry address;

public:
	CPalmAddressDat();
	~CPalmAddressDat();

	int read(ifstream &ifs);
	int print(FILE *fp = NULL);
};

int  read_address_dat(Addresses_DAT *, const char *);
void destroy_address_dat(Addresses_DAT *);
int  read_addresses(int , Addresses_DAT *);
int	print_address_dat(Addresses_DAT *pd, FILE *fp);

#endif //__ADDRESS_H__