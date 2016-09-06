/*	$Id: cpp.h,v 1.107 2016/08/10 17:33:23 ragge Exp $	*/

/*
 * Copyright (c) 2004,2010 Anders Magnusson (ragge@ludd.luth.se).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>	/* for debug/printf */
#if LIBVMF
#include <vmf.h>
#endif

typedef unsigned char usch;

extern	int	trulvl;
extern	int	flslvl;
extern	int	elflvl;
extern	int	elslvl;
extern	int	dflag;
extern	int	tflag, Aflag, Cflag, Pflag;
extern	int	Mflag, dMflag, MPflag, MMDflag;
extern	char	*Mfile, *MPfile;
extern	int	defining, inclevel;

/* args for lookup() */
#define FIND    0
#define ENTER   1

/* buffer used internally */
#if SIZEOF_INT_P == 2 || LIBVMF
#define CPPBUF  1024
#else
#define CPPBUF	16384
#endif

#define	MAXARGS	128	/* Max # of args to a macro. Should be enough */
#define	MAXIDSZ	63	/* Max length of C99 identifier; 5.2.4.1 */

#define	PBMAX	10	/* min pushbackbuffer size */

#define	FUNLIKE	0
#define	CTRLOC	1	/* __COUNTER__ */
#define	DEFLOC	2	/* defined */
#define	PRAGLOC	3	/* _Pragma */
#define	LINLOC	4	/* __LINE__ */
#define	FILLOC	5	/* __FILE__ */
#define GCCARG	6	/* has gcc varargs that may be replaced with 0 */
#define VARG	7	/* has varargs */
#define OBJCT	8

#define WARN	1	/* SOH, not legal char */
#define CONC	2	/* STX, not legal char */
#define SNUFF	3	/* ETX, not legal char */
#define	BLKID	4	/* EOT, not legal char */

/* Used in macro expansion */
#define RECMAX	10000			/* max # of recursive macros */
#define	MKB(l,h)	(l+((h)<<8))

/* quick checks for some characters */
#define C_SPEC	0001		/* for fastscan() parsing */
#define C_2	0002		/* for yylex() tokenizing */
#define C_WSNL	0004		/* ' ','\t','\r','\n' */
#define C_ID	0010		/* [_a-zA-Z0-9] */
#define C_ID0	0020		/* [_a-zA-Z] */
#define C_Q	0040		/* [\r\\\?] */
#define C_DIGIT	0100		/* [0-9] */
#define C_HEX	0200		/* [0-9a-fA-F] */

extern usch spechr[];

#define ISWSNL(x)	(spechr[x] & (C_WSNL))
#define ISWS(x)		((x) == '\t' || (x) == ' ')
#define ISID(x)		(spechr[x] & C_ID)
#define ISID0(x)	(spechr[x] & C_ID0)
#define	ISDIGIT(x)	(spechr[x] & C_DIGIT)
#define	ISCQ(x)		(spechr[x] & C_Q)

/* buffer definition */
#define	BNORMAL	0	/* standard buffer */
#define	BMAC	1	/* store macro definitions */
#define	BINBUF	2	/* read data from input files */
#define	BUTBUF	3	/* write data to stdout */
struct iobuf {
	usch *buf;
	int cptr;	/* current pointer */
	int bsz;	/* bufsize */
	int ro:1, inuse:1, type:4;
};
struct iobuf *getobuf(int);
void putob(struct iobuf *ob, int ch);
void bufree(struct iobuf *iob);
extern struct iobuf pb;

#define	curptr	ib->cptr
#define	maxread	ib->bsz
#define	buffer	ib->buf+PBMAX
#define	bbuf	ib->buf

#if LIBVMF
extern struct vspace ibspc;
#endif

/*
 * definition for include file info
 */
struct includ {
	struct includ *next;
	const usch *fname;	/* current fn, changed if #line found */
	const usch *orgfn;	/* current fn, not changed */
	int lineno;
	int escln;		/* escaped newlines, to be added */
	int infil;
	struct iobuf *ib;
	int idx;
	void *incs;
	const usch *fn;
#if LIBVMF
	struct vseg *vseg;
#endif
};
#define INCINC 0
#define SYSINC 1

extern struct includ *ifiles;

/* Symbol table entry  */
struct symtab {
	const usch *namep;
	const usch *value;
	const usch *file;
	int line;
	unsigned char type:4,	/* macro type */
		      wraps:1;	/* macro wraps in buffer */
	unsigned char narg;	/* # of args (if feasible) */
};

/*
 * Struct used in parse tree evaluation.
 * op is one of:
 *	- number type (NUMBER, UNUMBER)
 *	- zero (0) if divided by zero.
 */
struct nd {
	int op;
	union {
		long long val;
		unsigned long long uval;
	} n;
};
extern struct nd yynode;

#define nd_val n.val
#define nd_uval n.uval

enum { NUMBER = 257, UNUMBER, LS, RS, EQ, NE, STRING, WSPACE, CMNT, IDENT,
	OROR, ANDAND, DEFINED, LE, GE };

#define	SLO_IGNOREWS	001

struct symtab *lookup(const usch *namep, int enterf);
struct blocker;
struct iobuf *submac(struct symtab *nl, int, struct iobuf *, struct blocker *);
struct iobuf *kfind(struct symtab *nl);
void ppdir(void);

void define(void);
void include(void);
void include_next(void);
void line(void);

int pushfile(const usch *fname, const usch *fn, int idx, void *incs);
void prtline(int nl);
int yylex(void);
void cunput(int);
int yyparse(void);
void putch(int);
void putstr(const usch *s);
usch *sheap(const char *fmt, ...);
struct iobuf *bsheap(struct iobuf *, const char *fmt, ...);
struct iobuf *strtobuf(const usch *str, struct iobuf *iob);
struct iobuf *buftobuf(struct iobuf *in, struct iobuf *iob);
void warning(const char *fmt, ...);
void error(const char *fmt, ...);
int cinput(void);
int inc2(void);
void Ccmnt2(void (*d)(int), int);
usch *bufid(int ch, struct iobuf *);
usch *readid(int ch);
struct iobuf *faststr(int bc, struct iobuf *);
int fastnum(int ch, struct iobuf *);
void *xrealloc(void *p, int sz);
void *xmalloc(int sz);
void fastscan(void);
void cntline(void);
struct iobuf *savln(void);
