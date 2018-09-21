/****************************************************************************
 * pio.h
 *
 * $Id: pio.h,v 1.1.1.1 2005/09/28 20:32:42 ccpro Exp $
 ***************************************************************************/
#ifndef _PIO_H_
#define _PIO_H_

#include <libpq-fe.h>

#define PRODUCTION_DB     "10.1.1.1"
#define DEVELOPMENT_DB    "10.1.1.1"

extern int   psql_connect                    (PGconn **conn);
extern int   psql_disconnect                 (PGconn **conn);
extern int   dbscape                         (char **esc_str);
extern int   sql_run_command                 (PGresult **res, char **sql,
                                              int result_code, int error_msg,
                                              const char *func, const char *file,
                                              int line, int clear_and_free);
extern void  generic_table_based_scroller    (char *name, char *table,
                                              char *var, char *index,
                                              char *def);
extern void  generic_table_based_scroller2   (char *name, char *table,
                                              char *var, char *index,
                                              char *def, char *order,
                                              char *onchange);
extern char *generic_table_based_lookup      (char *table, char *var, 
                                              char *where, char *val,
                                              const char *caller, const char *caller_file,
                                              int caller_line);

#define XP_SHOW_ERROR           1
#define XP_DONT_SHOW_ERROR      0
#define XP_CLEAR_AND_FREE       1
#define XP_DONT_CLEAR_AND_FREE  0

#endif /* ifndef _PIO_H_ */
