#include "lib.h"

int peek(int addr)
{
	/* this can not work properly for machines in which the 
	   POINTERSIZE differs from the integer size
	*/
	char* p;
	int i;

	p = (char*)addr;
	i = *p;
#ifdef DEBUG
	printf("peek %d = %d\n", addr, i);
#endif
	return (i);
}

void _poke(int i, int j)
{
	char* p;
	p = (char*)i;
	*p = j;
}
