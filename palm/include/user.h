#ifndef __USER_H__
#define __USER_H__

typedef struct
{
	char	name[256];
	char	login[32];
	char	passwd[33];
	char	domain[256];
	int	access;
	int	uid;
} USER_INFO;

int user_info(const char *data, USER_INFO *uinfo);
int login_user();
int authorize(USER_INFO *uinfo);

#endif // __USER_H__
