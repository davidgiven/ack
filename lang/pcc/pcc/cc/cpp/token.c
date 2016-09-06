/*	$Id: token.c,v 1.182 2016/08/10 17:33:23 ragge Exp $	*/

/*
 * Copyright (c) 2004,2009 Anders Magnusson. All rights reserved.
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

/*
 * Tokenizer for the C preprocessor.
 * There are three main routines:
 *	- fastscan() loops over the input stream searching for magic
 *		characters that may require actions.
 *	- yylex() returns something from the input stream that
 *		is suitable for yacc.
 *
 *	Other functions of common use:
 *	- inpch() returns a raw character from the current input stream.
 *	- inch() is like inpch but \\n and trigraphs are expanded.
 *	- unch() pushes back a character to the input stream.
 *
 * Input data can be read from either stdio or a buffer.
 * If a buffer is read, it will return EOF when ended and then jump back
 * to the previous buffer.
 *	- setibuf(usch *ptr). Buffer to read from, until NULL, return EOF.
 *		When EOF returned, pop buffer.
 *	- setobuf(usch *ptr).  Buffer to write to
 *
 * There are three places data is read:
 *	- fastscan() which has a small loop that will scan over input data.
 *	- flscan() where everything is skipped except directives (flslvl)
 *	- inch() that everything else uses.
 *
 * 5.1.1.2 Translation phases:
 *	1) Convert UCN to UTF-8 which is what pcc uses internally (chkucn).
 *	   Remove \r (unwanted)
 *	   Convert trigraphs (chktg)
 *	2) Remove \\\n.  Need extra care for identifiers and #line.
 *	3) Tokenize.
 *	   Remove comments (fastcmnt)
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>

#include "compat.h"
#include "cpp.h"

static void cvtdig(usch **);
static int dig2num(int);
static int charcon(usch **);
static void elsestmt(void);
static void ifdefstmt(void);
static void ifndefstmt(void);
static void endifstmt(void);
static void ifstmt(void);
static void cpperror(void);
static void cppwarning(void);
static void undefstmt(void);
static void pragmastmt(void);
static void elifstmt(void);

static void unch(int c);

#define	UNCH(ib, ch)	ib->buf[--ib->cptr] = ch
/* protection against recursion in #include */
#define MAX_INCLEVEL	100
int inclevel;
int incmnt, instr;
extern int skpows;

struct includ *ifiles;

static void ucn(int n);
static void fastcmnt2(int);
static int chktg2(int ch);

/* some common special combos for init */
#define C_NL	(C_SPEC|C_WSNL)
#define C_DX	(C_SPEC|C_ID|C_DIGIT|C_HEX)
#define C_I	(C_SPEC|C_ID|C_ID0)
#define C_IX	(C_SPEC|C_ID|C_ID0|C_HEX)

usch spechr[256] = {
	C_SPEC|C_Q, 0,	0,	0,	C_SPEC,	C_SPEC,	0,	0,
	0,	C_WSNL,	C_NL,	0,	0,	C_WSNL|C_Q, 0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,

	C_WSNL,	C_2,	C_SPEC,	0,	0,	0,	C_2,	C_SPEC,
	0,	0,	0,	C_2,	0,	C_2,	0,	C_SPEC|C_Q,
	C_DX,	C_DX,	C_DX,	C_DX,	C_DX,	C_DX,	C_DX,	C_DX,
	C_DX,	C_DX,	0,	0,	C_2,	C_2,	C_2,	C_SPEC|C_Q,

	0,	C_IX,	C_IX,	C_IX,	C_IX,	C_IX,	C_IX,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	0,	C_SPEC|C_Q, 0,	0,	C_I,

	0,	C_IX,	C_IX,	C_IX,	C_IX,	C_IX,	C_IX,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	0,	C_2,	0,	0,	0,

/* utf-8 */
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,

	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,

	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,

	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,	C_I,
};

/*
 * fill up the input buffer
 * n tells how nany chars at least.  0 == standard.
 * 0 if EOF, != 0 if something could fill up buf.
 */
static int
inpbuf(int n)
{
	struct iobuf *ib = ifiles->ib;
	int len, sz = 0;

	if (n > 0) {
		if (ib->bsz > ib->cptr + n)
			return 1; /* enough in buffer */
		sz = ib->bsz - ib->cptr;
		memcpy(ib->buf+PBMAX - sz, ib->buf + ib->cptr, sz);
	}

	if (ifiles->infil == -1)
		return 0;
	len = read(ifiles->infil, ib->buf+PBMAX, CPPBUF-PBMAX);
	if (len == -1)
		error("read error on file %s", ifiles->orgfn);
	if (len > 0) {
		ib->cptr = PBMAX - sz;
		ib->bsz = PBMAX + len;
	}
	return len + sz;
}

/*
 * Return a quick-cooked character.
 * If buffer empty; return 0.
 */
static int
qcchar(void)
{
	struct iobuf *ib = ifiles->ib;
	int ch;

newone:	do {
		if (ib->cptr < ib->bsz) {
			if (!ISCQ(ch = ib->buf[ib->cptr++]))
				return ch;
			break;
		}
	} while ((ch = inpbuf(0)) > 0);

	switch (ch) {
	case 0:
		return 0; /* end of file */

	case '\r':
		goto newone;

	case '\\':
		if (ib->cptr == ib->bsz)
			inpbuf(0);
		switch (ch = ib->buf[ib->cptr]) {
		case 'u':
		case 'U': 
			if (incmnt) 
				return '\\';
			ib->cptr++;
			ucn(ch == 'u' ? 4 : 8);
			break;
		case '\r':
			ib->cptr++;
			if (ib->cptr == ib->bsz)
				inpbuf(0);
			/* FALLTHROUGH */
		case '\n':
			ib->cptr++;
			ifiles->escln++;
			break;
		default:
			return '\\';
		}
		goto newone;

	case '?':
		inpbuf(2);
		if (ib->buf[ib->cptr] == '?') {
			if ((ch = chktg2(ib->buf[ib->cptr+1])) == 0)
				return '?';
			ib->buf[++ib->cptr] = ch;
			goto newone;
		}
		return '?';

	case '/':
		if (Cflag || incmnt || instr)
			return '/';
		incmnt++;
		ch = qcchar();
		incmnt--;
		if (ch == '/' || ch == '*') {
			int n = ifiles->lineno;
			fastcmnt2(ch);
			if (n == ifiles->lineno)
				return ' ';
		} else {
			ib->buf[--ib->cptr] = ch;
			return '/';
		}
		goto newone;
	}
	error("ch error");
	return 0; /* XXX */
}

/*
 * push a character back to the input stream
 */
static void
unch(int c)
{
	if (c == -1)
		return;

	ifiles->ib->cptr--;
	if (ifiles->ib->cptr < 0)
		error("pushback buffer full");
	ifiles->ib->buf[ifiles->ib->cptr] = (usch)c;
}

/*
 * Return trigraph mapping char or 0.
 */
static int
chktg2(int ch)
{
	switch (ch) {
	case '=':  return '#';
	case '(':  return '[';
	case ')':  return ']';
	case '<':  return '{';
	case '>':  return '}';
	case '/':  return '\\';
	case '\'': return '^';
	case '!':  return '|';
	case '-':  return '~';
	}
	return 0;
}

/*
 * deal with comments in the fast scanner.
 */
static void
fastcmnt2(int ch)
{

	incmnt = 1;
	if (ch == '/') { /* C++ comment */
		while ((ch = qcchar()) != '\n')
			;
		unch(ch);
	} else if (ch == '*') {
		for (;;) {
			if ((ch = qcchar()) == 0)
				break;
			if (ch == '*') {
				if ((ch = qcchar()) == '/') {
					break;
				} else
					unch(ch);
			} else if (ch == '\n') {
				putch('\n');
				ifiles->lineno++;
			}
		}
	} else
		error("fastcmnt2");
	if (ch == 0)
		error("file ends in comment");
	incmnt = 0;
}

/*
 * check for universal-character-name on input, and
 * unput to the pushback buffer encoded as UTF-8.
 */
static void
ucn(int n)
{
	unsigned long cp, m;
	int ch;

	if (incmnt) {
		struct iobuf *ib = ifiles->ib;
		ib->cptr--; /* [uU] */
		ib->buf[--ib->cptr] = '\\';
		return;
	}

	cp = 0;
	while (n-- > 0) {
		if ((ch = qcchar()) == 0 || (spechr[ch] & C_HEX) == 0) {
			warning("invalid universal character name");
			// XXX should actually unput the chars and return 0
			unch(ch); // XXX eof
			break;
		}
		cp = cp * 16 + dig2num(ch);
	}

	if ((cp < 0xa0 && cp != 0x24 && cp != 0x40 && cp != 0x60)
	    || (cp >= 0xd800 && cp <= 0xdfff))	/* 6.4.3.2 */
		error("universal character name cannot be used");

	if (cp > 0x7fffffff)
		error("universal character name out of range");

	n = 0;
	m = 0x7f;
	while (cp > m) {
		unch(0x80 | (cp & 0x3f));
		cp >>= 6;
		m >>= (n++ ? 1 : 2);
	}
	unch(((m << 1) ^ 0xfe) | cp);
}

/*
 * deal with comments when -C is active.
 * Save comments in expanded macros???
 */
void
Ccmnt2(void (*d)(int), int ch)
{

	if (ch == '/') { /* C++ comment */
		d(ch);
		do {
			d(ch);
		} while ((ch = qcchar()) && ch != '\n');
		unch(ch);
	} else if (ch == '*') {
		d('/');
		d('*');
		for (;;) {
			ch = qcchar();
			d(ch);
			if (ch == '*') {
				if ((ch = qcchar()) == '/') {
					d(ch);
					break;
				} else
					unch(ch);
			} else if (ch == '\n') {
				ifiles->lineno++;
			}
		}
	}
}

/*
 * Traverse over spaces and comments from the input stream,
 * Returns first non-space character.
 */
static int
fastspc(void)
{
	int ch;

	while ((ch = qcchar()), ISWS(ch))
		;
	return ch;
}

/*
 * readin chars and store in buf. Warn about too long names.
 */
usch *
bufid(int ch, struct iobuf *ob)
{
	int n = ob->cptr;

	do {
		if (ob->cptr - n == MAXIDSZ)
			warning("identifier exceeds C99 5.2.4.1");
		if (ob->cptr < ob->bsz)
			ob->buf[ob->cptr++] = ch;
		else
			putob(ob, ch);
	} while (spechr[ch = qcchar()] & C_ID);
	ob->buf[ob->cptr] = 0; /* legal */
	unch(ch);
	return ob->buf+n;
}

usch idbuf[MAXIDSZ+1];
/*
 * readin chars and store in buf. Warn about too long names.
 */
usch *
readid(int ch)
{
	int p = 0;

	do {
		if (p == MAXIDSZ)
			warning("identifier exceeds C99 5.2.4.1, truncating");
		if (p < MAXIDSZ)
			idbuf[p] = ch;
		p++;
	} while (spechr[ch = qcchar()] & C_ID);
	idbuf[p] = 0;
	unch(ch);
	return idbuf;
}

/*
 * get a string or character constant and save it as given by d.
 */
struct iobuf *
faststr(int bc, struct iobuf *ob)
{
	struct iobuf *ib = ifiles->ib;
	int ch;

	if (ob == NULL)
		ob = getobuf(BNORMAL);

	instr = 1;
	putob(ob, bc);
	for (;;) {
		if (ib->bsz == ib->cptr)
			ch = qcchar();
		else if (ISCQ(ch = ib->buf[ib->cptr]))
			ch = qcchar();
		else
			ib->cptr++;
		switch (ch) {
		case '\\':
			putob(ob, ch);
			if (ib->cptr == ib->bsz)
				inpbuf(0);
			incmnt = 1;
			putob(ob, qcchar());
			incmnt = 0;
			continue;
		case '\n':
			warning("unterminated literal");
			instr = 0;
			unch(ch);
			return ob;
		}
		putob(ob, ch);
		if (ch == bc)
			break;
	}
	ob->buf[ob->cptr] = 0;
	instr = 0;
	return ob;
}

/*
 * get a preprocessing number and save it as given by ob.
 * returns first non-pp-number char.
 * We know that this is a valid number already.
 *
 *	pp-number:	digit
 *			. digit
 *			pp-number digit
 *			pp-number identifier-nondigit
 *			pp-number e sign
 *			pp-number E sign
 *			pp-number p sign
 *			pp-number P sign
 *			pp-number .
 */
int
fastnum(int ch, struct iobuf *ob)
{
	int c2;

	if ((spechr[ch] & C_DIGIT) == 0) {
		/* not digit, dot */
		putob(ob, ch);
		ch = qcchar();
	}
	for (;;) {
		putob(ob, ch);
		if ((ch = qcchar()) == 0)
			return 0;
		if ((c2 = (ch & 0337)) == 'E' || c2 == 'P') {
			if ((c2 = qcchar()) != '-' && c2 != '+') {
				if (c2 > 0)
					unch(c2);
				break;
			}
			putob(ob, ch);
			ch = c2;
		} else if (ch == '.' || (spechr[ch] & C_ID)) {
			continue;
		} else
			break;
	}
	return ch;
}

/*
 * Scan quickly the input file searching for:
 *	- '#' directives
 *	- keywords (if not flslvl)
 *	- comments
 *
 *	Handle strings, numbers and trigraphs with care.
 *	Only data from pp files are scanned here, never any rescans.
 *	This loop is always at trulvl.
 */
void
fastscan(void)
{
	struct iobuf *ob, rbs, *rb = &rbs;
	extern struct iobuf pb;
	struct iobuf *ib = ifiles->ib;
	struct symtab *nl;
	int ch, c2;
	usch *dp;

#define	IDSIZE	128
	rb->buf = xmalloc(IDSIZE+1);
	rb->cptr = 0;
	rb->bsz = IDSIZE;

	goto run;

	for (;;) {
		/* tight loop to find special chars */
		/* should use getchar/putchar here */
		for (;;) {
			if (ib->cptr < ib->bsz)
				ch = ib->buf[ib->cptr++];
			else
				ch = qcchar();
xloop:			if (ch < 0) ch = 0; /* XXX */
			if ((spechr[ch] & C_SPEC) != 0)
				break;
			putch(ch);
		}

		switch (ch) {
		case 0:
			free(rb->buf);
			return;

		case WARN:
		case CONC:
			error("bad char passed");
			break;

		case '/': /* Comments */
			ch = qcchar();
			if (ch  == '/' || ch == '*') {
				if (Cflag == 0) {
					int n = ifiles->lineno;
					fastcmnt2(ch);
					if (n == ifiles->lineno)
						putch(' '); /* 5.1.1.2 p3 */
				} else
					Ccmnt2(putch, ch);
			} else {
				putch('/');
				goto xloop;
			}
			break;

		case '\n': /* newlines, for pp directives */
			/* take care of leftover \n */
			while (ifiles->escln > 0) {
				putch('\n');
				ifiles->escln--;
				ifiles->lineno++;
			}
			putch('\n');
			ifiles->lineno++;

			/* search for a # */
run:			while ((ch = qcchar()) == '\t' || ch == ' ')
				putch(ch);
			if (ch == '%') {
				if ((c2 = qcchar()) != ':')
					unch(c2);
				else
					ch = '#';
			}
			if (ch  == '#')
				ppdir();
			else
				goto xloop;
			break;

		case '?':
			if (ib->cptr+1 >= ib->bsz)
				inpbuf(2);
			if (ib->buf[ib->cptr] == '?') {
				ib->cptr++;
				if ((ch = chktg2(ib->buf[ib->cptr++])))
					goto xloop;
				ib->cptr -= 2;
			}
			putch('?');
			break;

		case '\'': /* character constant */
			if (tflag) {
				putch(ch);
				break;	/* character constants ignored */
			}
			/* FALLTHROUGH */
		case '\"': /* strings */
			if (skpows)
				cntline();
			faststr(ch, &pb);
			break;

		case '.':  /* for pp-number */
			if ((spechr[c2 = qcchar()] & C_DIGIT) == 0) {
				putch('.');
				goto xloop;
			}
			unch(c2);
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			if (skpows)
				cntline();
			ch = fastnum(ch, &pb);
			goto xloop;

		case 'L':
		case 'U':
		case 'u':
			if (ib->cptr+2 >= ib->bsz)
				inpbuf(2);
			if ((c2 = ib->buf[ib->cptr]) == '\"' || c2 == '\'') {
				putch(ch);
				break;
			} else if (c2 == '8' && ch == 'u' &&
			    ib->buf[ib->cptr+1] == '\"') {
				ib->cptr++;
				putstr((usch *)"u8");
				break;
			}
			/* FALLTHROUGH */
		default:
#ifdef PCC_DEBUG
			if ((spechr[ch] & C_ID) == 0)
				error("fastscan");
#endif
			if (flslvl)
				error("fastscan flslvl");
			rb->cptr = 0;
			dp = bufid(ch, rb);
			if ((nl = lookup(dp, FIND)) != NULL) {
				if ((ob = kfind(nl)) != NULL) {
					if (*ob->buf == '-' || *ob->buf == '+')
						putch(' ');
					if (skpows)
						cntline();
					buftobuf(ob, &pb);
					if (ob->cptr > 0 &&
					    (ob->buf[ob->cptr-1] == '-' ||
					    ob->buf[ob->cptr-1] == '+'))
						putch(' ');
					bufree(ob);
				}
			} else {
				putstr(dp);
			}
			break;

		case '\\':
			ib->buf[--ib->cptr] = '\\';
			if ((ch = qcchar()) != '\\')
				goto xloop;
			putch('\\');
			break;
		}
	}
}

/*
 * Store an if/elif line on heap for parsing, evaluate macros and 
 * call yyparse().
 */
static usch *yyinp;
int inexpr;
static int
exprline(void)
{
	extern int nbufused;
	struct iobuf *ob, *rb;
	struct symtab *nl;
	int oCflag = Cflag;
	usch *dp;
	int c, d, ifdef;

	rb = getobuf(BNORMAL);
	nbufused--;
	Cflag = ifdef = 0;

	for (;;) {
		c = qcchar();
xloop:		if (c == '\n')
			break;
		if (c == '.') {
			putob(rb, '.');
			if ((spechr[c = qcchar()] & C_DIGIT) == 0)
				goto xloop;
		}
		if (ISDIGIT(c)) {
			c = fastnum(c, rb);
			goto xloop;
		}
		if (c == '\'' || c == '\"') {
			faststr(c, rb);
			continue;
		}
		if (c == 'L' || c == 'u' || c == 'U') {
			unch(d = qcchar());
			if (d == '\'')	/* discard wide designator */
				continue;
		}
		if (ISID0(c)) {
			dp = readid(c);
			nl = lookup(dp, FIND);
			if (nl && nl->type == DEFLOC) {
				ifdef = 1;
			} else if (ifdef) {
				putob(rb, nl ? '1' : '0');
				ifdef = 0;
			} else if (nl != NULL) {
				inexpr = 1;
				if ((ob = kfind(nl))) {
					ob->buf[ob->cptr] = 0;
					strtobuf(ob->buf, rb);
					bufree(ob);
				} else
					putob(rb, '0');
				inexpr = 0;
			} else
				putob(rb, '0');
		} else
			putob(rb, c);
	}
	rb->buf[rb->cptr] = 0;
	unch('\n');
	yyinp = rb->buf;
	c = yyparse();
	bufree(rb);
	nbufused++;
	Cflag = oCflag;
	return c;
}

int
yylex(void)
{
	int ch, c2, t;

	while ((ch = *yyinp++) == ' ' || ch == '\t')
		;
	t = ISDIGIT(ch) ? NUMBER : ch;
	if (ch < 128 && (spechr[ch] & C_2))
		c2 = *yyinp++;
	else
		c2 = 0;

	switch (t) {
	case 0: return WARN;
	case '=':
		if (c2 == '=') return EQ;
		break;
	case '!':
		if (c2 == '=') return NE;
		break;
	case '|':
		if (c2 == '|') return OROR;
		break;
	case '&':
		if (c2 == '&') return ANDAND;
		break;
	case '<':
		if (c2 == '<') return LS;
		if (c2 == '=') return LE;
		break;
	case '>':
		if (c2 == '>') return RS;
		if (c2 == '=') return GE;
		break;
	case '+':
	case '-':
		if (ch == c2)
			error("invalid preprocessor operator %c%c", ch, c2);
		break;

	case '\'':
		yynode.op = NUMBER;
		yynode.nd_val = charcon(&yyinp);
		return NUMBER;

	case NUMBER:
		cvtdig(&yyinp);
		return NUMBER;

	default:
		if (ISID0(t)) {
			yyinp--;
			while (ISID(*yyinp))
				yyinp++;
			yynode.nd_val = 0;
			return NUMBER;
		}
		return ch;
	}
	yyinp--;
	return ch;
}

/*
 * A new file included.
 * If ifiles == NULL, this is the first file and already opened (stdin).
 * Return 0 on success, -1 if file to be included is not found.
 */
int
pushfile(const usch *file, const usch *fn, int idx, void *incs)
{
	struct includ ibuf;
	struct includ *ic;
	int otrulvl;

	ic = &ibuf;
	ic->next = ifiles;

	if (file != NULL) {
		if ((ic->infil = open((const char *)file, O_RDONLY)) < 0)
			return -1;
		ic->orgfn = ic->fname = file;
		if (++inclevel > MAX_INCLEVEL)
			error("limit for nested includes exceeded");
	} else {
		ic->infil = 0;
		ic->orgfn = ic->fname = (const usch *)"<stdin>";
	}
#if LIBVMF
	if (ifiles) {
		vmmodify(ifiles->vseg);
		vmunlock(ifiles->vseg);
	}
	ic->vseg = vmmapseg(&ibspc, inclevel);
	vmlock(ic->vseg);
#endif
	ifiles = ic;
	ic->ib = getobuf(BINBUF);
	ic->lineno = 1;
	ic->escln = 0;
	ic->maxread = ic->curptr;
	ic->idx = idx;
	ic->incs = incs;
	ic->fn = fn;
	prtline(1);
	otrulvl = trulvl;

	fastscan();

	if (otrulvl != trulvl || flslvl)
		error("unterminated conditional");

	ifiles = ic->next;
	inclevel--;
#if LIBVMF
	vmmodify(ic->vseg);
	vmunlock(ic->vseg);
	ic->ib->ro = 1; /* XXX no free */
	if (ifiles) {
		int diff;

		ifiles->vseg = vmmapseg(&ibspc, inclevel);
		vmlock(ifiles->vseg);
		/* XXX recalc ptr diffs */
		diff = (usch *)ifiles->vseg->s_cinfo - ifiles->bbuf;
		ifiles->bbuf += diff;
		ifiles->maxread += diff;
		ifiles->curptr += diff;
	}
#endif
	close(ic->infil);
	bufree(ic->ib);
	return 0;
}

/*
 * Print current position to output file.
 */
void
prtline(int nl)
{
	struct iobuf *ob;

	if (Mflag) {
		if (dMflag)
			return; /* no output */
		if (ifiles->lineno == 1 &&
		    (MMDflag == 0 || ifiles->idx != SYSINC)) {
			ob = bsheap(0, "%s: %s\n", Mfile, ifiles->fname);
			if (MPflag &&
			    strcmp((const char *)ifiles->fname, (char *)MPfile))
				bsheap(ob, "%s:\n", ifiles->fname);
			write(1, ob->buf, ob->cptr);
			bufree(ob);
		}
	} else if (!Pflag) {
		skpows = 0;
		bsheap(&pb, "\n# %d \"%s\"", ifiles->lineno, ifiles->fname);
		if (ifiles->idx == SYSINC)
			strtobuf((usch *)" 3", &pb);
		if (nl) strtobuf((usch *)"\n", &pb);
	}
}

void
cunput(int c)
{
#ifdef PCC_DEBUG
//	if (dflag)printf(": '%c'(%d)\n", c > 31 ? c : ' ', c);
#endif
	unch(c);
}

static int
dig2num(int c)
{
	if (c >= 'a')
		c = c - 'a' + 10;
	else if (c >= 'A')
		c = c - 'A' + 10;
	else
		c = c - '0';
	return c;
}

/*
 * Convert string numbers to unsigned long long and check overflow.
 */
static void
cvtdig(usch **yyp)
{
	unsigned long long rv = 0;
	unsigned long long rv2 = 0;
	usch *y = *yyp;
	int rad;

	y--;
	rad = *y != '0' ? 10 : y[1] == 'x' ||  y[1] == 'X' ? 16 : 8;
	if (rad == 16)
		y += 2;
	while ((spechr[*y] & C_HEX)) {
		rv = rv * rad + dig2num(*y);
		/* check overflow */
		if (rv / rad < rv2)
			error("constant is out of range");
		rv2 = rv;
		y++;
	}
	yynode.op = NUMBER;
	while (*y == 'l' || *y == 'L' || *y == 'u' || *y == 'U') {
		if (*y == 'u' || *y == 'U')
			yynode.op = UNUMBER;
		y++;
	}
	yynode.nd_uval = rv;
	if ((rad == 8 || rad == 16) && yynode.nd_val < 0)
		yynode.op = UNUMBER;
	if (yynode.op == NUMBER && yynode.nd_val < 0)
		/* too large for signed, see 6.4.4.1 */
		error("constant is out of range");
	*yyp = y;
}

static int
charcon(usch **yyp)
{
	int val, c;
	usch *p = *yyp;

	val = 0;
	if (*p++ == '\\') {
		switch (*p++) {
		case 'a': val = '\a'; break;
		case 'b': val = '\b'; break;
		case 'f': val = '\f'; break;
		case 'n': val = '\n'; break;
		case 'r': val = '\r'; break;
		case 't': val = '\t'; break;
		case 'v': val = '\v'; break;
		case '\"': val = '\"'; break;
		case '\'': val = '\''; break;
		case '\\': val = '\\'; break;
		case 'x':
			while ((spechr[c = *p] & C_HEX)) {
				val = val * 16 + dig2num(c);
				p++;
			}
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7':
			p--;
			while ((spechr[c = *p] & C_DIGIT)) {
				val = val * 8 + (c - '0');
				p++;
			}
			break;
		default: val = p[-1];
		}

	} else
		val = p[-1];
	if (*p != '\'')
		error("bad charcon");
	*yyp = ++p;
	return val;
}

static void
chknl(int ignore)
{
	void (*f)(const char *, ...);
	int t;

	f = ignore ? warning : error;
	if ((t = fastspc()) != '\n') {
		if (t) {
			f("newline expected");
			/* ignore rest of line */
			while ((t = qcchar()) > 0 && t != '\n')
				;
		} else
			f("no newline at end of file");
	}
	unch(t);
}

static void
elsestmt(void)
{
	if (flslvl) {
		if (elflvl > trulvl)
			;
		else if (--flslvl!=0)
			flslvl++;
		else
			trulvl++;
	} else if (trulvl) {
		flslvl++;
		trulvl--;
	} else
		error("#else in non-conditional section");
	if (elslvl==trulvl+flslvl)
		error("too many #else");
	elslvl=trulvl+flslvl;
	chknl(1);
}

static void
ifdefstmt(void)
{
	usch *bp;
	int ch;

	if (!ISID0(ch = fastspc()))
		error("bad #ifdef");
	bp = readid(ch);

	if (lookup(bp, FIND) == NULL)
		flslvl++;
	else
		trulvl++;
	chknl(0);
}

static void
ifndefstmt(void)
{
	usch *bp;
	int ch;

	if (!ISID0(ch = fastspc()))
		error("bad #ifndef");
	bp = readid(ch);
	if (lookup(bp, FIND) != NULL)
		flslvl++;
	else
		trulvl++;
	chknl(0);
}

static void
endifstmt(void)
{
	if (flslvl)
		flslvl--;
	else if (trulvl)
		trulvl--;
	else
		error("#endif in non-conditional section");
	if (flslvl == 0)
		elflvl = 0;
	elslvl = 0;
	chknl(1);
}

static void
ifstmt(void)
{
	exprline() ? trulvl++ : flslvl++;
}

static void
elifstmt(void)
{
	if (flslvl == 0)
		elflvl = trulvl;
	if (flslvl) {
		if (elflvl > trulvl)
			;
		else if (--flslvl!=0)
			flslvl++;
		else if (exprline())
			trulvl++;
		else
			flslvl++;
	} else if (trulvl) {
		flslvl++;
		trulvl--;
	} else
		error("#elif in non-conditional section");
}

/* save line into iobuf */
struct iobuf *
savln(void)
{
	struct iobuf *ob = getobuf(BNORMAL);
	int c;

	while ((c = qcchar()) != 0) {
		if (c == '\n') {
			unch(c);
			break;
		}
		putob(ob, c);
	}
	ob->buf[ob->cptr] = 0;
	return ob;
}

static void
cpperror(void)
{
	struct iobuf *ob = savln();
	error("#error%s", ob->buf);
	bufree(ob);
}

static void
cppwarning(void)
{
	struct iobuf *ob = savln();
	warning("#warning%s", ob->buf);
	bufree(ob);
}

static void
undefstmt(void)
{
	struct symtab *np;
	usch *bp;
	int ch;

	if (!ISID0(ch = fastspc()))
		error("bad #undef");
	bp = readid(ch);
	if ((np = lookup(bp, FIND)) != NULL)
		np->value = 0;
	chknl(0);
}

static void
identstmt(void)
{
	struct iobuf *ob = NULL;
	struct symtab *sp;
	usch *bp;
	int ch;

	if (ISID0(ch = fastspc())) {
		bp = readid(ch);
		if ((sp = lookup(bp, FIND)))
			ob = kfind(sp);
		if (ob->buf[0] != '\"')
			goto bad;
		if (ob)
			bufree(ob);
	} else if (ch == '\"') {
		bufree(faststr(ch, NULL));
		
	} else
		goto bad;
	chknl(1);
	return;
bad:
	error("bad #ident directive");
}

static void
pragmastmt(void)
{
	int ch;

	putstr((const usch *)"\n#pragma");
	while ((ch = qcchar()) != '\n' && ch > 0)
		putch(ch);
	unch(ch);
	prtline(1);
}

int
cinput(void)
{

	return qcchar();
}

#define	DIR_FLSLVL	001
#define	DIR_FLSINC	002
static struct {
	const char *name;
	void (*fun)(void);
	int flags;
} ppd[] = {
	{ "ifndef", ifndefstmt, DIR_FLSINC },
	{ "ifdef", ifdefstmt, DIR_FLSINC },
	{ "if", ifstmt, DIR_FLSINC },
	{ "include", include, 0 },
	{ "else", elsestmt, DIR_FLSLVL },
	{ "endif", endifstmt, DIR_FLSLVL },
	{ "error", cpperror, 0 },
	{ "warning", cppwarning, 0 },
	{ "define", define, 0 },
	{ "undef", undefstmt, 0 },
	{ "line", line, 0 },
	{ "pragma", pragmastmt, 0 },
	{ "elif", elifstmt, DIR_FLSLVL },
	{ "ident", identstmt, 0 },
#ifdef GCC_COMPAT
	{ "include_next", include_next, 0 },
#endif
};
#define	NPPD	(int)(sizeof(ppd) / sizeof(ppd[0]))

static void
skpln(void)
{
	int ch;

	/* just ignore the rest of the line */
	while ((ch = qcchar()) != 0) {
		if (ch == '\n') {
			unch('\n');
			break;
		}
	}
}

/*
 * do an even faster scan than fastscan while at flslvl.
 * just search for a new directive.
 */
static void
flscan(void)
{
	int ch;

	for (;;) {
		ch = qcchar();
again:		switch (ch) {
		case 0:
			return;
		case '\n':
			putch('\n');
			ifiles->lineno++;
			while ((ch = qcchar()) == ' ' || ch == '\t')
				;
			if (ch == '#')
				return;
			if (ch == '%' && (ch = qcchar()) == ':')
				return;
			goto again;
		case '\'':
			while ((ch = qcchar()) != '\'')
				if (ch == '\n')
					return;
			break;
		case '\"':
			instr = 1;
			while ((ch = qcchar()) != '\"') {
				switch (ch) {
				case '\\':
					incmnt = 1;
					qcchar();
					incmnt = 0;
					break;
				case '\n':
					unch(ch);
					/* FALLTHROUGH */
				case 0:
					instr = 0;
					return;
				}
			}
			instr = 0;
			break;
		case '/':
			ch = qcchar();
			if (ch == '/' || ch == '*')
				fastcmnt2(ch);
			goto again;
		}
        }
}


/*
 * Handle a preprocessor directive.
 * # is already found.
 */
void
ppdir(void)
{
	int ch, i, oldC;
	usch *bp;

	oldC = Cflag;
redo:	Cflag = 0;
	if ((ch = fastspc()) == '\n') { /* empty directive */
		unch(ch);
		Cflag = oldC;
		return;
	}
	Cflag = oldC;
	if ((spechr[ch] & C_ID0) == 0)
		goto out;
	bp = readid(ch);

	/* got some keyword */
	for (i = 0; i < NPPD; i++) {
		if (bp[0] == ppd[i].name[0] &&
		    strcmp((char *)bp, ppd[i].name) == 0) {
			if (flslvl == 0) {
				(*ppd[i].fun)();
				if (flslvl == 0)
					return;
			} else {
				if (ppd[i].flags & DIR_FLSLVL) {
					(*ppd[i].fun)();
					if (flslvl == 0)
						return;
				} else if (ppd[i].flags & DIR_FLSINC)
					flslvl++;
			}
			flscan();
			goto redo;
		}
	}
	flscan();
	goto redo;

out:
	if (flslvl == 0 && Aflag == 0)
		error("invalid preprocessor directive");

	unch(ch);
	skpln();
}
