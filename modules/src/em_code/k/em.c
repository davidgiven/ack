/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* EM CODE OUTPUT ROUTINES */

#include <system.h>
#include "em_private.h"

#define	put8(x)		putbyte(x)
#define	put16(x)	put8((int) x); put8((int) (x >> 8))
#define	put32(x)	put16((int) x); put16((int) (x >> 16))

/*
	putbyte(), C_open() and C_close() are the basic routines for
	respectively write on, open and close the output file.
	The put_*() functions serve as formatting functions of the
	various EM language constructs.
	See "Description of a Machine Architecture for use with
	Block Structured Languages" par. 11.2 for the meaning of these
	names.
*/

/* supply a kind of buffered output */

static char obuf[BUFSIZ];
static char *opp = &obuf[0];
static File *ofp = 0;

static
flush() {
	if (sys_write(ofp, &obuf[0], opp - &obuf[0]) == 0) {
		C_failed();
	}
	opp = &obuf[0];
}

#define Xputbyte(c)	if (opp == &obuf[BUFSIZ]) flush(); *opp++ = (c)

putbyte(b)
	int b;
{
	Xputbyte(b);
}

#define putbyte(c) Xputbyte(c)

C_init(w, p)
	arith w, p;
{
}

C_open(nm)	/* open file for compact code output	*/
	char *nm;
{
	if (nm == 0)
		ofp = STDOUT;	/* standard output	*/
	else
	if (sys_open(nm, OP_WRITE, &ofp) == 0)
		return 0;
	return 1;
}

C_close()
{
	if (opp != &obuf[0]) flush();
	if (ofp != STDOUT)
		sys_close(ofp);
	ofp = 0;
}

C_busy()
{
	return ofp != 0; /* true if code is being generated */
}

C_magic()
{
	put16(sp_magic);
}

/***    the compact code generating routines	***/
#define	fit16i(x)	((x) >= (long)0xFFFF8000 && (x) <= (long)0x00007FFF)
#define	fit8u(x)	((x) <= 0xFF)		/* x is already unsigned */

put_ilb(l)
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

put_dlb(l)
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

put_cst(l)
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

put_doff(l, v)
	label l;
	arith v;
{
	if (v == 0) {
		put_dlb(l);
	}
	else	{
		put8(sp_doff);
		put_dlb(l);
		put_cst(v);
	}
}

put_noff(s, v)
	char *s;
	arith v;
{
	if (v == 0) {
		put_dnam(s);
	}
	else	{
		put8(sp_doff);
		put_dnam(s);
		put_cst(v);
	}
}

put_dnam(s)
	char *s;
{
	put8(sp_dnam);
	put_str(s);
}

put_pnam(s)
	char *s;
{
	put8(sp_pnam);
	put_str(s);
}

put_wcon(sp, v, sz)	/* sp_icon, sp_ucon or sp_fcon with int repr	*/
	int sp;
	char *v;
	arith sz;
{
	/* how 'bout signextension int --> long ???	*/
	put8(sp);
	put_cst(sz);
	put_str(v);
}

put_str(s)
	register char *s;
{
	register int len;

	put_cst((arith) (len = strlen(s)));
	while (--len >= 0) {
		put8(*s++);
	}
}

put_scon(b, n)
	register char *b;
	register arith n;
{
	put8(sp_scon);
	put_cst(n);
	while (--n >= 0) {
		put8(*b++);
	}
}
