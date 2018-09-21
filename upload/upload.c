/****************************************************************************
 * pdb.c
 * =====
 ***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "upload.h"
#include "pio.h"
#include "cgi.h"
#include "auth.h"
#include "html.h"
#include "base64.h"

#ifdef DEBUG
static char *rcsid = "$Id: upload.c,v 1.5 2007/03/30 22:08:57 ccpro Exp $";
#endif

PGconn *conn;
extern t_user user;
extern t_cgi_vars *cgi_vars;

unsigned int test_database = 0;
unsigned long upload_limit = 50*1024*1024;

int		upload_file();
unsigned long	uploaded_by_user();
void		set_up_limit();

extern char **environ;

// PATH_INFO

int main(int argc, char **argv)
{
 unsigned long user_upload;

   if (!psql_connect(&conn)) {
      print_http_header(NULL, "#c0c0c0", "text/html", "Error");
      fprintf(stderr,"cannot connect to database\n");
      printf("error: cannot connect to database\n");
      return 0;
   }

   init_cgi();
   init_user(argv[0]);

   set_up_limit();

   user_upload = uploaded_by_user();
   if ( user_upload > upload_limit ) {
      print_http_header(NULL, "#c0c0c0", "text/html", "");
      printf("<h2>Sorry, you've exceede upload limit (%ld:%ld)</H2>\n", upload_limit, user_upload);
      close_http();
   } else if (upload_file() != SUCCESS) {
      print_http_header(NULL, "#c0c0c0", "text/html", "");
      printf("<h2>Can not open file</H2>\n");
      close_http();
   }

   free_cgi();
   psql_disconnect(&conn);

   return 0;
}

int to_html(char **s)
{
   return 0;
}

#define RB_SIZE 1024

int upload_file()
{
 char *header_format = "Content-Type: application/force-download\n"
                       "Content-Disposition: attachment; filename=\"%s\"\n"
                       "Content-Transfer-Encoding: 8bit\n"
                       "Content-Length: %d\n"
                       "Content-Description: ccpro's File Downloader 0.2\n\n";
 char *sql_ins = "INSERT INTO upload_log (file_name, ip, upload_size) VALUES('%s', '%s', %llu)";
 char *sql_upd = "UPDATE upload_log SET upload_end = CURRENT_TIMESTAMP WHERE oid = %ld";
 char *sql;
 char *path = "/usr/local/www/data/%s%s";
 char full_path[1024];
 FILE *fp;
 struct stat buf;
 char rd[RB_SIZE];
 size_t readed;
 char *file_name, *path_info;
 PGresult *res;
 char *pid = NULL;
 unsigned long oid;
 int count;

   path_info = getenv("PATH_INFO");
   if ( NULL_OR_EMPTY(path_info) || !strcmp(path_info, "/")) {
      path_info = get_value("ul");
   } else {
      ++path_info;
   }

   if ( !NULL_OR_EMPTY(path_info) && path_info[0] == '_' ) {
      pid = path_info;
   }

   if (!NULL_OR_EMPTY(pid) ) {
      path_info = get_value(pid);
   }

   if ( NULL_OR_EMPTY(path_info) || strlen(path_info) < 2 ) {
      return 1;
   }

   file_name = (char*)base64_decode(path_info);

   snprintf(full_path, 1023, path, getenv("SERVER_NAME"), file_name);

   sql = (char*)malloc(strlen(sql_ins) + strlen(sql_upd));
   if (sql == NULL) {
      return 1;
   }

   fp = fopen(full_path, "r");
   if (fp == NULL) {
      return 1;
   }

   file_name = strrchr(full_path, '/');
   if ( file_name ) {
      ++file_name;
   } else {
      file_name = full_path;
   }

   fstat(fileno(fp), &buf);
   
   sprintf(sql, sql_ins, file_name, getenv("REMOTE_ADDR"), buf.st_size);
   count = sql_run_command(&res, &sql, PGRES_COMMAND_OK, XP_DONT_SHOW_ERROR,
                           __FUNCTION__, __FILE__, __LINE__,
                           XP_DONT_CLEAR_AND_FREE);
   if ( count == -1 ) {
      return 1;
   }
   oid = atol(PQoidStatus(res));
   PQclear(res);

   printf(header_format, file_name, buf.st_size);
   
   rewind(fp);
   
   while(!feof(fp)){
      readed = fread(rd, RB_SIZE, 1, fp);
      readed = fwrite(rd, RB_SIZE, 1, stdout);
      usleep(100000);
   }
   
   fclose(fp);

   sprintf(sql, sql_upd, oid);
   sql_run_command(&res, &sql, PGRES_COMMAND_OK, XP_DONT_SHOW_ERROR,
                   __FUNCTION__, __FILE__, __LINE__,
                   XP_CLEAR_AND_FREE);

   return 0;
}

unsigned long uploaded_by_user()
{
 unsigned long uploaded = 0L;
 char *sql_cmd = "SELECT SUM(upload_size) FROM upload_log "
                 "WHERE date(upload_start) = CURRENT_DATE AND ip = '%s'";
 char *sql;
 PGresult *res;
 int count;

   sql = (char*)malloc(strlen(sql_cmd) + 80);
   if ( sql == NULL ) {
      return upload_limit;
   }

   sprintf(sql, sql_cmd, user.ip);
   count = sql_run_command(&res, &sql, PGRES_TUPLES_OK,
                           XP_SHOW_ERROR, __FUNCTION__, __FILE__, __LINE__,
                           XP_DONT_CLEAR_AND_FREE);
                                             
   if ( count == -1 ) {
      return upload_limit;
   }
   if ( count ) {
      uploaded = atol(PQgetvalue(res, 0, 0));
   }
   PQclear(res);
   free(sql);

   return uploaded;
}

void set_up_limit()
{
 time_t now;
 struct tm *tm_now;

   now = time(NULL);
   tm_now = localtime(&now);

   if ( tm_now->tm_hour > 0 && tm_now->tm_hour < 7 ) {
      upload_limit *= 1.5;
   }
}
