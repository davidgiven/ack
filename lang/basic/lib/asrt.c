#include <stdio.h>
#include <stdlib.h>

void asrt(int b)
{
	if(!b){
		printf("ASSERTION ERROR\n");
		abort();
	}
}
