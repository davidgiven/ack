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
	puts("#ifndef size_type");
	if (sizeof(unsigned int) == sizeof(char *)) {
		puts("#define size_type unsigned int");
	}
	else if (sizeof(long) == sizeof(char *)) {
		puts("#define size_type long");
	}
	else {
		fputs("funny pointer size\n", stderr);
		exit(1);
	}
	puts("#endif");
	exit(0);
}
