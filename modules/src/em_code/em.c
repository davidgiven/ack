/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* EM CODE OUTPUT ROUTINES */

/*	I/O part of em_code module.
	Also contains C_open, C_close
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "alloc.h"
#include "em_arith.h"
#include "insert.h"
#include "em_private.h"

int		C_ontmpfile = 0;
int		C_sequential = 1;
Part		*C_curr_part;
int		(*C_outpart)(), (*C_swtout)(), (*C_swttmp)();

#ifdef INCORE
char		*C_BASE;
#endif

File		*C_ofp;

#ifndef INCORE
File		*C_tfr;
char		*C_tmpfile;
char		*strcpy(), *strcat();
char		*C_ibuf = 0;
long		C_current_out;
#endif

#if BUFSIZ <= 1024 && BIGMACHINE
#define BUFFERSIZ	8*BUFSIZ
#else
#define BUFFERSIZ	BUFSIZ
#endif

static char	obuf[BUFFERSIZ];
char		*C_top = &obuf[BUFFERSIZ];
#ifdef INCORE
char		*C_current_out = obuf;
#else
char		*C_opp = obuf;
#endif

void
C_flush() {
#ifdef INCORE
	static unsigned int bufsiz;

	if (C_ontmpfile) {
		if (C_BASE == 0) {
			C_BASE = Malloc(BUFFERSIZ);
			bufsiz = BUFFERSIZ;
			C_current_out = C_BASE;
		}
		else {
			C_BASE = Srealloc(C_BASE, (bufsiz << 1));
			C_current_out = C_BASE + bufsiz;
			bufsiz <<= 1;
		}
		C_top = C_BASE + bufsiz;
		return;
	}
#endif
	if (C_opp != obuf && sys_write(C_ofp, obuf, (int)(C_opp - obuf)) == 0) {
		C_ofp = 0;
		C_failed();
	}
	C_opp = obuf;
}

#ifndef INCORE
#define Xputbyte(c) if (C_ontmpfile) C_current_out++; put(c)
#else
#define Xputbyte(c) put(c)
#endif

void
C_putbyte(c)
	int c;
{
	Xputbyte(c);
}

#if BIGMACHINE
#define C_putbyte Xputbyte
#endif

/*ARGSUSED*/
void
C_init(w, p)
	arith w, p;
{
}

int
C_open(nm)
	char *nm;
{
	/*	Open file "nm" for output
	*/

	if (nm == 0)
		C_ofp = STDOUT;	/* standard output	*/
	else
	if (sys_open(nm, OP_WRITE, &C_ofp) == 0)
		return 0;
	return 1;
}

void
C_close()
{
	/*	Finish the code-generation.
	*/

#ifndef INCORE
	C_flush();
	if (C_tmpfile) {
		(*C_swttmp)();
		sys_close(C_ofp);
#else
	if (C_BASE) {
#endif
		if (C_curr_part) {
			C_curr_part->p_parts->pp_end = C_current_out - C_BASE;
		}
		(*C_swtout)();
		if (! C_sequential) {
			(*C_outpart)(0);
		}
#ifndef INCORE
		sys_close(C_tfr);
		sys_remove(C_tmpfile);
		if (C_ibuf) free(C_ibuf);
#else
		free(C_BASE);
#endif
	}
	C_flush();
	if (C_ofp != STDOUT)
		sys_close(C_ofp);
	C_ofp = 0;
}

int
C_busy()
{
	return C_ofp != 0; /* true if code is being generated */
}

#ifdef READABLE_EM
/*
	The C_pt_*() functions serve as formatting functions of the
	various EM language constructs.
	See "Description of a Machine Architecture for use with
	Block Structured Languages" par. 11.2 for the meaning of these
	names.
*/

void
C_magic()
{
}

/***    the readable code generating routines	***/

static
wrs(s)
	register char *s;
{
	while (*s) {
		C_putbyte(*s++);
	}
}

void
C_pt_dnam(s)
	char *s;
{
	wrs(s);
}

void
C_pt_ilb(l)
	label l;
{
	char buf[16];

	sprint(buf, "*%ld", (long) l);
	wrs(buf);
}

extern char em_mnem[][4];
extern char em_pseu[][4];

void
C_pt_op(x)
{
	C_putbyte(' ');
	wrs(em_mnem[x - sp_fmnem]);
	C_putbyte(' ');
}

void
C_pt_cst(l)
	arith l;
{
	char buf[16];

	sprint(buf, "%ld", (long) l);
	wrs(buf);
}

void
C_pt_scon(x, y)
	char *x;
	arith y;
{
	char xbuf[1024];
	register char *p;
	char *bts2str();

	C_putbyte('\'');
	p = bts2str(x, (int) y, xbuf);
	while (*p) {
		if (*p == '\'') {
			C_putbyte('\\');
		}
		C_putbyte(*p++);
	}
	C_putbyte('\'');
}

void
C_pt_ps(x)
{
	C_putbyte(' ');
	wrs(em_pseu[x - sp_fpseu]);
	C_putbyte(' ');
}

void
C_pt_dlb(l)
	label l;
{
	char buf[16];

	sprint(buf, ".%ld", (long) l);
	wrs(buf);
}

void
C_pt_doff(l, v)
	label l;
	arith v;
{
	char buf[16];

	C_pt_dlb(l);
	if (v != 0) {
		sprint(buf,"+%ld", (long) v);
		wrs(buf);
	}
}

void
C_pt_noff(s, v)
	char *s;
	arith v;
{
	char buf[16];

	wrs(s);
	if (v != 0) {
		sprint(buf,"+%ld", (long) v);
		wrs(buf);
	}
}

void
C_pt_pnam(s)
	char *s;
{
	C_putbyte('$');
	wrs(s);
}

void
C_pt_dfilb(l)
	label l;
{
	char buf[16];

	sprint(buf, "%ld", (long) l);
	wrs(buf);
}

void
C_pt_wcon(sp, v, sz)	/* sp_icon, sp_ucon or sp_fcon with int repr	*/
	int sp;
	char *v;
	arith sz;
{
	int ch = sp == sp_icon ? 'I' : sp == sp_ucon ? 'U' : 'F';

	wrs(v);
	C_putbyte(ch);
	C_pt_cst(sz);
}

void
C_pt_nl() {
	C_putbyte('\n');
}

void
C_pt_comma() {
	C_putbyte(',');
}

void
C_pt_ccend() { 
	C_putbyte('?');
}

#else /* READABLE_EM */

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

void
C_magic()
{
	put16(sp_magic);
}

/***    the compact code generating routines	***/
#define	fit16i(x)	((x) >= (long)(-0x8000) && (x) <= (long)0x7FFF)
#define	fit8u(x)	((x) <= 0xFF)		/* x is already unsigned */

void
C_pt_ilb(l)
	register label l;
{
	if (fit8u(l))	{
		put8(sp_ilb1);
		put8((int)l);
	}
	else	{
		put8(sp_ilb2);
		put16((int)l);
	}
}

void
C_pt_dlb(l)
	register label l;
{
	if (fit8u(l))	{
		put8(sp_dlb1);
		put8((int)l);
	}
	else	{
		put8(sp_dlb2);
		put16((int)l);
	}
}

void
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

void
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

void
C_pt_str(s)
	register char *s;
{
	register int len;

	C_pt_cst((arith) (len = strlen(s)));
	while (--len >= 0) {
		put8(*s++);
	}
}

void
C_pt_dnam(s)
	char *s;
{
	put8(sp_dnam);
	C_pt_str(s);
}

void
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

void
C_pt_pnam(s)
	char *s;
{
	put8(sp_pnam);
	C_pt_str(s);
}

void
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

void
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
#endif /* READABLE_EM */
