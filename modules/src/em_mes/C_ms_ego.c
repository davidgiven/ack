/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <em.h>
#include <em_mes.h>

void
C_ms_ego(hint, offs, siz, regno)
	int hint, regno;
	arith offs, siz;
{
	C_mes_begin(ms_ego);
	C_cst((arith)hint);
	C_cst(offs);
	C_cst(siz);
	C_cst((arith)regno);
	C_mes_end();
}
