#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include <pg_lib.h>
#include <user.h>

/*
typedef struct
{
	char *pghost;
	char *pgport;
	char *pgoption;
	char *pgtty;
	char *dbname;
	char *login;
	char *pwd;
} PG_CONNECT;
*/

PG_CONNECT pg_info;
static PGconn     *pg_conn	= NULL;
static PGresult   *pg_res	= NULL;

int db_connect(PG_CONNECT *cnct)
{
	char conn_info[256];
	
	if (cnct == NULL)
		return -1;

	if (cnct->dbname == NULL || strlen(cnct->dbname) == 0)
		return -2;

	if (cnct->dbname && strlen(cnct->dbname))
		sprintf(conn_info, "dbname=%s ", cnct->dbname);
	if (cnct->username && strlen(cnct->username))
		sprintf(conn_info+strlen(conn_info), "user=%s ", cnct->username);
	if (cnct->password && strlen(cnct->password))
		sprintf(conn_info+strlen(conn_info), "user=%s ", cnct->password);
	if (cnct->pgport > 0)
		sprintf(conn_info+strlen(conn_info), "port=%d ", cnct->pgport);

	/* Make a connection to the database */
	pg_conn = PQconnectdb(conn_info);
	                
	/* Check to see that the backend connection was successfully made */
	if (PQstatus(pg_conn) != CONNECTION_OK)
	{
		fprintf(stderr, "Connection to database '%s' failed.\n", PQdb(pg_conn));
		fprintf(stderr, "%s", PQerrorMessage(pg_conn));
		cgi_error("Connection to database '%s' failed.\n", PQdb(pg_conn));
		cgi_error("%s", PQerrorMessage(pg_conn));

		return -3;
	}
                                                                                
	return 0;                                                                        
}

int db_disconnect()
{
	if (pg_conn)
	{
		PQfinish(pg_conn);
		pg_conn = NULL;
	}
}

int read_user_info(const char *login, USER_INFO *uinfo)
{
	int num[5], i;
	int blen, ival;
	char *iptr[5];
	const char *params[1];
	
	if (pg_conn == NULL || login == NULL || !strlen(login) || uinfo == NULL)
		return -1;

	params[0] = login;

	pg_res = PQexecParams(pg_conn,
		"SELECT  id, name, login, passwd, service FROM users "
		"WHERE login = $1",
		1,           /* one param */
		NULL,        /* let the backend deduce param type */
		params,
		NULL,        /* don't need param lengths since text */
		NULL,        /* default to all text params */
		1);          /* ask for binary results */

	if (PQresultStatus(pg_res) != PGRES_TUPLES_OK)
	{
		fprintf(stderr, "SELECT failed: %s", PQerrorMessage(pg_conn));
		cgi_error("SELECT failed: %s<br>", PQerrorMessage(pg_conn));
		PQclear(pg_res);

		return -2;
	}
	
	num[0] = PQfnumber(pg_res, "id");
	num[1] = PQfnumber(pg_res, "name");
	num[2] = PQfnumber(pg_res, "login");
	num[3] = PQfnumber(pg_res, "passwd");
	num[4] = PQfnumber(pg_res, "service");

	if (PQntuples(pg_res) < 1)
	{
		PQclear(pg_res);
		return -4;
	}

	for(i=0; i< 5; i++)
		iptr[i] = PQgetvalue(pg_res, 0, num[i]);

	uinfo->uid = ntohl(*((uint32_t *) iptr[0]));
	uinfo->access = ntohl(*((uint32_t *) iptr[4]));

	strcpy(uinfo->name, iptr[1]);
	strcpy(uinfo->passwd, (char*)iptr[3]);
	strcpy(uinfo->login, (char*)iptr[2]);

	PQclear(pg_res);

	return 0;
}
