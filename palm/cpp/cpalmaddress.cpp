#include "common.h"
#include "cpalmaddress.hpp"

void Address_DAT::init()
{
   ftype1 = 
   record_id = 
   ftype2 = 
   status_field = 
   ftype3 = 
   position = 
   ftype4 = 
   padding4 = 
   ftype5 = 
   padding5 = 
   ftype6 = 
   padding6 = 
   ftype7 = 
   padding7 = 
   ftype8 = 
   phone_id1 = 
   ftype9 = 
   padding9 = 
   ftype10 = 
   phone_id2 = 
   ftype11 = 
   padding11 = 
   ftype12 = 
   phone_id3 = 
   ftype13 = 
   padding13 = 
   ftype14 = 
   phone_id4 = 
   ftype15 = 
   padding15 = 
   ftype16 = 
   phone_id5 = 
   ftype17 = 
   padding17 = 
   ftype18 = 
   padding18 = 
   ftype19 = 
   padding19 = 
   ftype20 = 
   padding20 = 
   ftype21 = 
   padding21 = 
   ftype22 = 
   padding22 = 
   ftype23 = 
   padding23 = 
   ftype24 = 
   is_private = 
   ftype25 = 
   category = 
   ftype26 = 
   padding26 = 
   ftype27 = 
   padding27 = 
   ftype28 = 
   padding28 = 
   ftype29 = 
   padding29 = 
   ftype30 = 
   display_phone = 0;

   name = "";
   last = "";
   title = "";
   company = "";
   phone1 = "";
   phone2 = "";
   phone3 = "";
   phone4 = "";
   phone5 = "";
   address = "";
   city = "";
   state = "";
   zip = "";
   country = "";
   note = "";
   custom1 = "";
   custom2 = "";
   custom3 = "";
   custom4 = "";
}

CPalmAddress::CPalmAddress()
{
   cout << "CPalmAddress start" << endl;
}

CPalmAddress::~CPalmAddress()
{
   if ( !m_aAddr.empty() ) {
      m_aAddr.clear();
   }
   cout << "CPalmAddress done" << endl;
}

int CPalmAddress::read(ifstream &ifs)
{
 uint i;
 uint num_entries;
 Address_DAT *pAddr;


   if ( !ifs.is_open() ) {
      cerr << "address file was not open" << endl;
      return 0;
   }

   m_cPalmHeader.read(ifs);

   num_entries = m_cPalmHeader.getHeader().num_entries / m_cPalmHeader.getHeader().field_count;
   
   cout << num_entries << "will be read " << endl;
   for(i=0; i < num_entries; i++) {

      pAddr = new Address_DAT;
      m_aAddr.push_back(pAddr);
      
      ifs.read((char*)(void*)&pAddr->ftype1, sizeof(pAddr->ftype1));
      if ( pAddr->ftype1 != m_cPalmHeader.getHeader().field_entry[0] ) {
         return -1;
      }

      ifs.read((char*)(void*)&pAddr->record_id, sizeof(pAddr->record_id));

      ifs.read((char*)(void*)&pAddr->ftype2, sizeof(pAddr->ftype2));
      if ( pAddr->ftype1 != m_cPalmHeader.getHeader().field_entry[1] ) {
         return -2;
      }

      ifs.read((char*)(void*)&pAddr->status_field, sizeof(pAddr->status_field));

      ifs.read((char*)(void*)&pAddr->ftype3, sizeof(pAddr->ftype3));
      if (pAddr->ftype3 != m_cPalmHeader.getHeader().field_entry[2])
         return -3;

      ifs.read((char*)(void*)&pAddr->position, sizeof(pAddr->position));

      ifs.read((char*)(void*)&pAddr->ftype4, sizeof(pAddr->ftype4));
      if (pAddr->ftype4 != m_cPalmHeader.getHeader().field_entry[3])
         return -4;

      ifs.read((char*)(void*)&pAddr->padding4, sizeof(pAddr->padding4));
      pAddr->name = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype5, sizeof(pAddr->ftype5));
      if (pAddr->ftype5 != m_cPalmHeader.getHeader().field_entry[4])
         return -5;

      ifs.read((char*)(void*)&pAddr->padding5, sizeof(pAddr->padding5));
      pAddr->last = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype6, sizeof(pAddr->ftype6));
      if (pAddr->ftype6 != m_cPalmHeader.getHeader().field_entry[5])
         return -6;

      ifs.read((char*)(void*)&pAddr->padding6, sizeof(pAddr->padding6));
      pAddr->title = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype7, sizeof(pAddr->ftype7));
      if (pAddr->ftype7 != m_cPalmHeader.getHeader().field_entry[6])
         return -7;

      ifs.read((char*)(void*)&pAddr->padding7, sizeof(pAddr->padding7));
      pAddr->company = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype8, sizeof(pAddr->ftype8));
      if (pAddr->ftype8 != m_cPalmHeader.getHeader().field_entry[7])
         return -8;

      ifs.read((char*)(void*)&pAddr->phone_id1, sizeof(pAddr->phone_id1));

      ifs.read((char*)(void*)&pAddr->ftype9, sizeof(pAddr->ftype9));
      if (pAddr->ftype9 != m_cPalmHeader.getHeader().field_entry[8])
         return -9;

      ifs.read((char*)(void*)&pAddr->padding9, sizeof(pAddr->padding9));
      pAddr->phone1 = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype10, sizeof(pAddr->ftype10));
      if (pAddr->ftype10 != m_cPalmHeader.getHeader().field_entry[9])
         return -10;

      ifs.read((char*)(void*)&pAddr->phone_id2, sizeof(pAddr->phone_id2));

      ifs.read((char*)(void*)&pAddr->ftype11, sizeof(pAddr->ftype11));
      if (pAddr->ftype11 != m_cPalmHeader.getHeader().field_entry[10])
         return -11;

      ifs.read((char*)(void*)&pAddr->padding11, sizeof(pAddr->padding11));
      pAddr->phone2 = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype12, sizeof(pAddr->ftype12));
      if (pAddr->ftype12 != m_cPalmHeader.getHeader().field_entry[11])
         return -12;
      ifs.read((char*)(void*)&pAddr->phone_id3, sizeof(pAddr->phone_id3));

      ifs.read((char*)(void*)&pAddr->ftype13, sizeof(pAddr->ftype13));
      if (pAddr->ftype13 != m_cPalmHeader.getHeader().field_entry[12])
         return -13;

      ifs.read((char*)(void*)&pAddr->padding13, sizeof(pAddr->padding13));
      pAddr->phone3 = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype14, sizeof(pAddr->ftype14));
      if (pAddr->ftype14 != m_cPalmHeader.getHeader().field_entry[13])
         return -14;

      ifs.read((char*)(void*)&pAddr->phone_id4, sizeof(pAddr->phone_id4));

      ifs.read((char*)(void*)&pAddr->ftype15, sizeof(pAddr->ftype15));
      if (pAddr->ftype15 != m_cPalmHeader.getHeader().field_entry[14])
         return -15;

      ifs.read((char*)(void*)&pAddr->padding15, sizeof(pAddr->padding15));
      pAddr->phone4 = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype16, sizeof(pAddr->ftype16));
      if (pAddr->ftype16 != m_cPalmHeader.getHeader().field_entry[15])
         return -16;

      ifs.read((char*)(void*)&pAddr->phone_id5, sizeof(pAddr->phone_id5));

      ifs.read((char*)(void*)&pAddr->ftype17, sizeof(pAddr->ftype17));
      if (pAddr->ftype17 != m_cPalmHeader.getHeader().field_entry[16])
         return -17;

      ifs.read((char*)(void*)&pAddr->padding17, sizeof(pAddr->padding17));
      pAddr->phone5 = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype18, sizeof(pAddr->ftype18));
      if (pAddr->ftype18 != m_cPalmHeader.getHeader().field_entry[17])
         return -18;

      ifs.read((char*)(void*)&pAddr->padding18, sizeof(pAddr->padding18));
      pAddr->address = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype19, sizeof(pAddr->ftype19));
      if (pAddr->ftype19 != m_cPalmHeader.getHeader().field_entry[18])
         return -19;
      ifs.read((char*)(void*)&pAddr->padding19, sizeof(pAddr->padding19));
      pAddr->city = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype20, sizeof(pAddr->ftype20));
      if (pAddr->ftype20 != m_cPalmHeader.getHeader().field_entry[19])
         return -20;
      ifs.read((char*)(void*)&pAddr->padding20, sizeof(pAddr->padding20));
      pAddr->state = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype21, sizeof(pAddr->ftype21));
      if (pAddr->ftype21 != m_cPalmHeader.getHeader().field_entry[20])
         return -21;

      ifs.read((char*)(void*)&pAddr->padding21, sizeof(pAddr->padding21));
      pAddr->zip = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype22, sizeof(pAddr->ftype22));
      if (pAddr->ftype22 != m_cPalmHeader.getHeader().field_entry[21])
         return -22;

      ifs.read((char*)(void*)&pAddr->padding22, sizeof(pAddr->padding22));
      pAddr->country = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype23, sizeof(pAddr->ftype23));
      if (pAddr->ftype23 != m_cPalmHeader.getHeader().field_entry[22])
         return -1;

      ifs.read((char*)(void*)&pAddr->padding23, sizeof(pAddr->padding23));
      pAddr->note = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype24, sizeof(pAddr->ftype24));
      if (pAddr->ftype24 != m_cPalmHeader.getHeader().field_entry[23])
         return -24;

      ifs.read((char*)(void*)&pAddr->is_private, sizeof(pAddr->is_private));

      ifs.read((char*)(void*)&pAddr->ftype25, sizeof(pAddr->ftype25));
      if (pAddr->ftype25 != m_cPalmHeader.getHeader().field_entry[24])
         return -25;

      ifs.read((char*)(void*)&pAddr->category, sizeof(pAddr->category));

      ifs.read((char*)(void*)&pAddr->ftype26, sizeof(pAddr->ftype26));
      if (pAddr->ftype26 != m_cPalmHeader.getHeader().field_entry[25])
         return -26;

      ifs.read((char*)(void*)&pAddr->padding26, sizeof(pAddr->padding26));
      pAddr->custom1 = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype27, sizeof(pAddr->ftype27));
      if (pAddr->ftype27 != m_cPalmHeader.getHeader().field_entry[26])
         return -27;

      ifs.read((char*)(void*)&pAddr->padding27, sizeof(pAddr->padding27));
      pAddr->custom2 = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype28, sizeof(pAddr->ftype28));
      if (pAddr->ftype28 != m_cPalmHeader.getHeader().field_entry[27])
         return -28;

      ifs.read((char*)(void*)&pAddr->padding28, sizeof(pAddr->padding28));
      pAddr->custom3 = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype29, sizeof(pAddr->ftype29));
      if (pAddr->ftype29 != m_cPalmHeader.getHeader().field_entry[28])
         return -29;

      ifs.read((char*)(void*)&pAddr->padding29, sizeof(pAddr->padding29));
      pAddr->custom4 = read_string(ifs);

      ifs.read((char*)(void*)&pAddr->ftype30, sizeof(pAddr->ftype30));
      if (pAddr->ftype30 != m_cPalmHeader.getHeader().field_entry[29])
         return -30;

      ifs.read((char*)(void*)&pAddr->display_phone, sizeof(pAddr->display_phone));   
    }

   return i;
}

int CPalmAddress::print()
{
 vector<Address_DAT*>::iterator iter;

   printf( "<Addresses>\n");

//   CPalmHeader::print(fp);

cout << "addresses:" << m_aAddr.size() << endl;

   for(iter = m_aAddr.begin(); iter != m_aAddr.end(); iter++) {

printf("%x\n", *iter);
continue;

cout << "line:" << __LINE__ << endl;

      printf("\t<Address record_id='%ld' "
                 "status_field='%ld' position='%ld' "
                 "is_private='%ld' category='%ld' "
                 "display_phone='%ld'>\n",
              (*iter)->record_id, 
              (*iter)->status_field, 
              (*iter)->position,
              (*iter)->is_private, 
              (*iter)->category,
              (*iter)->display_phone);
cout << "line:" << __LINE__ << endl;

//      skip_new_line((*iter)->name);
//      skip_new_line((*iter)->last);
      printf( "\t\t<Name first=\"%s\" last=\"%s\" />\n", (*iter)->name.data(), (*iter)->last.data());
      skip_new_line((*iter)->title);
      printf( "\t\t<Title name=\"%s\" />\n", (*iter)->title.data());
      skip_new_line((*iter)->company);
      printf( "\t\t<Company name=\"%s\" />\n", (*iter)->company.data());
      printf( "\t\t\t<Phones>\n");

cout << "line:" << __LINE__ << endl;

      skip_new_line((*iter)->phone1);
      skip_new_line((*iter)->phone2);
      skip_new_line((*iter)->phone3);
      skip_new_line((*iter)->phone4);
      skip_new_line((*iter)->phone5);
      printf( "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", (*iter)->phone_id1,  (*iter)->phone1.data());
      printf( "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", (*iter)->phone_id2,  (*iter)->phone2.data());
      printf( "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", (*iter)->phone_id3,  (*iter)->phone3.data());
      printf( "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", (*iter)->phone_id4,  (*iter)->phone4.data());
      printf( "\t\t\t\t<Phone id='%ld'>%s</Phone>\n", (*iter)->phone_id5,  (*iter)->phone5.data());
      printf( "\t\t\t</Phones>\n");

cout << "line:" << __LINE__ << endl;

      skip_new_line((*iter)->address);
      skip_new_line((*iter)->city);
      skip_new_line((*iter)->state);
      skip_new_line((*iter)->zip);
      skip_new_line((*iter)->country);
      printf( "\t\t\t<Address address=\"%s\" city=\"%s\" state=\"%s\" zip=\"%s\" country=\"%s\" />\n",
         (*iter)->address.data(),
         (*iter)->city.data(),
         (*iter)->state.data(),
         (*iter)->zip.data(),
         (*iter)->country.data());


cout << "line:" << __LINE__ << endl;

      skip_new_line((*iter)->note);
      printf("\t\t\t<Note value=\"%s\" />\n", (*iter)->note.data());

      skip_new_line((*iter)->custom1);
      skip_new_line((*iter)->custom2);
      skip_new_line((*iter)->custom3);
      skip_new_line((*iter)->custom4);
      printf( "\t\t\t<Customs>\n");
      printf( "\t\t\t\t<Custom id='1'>%s</Custom>\n", (*iter)->custom1.data());
      printf( "\t\t\t\t<Custom id='2'>%s</Custom>\n", (*iter)->custom2.data());
      printf( "\t\t\t\t<Custom id='3'>%s</Custom>\n", (*iter)->custom3.data());
      printf( "\t\t\t\t<Custom id='4'>%s</Custom>\n", (*iter)->custom4.data());
      printf( "\t\t\t</Customs>\n");
      printf("\t</Address>");

cout << "line:" << __LINE__ << endl;
   }

   printf( "</Addresses>\n");

   return 0;
}
