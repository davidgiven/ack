/* $Header$ */
/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <em.h>
#include <em_mes.h>

C_ms_lin(l)
	int l;
{
	C_mes_begin(ms_lin);
	C_cst((arith) l);
	C_mes_end();
}
