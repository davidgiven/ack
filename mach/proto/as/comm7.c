/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* @(#)comm7.c	1.10 */
/*
 * miscellaneous
 */

#include	"comm0.h"
#include	"comm1.h"
#include	"y.tab.h"
#include <stdarg.h>

valu_t
load(ip)
register item_t *ip;
{
#ifdef ASLD
	register typ;

	machload(ip);
	typ = ip->i_type & S_TYP;
	if ((typ -= S_MIN) < 0)		/* S_UND or S_ABS */
		return(ip->i_valu);
	return(ip->i_valu + sect[typ].s_base);
#else
	machload(ip);
	if ((ip->i_type & S_TYP) == S_UND || (ip->i_type & S_COM)) {
		if (pass == PASS_3) {
			if (relonami != 0)
				serror("relocation error (relonami=%d, type=%08x)", relonami, ip->i_type);
			relonami = ip->i_valu+1;
		}
		return(0);
	}
	return(ip->i_valu);
#endif
}

store(ip, val)
register item_t *ip;
valu_t val;
{
#ifdef ASLD
	register typ;

	typ = ip->i_type & S_TYP;
	if ((typ -= S_MIN) >= 0)
		val -= sect[typ].s_base;
#else
	if ((ip->i_type & S_TYP) == S_UND)
		return(0);
#endif
	assert(pass != PASS_3 || (ip->i_type & S_VAR) || ip->i_valu == val);
	ip->i_valu = val;
	return(1);
}

char *
remember(s)
register char *s;
{
	register char *p;
	register n;
	static nleft = 0;
	static char *next;

	p = s;
	n = 0;
	do
		n++;
	while (*p++);
	if ((nleft -= n) < 0) {
		next = malloc(MEMINCR);
		if (next == 0)
			fatal("out of memory");
		nleft = (MEMINCR / sizeof(char)) - n;
		assert(nleft >= 0);
	}
	p = next;
	while (*p++ = *s++)
		;
	s = next;
	next = p;
	return(s);
}

combine(typ1, typ2, op)
register typ1, typ2;
{
	switch (op) {
	case '+':
		if (typ1 == S_ABS)
			return(typ2);
		if (typ2 == S_ABS)
			return(typ1);
		break;
	case '-':
		if (typ2 == S_ABS)
			return(typ1);
		if ((typ1 & ~S_DOT) == (typ2 & ~S_DOT) && typ1 != S_UND)
			return(S_ABS|S_VAR);
		break;
	case '>':
		if (typ1 == S_ABS && typ2 == S_ABS)
			return(S_ABS);
		if (
		    ((typ1 & ~S_DOT) == (typ2 & ~S_DOT) && typ1 != S_UND)
		    || (typ1 == S_ABS)
		    || (typ2 == S_ABS)
		   )
			return(S_ABS|S_VAR);
		break;
	default:
		if (typ1 == S_ABS && typ2 == S_ABS)
			return(S_ABS);
		break;
	}
	if (pass != PASS_1)
		serror("illegal operator");
	return(S_UND);
}

#ifdef LISTING
printx(ndig, val)
valu_t val;
{
	static char buf[8];
	register char *p;
	register c, n;

	p = buf; n = ndig;
	do {
		*p++ = (int) val & 017;
		val >>= 4;
	} while (--n);
	do {
		c = "0123456789ABCDEF"[*--p];
		putchar(c);
	} while (p > buf);
	return(ndig);
}
#endif

#ifdef LISTING
listline(textline)
{
	register c;

	if ((listflag & 4) && (c = getc(listfile)) != '\n' && textline) {
		if (listcolm >= 24)
			printf(" \\\n\t\t\t");
		else
			do {
				putchar('\t');
				listcolm += 8;
			} while (listcolm < 24);
		do {
			assert(c != EOF);
			putchar(c);
		} while ((c = getc(listfile)) != '\n');
	}
	if (listflag & 7) {
		putchar('\n');
		fflush(stdout);
	}
	listeoln = 1;
	listcolm = 0;
	listflag = listtemp;
}
#endif /* LISTING */

/* ---------- code optimization ---------- */

#ifdef THREE_PASS
#define PBITTABSZ	128
static char *pbittab[PBITTABSZ];

small(fitsmall, gain)
{
	register bit;
	register char *p;

	if (DOTSCT == NULL)
		nosect();
	if (bflag)
		return(0);
	if (nbits == BITCHUNK) {
		bitindex++;
		nbits = 0;
		if (bitindex == PBITTABSZ) {
			static int w_given;
			if (pass == PASS_1 && ! w_given) {
				w_given = 1;
				warning("bit table overflow");
			}
			return(0);
		}
		if (pbittab[bitindex] == 0 && pass == PASS_1) {
			if ((pbittab[bitindex] = calloc(MEMINCR, 1)) == 0) {
				static int w2_given;

				if (!w2_given) {
					w2_given = 1;
					warning("out of space for bit table");
				}
			}
		}
		if (pbittab[bitindex] == 0)
			return (0);
	}
	bit = 1 << (nbits&7);
	p = pbittab[bitindex]+(nbits>>3);
	nbits++;
	switch (pass) {
	case PASS_1:
		return(0);
	case PASS_2:
		if (fitsmall) {
			DOTGAIN += gain;
			*p |= bit;
		}
		return(fitsmall);
	case PASS_3:
		assert(fitsmall || (*p & bit) == 0);
		return(*p & bit);
	}
	/*NOTREACHED*/
}
#endif

/* ---------- output ---------- */

emit1(arg)
{
	static int olddottyp = -1;
#ifdef LISTING
	if (listeoln) {
		if (listflag & 1) {
			listcolm += printx(VALWIDTH, (valu_t)DOTVAL);
			listcolm++;
			putchar(' ');
		}
		listeoln = 0;
	}
	if (listflag & 2)
		listcolm += printx(2, (valu_t) arg);
#endif
	switch (pass) {
	case PASS_1:
		if (DOTSCT == NULL)
			nosect();
		/* no break */
	case PASS_2:
		DOTSCT->s_zero = 0;
		break;
	case PASS_3:
		if (DOTTYP != olddottyp) {
			wr_outsect(DOTTYP-S_MIN);
			olddottyp = DOTTYP;
		}
		while (DOTSCT->s_zero) {
			wr_putc(0);
			DOTSCT->s_zero--;
		}
		wr_putc(arg);
		break;
	}
	DOTVAL++;
}

emit2(arg)
int arg;
{
#ifdef BYTES_REVERSED
	emit1((arg>>8)); emit1(arg);
#else
	emit1(arg); emit1((arg>>8));
#endif
}

emit4(arg)
long arg;
{
#ifdef WORDS_REVERSED
	emit2((int)(arg>>16)); emit2((int)(arg));
#else
	emit2((int)(arg)); emit2((int)(arg>>16));
#endif
}

emitx(val, n)
valu_t val;
int n;
{
	switch (n) {
	case RELO1:
		emit1((int)val); break;
	case RELO2:
#ifdef BYTES_REVERSED
		emit1(((int)val>>8)); emit1((int)val);
#else
		emit1((int)val); emit1(((int)val>>8));
#endif
		break;
	case RELO4:
#ifdef WORDS_REVERSED
		emit2((int)(val>>16)); emit2((int)(val));
#else
		emit2((int)(val)); emit2((int)(val>>16));
#endif
		break;
	default:
		assert(0);
	}
}

emitstr(zero)
{
	register i;
	register char *p;

	p = stringbuf;
	i = stringlen;
	while (--i >= 0)
		emit1(*p++);
	if (zero)
		emit1(0);
}

/* ---------- Error checked file I/O  ---------- */

ffreopen(s, f)
char *s;
FILE *f;
{
	if (freopen(s, "r", f) == NULL)
		fatal("can't reopen %s", s);
}

FILE *
ffcreat(s)
char *s;
{
	FILE *f;

	if ((f = fopen(s, "w")) == NULL)
		fatal("can't create %s", s);
	return(f);
}

#ifndef TMPDIR
#define TMPDIR "/tmp"
#endif
char *tmp_dir = TMPDIR;

FILE *
fftemp(path, tail)
char *path, *tail;
{
	register char *dir;

	if ((dir = getenv("TMPDIR")) == NULL)
		dir = tmp_dir;
	sprintf(path, "%s/%s", dir, tail);
	return(ffcreat(mktemp(path)));
}

/* ---------- Error handling ---------- */

/*VARARGS*/
yyerror(){}		/* we will do our own error printing */

nosect()
{
	fatal("no sections");
}

wr_fatal()
{
	fatal("write error");
}

void diag(const char* tail, const char* s, va_list ap)
{
	fflush(stdout);
	if (modulename)
		fprintf(stderr, "\"%s\", line %ld: ", modulename, lineno);
	else
		fprintf(stderr, "%s: ", progname);
	vfprintf(stderr, s, ap);
	fprintf(stderr, "%s", tail);
}

/* VARARGS1 */
void fatal(const char* s, ...)
{
	va_list ap;
	va_start(ap, s);

	nerrors++;
	diag(" (fatal)\n", s, ap);
	stop();

	va_end(ap);
}

#if DEBUG == 2
assert2(file, line)
char *file;
{
	fatal("assertion failed (%s, %d)", file, line);
}
#endif

#if DEBUG == 1
assert1()
{
	fatal("assertion failed");
	abort();
}
#endif

void serror(const char* s, ...)
{
	va_list ap;
	va_start(ap, s);

	nerrors++;
	diag("\n", s, ap);
	stop();

	va_end(ap);
}

/* VARARGS1 */
void warning(const char* s, ...)
{
	va_list ap;
	va_start(ap, s);

	nerrors++;
	diag(" (warning)\n", s, ap);
	stop();

	va_end(ap);
}

nofit()
{
	if (pass == PASS_3)
		warning("too big");
}
