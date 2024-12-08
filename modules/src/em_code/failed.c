/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <system.h>

void C_failed(void)
{
	fputs("read, write, or open failed\n", stderr);
	exit(1);
}
