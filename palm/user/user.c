#include <stdio.h>
#include <string.h>
#include <time.h>

#include <user.h>
#include <cgi.h>
#include <param.h>
#include <pg_lib.h>
#include <md5.h>

USER_INFO uinfo;

int login_user()
{
	char *login = "ccpro"; // get_param("login");
	char *pswd = "n0wh3r3"; // get_param("pswd");
	char *pswd_md5;
	int rc;

	if (login == NULL || !strlen(login) || pswd == NULL || !strlen(pswd))
		return -1;
	
	if (!(rc = read_user_info(login, &uinfo)))
	{
		pswd_md5 = md5(pswd);

		if (getenv("HTTP_HOST"))
			strcpy(uinfo.domain, (char*)getenv("HTTP_HOST"));

		if (!strncmp(uinfo.passwd, pswd_md5, strlen(pswd_md5)))
		{
			authorize(&uinfo);
		}
		else 
		{
			cgi_error ("Wrong password, try again<br>\n");
			return -2;
		}

		return 0;
	}
	else
	{
		cgi_error("something is wrong with db (code %d)<br>\n", rc);
	}

	return -3;
}

int user_info(const char *data, USER_INFO *uinfo)
{
	char *pc1, *pc2;
	int count = 0, length;
	char *ddata;

	if (data == NULL || strlen(data) == 0 || uinfo == NULL)
		return -1;

	length = strlen(data);
	ddata = (char*)base64_decode(data);

	if (strlen(ddata) == 0)
		return -2;

	pc1 = ddata;
	while(pc2 = strchr(pc1, ':'))
	{
		count ++;
		*pc2 = '\0';
		pc1 = pc2 + 1;
	}

	if (count != 6)
		return -3;

	pc1 = ddata;
	strcpy(uinfo->name, pc1);

	pc1 += strlen(pc1)+1;
	pc1 += strlen(pc1)+1;

	strcpy(uinfo->domain, pc1);

	pc1 += strlen(pc1)+1;
	pc1 += strlen(pc1)+1;

	uinfo->access = atoi(pc1);

	pc1 += strlen(pc1)+1;
	pc1 += strlen(pc1)+1;

	uinfo->uid = atoi(pc1);

	return 0;
}

int authorize(USER_INFO *uinfo)
{
	char	data[1024], encrypted_data[1024];
	HTTP_COOKIE  *cookie;
	time_t exp_date = time(NULL) + 20*600;
	char exp_time[64];

	cookie = (HTTP_COOKIE*)malloc(sizeof(HTTP_COOKIE));
	memset(cookie, 0, sizeof(HTTP_COOKIE));			
	strftime (exp_time, 48, "%A, %d-%b-%Y %H:%M:%S GMT", gmtime (&exp_date));

	sprintf(data, "%s:%d:%s:%d:%d:%d:%d",
		uinfo->name, srandom(99999),
		uinfo->domain, srandom(99999),
		uinfo->access, srandom(99999),
		uinfo->uid
	);

	strcpy(cookie->value, (char*)base64_encode(data));

	strcpy(cookie->name, "udata");
	strcpy(cookie->domain, uinfo->domain);
	strcpy(cookie->expires, exp_time);
	strcpy(cookie->path, "/");
	cookie->secure = 0;

	return add_cookie(cookie);
}
