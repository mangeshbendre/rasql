#include "defines.h"

#include <stdlib.h>
#include "RAParser.h"

#include <iostream>


// -----------------------------------------------------------

int main(int argc, char* argv[])
{


	int ret;
	RAParser *ra=new RAParser();

	char buf[2048];

	while (fgets(buf,sizeof(buf),stdin))
	{
		ret = ra->parseRA(buf);
		if (ret)
		{
			printf("Error - [%s]\n", ra->ErrorMessage.c_str());
		}
		else
		{
			printf("Success - [%s]\n", ra->getResult().c_str());
		}

	}
	return 0;
}

// -----------------------------------------------------------

