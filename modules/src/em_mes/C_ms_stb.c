/* $Id$ */
/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <em.h>
#include <em_mes.h>

void
C_ms_stb_cst(s, e1, e2, e3)
	char	*s;
	int	e1, e2;
	arith	e3;
{
	C_mes_begin(ms_stb);
	if (s) C_scon(s, (arith) (strlen(s)+1));
	C_cst((arith)e1);
	C_cst((arith)e2);
	C_cst(e3);
	C_mes_end();
}

void
C_ms_stb_dlb(s, e1, e2, l, off)
	char	*s;
	int	e1, e2;
	label	l;
	arith	off;
{
	C_mes_begin(ms_stb);
	if (s) C_scon(s, (arith) (strlen(s)+1));
	C_cst((arith)e1);
	C_cst((arith)e2);
	C_dlb(l, off);
	C_mes_end();
}

void
C_ms_stb_dnam(s, e1, e2, l, off)
	char	*s;
	int	e1, e2;
	char	*l;
	arith	off;
{
	C_mes_begin(ms_stb);
	if (s) C_scon(s, (arith) (strlen(s)+1));
	C_cst((arith)e1);
	C_cst((arith)e2);
	C_dnam(l, off);
	C_mes_end();
}

void
C_ms_stb_ilb(s, e1, e2, l)
	char	*s;
	int	e1, e2;
	label	l;
{
	C_mes_begin(ms_stb);
	if (s) C_scon(s, (arith) (strlen(s)+1));
	C_cst((arith)e1);
	C_cst((arith)e2);
	C_ilb(l);
	C_mes_end();
}

void
C_ms_stb_pnam(s, e1, e2, p)
	char	*s;
	int	e1, e2;
	char	*p;
{
	C_mes_begin(ms_stb);
	if (s) C_scon(s, (arith) (strlen(s)+1));
	C_cst((arith)e1);
	C_cst((arith)e2);
	C_pnam(p);
	C_mes_end();
}
