#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "auth.h"
#include "cgi.h"
#include "base64.h"

#ifdef DEBUG
static char *rcsid = "$Id: auth.c,v 1.3 2007/03/30 20:55:53 ccpro Exp $";
#endif


t_user user;

static int read_credential();

void init_user(const char *cgi_name)
{
 char *ptr;

   load_cookies();

   memset((void*)&user, 0, sizeof(t_user));

   ptr = strrchr(cgi_name, '/');
   if (ptr) {
      snprintf(user.cgi_name, STRLEN, "%s/%s", "/bin", ++ptr);
   } else {
      strncpy(user.cgi_name, cgi_name, STRLEN);
   }

   ptr = getenv("REMOTE_ADDR");
   if ( !NULL_OR_EMPTY(ptr) ) {
      strcpy(user.ip, ptr);
   }
   
   read_credential();
}

int get_access()
{
   return user.access;
}

int user_can_read()
{
   return user.access & USER_CAN_READ;
}

int read_credential()
{
 char *udata;
 char *decoded_data;
 char *ptr;

   udata = get_value("udata");

   if (NULL_OR_EMPTY(udata)) {
      return 0;
   }

   decoded_data = (char*)base64_decode(udata);

//
// sergey s prozhogin:9877:ccpro.no-ip.org:18319:49183:545:
//

   ptr = strchr(decoded_data, ':');
   if (ptr == NULL) {
      return 0;
   }

   *ptr = '\0';

   strncpy(user.user, decoded_data, STRLEN);
   decoded_data = ++ptr;

   ptr = strchr(decoded_data, ':')+1;
   ptr = strchr(decoded_data, ':')+1;

   decoded_data = ptr;

   ptr = strchr(decoded_data, ':')+1;
   *ptr = '\0';

   user.access = atoi(decoded_data);

   return 1;
}
