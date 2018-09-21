// SimpleAddressPDB.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "address.h"

int main(int argc, char* argv[])
{
 char *fname = "address.dat";
 CPalmAddressDat palm;
 ifstream ifs;
  
    ifs.open(fname, ios::binary);

    palm.read(ifs);

	return 0;
}

char *read_string(ifstream &ifs, char *str, int allocate_memory)
{
	byte b_size;
	ushort w_size;

	if (!ifs.is_open() || (!allocate_memory && str == NULL))
		return 0;

	ifs.read((char*)(void*)&b_size, sizeof(b_size));
	if (b_size == 0xFF)
	{
		ifs.read((char*)(void*)&w_size, sizeof(w_size));

		if (allocate_memory)
			str = (char*)malloc((size_t)w_size+1);

		ifs.read(str, w_size);
		str[w_size] = '\0';
	}
	else
	{
		if (allocate_memory)
			str = (char*)malloc((size_t)b_size+1);

		ifs.read(str, b_size);
		str[b_size] = '\0';
	}

	return str;
}


int skip_new_line(char *str)
{
	if (str == NULL || strlen(str) == 0)
		return 0;

	while(*str)
	{
		if (*str == '\r')
			*str = ',';
		if (*str == '\n')
			*str = ' ';

		str ++;
	}
	return 1;
}
