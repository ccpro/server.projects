#include "cpalmaddress.hpp"

int main(int argc, char **argv)
{
 CPalmAddress dat;
 char *fname = "address.dat";
 ifstream ifs;
 int i;

   ifs.open(fname, ios::binary);
   i = dat.read(ifs);
   
cout << "read result:" << i << endl;
   
   dat.print();

   ifs.close();
}
