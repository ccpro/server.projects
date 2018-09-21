#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <iostream>
#include <string>
using namespace std;

class Config
{
   private:
     string	m_sPath;
     long	m_bFlags;

     void usage(const string pname);

   public:
     static const int DEBUG = 0x0001;
     
     Config(const string& path="."):m_sPath(path), m_bFlags(0){}
     void parse(int argc, char **argv);

     long getFlags(){return m_bFlags;}
     string getPath() const {return m_sPath;}

     void setPath(const string& path) { if (!path.empty()) m_sPath = path;}

     void setFlag(long flag)  { m_bFlags |= flag;}
     void unsetFlag(long flag){ m_bFlags ^= flag;}
          
     bool isDebug(){return m_bFlags&DEBUG ? true : false;}

     void Debug (const string& s) { if (isDebug() && !s.empty() ) cout << s << endl;}
};

#endif // __CONFIG_H__
