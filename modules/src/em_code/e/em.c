/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* EM CODE OUTPUT ROUTINES */

#include <system.h>
#include "em_private.h"

/*
	putbyte(), C_open() and C_close() are the basic routines for
	respectively write on, open and close the outpt file.
	The C_pt_*() functions serve as formatting functions of the
	various EM language constructs.
	See "Description of a Machine Architecture for use with
	Block Structured Languages" par. 11.2 for the meaning of these
	names.
*/

static File *ofp = 0;
static char obuf[BUFSIZ];
static char *opp = obuf;

static
flush() {
	if (sys_write(ofp, &obuf[0], opp - &obuf[0]) == 0) {
		C_failed();
	}
	opp = &obuf[0];
}

#define Xputbyte(c)	if (opp == &obuf[BUFSIZ]) flush(); *opp++ = (c)

static
C_putbyte(b)
	int b;
{
	Xputbyte(b);
}

C_init(w, p)
	arith w, p;
{
}

C_open(nm)	/* open file for readable code outpt	*/
	char *nm;
{
	if (nm == 0)
		ofp = STDOUT;	/* standard outpt	*/
	else
	if (sys_open(nm, OP_WRITE, &ofp) == 0)
		return 0;
	return 1;
}

C_close()
{
	if (opp != obuf) flush();
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
}


/***    the readable code generating routines	***/

static char buf[512];

static
wrs(s)
	register char *s;
{
	while (*s) C_putbyte(*s++);
}

C_pt_dnam(s)
	char *s;
{
	wrs(s);
}

C_pt_ilb(l)
	label l;
{
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
	sprint(buf, "%ld", (long) l);
	wrs(buf);
}

C_pt_scon(x, y)
	char *x;
	arith y;
{
	char buf[1024];
	char sbuf[1024];
	register char *p, *q = &sbuf[0];
	char *bts2str();

	C_putbyte('\'');
	p = bts2str(x, (int) y, buf);
	while (*p) {
		if (*p == '\'')
			C_putbyte('\\');
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
	sprint(buf, ".%ld", (long) l);
	wrs(buf);
}

C_pt_doff(l, v)
	label l;
	arith v;
{
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
	sprint(buf, "%ld", (long) l);
	wrs(buf);
}

C_pt_wcon(sp, v, sz)	/* sp_icon, sp_ucon or sp_fcon with int repr	*/
	int sp;
	char *v;
	arith sz;
{
	wrs(v);
	C_putbyte(sp == sp_icon ? 'I' : sp == sp_ucon ? 'U' : 'F');
	C_pt_cst(sz);
}

C_pt_nl() { C_putbyte('\n'); }
C_pt_comma() { C_putbyte(','); }
C_pt_ccend() { wrs(" ?"); }
