#include <signal.h>
foo()
{
	printf("signal received\n");
}

main()
{
	signal(SIGINT,foo);
	while(1);
}
