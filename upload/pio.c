/****************************************************************************
 * pio.c
 *
 *    Generic Postgres IO stuff
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pio.h"
#include "auth.h"

#ifdef DEBUG
static char *rcsid = "$Id: pio.c,v 1.3 2007/12/08 02:16:11 ccpro Exp $";
#endif

extern void       fancy_header            ( char *t, char *r );
extern int        to_html                 (char **esc_str);

#ifndef STRLEN
#define STRLEN 80
#endif

typedef struct tt_cache_entry {
 char *table;
 char *var;
 char *where;
 char *val;
 char *result;
 struct tt_cache_entry *next;
} t_cache_entry;

t_cache_entry *cache_entry = NULL;

extern t_user user;

PGconn *conn;
PGresult *res;

unsigned long pio_query_count = 0;

/****************************************************************************
 * psql_connect()
 ***************************************************************************/
int psql_connect(PGconn **conn)
{
 char *pghost;
 char *pgport;
 char *pgoptions;
 char *pgtty;
 char *dbName;

   pghost = user.db_server;
   pgport = "5432";
   pgoptions = NULL;
   pgtty = NULL;
   //dbName = "test";
   dbName = "ccpro_noip_org";

   *conn = PQsetdbLogin(pghost, pgport, pgoptions, pgtty, dbName, "ccpro", "");

   if (PQstatus(*conn) == CONNECTION_BAD) {
#ifdef DEBUG
      fprintf(stderr, "Connection to database '%s' failed.\n", dbName);
      fprintf(stderr, "%s", PQerrorMessage(*conn));
#endif
      return 0;
   }
   return 1;
}
/****************************************************************************
 * free_generic_cache()
 ***************************************************************************/
int free_generic_cache()
{
 t_cache_entry *tmp_cache = cache_entry;

   while (tmp_cache != NULL) {
      if (tmp_cache->table != NULL) {
         free(tmp_cache->table);
      }
      if (tmp_cache->var != NULL) {
         free(tmp_cache->var);
      }
      if (tmp_cache->where != NULL) {
         free(tmp_cache->where);
      }
      if (tmp_cache->val != NULL) {
         free(tmp_cache->val);
      }
      if (tmp_cache->result != NULL) {
         free(tmp_cache->result);
      }
      cache_entry = tmp_cache->next;
      free(tmp_cache);
      tmp_cache = cache_entry;
   }
   return 1;
}
/****************************************************************************
 * psql_disconnect()
 ***************************************************************************/
int psql_disconnect(PGconn **conn)
{
   PQfinish(*conn);
   free_generic_cache();
   return 1;
}
/****************************************************************************
 * dbscape()
 ***************************************************************************/
int dbscape(char **esc_str)
{
 char *tmp;
 int x,y;
 int tlen;
 unsigned short changed = 0;

   if (*esc_str == NULL)
      return(-1);

   /* max length would be all characters need escaping, so 2*strlen + CR */
   tlen = (strlen(*esc_str) * 2) + 1;

   tmp = (char *) malloc (tlen+1);

   if (tmp == NULL) {
      /* malloc failed */
      return(-1);
   }

   memset(tmp,0,tlen);

   for (y=0,x=0;x<strlen(*esc_str);x++) {
      switch ((*esc_str)[x]) {
       case '\'':
         /* replace ' with '' */
         tmp[y++] = '\'';
         tmp[y++] = '\'';
         changed  = 1;
         break;
       case '\\':
         /* replace \ with \\ */
         tmp[y++] = '\\';
         tmp[y++] = '\\';
         changed  = 1;
         break;
       default:
         /* this is just a normal character, don't escape it */
         tmp[y++]=(*esc_str)[x];
      }
   }
   tmp[y++] = 0;

   if (changed) {
      *esc_str = tmp;
      return(1);
   } else {
      /* this didn't containt any escape strings.. no sense using up mem */
      /* leave pointer unchanged, and free up buffer space.              */
      free(tmp);
      return(0);
   }
}
/****************************************************************************
 * sql_run_command()
 ***************************************************************************/
int sql_run_command(PGresult **res, char **sql, int result_code, int error_msg,
                    const char *func, const char *file, int line, int clear_and_free)
{
//#define DEBUG_SQL
#ifdef DEBUG_SQL
   fprintf(stderr,"%s:%d %s() SQL: %s\n",file,line,func,*sql);
fflush(0);
#endif

   pio_query_count++;

#undef PROFILE_SQL
#ifdef PROFILE_SQL
      fprintf(stderr,"%s:%d %s() sql: %s\n", file,line,func,*sql);
#endif
//printf("before SQL exec<br>\n");
//fflush(0);
   *res = PQexec(conn,*sql);
//printf("after SQL exec<br>\n");
//fflush(0);

#define DEBUG
   if (*res == NULL) {
#ifdef DEBUG
      fprintf(stderr,"%s:%d %s() invalid sql: %s\n", file,line,func,*sql);
#endif
      if (error_msg) {
         printf("error: sql error\n");
      }
//fflush(0);
//      if(clear_and_free)
          free(*sql);
      return -1;
   }

   if (PQresultStatus(*res) != result_code) {
      fprintf(stderr,"%s:%d %s() %s: %s\n", file,line,func,
              PQerrorMessage(conn),*sql);
      if (error_msg) {
         fancy_header("SQL Error","");
         printf("<CENTER>\n");
         printf("%s:%d %s() %s%s\n", file,line,func,
                PQerrorMessage(conn),*sql);
         printf("</CENTER>\n");
      }
      if(1) //if(clear_and_free)
          {
          PQclear(*res);
          free(*sql);
          }
      return -1;
   }

   if (clear_and_free) {
    int rc = 0;

      if (result_code == PGRES_TUPLES_OK) {
         rc = PQntuples(*res);
      }

      PQclear(*res);
      free(*sql);
      return rc;
   }

   return PQntuples(*res);
}
/****************************************************************************
 * cache_generic_value()
 * ===================
 *
 *   For some lists, generic_table_based_lookup() is called over and over 
 *   again with the same or similiar values 
 ***************************************************************************/
void cache_generic_value(char *table, char *var, char *where, char *val, 
                         char *result)
{
 t_cache_entry *tmp_cache;

   tmp_cache = (t_cache_entry *) malloc(sizeof(t_cache_entry));

   if (tmp_cache == NULL) {
      return;
   }

   tmp_cache->table  = (char *) malloc(strlen(table) + 1);
   tmp_cache->var    = (char *) malloc(strlen(var) + 1);
   tmp_cache->where  = (char *) malloc(strlen(where) + 1);
   tmp_cache->val    = (char *) malloc(strlen(val) + 1);
   tmp_cache->result = (char *) malloc(strlen(result) + 1);

   if ( (tmp_cache->table == NULL) || (tmp_cache->var == NULL) ||
        (tmp_cache->where == NULL) || (tmp_cache->val == NULL) ||
        (tmp_cache->result == NULL) ) {
      free(tmp_cache);
      return;
   }

   strcpy(tmp_cache->table,  table);
   strcpy(tmp_cache->var,    var);
   strcpy(tmp_cache->where,  where);
   strcpy(tmp_cache->val,    val);
   strcpy(tmp_cache->result, result);
   tmp_cache->next = cache_entry;
   cache_entry = tmp_cache;
   return;
}
/****************************************************************************
 * cache_generic_lookup()
 ***************************************************************************/
char *cache_generic_lookup(char *table, char *var, char *where, char *val)
{
 t_cache_entry *tmp_cache = cache_entry;

   while (tmp_cache != NULL) {
      if ( (!strcmp(tmp_cache->table, table)) &&
           (!strcmp(tmp_cache->var, var)) &&
           (!strcmp(tmp_cache->where, where)) &&
           (!strcmp(tmp_cache->val, val)) ) {
         return tmp_cache->result;
      }
      tmp_cache = tmp_cache->next;
   }
   return NULL;
}
/****************************************************************************
 * generic_table_based_lookup()
 ***************************************************************************/
char *generic_table_based_lookup(char *table, char *var, char *where, char *val,
                                 const char *caller, const char *caller_file, 
                                 int caller_line)
{
 char *sql_cmd = "SELECT %s FROM %s WHERE %s = %s";
 char *sql;
 PGresult *res;
 int count;
 char *the_result;
 static char result[STRLEN+1];

   if (caller == NULL)
      caller = __FUNCTION__;

   if (caller_file == NULL) 
      caller_file = __FILE__;
 
   if (caller_line == 0) 
      caller_line = __LINE__;

   result[0] = 0;

   the_result = cache_generic_lookup(table,var,where,val);

   if (the_result != NULL) {
      return the_result;
   }

   sql = (char *) malloc(strlen(sql_cmd) + strlen(table) + strlen(var) + 
                         strlen(where) + strlen(val) + STRLEN);
   if (sql == NULL) {
      return result;
   }

   if (val[0] == 0) {
      if (strstr(where, "_id") != NULL) {
         val = "0";
      }
   }

   sprintf(sql, sql_cmd, var, table, where, val);

   count = sql_run_command(&res, &sql, PGRES_TUPLES_OK, XP_DONT_SHOW_ERROR,
                           caller, caller_file, caller_line,
                           XP_DONT_CLEAR_AND_FREE);

   switch(count) {
    case 0:
      PQclear(res);
      free(sql);
    case -1:
      return result;
   }

   the_result = PQgetvalue(res,0,0);
   to_html(&the_result);
   strncpy(result, the_result, STRLEN);

   /* cache it for later.. */
   cache_generic_value(table,var,where,val,the_result);

   PQclear(res);
   free(sql);
   return result;
}
