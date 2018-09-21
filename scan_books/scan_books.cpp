#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <sys/stat.h>
#include <libpq++.h>

#include "data_path.h"
#include "config.h"
#include "path.h"
#include "book.h"
#include "sql.h"
#include "news.h"

int debug_flag = 0;
vector<string> new_books_list;
vector<string> removed_books_list;
Config conf;
//PgDatabase data;

int scan_new_files(PgDatabase &pg, const string& path);

int main(int argc, char **argv)
{
 string result;

   conf.parse(argc, argv);
   conf.Debug("Debug Output ON");

   PgDatabase pg("dbname=ccpro_noip_org");

   if ( pg.ConnectionBad() ) {
      cerr << "Connection to database failed." << endl
           << "Error returned: " 
           << pg.ErrorMessage() << endl;
      exit(1);
   }
   if ( !conf.getPath().empty() ) {
      chdir(conf.getPath().c_str());
   }
   scan_new_files(pg, ".");

   {
    vector<string>::iterator vi;

      result = "";
      if ( new_books_list.size() ) {
         result = "New files:\n";
         for (vi=new_books_list.begin(); vi != new_books_list.end(); ++vi) {
            result += *vi;
            result += "\n";
         }
      }
      if ( removed_books_list.size() ) {
         result += "Removed files:\n";
         for (vi=removed_books_list.begin(); vi != removed_books_list.end(); ++vi) {
            result += *vi;
            result += "\n";
         }
      }
   }

   if ( conf.isDebug() ) {
      if ( !result.empty() ) {
         cout << result;
      } else {
         cout << "Nothing to say" << endl;
      }
   }
   
   if ( !result.empty() ) {
    News news;
    string subject = "New Books";

      news.addNews(pg, News::TYPE_MUSIC_NEWS, subject, result, 1, 1);
   }

   return 0;
}

int add_file(PgDatabase &pg, const Book& book)
{
 InsertSQL insSql;
 ExecStatusType status;
 string sql;

   insSql.SetTableName("books_list");
   insSql.AddField("path_id", book.getPathId());
   insSql.AddField("book", book.getName());
   insSql.AddField("isbn", book.getISBN());
   insSql.AddField("hash", book.getHash());

   sql = insSql.BuildSQL();

   conf.Debug("going to add" + conf.getPath()+"/"+book.getPath() + "/" + book.getName());
   conf.Debug(book.getISBN());
   conf.Debug(book.getHash());
   conf.Debug(sql);

   status = pg.Exec(sql.c_str());
   if ( status != PGRES_COMMAND_OK ) {
    string throw_msg = "SQL ERROR:" + string(pg.ErrorMessage()) + " " + sql;

      throw throw_msg.c_str();
   }

   new_books_list.push_back(book.getName());

   return 0;
}

int remove_record(PgDatabase &pg, const string& full_name)
{
 DeleteSQL delSql;
 Book book;
 string sql;
 ExecStatusType status;

   if (full_name.empty() || !full_name.length()) {
      return 1;
   }
   
   book.parse(full_name);
   delSql.SetTableName("books_list");
   delSql.AddField("hash", book.getHash());
   
   sql = delSql.BuildSQL();
   conf.Debug(sql);

   status = pg.Exec(sql.c_str());
   if ( status != PGRES_COMMAND_OK ) {
    string throw_msg = "SQL ERROR:" + string(pg.ErrorMessage()) + " " + sql;

      throw throw_msg.c_str();
   }
   
   cout << "File " << full_name << " was deleted" << endl;
   
   return 1;
}

int file_removed(PgDatabase &pg, const Path& path)
{
 SelectSQL selSql;
 ExecStatusType status;
 string sql;
 string file_name;
 string full_file_name;
 vector<string> files_delete;
 vector<string>::iterator vi;
 struct stat results;
   
   selSql.SetTableName("books_list");
   selSql.AddField("book");
   selSql.AddEqual("path_id", path.getPathId());

   sql = selSql.BuildSQL();
   conf.Debug(sql);

   status = pg.Exec(sql.c_str());
   if ( status != PGRES_TUPLES_OK ) {
    string throw_msg = "SQL ERROR:" + string(pg.ErrorMessage()) + " " + sql;

      throw throw_msg.c_str();
   }
   for (int i=0; i<pg.Tuples(); ++i) {
      file_name = pg.GetValue(i, 0);
      full_file_name = conf.getPath() + "/" + path.getPath() + "/" + file_name;
      conf.Debug("Going to check file " + full_file_name);

      if ( stat(full_file_name.c_str(), &results) ) {
         files_delete.push_back(path.getPath() + "/" + file_name);
      }
   }
   conf.Debug("Deleted files. Going to remove from DB");
   for (vi = files_delete.begin(); vi != files_delete.end(); ++vi ) {
      conf.Debug(*vi);
      removed_books_list.push_back(*vi);
      
      remove_record(pg, *vi);
   }

   return 0;
}

int file_exists(PgDatabase &pg, const Book& book)
{
 SelectSQL selSql;
 ExecStatusType status;
 int count = 0;
 string sql;

   selSql.SetTableName("books_list");
   selSql.AddField("count(*)");
   selSql.AddEqual("hash", book.getHash());

   sql = selSql.BuildSQL();
   conf.Debug(sql);

   status = pg.Exec(sql.c_str());
   if ( status != PGRES_TUPLES_OK ) {
    string throw_msg = "SQL ERROR:";
      throw_msg += string(pg.ErrorMessage()) + " " + sql;
      throw throw_msg.c_str();
   }
   for (int i=0; i<pg.Tuples(); ++i) {
      count = atoi(pg.GetValue(i, 0));
   }
   
   return count;
}

long check_path(PgDatabase &pg, const Path& path)
{
 SelectSQL selSql;
 string sql;
 long id = 0;
 ExecStatusType status;

   selSql.SetTableName("book_paths");
   selSql.AddField("path_id");
   selSql.AddEqual("path", path.getPath());

   sql = selSql.BuildSQL();
   conf.Debug(sql);

   status = pg.Exec(sql.c_str());
   if ( status != PGRES_TUPLES_OK ) {
    string throw_msg = "SQL ERROR:" + string(pg.ErrorMessage()) + "\n" + sql;

      throw throw_msg.c_str();
   }
   if (pg.Tuples()) {
      id = atol(pg.GetValue(0, 0));
   }

   if ( !id ) {
    InsertSQL isql;
    string oid;

      isql.SetTableName("book_paths");
      isql.AddField("path", path.getPath().empty() ? "." : path.getPath());
      
      sql = isql.BuildSQL();
      conf.Debug(sql);
         
      status = pg.Exec(sql.c_str());
      if ( status != PGRES_COMMAND_OK ) {
       string throw_msg = "SQL ERROR:" + string(pg.ErrorMessage()) + " " + sql;

         throw throw_msg.c_str();
      } else {

         selSql.Clear();
         selSql.SetTableName("book_paths");
         selSql.AddField("path_id");
         selSql.AddEqual("oid", pg.OidStatus());

         sql = selSql.BuildSQL();
         conf.Debug(sql);
         status = pg.Exec(sql.c_str());         
         if ( status != PGRES_TUPLES_OK ) {
          string throw_msg = "SQL ERROR:" + string(pg.ErrorMessage()) + "\n" + sql;
             
            throw throw_msg.c_str();
         }
         if (pg.Tuples()) {
            id = atol(pg.GetValue(0, 0));
            conf.Debug(string("Found new PathId : ") + pg.GetValue(0, 0));
         }
      }
   }
   
   return id;
}

int scan_new_files(PgDatabase &pg, const string& path)
{
 DataPath dPath(path);
 vector<string> files;
 vector<string> dirs;
 vector<string>::iterator iter;
 string file_name;
 Book book;
 int exist = 0;

   try {
      book.setPath(path == "." ? "" : path);
      book.setPathId(check_path(pg, (Path)book));
   }
   catch (const char *e) {
      cerr << "CheckPath Error: " << e << endl;
      return -1;
   }

   files = dPath.getAllFiles();

   for(iter = files.begin(); iter != files.end(); ++iter) {

      file_name = path+"/"+(*iter);
      conf.Debug(file_name);

      book.parse(file_name);
      try {
         exist = file_exists(pg, book);
      }
      catch (const char *e) {
         cerr << "Error: " << e << endl;
         break;
      }
      if ( debug_flag ) {
         cout << (exist ? "exists" : "do not exists") << endl;
      }
      if ( !exist ) {
         try {
            add_file(pg, book);
         }
         catch (const char *e) {
            cerr << "Error: " << e << endl;
            break;
         }
      }
   }

   try {
      file_removed(pg, (Path)book);
   }
   catch (const char *e) {
      cerr << "Error: " << e << endl;
      return -2;
   }

   dirs  = dPath.getAllDirs();

   for(iter = dirs.begin(); iter != dirs.end(); ++iter) {
      scan_new_files(pg, path+"/"+(*iter));
   }

   return 0;
}
