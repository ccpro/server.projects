#include <string>
#include <iostream>
#include <sstream>
using namespace std;

#include "sha1.h"
#include "uniq_value.h"

string UniqValue::value(const string s)
{
 SHA1 sha;
 unsigned        message_digest[5];
 string res;

   if ( s.empty() ) {
      return res;
   }
   sha << s.c_str();
   
   if (sha.Result(message_digest)) {
    stringstream out;
      for (int i=0; i<5; ++i) {
         out << hex << message_digest[i];
      }
      res = out.str();
   }
   
   return res;
}
/*
int main(int argc, char ** argv)
{
 string res;

   if (argc > 1) {
      for (int i=1; i< argc; ++i) {
         res = UniqValue::value(argv[i]);
         if ( !res.empty() ) {
            cout << argv[i] << " :: " << res.length() << " :: " << res << endl;
         } else {
            cout << argv[i] << " -- failed" << endl;
         }
      }
   }
   
   return 0;
}


sub uniq_val($)
{
 my $val = shift || return 0;
 
  my $uniq = 0;   
   my $len = length($val);
    my @arr = split('', sha1_hex(reverse($val)));
     my $i = 0;
             
                foreach (@arr) {
                      $uniq += (ord($_) << $i++);
                            $i %= 16;
                               }
                               
                                  $debug && print "$val ".($uniq * 100 + $len)."\n";
                                  
                                     $uniq * 100 + $len;
                                     }
*/
