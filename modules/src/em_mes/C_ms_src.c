/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <em.h>
#include <em_mes.h>

void
C_ms_src(nlines, filnam)
	int nlines;
	char *filnam;
{
	C_mes_begin(ms_src);
	C_cst((arith)nlines);
	C_scon(filnam, (arith)(strlen(filnam) + 1));
	C_mes_end();
}
