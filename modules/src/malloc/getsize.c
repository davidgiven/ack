/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	find out if a pointer-sized integer, preferably unsigned,
	must be declared as an unsigned int or a long
*/

#include <stdio.h>

main()
{
	if (sizeof(unsigned int) == sizeof(char *)) {
		puts("typedef unsigned int size_type;");
		return 0;
	}
	if (sizeof(long) == sizeof(char *)) {
		puts("typedef long size_type;");
		return 0;
	}
	fputs(stderr, "funny pointer size\n");
	return 1;
}
