#include <signal.h>

char rcs_id[] = "$Header$" ;

foo()
{
	printf("signal received\n");
}

main()
{
	signal(SIGINT,foo);
	while(1);
}
