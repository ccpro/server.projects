#ifndef __HTML_H__
#define __HTML_H__

void table_100_start();
void table_500_start();
void table_start(int width);
void table_end();
void form_start(char *s,char *a);
void form_start_get(char *s,char *a);
void form_button(char *n,char *v,char *js);
void form_start_name(char *s, char *a, char *n);
void form_submit_name(char *s, char *n);
void form_submit(char *s);
void form_clear(char *s);
void form_image_submit(char *s,char *alt);
void form_end();
void form_hidden(char *s, char *v);
void form_hidden_formatted(char *s, char *fmt, ...);
void form_text_input_ro(char *n,int s,int m,char *v);
void js_form_text_input(char *n,int s,int m,char *v, char *js);
void form_text_input(char *n,int s,int m,char *v);
void form_textarea_input(char *n, int r, int c, char *v, int max);
int print_maxlenfunc();
void form_checkbox(char *n, char *v, int c);
void form_radio(char *n, char *v, int c);
void form_password_input(char *n,int s,int m,char *v);
int yes_no_select(char *name,int def);
int yes_no_select_onchange(char *name,int def,char *onchange);
char *yes_no_print(int def);
int cc_month_select(char *name,int def);
int cc_year_select(char *name,int def);
void fancy_header(char *t,char *r);
int print_semi_formatted(char *s);
void generic_table_based_scroller(char *name, char *table, char *var, 
                                  char *index, char *def);
void generic_table_based_scroller2(char *name, char *table, char *var,
                                   char *index, char *def, char *order, 
                                   char *onchange);
int html_error_warning(char *msg);
int missing_data_warning();
int access_denied_warning();
int memory_error();
int print_http_header(const char *sid, const char *bgcolor, 
                      const char *content_type, const char *title);
int close_http();

#endif // __HTML_H__
