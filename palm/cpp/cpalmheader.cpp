#include "common.h"
#include "cpalmcategory.hpp"
#include "cpalmheader.hpp"

void PalmHeader_DAT::init()
{
   version_tag = next_free = category_count = resource_id = fields_per_row =
   rec_id_pos = rec_status_pos = placement_pos = num_entries = 0L;
   field_count = 0;
   
   memset(field_entry, 0, sizeof(field_entry));
}


CPalmHeader::CPalmHeader()
{
   palmCategory = NULL;
}

CPalmHeader::~CPalmHeader()
{
   if ( palmCategory ) {
      delete palmCategory;
   }
   
   cout << "CPalmHeader done" << endl;
}

int CPalmHeader::print()
{
   cout << "palm header" << endl;
   cout << "version_tag:" << palmHeader.version_tag << " filename:" << palmHeader.fname << endl;
   cout << "table:" << palmHeader.table << endl;
   cout << "next_free:" << palmHeader.next_free << ", category_count:" << palmHeader.category_count << endl;
   
   if ( palmCategory ) {
      palmCategory->print();
   }
   
   cout << "resource_id:" << palmHeader.resource_id << " fields per row:" << palmHeader.fields_per_row << endl;
   cout << "rec_id_pos:" << palmHeader.rec_id_pos << ", rec_status_pos:" << palmHeader.rec_status_pos << endl;
   cout << "placement pos:" << palmHeader.placement_pos << ", field count:" << palmHeader.field_count << endl;

//   for(int i=0; i < palmHeader.field_count; i++) {
//      cout << i << " field_entry:" << palmHeader.field_entry[i] << endl;
//   }
   cout << "num entries:" << palmHeader.num_entries << endl;

   return 0;
}

int CPalmHeader::read(ifstream &ifs)
{
 PalmHeader_DAT *pph = &palmHeader;
 string tmp_str;

   if ( !ifs.is_open() ) {
      cerr << "CPalmHeader::file wasn't open" << endl;
      return 1;
   }

   ifs.read((char*)(void*)&pph->version_tag, sizeof(pph->version_tag));
   pph->fname = read_string(ifs);
   skip_new_line(pph->table);
   pph->table = read_string(ifs);

   ifs.read((char*)(void*)&pph->next_free, sizeof(pph->next_free));
   ifs.read((char*)(void*)&pph->category_count, sizeof(pph->category_count));

   if ( palmCategory ) {  
      delete palmCategory;
   }
   palmCategory = new CPalmCategory(pph->category_count);
   palmCategory->read(ifs);

   ifs.read((char*)(void*)&pph->resource_id, sizeof(pph->resource_id));
   ifs.read((char*)(void*)&pph->fields_per_row, sizeof(pph->fields_per_row));

   ifs.read((char*)(void*)&pph->rec_id_pos, sizeof(pph->rec_id_pos));
   ifs.read((char*)(void*)&pph->rec_status_pos, sizeof(pph->rec_status_pos));
 
   ifs.read((char*)(void*)&pph->placement_pos, sizeof(pph->placement_pos));
   ifs.read((char*)(void*)&pph->field_count, sizeof(pph->field_count));

   for(int i=0; i < pph->field_count; i++) {
      ifs.read((char*)(void*)&pph->field_entry[i], sizeof(pph->field_entry[i]));
   }

   ifs.read((char*)(void*)&pph->num_entries, sizeof(pph->num_entries));

   return 0;

}
