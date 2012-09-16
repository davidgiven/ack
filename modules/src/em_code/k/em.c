/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* EM CODE OUTPUT ROUTINES */

#include "io.c"
#include "em_private.h"

#define	put8(x)		C_putbyte(x)
#define	put16(x)	put8((int) x); put8((int) (x >> 8))
#define	put32(x)	put16((int) x); put16((int) (x >> 16))

/*
	The C_pt_*() functions serve as formatting functions of the
	various EM language constructs.
	See "Description of a Machine Architecture for use with
	Block Structured Languages" par. 11.2 for the meaning of these
	names.
*/

C_magic()
{
	put16(sp_magic);
}

/***    the compact code generating routines	***/
#define	fit16i(x)	((x) >= (long)(-0x8000) && (x) <= (long)0x7FFF)
#define	fit8u(x)	((x) <= 0xFF)		/* x is already unsigned */

C_pt_ilb(l)
	register label l;
{
	if (fit8u(l))	{
		put8(sp_ilb1);
		put8(l);
	}
	else	{
		put8(sp_ilb2);
		put16(l);
	}
}

C_pt_dlb(l)
	register label l;
{
	if (fit8u(l))	{
		put8(sp_dlb1);
		put8(l);
	}
	else	{
		put8(sp_dlb2);
		put16((int)l);
	}
}

C_pt_cst(l)
	register arith l;
{
	if (l >= (arith) -sp_zcst0 && l < (arith) (sp_ncst0 - sp_zcst0)) {
		/*	we can convert 'l' to an int because its value
			can be stored in a byte.
		*/
		put8((int)l + (sp_zcst0 + sp_fcst0));
	}
	else
	if (fit16i(l)) { /* the cast from long to int causes no trouble here */
		put8(sp_cst2);
		put16((int) l);
	}
	else	{
		put8(sp_cst4);
		put32(l);
	}
}

C_pt_doff(l, v)
	label l;
	arith v;
{
	if (v == 0) {
		C_pt_dlb(l);
	}
	else	{
		put8(sp_doff);
		C_pt_dlb(l);
		C_pt_cst(v);
	}
}

C_pt_noff(s, v)
	char *s;
	arith v;
{
	if (v == 0) {
		C_pt_dnam(s);
	}
	else	{
		put8(sp_doff);
		C_pt_dnam(s);
		C_pt_cst(v);
	}
}

C_pt_dnam(s)
	char *s;
{
	put8(sp_dnam);
	C_pt_str(s);
}

C_pt_pnam(s)
	char *s;
{
	put8(sp_pnam);
	C_pt_str(s);
}

C_pt_wcon(sp, v, sz)	/* sp_icon, sp_ucon or sp_fcon with int repr	*/
	int sp;
	char *v;
	arith sz;
{
	/* how 'bout signextension int --> long ???	*/
	put8(sp);
	C_pt_cst(sz);
	C_pt_str(v);
}

C_pt_str(s)
	register char *s;
{
	register int len;

	C_pt_cst((arith) (len = strlen(s)));
	while (--len >= 0) {
		put8(*s++);
	}
}

C_pt_scon(b, n)
	register char *b;
	register arith n;
{
	put8(sp_scon);
	C_pt_cst(n);
	while (--n >= 0) {
		put8(*b++);
	}
}
