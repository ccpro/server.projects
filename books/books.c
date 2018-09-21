/****************************************************************************
 * pdb.c
 * =====
 ***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "books.h"
#include "pio.h"
#include "cgi.h"
#include "auth.h"
#include "html.h"
#include "list.h"

#ifdef DEBUG
static char *rcsid = "$Id: books.c,v 1.1.1.1 2005/09/28 20:32:42 ccpro Exp $";
#endif

PGconn *conn;
extern t_user user;
extern t_cgi_vars *cgi_vars;

unsigned int test_database = 0;

int main(int argc, char **argv)
{
   if (!psql_connect(&conn)) {
      print_http_header(NULL, "#c0c0c0", "text/html", "Error");
      fprintf(stderr,"cannot connect to database\n");
      printf("error: cannot connect to database\n");
      return 0;
   }

   init_user(argv[0]);
   init_cgi();

   print_http_header(NULL, "#c0c0c0", "text/html", "");

   show_list();

   close_http();

   free_cgi();
   psql_disconnect(&conn);

   return 0;
}

int to_html(char **s)
{
   return 0;
}
