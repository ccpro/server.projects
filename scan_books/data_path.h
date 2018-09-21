#ifndef __H_DATA_PATH__
#define __H_DATA_PATH__
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <dirent.h> 

class DataPath
{
   private:
      string m_Path;
      
   public:
      DataPath(string s):m_Path(s){}
      
      string getPath() const {return m_Path;}
      void   setPath(const string s);
      
      vector<string> getAllDirs() const;
      vector<string> getAllFiles() const;
};

#endif // __H_DATA_PATH__
