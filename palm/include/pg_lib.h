#ifndef __PGLIB_H__
#define __PGLIB_H__

#include <libpq-fe.h>

typedef struct
{
	char *pghost;
	int   pgport;
	char *pgoption;
	char *pgtty;
	char *dbname;
	char *username;
	char *password;
} PG_CONNECT;

int db_connect(PG_CONNECT *);
int db_disconnect();

#endif // __PGLIB_H__
