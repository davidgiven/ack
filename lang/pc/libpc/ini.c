/* $Header$ */
/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/* Author: J.W. Stevenson */

#include        <pc_file.h>
#include        <pc_err.h>

extern          (*_sig())();
extern          _catch();
#ifndef CPM
extern int      gtty();
#endif

char            *_hbase;
int             *_extfl;
char            *_m_lb;         /* LB of m_a_i_n */
struct file     *_curfil;       /* points to file struct in case of errors */
int             _pargc;
char            **_pargv;
char            **_penvp;

_ini(args,hb,p,mainlb) char *args,*hb,*mainlb; int *p; {
	struct file *f;
	char buf[128];

	_pargc= *(int *)args; args += sizeof (int);
	_pargv= *(char ***)args; args += sizeof (char **);
	_penvp= *(char ***)args;
	_sig(_catch);
	_extfl = p;
	_hbase = hb;
	_m_lb = mainlb;
	if (_extfl[1] != -1) {
		f = EXTFL(1);
		f->ptr = f->bufadr;
		f->flags = MAGIC|TXTBIT;
		f->fname = "INPUT";
		f->ufd = 0;
		f->size = 1;
		f->count = 0;
		f->buflen = 512;
	}
	if (_extfl[2] != -1) {
		f = EXTFL(2);
		f->ptr = f->bufadr;
		f->flags = MAGIC|TXTBIT|WRBIT|EOFBIT|ELNBIT;
		f->fname = "OUTPUT";
		f->ufd = 1;
		f->size = 1;
#ifdef CPM
		f->count = 1;
#else
		f->count = (gtty(1,buf) >= 0 ? 1 : 512);
#endif
		f->buflen = f->count;
	}
}
