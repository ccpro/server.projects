/****************************************************************************
 * html.c
 *
 *  Generic html stuff.
 ***************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "auth.h"
#include "pio.h"
#include "cgi.h"

#ifdef DEBUG
static char *rcsid = "$Id: html.c,v 1.2 2007/03/30 18:31:42 ccpro Exp $";
#endif

extern int to_html(char **esc_str);
int memory_error();

unsigned short header_printed = 0;

extern t_user user;

/****************************************************************************
 * table_100_start()
 ***************************************************************************/
void table_100_start()
{
   puts("<TABLE BORDER=0 CELLSPACING=1 WIDTH=100%>");
} 
/****************************************************************************
 * table_500_start()
 ***************************************************************************/
void table_500_start()
{
   puts("<TABLE BORDER=0 CELLSPACING=1 CELLPADDING=3 WIDTH=500>");
} 
/****************************************************************************
 * table_start()
 ***************************************************************************/
void table_start(int width)
{
 char swidth[STRLEN];

   if (width > 0) {
      snprintf(swidth, STRLEN, "WIDTH=%d", width);
   } else {
      swidth[0] = '\0';
   }

   printf("<TABLE BORDER=1 CELLSPACING=1 CELLPADDING=3 %s>\n", swidth);
}                  
/****************************************************************************
 * table_end()
 ***************************************************************************/
void table_end()        
{
   puts("</TABLE>");
}
/****************************************************************************
 * form_start()
 ***************************************************************************/
void form_start(char *s,char *a)  
{
   printf("<FORM ACTION=\"%s\" METHOD=\"post\">\n",s);
   printf("<INPUT TYPE=\"hidden\" NAME=\"action\" VALUE=\"%s\">\n",a);
}
/****************************************************************************
 * form_start_get()
 ***************************************************************************/
void form_start_get(char *s,char *a)  
{
   printf("<FORM ACTION=\"%s\" METHOD=\"get\">\n",s);
   printf("<INPUT TYPE=\"hidden\" NAME=\"action\" VALUE=\"%s\">\n",a);
}
/****************************************************************************
 * form_button()
 ***************************************************************************/
void form_button(char *n,char *v,char *js)  
{
   printf("<INPUT TYPE=\"button\" NAME=\"%s\" VALUE=\"%s\" %s>\n",
          n, v, js ? js : "");
}

/****************************************************************************
 * form_start_name()
 ***************************************************************************/
void form_start_name(char *s, char *a, char *n)  
{
   printf("<FORM ACTION=\"%s\" METHOD=\"post\" NAME=\"%s\" ID=\"%s\">\n",
      s, n, n);
   printf("<INPUT TYPE=\"hidden\" NAME=\"action\" VALUE=\"%s\">\n", a);
}
/****************************************************************************
 * form_submit_name()
 ***************************************************************************/
void form_submit_name(char *s, char *n)
{
   printf("<INPUT TYPE=\"submit\" VALUE=\"%s\" NAME=\"%s\">\n",s,n);
}
/****************************************************************************
 * form_submit()
 ***************************************************************************/
void form_submit(char *s)
{
   printf("<INPUT TYPE=\"submit\" VALUE=\"%s\">\n",s);
}
/****************************************************************************
 * void form_clear(char *s)
 ***************************************************************************/
void form_clear(char *s)
{
   printf("<INPUT TYPE=\"reset\" VALUE=\"%s\">\n",s);
}
#define form_reset(s) form_clear(s)

/****************************************************************************
 * form_image_submit()
 ***************************************************************************/
void form_image_submit(char *s,char *alt)
{
   printf("<INPUT TYPE=IMAGE SRC=\"%s\" ALT=\"%s\" BORDER=0 HEIGHT=21 WIDTH=67 HSPACE=0 NAME=%s>\n",s,alt,alt);
}
/****************************************************************************
 * form_end()
 ***************************************************************************/
void form_end()
{
   printf("</FORM>\n");
}
/****************************************************************************
 * form_hidden()
 * Use this for form_hidden("blah", "17% of foo");
 ***************************************************************************/
void form_hidden(char *s, char *v)
{
 char *esc_var;

   if (v) {
      esc_var = v;
      to_html(&esc_var);

      printf("<INPUT TYPE=\"hidden\" NAME=\"%s\" ID=\"%s\" VALUE=\"%s\">\n",
         s, s, esc_var);
   }
}
/****************************************************************************
 * form_hidden_formatted()
 * Use this for form_hidden_formatted("blah", "%d%% of foo", j);
 ***************************************************************************/
void form_hidden_formatted(char *s, char *fmt, ...)
{
#define BUF_LEN 501
 va_list ap;
 char buff[BUF_LEN+1];
 char *esc_var;

   va_start(ap,fmt);
   vsnprintf(buff,BUF_LEN,fmt,ap);
   va_end(ap);

   esc_var = buff;
   to_html(&esc_var);

   printf("<INPUT TYPE=\"hidden\" NAME=\"%s\" ID=\"%s\" VALUE=\"%s\">\n",
      s, s, esc_var);
}

/****************************************************************************
 * form_text_input_ro()
 ***************************************************************************/
void form_text_input_ro(char *n,int s,int m,char *v)
{
   printf("<INPUT TYPE=\"text\" NAME=\"%s\" SIZE=%d MAXLENGTH=%d VALUE=\"%s\" READONLY>\n",n,s,m,v);
}

/****************************************************************************
 * js_form_text_input()
 ***************************************************************************/
void js_form_text_input(char *n,int s,int m,char *v, char *js)
{
   printf("<INPUT TYPE=\"text\" NAME=\"%s\" SIZE=%d MAXLENGTH=%d VALUE=\"%s\" %s>\n",n,s,m,v, js);
}

/****************************************************************************
 * form_text_input()
 ***************************************************************************/
void form_text_input(char *n,int s,int m,char *v)
{
   printf("<INPUT TYPE=\"text\" NAME=\"%s\" SIZE=%d MAXLENGTH=%d VALUE=\"%s\">\n",n,s,m,v);
}

/****************************************************************************
 * form_textarea_input()
 ***************************************************************************/
void form_textarea_input(char *n, int r, int c, char *v, int max)
{

if(max > 0)
   {
   printf("<textarea NAME=\"%s\" rows=%d cols=%d id=\"%s\" ", n, r, c, n);
   printf("wrap=\"soft\" onKeyup=\"check_max_textarea('%s', '%d')\">",
       n, max);
   printf("%s</textarea>\n", v);
   }
else
   {
   printf("<textarea NAME=\"%s\" rows=%d cols=%d", n, r, c);
   printf(">%s</textarea>\n", v);
   }
}

/****************************************************************************
 * print_maxlenfunc()
 ***************************************************************************/

int print_maxlenfunc()
    {
    printf("<script LANGUAGE=\"JavaScript\">\n\n<!--\n\n");
    printf("function check_max_textarea(id_str, max)\n    {\n");
    printf("    if(document.getElementById(id_str).value.length >= max)\n");
    printf("        {\n        ");
    printf("alert(\"Maximum text length is \" + max + \" characters\");\n");
    printf("        ");
    printf("document.getElementById(id_str).value = ");
    printf("document.getElementById(id_str).value.substring(0, max);\n");
    printf("        }\n    }\n\n//-->\n\n</script>\n\n");
    return(1);
    }

/****************************************************************************
 * form_checkbox()
 ***************************************************************************/
void form_checkbox(char *n, char *v, int c)
{
   if (c) {
      printf("<INPUT TYPE=\"checkbox\" NAME=\"%s\" VALUE=\"%s\" CHECKED>\n",n,v);
   } else {
      printf("<INPUT TYPE=\"checkbox\" NAME=\"%s\" VALUE=\"%s\">\n",n,v);
   }
}
/****************************************************************************
 * form_radio()
 ***************************************************************************/
void form_radio(char *n, char *v, int c)
{
   if (c) {
      printf("<INPUT TYPE=\"radio\" NAME=\"%s\" VALUE=\"%s\" CHECKED>\n",n,v);
   } else {
      printf("<INPUT TYPE=\"radio\" NAME=\"%s\" VALUE=\"%s\">\n",n,v);
   }
}
/****************************************************************************
 * form_password_input()
 ***************************************************************************/
void form_password_input(char *n,int s,int m,char *v)
{
   printf("<INPUT TYPE=\"password\" NAME=\"%s\" SIZE=%d MAXLENGTH=%d VALUE=\"%s\">\n",n,s,m,v);
}
/****************************************************************************
 * yes_no_select()
 ***************************************************************************/
int yes_no_select(char *name,int def)
{
   printf("<SELECT NAME=\"%s\">\n",name);
   if (def == -1) {
      printf("<OPTION VALUE=-1 SELECTED>-any-</OPTION>\n");
   }
   printf("<OPTION VALUE=1%s>YES</OPTION>\n",(def == 1)?" SELECTED":"");
   printf("<OPTION VALUE=0%s>NO</OPTION>\n",(def == 0)?" SELECTED":"");
   printf("</SELECT>\n");

   return 1;
}
/****************************************************************************
 * yes_no_select_onchange()
 * yes_no_select with JavaScript onChange function call
 ***************************************************************************/
int yes_no_select_onchange(char *name,int def,char *onchange)
{
   printf("<SELECT NAME=\"%s\" onChange=\"%s(this.value)\">\n",
      name, onchange);
   if (def == -1) {
      printf("<OPTION VALUE=-1 SELECTED>-any-\n");
   }
   printf("<OPTION VALUE=1%s>YES\n",(def == 1)?" SELECTED":"");
   printf("<OPTION VALUE=0%s>NO\n",(def == 0)?" SELECTED":"");
   puts("</SELECT>\n");
   return 1;
}
/****************************************************************************
 * yes_no_print()
 ***************************************************************************/
char *yes_no_print(int def)
{
   switch (def) {
    case 1:
      return "Yes";
    case 0:
    default:
      return "No";
   }
}
/****************************************************************************
 * yes_no_print()
 ***************************************************************************/
char *paymethod_print(int def)
{
   switch (def) {
    case 1:
      return "CC";
    case 2:
      return "CK";
    case 3:
      return "EFT";
    case 0:
    default:
      return "Unknow";
   }
}
/****************************************************************************
 * cc_month_select()
 ***************************************************************************/
int cc_month_select(char *name,int def)
{
 int i;

   printf("<SELECT NAME=\"%s\">\n",name);
   for (i=1; i<13; ++i) {
      printf("<OPTION VALUE=%02d%s>%02d</OPTION>\n", i, (def == i)?" SELECTED":"", i);
   }
   printf("</SELECT>\n");

   return 1;
}
/****************************************************************************
 * cc_year_select()
 ***************************************************************************/
int cc_year_select(char *name,int def)
{
 int i;
 time_t now;
 struct tm *tm_now;

   now = time(NULL);
   tm_now = localtime(&now);

   tm_now->tm_year += 1900;

   printf("<SELECT NAME=\"%s\">\n",name);

   if (def < tm_now->tm_year) {
      printf("<OPTION VALUE=%04d SELECTED>%04d</OPTION>\n", def, def);
   }

   for (i=0; i<6; ++i) {
      printf("<OPTION VALUE=%04d%s>%04d</OPTION>\n", 
             i+tm_now->tm_year, 
             (def == i+tm_now->tm_year)?" SELECTED":"", 
             i+tm_now->tm_year);
   }
   printf("</SELECT>\n");

   return 1;
}
/****************************************************************************
 * fancy_header()
 ***************************************************************************/
void fancy_header(char *t,char *r)
{
   printf("<CENTER>\n");
   printf("<P>\n");
   printf("<TABLE BORDER=0 WIDTH=500 CELLSPACING=0>");
   if (r == NULL) {
      printf("<TR BGCOLOR=\"#FFCC33\"><TD>%s</TD></TR>\n",t);
      printf("<TR><TD>&nbsp;</TD></TR>\n");
   } else {
      printf("<TR BGCOLOR=\"#FFCC33\"><TD>%s</TD><TD ALIGN=right>%s</TD></TR>\n",t,r);
      printf("<TR><TD COLSPAN=2>&nbsp;</TD></TR>\n");
   }
   printf("</TABLE><P></CENTER>\n");
}
/****************************************************************************
 * print_semi_formatted()
 *   This routine adds <BR>'s where ever it seens a carriage return.
 ***************************************************************************/
int print_semi_formatted(char *s)
{
 int i,len;

   len = strlen(s);
   for (i=0;i<len;i++) {
      if (s[i] == '\n') {
         puts("<BR>\n");
      } else {
         putchar(s[i]);
      }
   }
   return 1;
}
/****************************************************************************
 * generic_table_based_scroller()
 ***************************************************************************/
void generic_table_based_scroller(char *name, char *table, char *var, 
                                   char *index, char *def)
{
 char *sql_cmd = "SELECT %s, %s FROM %s ORDER BY %s";
 char *sql;
 PGresult *res;
 int i,count;
 char *tb_ind, *tb_val;
 unsigned long my_def, my_ind;
 unsigned short found = 0;

   sql = (char *) malloc(strlen(sql_cmd) + strlen(table) + strlen(var) + 
                         (2*strlen(index)) + STRLEN);
   if (sql == NULL) {
      form_hidden(name,"0");
      return;
   }

   my_def = atol(def);

   sprintf(sql, sql_cmd, index, var, table, index);

   count = sql_run_command(&res, &sql, PGRES_TUPLES_OK, XP_DONT_SHOW_ERROR,
                           __FUNCTION__, __FILE__, __LINE__,
                           XP_DONT_CLEAR_AND_FREE);

   if (count == -1) {
      form_hidden(name,"0");
      return;
   }

   if (get_value("onchange")) {
      printf("<SELECT NAME=\"%s\" onChange=\"%s(this.value)\">\n",
         name, get_value("onchange"));
   } else {
      printf("<SELECT NAME=\"%s\">\n",name);
   }
   for (i=0;i<count;i++) {
    char *sel;

      tb_ind = PQgetvalue(res,i,0);
      tb_val = PQgetvalue(res,i,1);
      my_ind = atol(tb_ind);

      if (my_ind == my_def) {
         found = 1;
         sel = " SELECTED";
      } else {
         sel = "";
      }
      printf("<OPTION VALUE=\"%lu\" %s>%s\n",my_ind, sel, tb_val);
   }
   if ((!found) && (my_def != -1)) {
      printf("<OPTION VALUE=\"%lu\" SELECTED>Unknown\n",my_def);
   }
   printf("</SELECT>\n");
   PQclear(res);
   free(sql);
   return;
}
/****************************************************************************
 * generic_table_based_scroller2()
 * This function works much like generic_table_based_scroller above with
 * a few differences:
 * 1. An additional parameter is used to permit the sort order to be based
 *    on an arbitrary field within the table rather than the select value.
 * 2. A JavaScript onChange=func(this.value) function name may be specified.
 *    If this is done, a JavaScript function to receive the onChange events
 *    must be included in the form by the caller.
 ***************************************************************************/
void generic_table_based_scroller2(char *name, char *table, char *var,
                  char *index, char *def, char *order, char *onchange)
{
 char *sql_cmd = "SELECT %s, %s FROM %s ORDER BY %s";
 char *sql;
 PGresult *res;
 int i,count;
 char *tb_ind, *tb_val;
 unsigned long my_def, my_ind;
 unsigned short found = 0;

   sql = (char *) malloc(strlen(sql_cmd) + strlen(table) + strlen(var) +
                         (2*strlen(index)) + STRLEN);
   if (sql == NULL) {
      form_hidden(name,"0");
      return;
   }
   if (NULL_OR_EMPTY(def)) {
      def = "0";
   }
   my_def = atol(def);

   if (order && *order) {
      sprintf(sql, sql_cmd, index, var, table, order);
   } else {
      sprintf(sql, sql_cmd, index, var, table, index);
   }

   count = sql_run_command(&res, &sql, PGRES_TUPLES_OK, XP_DONT_SHOW_ERROR,
                           __FUNCTION__, __FILE__, __LINE__,
                           XP_DONT_CLEAR_AND_FREE);

   if (count == -1) {
      form_hidden(name,"0");
      return;
   }

   printf("<SELECT NAME=\"%s\"", name);
   if (onchange && *onchange) {
      printf(" onChange=\"%s(this.value)\"", onchange);
   }
   puts(">");

   for (i=0;i<count;i++) {
    char *sel;

      tb_ind = PQgetvalue(res,i,0);
      tb_val = PQgetvalue(res,i,1);
      my_ind = atol(tb_ind);

      if (my_ind == my_def) {
         found = 1;
         sel = " SELECTED";
      } else {
         sel = "";
      }
      printf("<OPTION VALUE=\"%lu\" %s>%s\n",my_ind, sel, tb_val);
   }
   if ((!found) && (my_def != -1)) {
      printf("<OPTION VALUE=\"%lu\" SELECTED>Unknown\n",my_def);
   }
   printf("</SELECT>\n");
   PQclear(res);
   free(sql);
   return;
}
/****************************************************************************
 * html_error_warning()
 ***************************************************************************/
int html_error_warning(char *msg)
{
   fancy_header("Error", "");
   printf("<CENTER>%s</CENTER>\n", msg);
   return 0;
}
/****************************************************************************
 * missing_data_warning()
 ***************************************************************************/
int missing_data_warning()
{
   html_error_warning("Missing important data. Internal Error!");
   return 0;
}
/****************************************************************************
 * access_denied_warning()
 ***************************************************************************/
int access_denied_warning()
{
   html_error_warning("Access Denied!");
   return -2;
}
/****************************************************************************
 * memory_error()
 ***************************************************************************/
int memory_error()
{
   fancy_header("Error", "");
   printf("<CENTER>Error: memory error</CENTER>\n");
   return -1;
}
/****************************************************************************
 * print_http_header()
 ***************************************************************************/
int print_http_header(const char *sid, const char *bgcolor, 
                      const char *content_type, const char *title)
{
   printf("Content-type: %s%c", content_type, 10);
//   if (!NULL_OR_EMPTY(sid)) {
//      printf("Set-Cookie: sid=%s; path=/; domain=.interquest.net%c",sid,10);
//      printf("Set-Cookie: sid=%s; path=/; domain=.ygnition.com%c",sid,10);
//   }
   printf("%c",10);
   if (strcmp(content_type, "text/plain")) {
      puts("<HTML>");
      printf("<HEADER>\n"
             "<TITLE>%s</TITLE>\n"
             "<META ENCODING='windows-1251' HTTP-EQUIV='Content-Type' CONTENT='text/html'>\n"
             "<LINK REL='stylesheet' TYPE='text/css' HREF='/style.css'>\n"
             "</HEADER>\n", title ? title : "");
      printf("<BODY BGCOLOR=\"%s\">\n", bgcolor);
   }
   if (header_printed) {
      fprintf(stderr, "Error: print_http_header called twice, first time [%d]\n",
        header_printed);
   }
   header_printed = 1;
   return 1;
}
/****************************************************************************
 * close_http()
 ***************************************************************************/
int close_http()
{
   puts("</BODY></HTML>");
   return 1;
}

extern char **environ;
int print_env()
{
 char **ep = environ;
 char *p;

   puts("<PRE>");
   while ((p = *ep++)) {
      puts(p);
   }
   puts("</PRE>");

   return 0;
}