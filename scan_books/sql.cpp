#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
using namespace std;

#include "sql.h"

void SQL::SetTableName(const string& table_name)
{
   if ( !table_name.empty() ) {
      m_sTableName = table_name;
   }
}

string SQL::Quote(const string str)
{
 string quoted;
 string::const_iterator i;

   quoted = "'";
   for (i=str.begin(); i != str.end(); ++i) {
      if ( *i == '\'' ) {
         quoted += "\'";
      }
      quoted += *i;
   }
   quoted += "'";

   return quoted;
}

void SQLWhereClause::AddEqual(const string& field_name, const int data)
{
   if ( field_name.empty() || !field_name.length() ) {
      return;
   }
 stringstream si;
 
   si << data;
   m_sWhere += Prefix() + field_name + "=" + si.str();
}

void SQLWhereClause::AddEqual(const string& field_name, const string& data)
{
   if ( field_name.empty() || !field_name.length() || data.empty() ) {
      return;
   }

   m_sWhere += Prefix() + field_name + "=" + SQL::Quote(data);
}

void SQLWhereClause::AddLess(const string& field_name, const int data)
{
   if ( field_name.empty() || !field_name.length() ) {
      return;
   }
 stringstream si;
 
   si << data;
   m_sWhere += Prefix() + field_name + "<" + si.str();
}
void SQLWhereClause::AddLess(const string& field_name, const string& data)
{
   if ( field_name.empty() || !field_name.length() || data.empty() ) {
      return;
   }

   m_sWhere += Prefix() + field_name + "<" + SQL::Quote(data);
}
void SQLWhereClause::AddGreater(const string& field_name, const int data)
{
   if ( field_name.empty() || !field_name.length() ) {
      return;
   }
 stringstream si;
 
   si << data;
   m_sWhere += Prefix() + field_name + ">" + si.str();
}
void SQLWhereClause::AddGreater(const string& field_name, const string& data)
{
   if ( field_name.empty() || !field_name.length() || data.empty() ) {
      return;
   }

   m_sWhere += Prefix() + field_name + ">" + SQL::Quote(data);
}
void SQLWhereClause::AddLike(const string& field_name, const string& data)
{
   if ( field_name.empty() || !field_name.length() || data.empty() ) {
      return;
   }

   m_sWhere += Prefix() + field_name + "LIKE" + SQL::Quote(data);
}
void SQLWhereClause::AddIsNull(const string& field_name)
{
   if ( field_name.empty() || !field_name.length() ) {
      return;
   }

   m_sWhere += Prefix() + field_name + "IS NULL";
}
void SQLWhereClause::AddBetween(const string& field_name, const int i1, const int i2)
{
   if ( field_name.empty() || !field_name.length() ) {
      return;
   }
 stringstream si;
 
   si << i1;
   m_sWhere += Prefix() + field_name + " BETWEEN " + si.str();
   si << i2;
   m_sWhere += " AND " + si.str();
   
}
void SQLWhereClause::AddBetween(const string& field_name, const string& d1, const string& d2)
{
   if ( field_name.empty() || !field_name.length() || (d1.empty() && d2.empty()) ) {
      return;
   }

   m_sWhere += Prefix() + field_name + "BETWEEN" + SQL::Quote(d1) + 
                                        " AND " + SQL::Quote(d2);
}
/*
void SQLWhereClause::AddIn(const string& field_name, const vector<int>& data)
{
}
void SQLWhereClause::AddIn(const string& field_name, const vector<string>& data)
{
}
*/
void SelectSQL::AddField(const string& field_name, const int data)
{
   if ( field_name.empty() || !field_name.length() ) {
      return;
   }
 stringstream si;
  
   si << data;
   m_mapFields[field_name] = si.str();
}

void SelectSQL::AddField(const string& field_name, const string& data)
{
   if ( field_name.empty() || !field_name.length() ) {
      return;
   }
   if ( data.empty() ) {
      m_mapFields[field_name] = string();
      return;
   }
   
   m_mapFields[field_name] = Quote(data);
}

string SelectSQL::BuildSQL() const
{
 string sql;
 string data;
 string as;
 TStrStrMap::const_iterator iter;
 
   if ( GetTableName().empty() ) {  
      throw "Table Name is Empty";
   }

   for (iter = m_mapFields.begin(); iter != m_mapFields.end(); ++iter) {
      as = iter->second;
      data += ", "+iter->first+ (as.empty() ? "" : " AS "+as);
   }
   if ( !data.empty() ) {
      data.at(0) = ' ';
      sql = "SELECT " + data + " FROM " + GetTableName() + " " + GetWhereClause();
   }
   
   return sql;
}

void UpdateSQL::AddField(const string& field_name, const int data)
{
   if ( field_name.empty() || !field_name.length() ) {
      return;
   }
 stringstream si;
  
   si << data;
   m_mapData[field_name] = si.str();
}

void UpdateSQL::AddField(const string& field_name, const string& data)
{
   if ( field_name.empty() || !field_name.length() ) {
      return;
   }
   if ( data.empty() ) {
      m_mapData[field_name] = "NULL";
      return;
   }
   
   m_mapData[field_name] = Quote(data);
}

string UpdateSQL::BuildSQL() const
{
 string sql;
 string data;
 TStrStrMap::const_iterator iter;
 
   if ( GetTableName().empty() ) {  
      throw "Table Name is Empty";
   }

   for (iter = m_mapData.begin(); iter != m_mapData.end(); ++iter) {
      data += ", "+iter->first+"="+iter->second;
   }
   if ( !data.empty() ) {
      data.at(0) = ' ';
      sql = "UPDATE " + GetTableName() + " SET" + data + " " + GetWhereClause();
   }
   
   return sql;
}

string DeleteSQL::BuildSQL() const
{
 string sql;
 
   if ( GetTableName().empty() ) {  
      throw "Table Name is Empty";
   }

   sql = "DELETE FROM " + GetTableName() + " " + GetWhereClause();
   
   return sql;
}

void InsertSQL::AddField(const string& field_name, const int data)
{
   if ( field_name.empty() || !field_name.length() ) {
      return;
   }
 stringstream si;
  
   si << data;
   m_mapData[field_name] = si.str();
}

void InsertSQL::AddField(const string& field_name, const string& data)
{
   if ( field_name.empty() || !field_name.length() ) {
      return;
   }
   if ( data.empty() ) {
      m_mapData[field_name] = "NULL";
      return;
   }
   
   m_mapData[field_name] = Quote(data);
}

string InsertSQL::BuildSQL() const
{
 string sql;
 string fields, values;
 TStrStrMap::const_iterator iter;

   if ( GetTableName().empty() ) {
      throw "Table Name is Empty";
   }
   
   for (iter = m_mapData.begin(); iter != m_mapData.end(); ++iter) {
      fields += ","+iter->first;
      values += ","+iter->second;      
   }

   if ( !fields.empty() && !values.empty() ) {
      fields.at(0) = '(';
      values.at(0) = '(';
      sql = "INSERT INTO " + GetTableName() + fields + ") VALUES" + values + ")";
   }

   return sql;
}

/*
int main(int argc, char **argv)
{
 InsertSQL iSql;
 UpdateSQL uSql;
 SelectSQL sSql;
 DeleteSQL dSql;

   iSql.SetTableName("insert_table");
   iSql.AddField("111", "O'Hara");
   iSql.AddField("333", 444);

   uSql.SetTableName("update_table");
   uSql.AddField("111", "O'Hara");
   uSql.AddField("333", "444");
   uSql.AddEqual("id", 2);

   sSql.SetTableName("select_table");
   sSql.AddField("f1", string());
   sSql.AddField("substr(f2, 0, 4)", "ff");
   sSql.AddBetween("id", 1, 2);
   sSql.AddEqual("name", "Keky");

   dSql.SetTableName("delete_table");
   dSql.AddLess("id", 1);
   dSql.AddEqual("name", "Keky");

   try {
      cout << iSql.BuildSQL() << endl;
      cout << uSql.BuildSQL() << endl;
      cout << sSql.BuildSQL() << endl;
      cout << dSql.BuildSQL() << endl;
   }
   catch (const char *e) {
      cerr << "BuildSQL failed " << e << endl;
   }

   return 0;
}
*/
