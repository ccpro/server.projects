/****************************************************************************
 * cgi.h
 *
 *    Header for Nik's CGI stuff.. definition of t_cgi_vars and prototypes
 *    for the cgi handling code.
 *
 * $Id: cgi.h,v 1.1.1.1 2005/09/28 20:32:42 ccpro Exp $
 ***************************************************************************/

#ifndef _CGI_H_
#define _CGI_H_

/****************************************************************************
 * The t_cgi_vars structure used to store all the cgi variables.
 *
 ***************************************************************************/

typedef struct tt_cgi_vars {
        char                    *name;         /* The CGI variable name      */
        char                    *value;        /* The CGI variable value     */
        int                     nsize;         /* Length of 'name'           */
        int                     vsize;         /* Length of 'value'          */
        int                     ndyn:1;        /* 1 if 'name' was malloc-ed  */
        int                     vdyn:1;        /* 1 if 'value' was malloc-ed */
        struct tt_cgi_vars      *next;         /* pointer to next cgi struct */
} t_cgi_vars;

/* these are the two types of METHOD's that we know about */
typedef enum {
   post,
   get
} t_qtype;

/****************************************************************************
 * prototypes
 *
 ***************************************************************************/

char *get_value     (char *name);
void  list_values   (FILE *stream);
int   free_cgi      ();
int   init_cgi      ();
int   remove_value  (char *name);
int   rename_cgi    (char *old, char *new);
int   add_cgi_value (char *var, char *val);
int   multi_get_value(char *name, char **buf, int buf_size);
int   load_cookies  ();

#endif /* ifndef _CGI_H_ */
