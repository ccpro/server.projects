#include <stdio.h>
#include <string.h>

#include <cgi.h>
#include <user.h>
#include <llist.h>
#include <cookie.h>

LLIST cgi_cookie;
LLIST s_cookie;
extern int init_header;

int parse_cgi_cookie()
{
	char *cookie = NULL;
	int cookie_count = 0;
	HTTP_COOKIE *ck;
	char *pc1, *pc2, *pc3;
	int break_flag = 0;

	ll_init(&cgi_cookie);
	ll_init(&s_cookie);

	if (getenv("HTTP_COOKIE"))
	{
		cookie = strdup((char*)getenv("HTTP_COOKIE"));
		if (cookie == NULL)
			return 0;
	}
	else
		return 0;

	pc1 = cookie;
	while(!break_flag)
	{
		if (!(pc2 = strstr(pc1, "; ")))
			break_flag = 1;
		else
		{
			*pc2 = '\0';
			pc2 += 2;
		}

		ck = malloc(sizeof(HTTP_COOKIE));
		if ((pc3 = strchr(pc1, '=')))
		{
			*pc3 = '\0';
			pc3 ++;

			strcpy(ck->name, pc1);
			strcpy(ck->value, pc3);

			ll_add(&cgi_cookie, ck);

			cookie_count ++;
		}

		pc1 = pc2;
	}

	free (cookie);

	return cookie_count;
}

char *get_cookie_value(const char *name)
{
	int i=0;
	HTTP_COOKIE *ck;

	for (i=0; i < ll_get_count(&cgi_cookie); i++)
	{
		ck = ll_get(&cgi_cookie, i);
		if (!strcmp(ck->name, name))
			return ck->value;
	}

	return NULL;
}

int add_cookie(HTTP_COOKIE *scookie)
{
	if (scookie == NULL)
		return 0;

	ll_add(&s_cookie, scookie);

	return ll_get_count(&s_cookie);
}

int send_cookie(HTTP_COOKIE *scookie)
{
	char cookie_string[1024*10];

	if (scookie == NULL || init_header)
		return -1;

	if (scookie->name && strlen(scookie->name) && scookie->value)
		sprintf(cookie_string, "%s=%s", scookie->name, scookie->value);
	else
		return -2;

	if (scookie->expires && strlen(scookie->expires))
		sprintf(cookie_string+strlen(cookie_string), "; expires=%s", scookie->expires);

	if (scookie->path && strlen(scookie->path))
		sprintf(cookie_string+strlen(cookie_string), "; Path=%s", scookie->path);

	if (scookie->domain && strlen(scookie->domain))
		sprintf(cookie_string+strlen(cookie_string), "; Domain=%s", scookie->domain);

	if (scookie->secure)
		strcat(cookie_string, "; Secure");

//	cgi_error("%s\ncookie %s\n", cookie_string, scookie->expires);

	fprintf(stdout, "Set-Cookie: %s\n\n", cookie_string);

	return 0;
}
