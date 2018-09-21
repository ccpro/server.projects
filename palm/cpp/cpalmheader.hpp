#ifndef __PALM_HEADER_HPP_
#define __PALM_HEADER_HPP_

#include "cpalmcategory.hpp"

struct PalmHeader_DAT
{
   ulong	version_tag;
   string	fname;
   string	table;
   ulong	next_free;
   ulong	category_count;
   Category_DAT *category;
   ulong	resource_id;
   ulong	fields_per_row;
   ulong	rec_id_pos;
   ulong	rec_status_pos;
   ulong	placement_pos;
   ushort	field_count;
   ushort	field_entry[256];
   ulong	num_entries;

   PalmHeader_DAT(){init();};

   void init();

};

class CPalmHeader
{
   private:
      PalmHeader_DAT palmHeader;
      CPalmCategory *palmCategory;

   public:
      CPalmHeader();
      ~CPalmHeader();

   int read(ifstream &ifs);
   int print();
   PalmHeader_DAT &getHeader(){return palmHeader;};

};

#endif
