/* EM CODE OUTPUT ROUTINES */

#include <system.h>
#include "em_private.h"

/*
	putbyte(), C_open() and C_close() are the basic routines for
	respectively write on, open and close the output file.
	The put_*() functions serve as formatting functions of the
	various EM language constructs.
	See "Description of a Machine Architecture for use with
	Block Structured Languages" par. 11.2 for the meaning of these
	names.
*/

static File *ofp = 0;

C_open(nm)	/* open file for readable code output	*/
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

put_ilb(l)
	label l;
{
	_prnt("*%ld", (long) l);
}

extern char em_mnem[][4];
extern char em_pseu[][4];

put_op(x)
{
	_prnt(" %s ", em_mnem[x - sp_fmnem]);
}

put_cst(l)
	arith l;
{
	_prnt("%ld", (long) l);
}

put_scon(x, y)
	char *x;
	arith y;
{
	char buf[1024];
	char sbuf[1024];
	register char *p, *q = &sbuf[0];
	char *bts2str();

	p = bts2str(x, (int) y, buf);
	while (*p) {
		if (*p == '\'')
			*q++ = '\\';
		*q++ = *p++;
	}
	*q = '\0';
	_prnt("'%s'", buf);
}

put_ps(x)
{
	_prnt(" %s ", em_pseu[x - sp_fpseu]);
}

put_dlb(l)
	label l;
{
	_prnt(".%ld", (long) l);
}

put_doff(l, v)
	label l;
	arith v;
{
	if (v == 0) put_dlb(l);
	else _prnt(".%ld+%ld", (long) l, (long) v);
}

put_noff(s, v)
	char *s;
	arith v;
{
	if (v == 0) _prnt(s);
	else _prnt("%s+%ld", s, (long) v);
}

put_pnam(s)
	char *s;
{
	_prnt("$%s", s);
}

put_dfilb(l)
	label l;
{
	_prnt("%ld", (long) l);
}

put_wcon(sp, v, sz)	/* sp_icon, sp_ucon or sp_fcon with int repr	*/
	int sp;
	char *v;
	arith sz;
{
	_prnt("%s%c%ld", v, sp == sp_icon ? 'I' : sp == sp_ucon ? 'U' : 'F',
		(long) sz);
}

_prnt(fmt, args)
	char *fmt;
	int args;
{
	doprnt(ofp, fmt, (int *)&args);
}

put_nl() { _prnt("\n"); }
put_comma() { _prnt(","); }
put_ccend() { _prnt(" ?"); }
