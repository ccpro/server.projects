#include <stdio.h>
#include <stdlib.h>

#include <cookie.h>
#include <cgi.h>

void upload_dialog()
{
	start_form("upload", "post", NULL, NULL);
//	hidden ("action", "upload", 0);

	upload("file", 60);
	br();

	dropdown("type", 1, "1=address||2=datebook||3=memopad||4=todo", 0);
	br();
	
	submit("action", "upload");
	
	end_form();
}

void login_dialog()
{
	start_form("login", "post", NULL, NULL);
//	hidden("action", "login", 0);
	
	text("login", NULL, 10, 30);
	br();
	password("pswd", NULL, 10, 16);

	br();
	submit("action", "login");

	end_form();
}
