#ifndef __PATH_H__
#define __PATH_H__

class Path
{
   private:
      long m_lId;
      string m_sPath;

   public:
      Path():m_lId(0){}
      Path(const string& path):m_lId(0), m_sPath(path){}
      Path(const long id, const string& path):m_lId(id), m_sPath(path){}

      void setPathId(const long id){m_lId=id;}
      void setPath(const string& path){m_sPath = path;}
      
      long getPathId() const {return m_lId;}
      string getPath() const {return m_sPath;}
};

#endif // __PATH_H__
