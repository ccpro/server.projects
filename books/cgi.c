/****************************************************************************
 * cgi.c
 *
 *   This is a module for retrieving cgi variables. This version mallocs 
 * the appropriate amount of space and then just stores pointers into that
 * buffer.
 *   init_cgi() - call before anything else.
 *   get_value(n,v) - points v to the cgi variable with the name n
 *   free_cgi() - free's all relevant buffers.
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cgi.h"

#ifdef DEBUG
static char *rcsid = "$Id: cgi.c,v 1.1.1.1 2005/09/28 20:32:42 ccpro Exp $";
#endif

/* these are our states for the finate state machine */
typedef enum {
   do_malloc,            /* first fsm  */
   get_var,
   get_val,

   obtain_separator,     /* second fsm */
   read_header,
   read_header_content,
   finish_header_content,
   read_body,
   read_separator,
} t_state;

t_cgi_vars *cgi_vars;       /* a linked list containing cgi values */
int content_length;         /* the CONTENT_LENGTH, size of the cgi buffer */
t_qtype query_type;         /* the query type (see t_qtype) */
char *cgi_buff;             /* a buffer we keep all the cgi variables in */
char *env;                  /* pointer to environment variables */

/****************************************************************************
 * free_cgi()
 * ========
 *    called at the end of the cgi program to free all internal 
 *    variables allocated by init_cgi()
 ***************************************************************************/
int free_cgi()
{
 t_cgi_vars *walker = cgi_vars;

   while (cgi_vars != NULL) {
      walker=walker->next;
      cgi_vars->next = NULL;
      if (cgi_vars->vdyn) {
         free(cgi_vars->value);
      }
      if (cgi_vars->ndyn) {
         free(cgi_vars->name);
      }
      free(cgi_vars);
      cgi_vars = walker;
   }
   free(cgi_buff);
   return 1;
}

/****************************************************************************
 * list_values() 
 * ===========
 *    mostly a debugging routine for listing the cgi values
 *    passed to the cgi program
 ***************************************************************************/
void list_values(FILE *stream)
{
 t_cgi_vars     *walker = cgi_vars;

   if (stream == NULL) {
      stream = stdout;
   }

   fprintf(stream,"<hr><pre>\n");
   while (walker != NULL) {
      fprintf(stream,"%s [%s] %d,%d\n",walker->name,walker->value,
              walker->nsize,walker->vsize);
      walker = walker->next;
   }
   fprintf(stream,"</pre><hr>\n");
}

/****************************************************************************
 * get_value() 
 * =========
 *    searchs for the value of a cgi variable
 ***************************************************************************/
char *get_value(char *name)
{
 t_cgi_vars *walker = cgi_vars;

   while (walker != NULL) {
      if (!strcmp(walker->name,name)) { 
         return walker->value;
      }
      walker = walker->next;
   }
   return NULL;
}
/****************************************************************************
 * multi_get_value()
 * ===============
 *   like get_value() except it handles multiple values..
 ***************************************************************************/
int multi_get_value(char *name, char **buf, int buf_size)
{
 t_cgi_vars *walker = cgi_vars;
 int buf_idx;

   if (buf_size < 1) {
      return 0;
   }

   for (buf_idx = 0; buf_idx < buf_size; buf_idx++) {
      buf[buf_idx] = NULL;
   }
   buf_idx = 0;

   while (walker != NULL) {
      if (!strcmp(walker->name,name)) {
         buf[buf_idx++] = walker->value;
         if (buf_idx >= buf_size) {
            break;
         }
      }
      walker = walker->next;
   }
   return buf_idx;
}
/****************************************************************************
 * remove_value()
 * ============
 *    This routine removes a CGI value from the list
 ***************************************************************************/
int remove_value(char *name)
{
 t_cgi_vars *tmp = cgi_vars;
 t_cgi_vars *caboose = NULL;
 int rc = 0;

   while (tmp != NULL) {
      if (!strcmp(tmp->name,name)) {
         /* this is the one we want to remove, extract it from the tree */
         if (caboose == NULL) {
            /* this is the first entry on the list */
            cgi_vars  = tmp->next;     /* set the head to be the next entry   */
            if (tmp->ndyn) {
               free(tmp->name);
            }
            if (tmp->vdyn) {
               free(tmp->value);
            }
            tmp->next = NULL;          /* remove pointer - garbage collection */
            free(tmp);                 /* free memory                         */
            tmp = cgi_vars;            /* set tmp to the new head of list     */
         } else {
            /* this entry is in the middle/end of list */
            caboose->next = tmp->next; /* remove it from the list */
            if (tmp->ndyn) {
               free(tmp->name);
            }
            if (tmp->vdyn) {
               free(tmp->value);
            }
            tmp->next = NULL;          /* remove pointer - garbage collection */
            free(tmp);                 /* free memory                         */
            tmp = caboose->next;       /* set tmp to next entry               */
         }
         rc = 1;
         /* now tmp and caboose should be correctly set as if the entry       */
         /* never existed - so let's continue on                              */
         continue;
      }

      caboose = tmp;
      tmp = tmp->next;
   }
   return rc;
}

/****************************************************************************
 * rename_cgi()
 * ==========
 *    This routine renames a cgi variable name
 ***************************************************************************/
int rename_cgi(char *old, char *new)
{
 t_cgi_vars *tmp = cgi_vars;
 int rc = 0;
 char *buf;

   while (tmp != NULL) {
      if (!strcmp(tmp->name,old)) {
         buf = (char *) malloc (strlen(new) + 1);
         if (buf != NULL) {
            strcpy(buf,new);
            if (tmp->ndyn) {
               free(tmp->name);
            }
            tmp->name = buf;
            rc = 1;
         }
      }
      tmp = tmp->next;
   }
   return rc;
}

/****************************************************************************
 * add_cgi_value()
 * =============
 *    This routine adds a cgi variable name
 ***************************************************************************/
int add_cgi_value(char *var, char *val)
{
 char *tmp_var;
 char *tmp_val;
 t_cgi_vars *tmp_cgi;
 t_cgi_vars *tmp_buf;
 int nsize;
 int vsize;

   nsize = strlen(var);
   vsize = strlen(val);

   tmp_var = (char *) malloc (nsize + 1);
   tmp_val = (char *) malloc (vsize + 1);
   tmp_buf = (t_cgi_vars *) malloc(sizeof(t_cgi_vars));
   
   if ((tmp_var == NULL) || (tmp_val == 0) || (tmp_buf == NULL)) {
      return 0;
   }

   strcpy(tmp_var,var);
   strcpy(tmp_val,val);
   tmp_buf->name = tmp_var;
   tmp_buf->value = tmp_val;
   tmp_buf->nsize = nsize;
   tmp_buf->vsize = vsize;
   tmp_buf->vdyn = 1;
   tmp_buf->ndyn = 1;
   tmp_buf->next = NULL;

   tmp_cgi = cgi_vars;
   if (tmp_cgi == NULL) {
      /* first record! */
      cgi_vars = tmp_buf;
   } else {
      while (tmp_cgi->next != NULL) {
         tmp_cgi = tmp_cgi->next;
      }
      tmp_cgi->next = tmp_buf;
   }
   return 1;
}
/****************************************************************************
 * header_process_content()
 ***************************************************************************/
static unsigned short header_process_content(char *s)
{
 int len;

   len = strlen(s);

   if (s[len-1] == ';')
      s[--len] = 0;

   if (!strncmp(s,"name=",5)) {
      if (s[len-1] == '"') {
         s[len-1] = 0;
      }
      return 1;
   } 
   return 0;
}
/****************************************************************************
 * read_query()
 * ==========
 ***************************************************************************/
int read_query()
{
 char c,b,old_c;
 int len,elen,nsize=0,vsize=0;
 t_state state;
 t_cgi_vars *tmp = NULL;
 char sbuf[3];
 int h;
 unsigned short first_char = 1;
 char *mime_separator = NULL;
 char *header_tag = NULL;
 char *this_line = NULL;
 int my_index = 0;

   state = do_malloc;
   old_c = 0;

   for (len=content_length,elen=content_length;elen>0;len--,elen--) {
      h = 0;
      c = (query_type == post) ? getchar():env[content_length-elen];

      if (first_char) {
         first_char = 0;
         if (c == '-') {
            /* damn! multi-part, we need alternate parser.. */
            state = obtain_separator;
         }
      }
      if ((c == 0x0a) && (old_c == 0x0d)) {
         /* when we see \r\n make it just be \n */
         /* need to worry about the 0 in the stream.. */
         len++;
         continue;
      } else if (c == 0x0d) {
         old_c = c;
         c = 0x0a;
      } else {
         old_c = c;
      }

      /* first we un-escape the character */
      if (c == '+')
         c = ' ';

      if (c == '%') {
         elen--;
         b = (query_type == post) ? getchar():env[content_length-elen];
         sbuf[0] = b;
         elen--;
         b = (query_type == post) ? getchar():env[content_length-elen];
         sbuf[1] = b;
         sbuf[2] = 0;
         sscanf(sbuf,"%X",&h);
         c = h;
      }

      switch(state) {
       case do_malloc:
         tmp = (t_cgi_vars *) malloc(sizeof(t_cgi_vars));
         if (tmp==NULL) {
            printf("<h1>ERROR</h1>malloc(%d) failed.<p>\nMemory error",
                   sizeof(t_cgi_vars));
            return 0;
         }
         cgi_buff[content_length-len] = c;
         tmp->name = &cgi_buff[content_length-len];
         tmp->next = NULL;
         if (cgi_vars == NULL) {
            cgi_vars = tmp;
         } else {
            tmp->next = cgi_vars;
            cgi_vars = tmp;
         }
         state = get_var;
         nsize = 1;     /* because this time counts as one */
         vsize = 0;
         tmp->vdyn = 0; /* this is not malloced.. */
         tmp->ndyn = 0; /* this is not malloced.. */
         break;
       case get_var:
         if (c == '=') {
            cgi_buff[content_length-len] = 0;
            state = get_val;
            tmp->value = &cgi_buff[content_length-len+1];
            tmp->nsize = nsize;
         } else {
            cgi_buff[content_length-len] = c;
            nsize++;
         }
         break;
       case get_val:
         /* check h since we may have had an escaped '&' */
         if ((c == '&')&&(h == 0)) {
            cgi_buff[content_length-len] = 0;
            tmp->vsize = vsize;
            state = do_malloc;
         } else {
            cgi_buff[content_length-len] = c;
            vsize++;
         }
         break;
       case obtain_separator:
         cgi_buff[content_length-len] = c;
         if (mime_separator == NULL) {
            mime_separator = &cgi_buff[content_length-len];
         }
         if ((c == '\n')||(c == '\r')) {
            cgi_buff[content_length-len] = 0;
            state = read_header;
         }
         break;
       case read_header:
         cgi_buff[content_length-len] = c;

         if ((c == '\n') && (header_tag == NULL)) {
            /* empty line. end of header. */
            state = read_body;
            this_line = NULL;
            my_index = 0;
            continue;
         }

         if (header_tag == NULL) {
            header_tag = &cgi_buff[content_length-len];
         }

         if ((c == ':')||(c == '\n')) {
            cgi_buff[content_length-len] = 0;
            this_line = NULL;
            state = read_header_content;
            header_tag = NULL;
         }
         break;
       case read_header_content:
         {
          unsigned short do_add, do_null;

            cgi_buff[content_length-len] = c;
            if (this_line == NULL) {
               this_line = &cgi_buff[content_length-len];
            }

            do_add = 0;
            do_null = 0;
            if (c == ' ') {
               cgi_buff[content_length-len] = 0;
               do_add = header_process_content(this_line);
               do_null = 1;
            } else if (c == '\n') {
               cgi_buff[content_length-len] = 0;
               do_add = header_process_content(this_line);
               do_null = 1;
               state = read_header;
            }

            if (do_add) {
               tmp = (t_cgi_vars *) malloc(sizeof(t_cgi_vars));
               if (tmp==NULL) {
                  printf("<h1>ERROR</h1>malloc(%d) failed.<p>\nMemory error",
                         sizeof(t_cgi_vars));
                  return 0;
               }
               tmp->name = &this_line[6];
               tmp->next = NULL;
               if (cgi_vars == NULL) {
                  cgi_vars = tmp;
               } else {
                  tmp->next = cgi_vars;
                  cgi_vars = tmp;
               }
               tmp->nsize = strlen(&this_line[6]);
               tmp->vsize = 0;
               tmp->vdyn  = 0; /* this is not malloced.. */
               tmp->ndyn  = 0; /* this is not malloced.. */
               if (state != read_header) {
                  state = finish_header_content;
               }
            }
            if (do_null) {
               this_line = NULL;
            }
         }
         break;
       case finish_header_content:
         cgi_buff[content_length-len] = c;
         if (c == '\n') {
            state = read_header;
         }
         break;
       case read_body:
         my_index++;

         cgi_buff[content_length-len] = c;
         if (this_line == NULL) {
            this_line = &cgi_buff[content_length-len];
         }

         if ((tmp != NULL) && (tmp->value == NULL)) {
            tmp->value = this_line;
            tmp->vsize = 1;
            vsize = 0;
         }

         /* need to keep an eye out for mime_separator */
         if (!strcmp(this_line,mime_separator)) {
            cgi_buff[content_length-len] = 0;
            /* remove the separator from the stream.. */
            cgi_buff[content_length-len-strlen(mime_separator)] = 0;

            if ((tmp != NULL) && (tmp->value != NULL)) {
             int my_len;
               /* remove trailing C/R */
               my_len = strlen(tmp->value);
               if (tmp->value[my_len-1] == '\n') {
                  tmp->value[--my_len] = 0;
               }
               tmp->vsize = my_len;
            }
            header_tag = NULL;
            state = read_separator;
         } else if (c == '\n') {
            this_line = NULL;
         }
         break;
       case read_separator:
         cgi_buff[content_length-len] = c;
         if (c == '\n') {
            cgi_buff[content_length-len] = 0;
            state = read_header;
         }
         break;
      }
   }
   if (tmp!= NULL) {
      if (state == read_header) {
         tmp->vsize = strlen(tmp->value);
      } else {
         tmp->vsize = vsize;
         tmp->vsize = state;
      }
      cgi_buff[content_length-len] = 0;
   }

   return 1;
}

/****************************************************************************
 * init_cgi()
 * ========
 ***************************************************************************/
int init_cgi()
{
 int rc;

   env = (char *) getenv("CONTENT_LENGTH");
   if (env == NULL) {
      content_length = 0;
      query_type = get;
   } else {
      content_length = atoi(env);
      query_type = post;
   }

   if (query_type == get) {
      env = (char *) getenv("QUERY_STRING");
      if (env == NULL) 
         content_length = 0;
      else 
         content_length = strlen(env);
   }

   cgi_buff = (char *) malloc(content_length+1);

   if (cgi_buff == NULL) {
      printf("<h1>ERROR</h1>malloc(%d) failed.<p>\nMemory error",
             content_length+1);
      return 0;
   }

   cgi_vars = NULL;

   rc = read_query();
   return rc;
}

/****************************************************************************
 * load_cookies()
 * Stuff HTTP cookies into cgi vars
 * HTTP_COOKIE is of the form "name1=val1; name2=val2; name3=val3"
 * This routine is optionally called after calling init_cgi()
 * It is only needed if using cookies, and by making it a separate function,
 * allows for a remove_value() to be performed between the init_cgi() and
 * load_cookies() calls.  To set session cookies, just do this:
 *   printf("Content-type: text/html%c", 10);
 *   printf("Set-Cookie: name1=val1; path=/; domain=.yourdomain1.net%c",10);
 *   printf("Set-Cookie: name2=val2; path=/; domain=.yourdomain2.com%c",10);
 *   printf("%c",10);
 ***************************************************************************/
int load_cookies() {
 char *env;
 char *name;
 char *value;
 int state;
 int count;
 int maxlen;
 int len = 0;

   env = (char *) getenv("HTTP_COOKIE");
   if (env == NULL || *env == 0) {
      return 0;
   }
   maxlen = strlen(env);
   name  = (char *)malloc(maxlen + 1); /* malloc for no buffer 'sploit */
   if (!name) {
      return 0;
   }
   value = (char *)malloc(maxlen + 1);
   if (!value) {
      free(name);
      return 0;
   }
   state = 0;
   count = 0;
   while (*env) {
      switch (state) {
       case 0:
         len = 0;
         name[len] = 0;
         state = 1;
       case 1:
         if (*env != ' ') {
            state = 2;
         } else {
            env++; /* Skip leading whitespace on cookie name */
         }
         break;
       case 2:
         if (*env != '=' && len < maxlen) {
            name[len++] = *env++;  /* Copy characters of cookie name */
         } else {
            name[len] = 0;
            env++;
            state = 3;
         }
         break;
       case 3:
         len = 0;
         value[len] = 0;
         state = 4;
       case 4:
         if (*env != ' ') {
            state = 5;
         } else {
            env++; /* Skip leading whitespace on cookie value */
         }
         break;
       case 5:
         if (*env != ';' && len < maxlen) {
            value[len++] = *env++; /* Copy characters of cookie value */
         } else {
            value[len] = 0;
            if (name[0] != 0 && value[0] != 0) {
               add_cgi_value(name, value);
               count++;
            }
            env++;
            state = 0;
         }
         break;
      }
   }
   if (state == 5 && name[0] != 0 && value[0] != 0) {
      value[len] = 0;
      add_cgi_value(name, value);
      count++;
   }
   free(value);
   free(name);
   return count;
}

