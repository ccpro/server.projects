#include <unistd.h>
#include "config.h"

void Config::parse(int argc, char **argv)
{
 int option_char;
 extern char *optarg;

   while ((option_char = getopt(argc, argv, "dp:")) != EOF) {
      switch (option_char) {  
       case 'd': setFlag(Config::DEBUG); break;
       case 'p': setPath(optarg); break;
       case '?': usage(argv[0]);
      }
   }
}

void Config::usage(const string pname)
{
   cout << "Use: " << pname << " args " << endl
        << "Where:" << endl
        << "\t-d - turn debug on" << endl
        << "\t-p - path to dir for scan" << endl;
   exit(1);
}
