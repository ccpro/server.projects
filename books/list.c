#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pio.h"
#include "cgi.h"
#include "html.h"
#include "auth.h"
#include "config.h"

#ifdef DEBUG
static char *rcsid = "$Id: list.c,v 1.1.1.1 2005/09/28 20:32:42 ccpro Exp $";
#endif

typedef struct book_tt
{
   struct book_t *next;

   char name[STRLEN*2];
   char isbn[STRLEN];
   char date[STRLEN];
   char uniq_path[STRLEN];
   unsigned long size;
} book_t;

typedef struct list_tt
{
   struct list_tt *next;

   struct book_tt *books_list;
   char path[STRLEN*2];
   char uniq_path[STRLEN];
   unsigned short part;
} list_t;

extern t_user user;

static char *order[] = {"path", "path", "date desc", "size desc, path"};
static unsigned short order_num = 0;

static int read_catalog_list(char *theme, list_t **blist);
static int clear_list(list_t **blist);
static int add_path(list_t **list, const char *path, const char *uniq_path);

int show_list()
{
 char *theme;
 list_t *blist, *bchase, *btemp;

   theme = get_value("theme");

   read_catalog_list(NULL, &blist);

   bchase = blist;

   printf("<BR><CENTER>\n");
   table_start(850);
   printf("<TR>"
             "<TH COLSPAN=2 WIDTH=\"70%%\">"
                "<A onClick=\"document.cookie='SORT=1'\" HREF=\"%s\">"
                   "<FONT COLOR=\"#cccc44\">Category & Name</FONT>"
                "</A>"
              "</TH>"
              "<TH WIDTH=\"10%%\">"
                 "<A onClick=\"document.cookie='SORT=3'\" HREF=\"%s\">"
                    "<FONT COLOR=\"#cccc44\">Size</FONT>"
                 "</A>"
              "</TH>"
              "<TH WIDTH=\"10%%\">"
                 "<FONT COLOR=\"#cccc44\">ISBN/ISSN</FONT>"
              "</TH>"
              "<TH WIDTH=\"10%%\">"
                 "<A onClick=\"document.cookie='SORT=2'\" HREF=\"%s\">"
                    "<FONT COLOR=\"#cccc44\">Date</font>"
                 "</A>"
              "</TH></TR>\n",
            user.cgi_name, user.cgi_name, user.cgi_name);

   while (bchase) {
      if (bchase->part == 1) {
         printf("<TR><TD><PRE>[+]</PRE></TD>"
                "<TD COLSPAN=3>%s</TD></TR>\n",
                bchase->path);
      }

      bchase = bchase->next;
   }
   table_end();
   
   clear_list(&blist);

   return 0;
}

static int count_chars(const char *path, int ch)
{
 int i, count;

   count = 0;
   if (path == NULL) {
      return count;
   }

   for (i=0; i<strlen(path); ++i) {
      if (ch == path[i]) {
         ++count;
      }
   }

   return count;
}

static int clear_list(list_t **blist)
{
 list_t *bchase;

   while (*blist) {
      bchase = (*blist)->next;
      free(*blist);
      *blist = bchase;
   }

   return 0;
}

static char *get_order()
{
 char *sorder;
 int iorder;

   sorder = get_value("order");
   if (NULL_OR_EMPTY(sorder)) {
      sorder = "0";
   }

   iorder = atoi(sorder);
   
   if (iorder < 0 || iorder > 3) {
      return order[0];
   }

   order_num = iorder;

   return order[iorder];
}

int read_catalog_list(char *theme, list_t **blist)
{
 char *sql_cmd = "SELECT uniq_path, path, size, isbn, date "
                 "%s "
                 "FROM books_catalog "
                 "ORDER BY %s";
 PGresult *res;
 int i, count;
 char *sql;
 list_t *bchase;
 char *path, *book_name;
 int   add_flag;
 char  where[STRLEN];

   *blist = NULL;
   bchase = NULL;

   sql = (char *)malloc(strlen(sql_cmd) + STRLEN);
   if (sql == NULL) {
      return memory_error();
   }

   if (!NULL_OR_EMPTY(theme)) {
      snprintf(where, STRLEN, "WHERE path LIKE './%s%%'", theme);
   } else {
      where[0] = '\0';
   }

   sprintf(sql, sql_cmd, where, get_order());

   if (get_value("web_dmp")) {
      printf("\n<!--SQL:'%s'-->\n", sql);
   }

   count = sql_run_command(&res, &sql, PGRES_TUPLES_OK, XP_DONT_SHOW_ERROR,
                           __FUNCTION__, __FILE__, __LINE__,
                           XP_DONT_CLEAR_AND_FREE);

   if (get_value("web_dmp")) {
      printf("\n<!-- COUNT:%d -->\n", count);
   }

   if (count == -1) {
      fancy_header("ERROR", "Bad SQL");
      return -1;
   }

   for (i=0; i<count; ++i) {

      add_flag = 0;
      path = PQgetvalue(res, i, 1)+2;
      book_name = strrchr(path, '/');
      if (book_name) {
         *book_name = '\0';
         ++book_name;
      } else {
         book_name = "";
      }

      if (theme && !strcmp(path, theme)) {
         add_flag = 2;
      } else if (count_chars(path, '/') == 0){
         add_flag = 1;
      }

      if (add_flag) {
         add_path(blist, path, PQgetvalue(res, i, 0));
      }
   }

   free(sql);
   PQclear(res);
}

int add_path(list_t **list, const char *path, const char *uniq_path)
{
 list_t *chase, *last;

   if (*list == NULL) {
      *list = chase = (list_t*)calloc(sizeof(list_t), 1);
      if (*list == NULL){
         return -1;
      }
   } else {
      last = chase = *list;
      while (chase) {
         if (!strcmp(chase->uniq_path, uniq_path)) {
            break;
         }
         last = chase;
         chase = chase->next;
      }
      
      if (chase){
         return 0;
      }

      last->next = (list_t*)calloc(sizeof(list_t), 1);
      if (last->next == NULL){
         return -1;
      }
      chase = last->next;
   }

   fprintf(stderr, "path:%s\n", path);
    
   strncpy(chase->uniq_path, uniq_path, STRLEN);
   strncpy(chase->path,      path,      STRLEN*2);

   return 0; 
}
