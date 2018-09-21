#include	<stdio.h>
#include	<time.h>
#include	<stdlib.h>

#include	<cgi.h>
#include	<cookie.h>
#include	<dialog.h>
#include	<param.h>
#include	<llist.h>
#include	<pg_lib.h>

extern USER_INFO uinfo;
extern LLIST cgi_param;
extern LLIST cgi_cookie;
extern LLIST s_cookie;

extern PG_CONNECT pg_info;

static int InitCode();
static int ExitCode(int code);

int main()
{
	char title[256];
	char *pt, *udata;
	char *action;
	int i;

	cgi_error("before Init");
	InitCode();

	sprintf(title, "PALM Project %s %s %s", VERSION, DATE_COMPILE, OS);

	cgi_error("after Init");
	action = "login"; // get_param("action");

	udata = (char*)get_cookie_value("udata");
	cgi_error("action:udata %s\n", action, udata);
	if (udata && !user_info(udata, &uinfo))
	{
		cgi_error("here: action %s\n", action);
			printf("name %s\nlogin %s\npasswd %s\ndomain %s\n access %d\nuid %d\n",
				uinfo.name,
				uinfo.login,
				uinfo.passwd,
				uinfo.domain,
				uinfo.access,
				uinfo.uid
			);
		if (action)
		{
			if (!strcmp(action, "upload"))
			{
				start_html(NULL);
				header(title, NULL);

				upload_file();
			}
			else
			{
				start_html(NULL);
				header(title, NULL);

				upload_dialog();
			}
		}
	}
	else 
	{
		if (action)
		{
			if (!strcmp(action, "login"))
			{
				login_user();

				start_html(NULL);
				header(title, NULL);
				
				printf("login 1\n");
			}
		}
		else
		{
			start_html(NULL);
			header(title, NULL);

			login_dialog();

			printf("<pre>login 2\n%s\n", udata);
			printf("name %s\nlogin %s\npasswd %s\ndomain %s\n access %d\nuid %d\n",
				uinfo.name,
				uinfo.login,
				uinfo.passwd,
				uinfo.domain,
				uinfo.access,
				uinfo.uid
			);
		}
	}

	end_html();
	
	ExitCode(0);
}

int InitCode()
{
	init_hex_table();

	ll_init(&cgi_param);
	ll_init(&cgi_cookie);

	memset((void*)&pg_info, 0, sizeof(PG_CONNECT));
	pg_info.dbname = strdup("ccpro_spb_ru");
	pg_info.username = strdup("ccpro");
//	pg_info.password = strdup("ccpro_spb_ru");

	db_connect(&pg_info);

	memset((void*)&uinfo, 0, sizeof(USER_INFO));
	
	parse_cgi_param();
	parse_cgi_cookie();
}

int ExitCode(int code)
{
	int i;

	for(i=0; i < ll_get_count(&cgi_param); i++)
		free (ll_get(&cgi_param, i));

	ll_destroy(&cgi_param);

	for(i=0; i < ll_get_count(&cgi_cookie); i++)
		free (ll_get(&cgi_cookie, i));

	ll_destroy(&cgi_cookie);
	ll_destroy(&s_cookie);

	db_disconnect();

	if (pg_info.dbname)
		free (pg_info.dbname);
	if (pg_info.username)
		free (pg_info.username);

	exit (code);
}
