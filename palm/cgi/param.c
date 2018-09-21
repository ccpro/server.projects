#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <llist.h>
#include <param.h>

unsigned long UniqueID = 0;

LLIST cgi_param;

static int parse_post_param(char *sparam);

int parse_cgi_param(const char *name)
{
	int   param_count;
	char *method = getenv("REQUEST_METHOD");

	if (method && !strcasecmp("POST", method))
	{
		int length = atol(getenv("CONTENT_LENGTH"));
		char *buffer = NULL;

		if (length)
		{
			buffer = (char *)malloc(length+1);

			fread(buffer, 1, length, stdin);
			
			param_count = parse_post_param(buffer);

			free (buffer);
		}
	}
	else if (method && !strcasecmp("GET", method))
	{
		char	*query = getenv("QUERY_STRING");
		char	*dq;

		if (!query)
			return 0;
		if (!UniqueID)
			UniqueID = random();

		if (!(dq = strdup(query)))
			return 0;

		param_count = parse_get_param(dq);

		free (dq);
	}

	
	return param_count;
}

char *get_param(const char *param)
{
	int i;
	HTTP_PARAM *prm;

	for (i=0; i < ll_get_count(&cgi_param); i++)
	{
		prm = ll_get(&cgi_param, i);
		if (!strcmp(param, prm->name))
			return prm->data;
	}
	
	return NULL;
}

int parse_post_param(char *sparam)
{
	int   param_count;
	int   break_flag = 0;
	HTTP_PARAM *prm;
	char *pc1, *pc2, *pc3;

	pc1 = pc2 = sparam;

	if (!(pc2 = strchr(pc1, '\n')))
		return 0;
	*pc2 = '\0';

	while(*pc1 == '-')
		pc1 ++;

	param_count = 0;
	UniqueID = atol(pc1);

	pc1 = ++pc2 ;
	while(!break_flag)
	{
		if ((pc2 = strchr(pc1, '\n')))
			*pc2 = '\0';
		else
			break_flag = 1;

		if (!strncmp("Content-Disposition", pc1, 19))
		{
			if ((pc1 = strstr(pc1, "form-data;")))
			{
				prm = (HTTP_PARAM*) malloc(sizeof(HTTP_PARAM));
				pc1 += 11;

				if (!strncmp("name", pc1, 4))
				{
					prm->file = 0;

					pc1 += 6;
					if (*pc1 == '"')
						pc1 ++;

					if ((pc3 = strchr(pc1, '"')))
						*pc3 = '\0';

					pc2 = pc1 + strlen(pc1) + 5;

					strcpy(prm->name, pc1);

					pc1 = pc2;

					if ((pc2 = strchr(pc1, '\n')))
					{
						*(--pc2) = '\0';
						strcpy(prm->data, pc1);
					}

					pc1 = strchr(pc2+2, '\n');
					pc1 ++;


					ll_add(&cgi_param, prm);
				}
				else if (!strncmp("filename", pc1, 8))
				{
					prm->file = 1;
				}
			}
			else
			{
				return param_count;
			}

			param_count ++;
		}
	}

	return param_count;
}

int parse_get_param(char *query)
{
	HTTP_PARAM *pr;
	char *pc1, *pc2, *pc3;
	int break_flag = 0;
	int i;
	int param_count = 0;

	pc1 = query;
	while(!break_flag)
	{
		if (!(pc2 = strchr(pc1, '&')))
			break_flag = 1;
		else
			*(pc2++) = '\0';

		pr = malloc(sizeof(HTTP_PARAM));
		if ((pc3 = strchr(pc1, '=')))
		{
			*pc3 = '\0';
			pc3 ++;

			pr->file = 0;
			strcpy(pr->name, pc1);
			strcpy(pr->data, pc3);

			ll_add(&cgi_param, pr);

			param_count ++;
		}

		pc1 = pc2;
	}

	return param_count;
}
