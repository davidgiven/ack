/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <signal.h>

char rcs_id[] = "$Id$" ;

foo()
{
	printf("signal received\n");
}

main()
{
	signal(SIGINT,foo);
	while(1);
}
