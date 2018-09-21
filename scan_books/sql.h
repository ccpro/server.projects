#ifndef __SQL_H__
#define __SQL_H__

#include <map>
using namespace std;

typedef map<string, string, less<string> > TStrStrMap;

class SQLWhereClause
{
   private:
      string m_sWhere;
      
      string Prefix() const {return m_sWhere.empty() ? "WHERE " : " AND ";}

   public:
      void Clear(){m_sWhere.clear();}

      void AddEqual(const string& field_name, const int data);
      void AddEqual(const string& field_name, const string& data);

      void AddLess(const string& field_name, const int data);
      void AddLess(const string& field_name, const string& data);

      void AddGreater(const string& field_name, const int data);
      void AddGreater(const string& field_name, const string& data);

      void AddLike(const string& field_name, const string& data);

      void AddIsNull(const string& field_name);

      void AddBetween(const string& field_name, const int i1, const int i2);
      void AddBetween(const string& field_name, const string& s1, const string& s2);

//      void AddIn(const string& field_name, const vector<int>& data);
//      void AddIn(const string& field_name, const vector<string>& data);

      string GetWhereClause() const {return m_sWhere;}
};

class SQL
{
   private:
      string m_sTableName;

   public:
      virtual ~SQL(){};
   
      static string Quote(const string s);
      virtual void AddField(const string& field_name, const int data) = 0;
      virtual void AddField(const string& field_name, const string& data=string()) = 0;

      void SetTableName(const string& table_name);
      string GetTableName() const { return m_sTableName;}

      virtual string BuildSQL() const = 0;
};

class TransactionSQL
{
   public:
      string startTransaction() const {return "BEGIN";};
      string endTransaction() const {return "END";};
};

class SelectSQL : public SQL, public SQLWhereClause
{
   private:
      TStrStrMap m_mapFields;
      bool m_bDistinct;

   public:
      ~SelectSQL(){m_mapFields.clear();}

      void AddField(const string& field_name, const int data);
      void AddField(const string& field_name, const string& data=string());
      
      void SetDistinct(bool bDistinct){m_bDistinct=bDistinct;}
      bool IsDistinct(){ return m_bDistinct;}

      void Clear(){m_mapFields.clear(); m_bDistinct=false;}
      string BuildSQL() const;
};

class InsertSQL : public SQL
{
   private:
      TStrStrMap m_mapData;

   public:
      ~InsertSQL(){m_mapData.clear();}

      void AddField(const string& field_name, const int data);
      void AddField(const string& field_name, const string& data=string());
   
      void Clear(){m_mapData.clear();}
     string BuildSQL() const;
};

class UpdateSQL : public SQL, public SQLWhereClause
{
   private:
      TStrStrMap m_mapData;

   public:
      ~UpdateSQL(){m_mapData.clear();}

      void AddField(const string& field_name, const int data);
      void AddField(const string& field_name, const string& data=string());

      void Clear(){m_mapData.clear();}
      string BuildSQL() const;
};

class DeleteSQL : public SQL, public SQLWhereClause
{
   public:
      void AddField(const string& field_name, const int data){};
      void AddField(const string& field_name, const string& data){};

      string BuildSQL() const;
};

#endif // __SQL_H__
