/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <system.h>

C_failed()
{
	sys_write(STDERR,"write failed\n",13);
	sys_stop(S_EXIT);
}
