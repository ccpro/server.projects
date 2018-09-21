#ifndef __ADDRESS_H__
#define __ADDRESS_H__

#include "common.h"
#include "cpalmheader.hpp"

class Address_DAT
{
   public:
      ulong	ftype1;
      ulong	record_id;
      ulong	ftype2;
      ulong	status_field;
      ulong	ftype3;
      ulong	position;
      ulong	ftype4;
      ulong	padding4;
      string	name;
      ulong	ftype5;
      ulong	padding5;
      string	last;
      ulong	ftype6;
      ulong	padding6;
      string	title;
      ulong	ftype7;
      ulong	padding7;
      string	company;
      ulong	ftype8;
      ulong	phone_id1;
      ulong	ftype9;
      ulong	padding9;
      string	phone1;
      ulong	ftype10;
      ulong	phone_id2;
      ulong	ftype11;
      ulong	padding11;
      string	phone2;
      ulong	ftype12;
      ulong	phone_id3;
      ulong	ftype13;
      ulong	padding13;
      string	phone3;
      ulong	ftype14;
      ulong	phone_id4;
      ulong	ftype15;
      ulong	padding15;
      string	phone4;
      ulong	ftype16;
      ulong	phone_id5;
      ulong	ftype17;
      ulong	padding17;
      string	phone5;
      ulong	ftype18;
      ulong	padding18;
      string	address;
      ulong	ftype19;
      ulong	padding19;
      string	city;
      ulong	ftype20;
      ulong	padding20;
      string	state;
      ulong	ftype21;
      ulong	padding21;
      string	zip;
      ulong	ftype22;
      ulong	padding22;
      string	country;
      ulong	ftype23;
      ulong	padding23;
      string	note;
      ulong	ftype24;
      ulong	is_private;
      ulong	ftype25;
      ulong	category;
      ulong	ftype26;
      ulong	padding26;
      string	custom1;
      ulong	ftype27;
      ulong	padding27;
      string	custom2;
      ulong	ftype28;
      ulong	padding28;
      string	custom3;
      ulong	ftype29;
      ulong	padding29;
      string	custom4;
      ulong	ftype30;
      ulong	display_phone;
      
      Address_DAT(){init();};
      void init();
};

class CPalmAddress : public CPalmHeader
{
   private:
      vector<Address_DAT*> m_aAddr;
      CPalmHeader m_cPalmHeader;

   public:
	CPalmAddress();
	~CPalmAddress();

	int read(ifstream &ifs);
	int print();
};

#endif //__ADDRESS_H__
