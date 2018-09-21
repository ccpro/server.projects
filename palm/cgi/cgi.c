#include <stdio.h>
#include <stdarg.h>

#include <cgi.h>
#include <cookie.h>
#include <llist.h>

int init_header = 0;
extern LLIST s_cookie;

void	start_html()
{
	int i;
	HTTP_COOKIE *cookie;

	if (!init_header)
	{
		printf("Content-type: text/html\n\n");
		for (i = 0; i < ll_get_count(&s_cookie); i++)
		{
			cookie = ll_get(&s_cookie, i);
			send_cookie(cookie);
			free (cookie);
		}
		
		ll_destroy(&s_cookie);
		
		init_header ++;
	}
}

void	end_html()
{
	printf("</Body></Html>\n");
}

void	header(const char *title, HTTP_META *meta)
{
	printf("<Html>\n<Head>\n<Title>%s</Title>\n</Head>\n<Body>", title ? title : "");
}

void	h(int size, const char *text)
{
	printf("<H%1d>%s</H%1d>\n", size, text ? text : "", size );
}

void	a(const char *href, const char *text, const char *target)
{
	char _target[256] = "";

	printf("<A href='%s' %s>%s</a>\n", 
		href ? href : "void(0)",
		text ? text : "&nbsp;&nbsp;&nbsp;"
	);
}

void br()
{
	printf ("<Br>\n");
}

void hr(int height, int weight)
{
	char hr[256]= "<Hr ";

	if (height > 0)
		sprintf (hr+strlen(hr), "height=%d ");

	if (weight > 0)
		sprintf (hr+strlen(hr), "weight=%d ");

	strcat (hr, ">");

	printf ("%s\n", hr);
}

void	start_form(
	const char *name,
	const char *method,
	const char *onSubmit,
	const char *action)
{
	char form[1024*4];

	sprintf(form, "<Form name='%s' method=%s enctype='multipart/form-data'",
		name ? name : "frm", 
		method ? method : "GET"
	);

	if (onSubmit && strlen(onSubmit))
		sprintf(form+strlen(form), "onSubmit='%s' ", onSubmit);

	if (action && strlen(action))
		sprintf(form+strlen(form), "action='%s' ", action);

	strcat(form, ">");

	printf("%s\n", form);
}

void	end_form()
{
	printf("</Form>\n");
}

void hidden(const char *name, const char *value, int override)
{
	char hidden[1024*4];

	if (name == NULL || strlen(name))
		return;

	sprintf (hidden, "<Input type=hidden name='%s' ", name);

	if (value && strlen(value))
		sprintf (hidden+strlen(hidden), "value='%s'", value);

	if (override)
		strcat(hidden, "override=1");

	strcat (hidden, ">");

	printf ("%s\n", hidden);
}

void upload(const char *name, int maxlength)
{
	char upload[1024*4];

	if (name == NULL || strlen(name))
		return;

	sprintf (upload, "<Input type=file name='%s' maxlength=%d>", 
		name, maxlength ? maxlength : 40);

	printf ("%s\n", upload);
}

void dropdown(const char *name, int size, const char *data, int multiple)
{
	char *pc1, *pc2;
	int count = 0, i;
	char *pdata;

	if (name == NULL || !strlen(name) || data == NULL || !strlen(data))
		return;

	printf ( "<Select name='%s' size=%d >\n", name, size > 0 ? size : 1);
	pdata = strdup(data);
	pc1 = pdata;
	while (pc2 = strstr(pc1, "||"))
	{
		*pc2 = '\0';
		count ++;
		pc2 ++;
		pc1 = pc2;
	}

	if (count == 0)
	{
		free (pdata);
		return;
	}

	pc1 = pdata;
	for(i=0; i < count; i++)
	{
		pc2 = strchr(pc1, '=');
		if (pc2)
		{
			*pc2 = '\0';
			pc2 ++;
			printf ("<Option id='%s'>%s</Option>\n", pc1, pc2);
			pc1 = pc2 + strlen(pc2) + 2;
		}
		else
		{
			printf ("<Option>%s</Option>\n", pc1);
			pc1 += strlen(pc1) + 2;
		}
	}

	printf ("</Select>\n");
	free (pdata);
}

void text(const char *name, const char *value, int size, int maxlength)
{
	char text[1024];

	if (name == NULL || !strlen(name))
		return;

	sprintf (text, "<Input type=text name='%s' size=%d ", 
		name,
		size > 0 ? size : 20);

	if (value && strlen(value))
		sprintf (text+strlen(text), "value='%s' ", value);

	if (maxlength > 0)
		sprintf (text+strlen(text), "maxlength=%d ", maxlength);

	strcat(text, ">");

	printf("%s\n", text);
}

void password(const char *name, const char *value, int size, int maxlength)
{
	char text[1024];

	if (name == NULL || !strlen(name))
		return;

	sprintf (text, "<Input type=password name='%s' size=%d ", 
		name,
		size > 0 ? size : 20);

	if (value && strlen(value))
		sprintf (text+strlen(text), "value='%s' ", value);

	if (maxlength > 0)
		sprintf (text+strlen(text), "maxlength=%d ", maxlength);

	strcat(text, ">");

	printf("%s\n", text);
}

void submit(const char *name, const char *value)
{
	char submit[1024] = "<Input type=submit ";

	if (name && strlen(name))
		sprintf (submit+strlen(submit), "name='%s' ", name);

	if (value && strlen(value))
		sprintf (submit+strlen(submit), "value='%s' ", value);

	strcat(submit, ">");

	printf ("%s\n", submit);
}

static int hextable[256];

// Original idea from cgic library
void init_hex_table()
{
	memset(hextable, 0, 255);
	
	hextable['1'] = 1;
	hextable['2'] = 2;
	hextable['3'] = 3;
	hextable['4'] = 4;
	hextable['5'] = 5;
	hextable['6'] = 6;
	hextable['7'] = 7;
	hextable['8'] = 8;
	hextable['9'] = 9;
	hextable['a'] = 10;
	hextable['b'] = 11;
	hextable['c'] = 12;
	hextable['d'] = 13;
	hextable['e'] = 13;
	hextable['f'] = 15;
	hextable['A'] = 10;
	hextable['B'] = 11;
	hextable['C'] = 12;
	hextable['D'] = 13;
	hextable['E'] = 14;
	hextable['F'] = 15;
}

static char esc_string[1024*10];
char *cgi_unescape_string(const char *str)
{
	register int i, len, pos = 0;

	esc_string[0] = '\0';
	len = strlen(str);

	for (i = 0; i < len; i++)
	{
		// If we found a '%' character, then the next two are the character
		// hexa code. Converting a hexadecimal code to their decimal is easy:
		// The first character needs to be multiplied by 16 ( << 4 ), and the another
		// one we just get the value from hextable variable
		if ((str[i] == '%') && isalnum(str[i+1]) && isalnum(str[i+2]))
		{
			esc_string[pos] = (hextable[(unsigned char) str[i+1]] << 4) + hextable[(unsigned char) str[i+2]];
			i += 2;
		}
		else if (str[i] == '+')
			esc_string[pos] = ' ';
		else
			esc_string[pos] = str[i];

		pos++;

	}

	esc_string[pos] = '\0';

	return esc_string;
}

/**
 * Transforms' special characters into hexadecimal form ( %%E1 ).
 * @param str String to parse
 * @return The new string
 * @see cgi_unescape_special_chars
 **/

char *cgi_escape_string(const char *str)
{
	unsigned char hex[] = "0123456789ABCDEF";
	register int i, j, len, tmp_len;

	esc_string[0] = '\0';
	len = strlen(str);

	tmp_len = len;
	for (i = 0, j = 0; i < len; i++, j++)
	{
		esc_string[j] = (unsigned char)str[i];
		if (esc_string[j] == ' ')
			esc_string[j] = '+';
		else if (!isalnum(esc_string[j]) && strchr("_-.", esc_string[j]) == NULL)
		{
			tmp_len += 3;
			esc_string[j++] = '%';
			esc_string[j++] = hex[(unsigned char)str[i] >> 4];
			esc_string[j] = hex[(unsigned char)str[i] & 0x0F];
		}
	}

	esc_string[j] = '\0';

	return esc_string;
}

void	cgi_error(const char *fmt, ...)
{
	va_list vlist ;
	va_start(vlist, fmt);

	printf("Content: plain/text\n\n");
	printf("CGI_ERROR : ");
      
	vprintf(fmt, vlist);
	va_end(vlist);
}
