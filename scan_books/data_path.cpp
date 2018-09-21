#include <dirent.h> 
#include "data_path.h"

void DataPath::setPath(const string s)
{
   if ( m_Path != s ) {
      m_Path = s; 
   }
}

vector<string> DataPath::getAllDirs() const
{
 DIR *dir;
 struct dirent *ent;
 vector<string> l_paths;
 
   dir = opendir( m_Path.empty() ? "." : m_Path.c_str() );
   
   if ( !dir ) {
      return l_paths;
   }

   while ( (ent = readdir(dir)) ) {
      if ( strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..") && ent->d_type == DT_DIR ) {
         l_paths.push_back(string(ent->d_name));
      }
   }
   
   closedir(dir);

   return l_paths;
}

vector<string> DataPath::getAllFiles() const
{
 DIR *dir;
 struct dirent *ent;
 vector<string> l_paths;
 
   dir = opendir( m_Path.empty() ? "." : m_Path.c_str() );
   
   if ( !dir ) {
      return l_paths;
   }

   while ( (ent = readdir(dir)) ) {
      if ( strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..") && ent->d_type != DT_DIR ) {
         l_paths.push_back(string(ent->d_name));
      }
   }
   
   closedir(dir);

   return l_paths;
}
/*
int walker(const string path)
{
 DataPath dPath(path);
 vector<string> dirList = dPath.getAllDirs();
 vector<string> fileList = dPath.getAllFiles();
 vector<string>::iterator iter;

   cout << "Files in dir " << path << endl;
   for (iter=fileList.begin(); iter != fileList.end(); iter++) {
      cout << (*iter) << endl;
   }
   for (iter=dirList.begin(); iter != dirList.end(); iter++) {
      walker(path+"/"+(*iter));
   }
}

int main(int argc, char **argv)
{
   walker("/usr/local/books");

   return 0;
}
*/
