/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include	<system.h>
#include	"alloc.h"

No_Mem()
{
	sys_write(STDERR, "Out of memory\n", 14);
	sys_stop(S_EXIT);
}
