/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* EM CODE OUTPUT ROUTINES */

#include "io.c"
#include "em_private.h"

/*
	The C_pt_*() functions serve as formatting functions of the
	various EM language constructs.
	See "Description of a Machine Architecture for use with
	Block Structured Languages" par. 11.2 for the meaning of these
	names.
*/

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

C_pt_dnam(s)
	char *s;
{
	wrs(s);
}

C_pt_ilb(l)
	label l;
{
	char buf[16];

	sprint(buf, "*%ld", (long) l);
	wrs(buf);
}

extern char em_mnem[][4];
extern char em_pseu[][4];

C_pt_op(x)
{
	C_putbyte(' ');
	wrs(em_mnem[x - sp_fmnem]);
	C_putbyte(' ');
}

C_pt_cst(l)
	arith l;
{
	char buf[16];

	sprint(buf, "%ld", (long) l);
	wrs(buf);
}

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

C_pt_ps(x)
{
	C_putbyte(' ');
	wrs(em_pseu[x - sp_fpseu]);
	C_putbyte(' ');
}

C_pt_dlb(l)
	label l;
{
	char buf[16];

	sprint(buf, ".%ld", (long) l);
	wrs(buf);
}

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

C_pt_pnam(s)
	char *s;
{
	C_putbyte('$');
	wrs(s);
}

C_pt_dfilb(l)
	label l;
{
	char buf[16];

	sprint(buf, "%ld", (long) l);
	wrs(buf);
}

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

C_pt_nl() { C_putbyte('\n'); }
C_pt_comma() { C_putbyte(','); }
C_pt_ccend() { C_putbyte('?'); }
