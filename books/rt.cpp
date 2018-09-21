#include <string>
#include <iostream>
#include <sstream>
#include <boost/regex.hpp>
using namespace std;

string bname = "2008 - Professional SlickEdit (ISBN 978-0-470-12215-0).rev.rar";

int main(int argc, char **argv)
{
 string sre = "^.*\\([isbna]{4}\\s*(.*)\\).*$";
 boost::regex re;
 boost::cmatch matches;

   try {
      // Set up the regular expression for case-insensitivity
      re.assign(sre, boost::regex_constants::icase);
   }
   catch (boost::regex_error& e) {
      cout << sre << " is not a valid regular expression: \""
           << e.what() << "\"" << endl;
      return 1;
   }
   if (boost::regex_match(bname.c_str(), matches, re)) {
      // matches[0] contains the original string.  matches[n]
      // contains a sub_match object for each matching
      // subexpression
      for (int i = 1; i < matches.size(); i++) {
         // sub_match::first and sub_match::second are iterators that
         // refer to the first and one past the last chars of the
         // matching subexpression
         string match(matches[i].first, matches[i].second);
         cout << "\tmatches[" << i << "] = " << match << endl;
      }
   } else {
      cout << "The regexp \"" << re << "\" does not match \"" << bname << "\"" << endl;
   }
   
   return 0;
}
