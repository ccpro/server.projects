#include <stdio.h>
#include <string.h>

#include "datebook.h"

const char *fname = "./datebook.dat";

int main()
{
 DateBook_DAT db;

   read_datebook_dat(&db, fname);
   print_datebook(&db);
   destroy_datebook_dat(&db);

   return 0;
}
