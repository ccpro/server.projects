#ifndef __CGI_H__
#define __CGI_H__

#define	COOKIE_OK		 0
#define	COOKIE_NOT_FOUND	-1
#define	BAD_COOKIE		-2

#include <cookie.h>

typedef struct
{
} HTTP_META;


void start_html();
void end_html();

void header(const char *title, HTTP_META *meta);
void h(int size, const char *text);
void a(const char *href, const char *text, const char *target);
void br();
void hr(int height, int weght);

void start_form(
	const char *name,
	const char *method,
	const char *onSumbit,
	const char *action);
void end_form();                            

void hidden(const char *name, const char *value, int override);
void upload(const char *name, int maxlength);

void dropdown(const char *name, int size, const char *data, int multiple);

void text(const char *name, const char *value, int size, int maxlength);
void password(const char *name, const char *value, int size, int maxlength);

void submit(const char *name, const char *value);

void init_hex_table();
char *cgi_escape_string(const char *str);
char *cgi_unescape_string(const char *str);

void cgi_error(const char *fmt, ...);

#endif //__CGI_H__
