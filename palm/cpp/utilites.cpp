#include "common.h"

std::string read_string(ifstream &ifs)
{
 byte b_size;
 ushort w_size;
 char *buf;
 static std::string str = "";
    
   if ( !ifs.is_open() ) {
      return str;
   }

   ifs.read((char*)(void*)&b_size, sizeof(b_size));

   if (b_size == 0xFF) {
      ifs.read((char*)(void*)&w_size, sizeof(w_size));
                    
      buf = new char[w_size+1];
                              
      ifs.read(buf, w_size);
      buf[w_size] = '\0';
      str = buf;
      delete buf;
      
   } else {
     
      buf = new char(b_size+1);
                                                      
      ifs.read(buf, b_size);
      buf[b_size] = '\0';
      
      str = buf;
      delete buf;
   }
                                                                
   return str;
}

int skip_new_line(string &str)
{
   if (str.length() == 0) {
      return 0;
   }

   for (uint i=0; i<str.length(); ++i) {
      if (str.at(i) == '\r') {
         str.at(i) = ',';
      } else if (str.at(i) == '\n') {
         str.at(i) = ' ';
      }
   }

   return 1;
}
                                                                                                        
