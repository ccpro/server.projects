#include <string>
#include <iostream>
#include <sstream>
using namespace std;

#include <libpq++.h>

#include "config.h"
#include "sql.h"
#include "news.h"

extern Config conf;

int News::addNews(PgDatabase &pg, const int type, const string subject, const string news, const int owner, const int level)
{
 InsertSQL insSql;
 SelectSQL selSql;
 TransactionSQL trans;
 ExecStatusType status;
 string sql;
 int news_id;
   
   if ( news.empty() ) {
      return 1;
   }

   pg.Exec(trans.startTransaction().c_str());

   selSql.AddField("nextval('news_id_seq')");
   sql = selSql.BuildSQL();
   conf.Debug(sql);

   status = pg.Exec(sql.c_str());
   if ( status != PGRES_TUPLES_OK ) {
    string throw_msg = "SQL ERROR:" + string(pg.ErrorMessage()) + " " + sql;

      throw throw_msg.c_str();
   }

   news_id = atoi(pg.GetValue(0, 0));

   insSql.SetTableName("news");
   insSql.AddField("id", news_id);
   insSql.AddField("news", news);
   insSql.AddField("subject", subject);
   insSql.AddField("owner", owner);
   insSql.AddField("level", level);

   sql = insSql.BuildSQL();
   conf.Debug(sql);
         
   status = pg.Exec(sql.c_str());
   if ( status != PGRES_COMMAND_OK ) {
    string throw_msg = "SQL ERROR:" + string(pg.ErrorMessage()) + " " + sql;

      throw throw_msg.c_str();
   }

   insSql.Clear();
   insSql.SetTableName("news_types");
   insSql.AddField("news_id", news_id);
   insSql.AddField("type_id", type);

   sql = insSql.BuildSQL();
   conf.Debug(sql);
         
   status = pg.Exec(sql.c_str());

   pg.Exec(trans.endTransaction().c_str());

   return 0;
}
