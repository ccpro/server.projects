#ifndef __BOOK_H__
#define __BOOK_H__

#include <boost/regex.hpp>
using namespace std;

#include "path.h"

class Book : public Path
{
   private:
      string m_sName;
      string m_sISBN;
      string m_sSHA1;
      boost::regex m_Regex;
      int m_iFlags;

      void initRegex();
      string parseISBN(const string &name) const;

   public:
   
      static const int ISBN_FOUND  = 0x0001;

      Book():m_iFlags(0){initRegex();}
      Book(const string& name);

      int getFlags(){return m_iFlags;}

      int parse(const string& name);

//      string getPath() const {return m_sPath;}
      string getName() const {return m_sName;}
      string getISBN() const {return m_sISBN;}
      string getHash() const {return m_sSHA1;}
};


#endif // __BOOK_H__
