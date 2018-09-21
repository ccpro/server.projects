#include <string>
#include <iostream>
#include <sstream>
//#include <boost/regex.hpp>
using namespace std;

#include "uniq_value.h"
#include "book.h"

Book::Book(const string& name):m_iFlags(0)
{
   initRegex();
   parse(name);
}

void Book::initRegex()
{
 string sre = "^.*\\([isbna ]{4,8}(.*)\\).*$";

   try {
      // Set up the regular expression for case-insensitivity
      m_Regex.assign( sre, boost::regex_constants::icase);
   }
   catch (boost::regex_error& e) {
      cout << sre << " is not a valid regular expression: \"" 
           << e.what() << "\"" << endl;
   }
}

int Book::parse(const string& name)
{
 char *buf;
 int pos = 0;
 string _path;

   if ( name.empty() ) {
      return 0;
   }

   buf = (char *)name.c_str();
   while (*buf=='.' || *buf=='/') {
      ++buf;
   }

   _path = string(buf);
   pos = _path.find_last_of("/");
   
//   path.setPath(path.substr(0, pos));
   m_sName = _path.substr(pos+1);
   m_sSHA1 = UniqValue::value(m_sName);
   m_sISBN = parseISBN(m_sName);
   
   if ( !m_sISBN.empty() ) {
      m_iFlags |= ISBN_FOUND;
   }
   
   return m_iFlags;
}

string Book::parseISBN(const string& name) const
{
 string isbn;
 boost::cmatch matches;

   if ( name.empty() ) {
      return isbn;
   }

   if ( boost::regex_match(name.c_str(), matches, m_Regex) ) {
      if ( matches.size() > 1 ) {
       string match(matches[1].first, matches[1].second);
         isbn = match;
      }
   }
   
   if ( !isbn.empty() ) {
    string tmp;

      for (string::iterator i = isbn.begin(); i != isbn.end(); ++i) {
         if ( *i == ' ' || *i == '-' || *i == '.' ) {
            continue;
         }
         tmp += *i;
      }
      isbn = tmp;
   }

   return isbn;
}

/*
// g++ book.cpp -I /usr/local/include -L/usr/local/lib -lm -lboost_regex uniq_value.cpp  sha1.cpp
int main(int argc, char **argv)
{
 string str = "./Programminig/Perl/2005 - Advanced Perl Programming (ISBN 0-596-00456-7).rev.rar";
 Book book(str);

   cout << book.getPath() << endl
        << book.getName() << endl
        << book.getISBN() << endl
        << book.getHash() << endl;
}
*/
