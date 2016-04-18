/*	$Id: cpp.c,v 1.274 2016/04/12 18:49:35 ragge Exp $	*/

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

/*
 * The C preprocessor.
 * This code originates from the V6 preprocessor with some additions
 * from V7 cpp, and at last ansi/c99 support.
 *
 * 	- kfind() expands the input buffer onto an output buffer.
 *	- exparg() expand one buffer into another.
 *		Recurses into submac() for fun-like macros.
 *	- submac() replaces the given macro.
 *		Recurses into subarg() for fun-like macros.
 *	- subarg() expands fun-like macros.
 *		Create strings, concats args, recurses into exparg.
 */

#include "config.h"

#include <sys/stat.h>

#include <fcntl.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "compat.h"
#include "cpp.h"

#ifndef S_ISDIR
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#endif

/*
 * Buffers used:
 *	- expansion buffers (BNORMAL)
 *	- string buffers (used to store macros)
 */

static int	counter;
/* C command */

int tflag;	/* traditional cpp syntax */
#ifdef PCC_DEBUG
int dflag;	/* debug printouts */
static void prline(const usch *s);
static void prrep(const usch *s);
#define	DPRINT(x) if (dflag) printf x
#define	DDPRINT(x) if (dflag > 1) printf x
#define	IMP(x) if (dflag > 1) imp(x)
#else
#define DPRINT(x)
#define DDPRINT(x)
#define IMP(x)
#endif

int Aflag, Cflag, Eflag, Mflag, dMflag, Pflag, MPflag, MMDflag;
char *Mfile, *MPfile;
char *Mxfile;
int warnings, Mxlen;
static usch utbuf[CPPBUF];
struct iobuf pb = { utbuf, 0, CPPBUF, 0, 1, BUTBUF };
#if LIBVMF
struct vspace ibspc;
#endif

/* include dirs */
struct incs {
	struct incs *next;
	usch *dir;
	dev_t dev;
	ino_t ino;
} *incdir[2];

static struct symtab *filloc;
static struct symtab *linloc;
static struct symtab *pragloc;
static struct symtab *defloc;
static struct symtab *ctrloc;
int	trulvl;
int	flslvl;
int	elflvl;
int	elslvl;

/*
 * Macro replacement list syntax:
 * - For object-type macros, replacement strings are stored as-is.
 * - For function-type macros, macro args are substituted for the
 *   character WARN followed by the argument number.
 * - The value element points to the beginning of the string.
 *
 * The first character in the replacement list is the number of arguments:
 *   VARG  - ends with ellipsis, next char is argcount without ellips.
 *   OBJCT - object-type macro
 *   0	   - empty parenthesis, foo()
 *   1->   - number of args.
 *
 * WARN is used:
 *	- in stored replacement lists to tell that an argument comes
 *	- When expanding replacement lists to tell that the list ended.
 *
 * To ensure that an already expanded identifier won't get expanded
 * again a EBLOCK char + its number is stored directly before any
 * expanded identifier.
 */

/* args for lookup() */
#define	FIND	0
#define	ENTER	1

/*
 * No-replacement array.  If a macro is found and exists in this array
 * then no replacement shall occur.
 */
struct blocker {
	struct blocker *next;
	struct symtab *sp;
};
struct blocker *blkidx[RECMAX];
int blkidp;

static struct iobuf *readargs2(struct iobuf *, struct symtab *sp, const usch **args);
static struct iobuf *readargs1(struct symtab *sp, const usch **args);
static struct iobuf *exparg(int, struct iobuf *, struct iobuf *, struct blocker *);
static struct iobuf *subarg(struct symtab *sp, const usch **args, int, struct blocker *);
static void usage(void);
static usch *xstrdup(const usch *str);
static void addidir(char *idir, struct incs **ww);
static void vsheap(struct iobuf *, const char *, va_list);
static int skipws(struct iobuf *ib);
static int getyp(usch *s);

int
main(int argc, char **argv)
{
	struct includ bic;
	struct iobuf *fb = getobuf(BNORMAL);
	register int ch;
	const usch *fn1, *fn2;
	char *a;

#ifdef TIMING
	struct timeval t1, t2;

	(void)gettimeofday(&t1, NULL);
#endif

#if LIBVMF
	if (vminit(2))
		error("vminit");
	if (vmopen(&ibspc, NULL) < 0)
		error("vmopen ibspc");
#endif

	while ((ch = getopt(argc, argv, "ACD:d:EI:i:MPS:tU:Vvx:")) != -1) {
		switch (ch) {
		case 'A': /* assembler input */
			Aflag++;
			break;

		case 'C': /* Do not discard comments */
			Cflag++;
			break;

		case 'E': /* treat warnings as errors */
			Eflag++;
			break;

		case 'D': /* define something */
			if ((a = strchr(optarg, '=')) != NULL)
				*a = ' ';
			bsheap(fb, "#define %s%s", optarg, a ? "\n" : " 1\n");
			break;

		case 'i': /* include */
			bsheap(fb, "#include \"%s\"\n", optarg);
			break;

		case 'U': /* undef */
			bsheap(fb, "#undef %s\n", optarg);
			break;

		case 'd':
			while (*optarg) {
				switch(*optarg) {
				case 'M': /* display macro definitions */
					dMflag = 1;
					Mflag = 1;
					break;

				default: /* ignore others */
					break;
				}
				optarg++;
			}
			break;

		case 'I':
		case 'S':
			addidir(optarg, &incdir[ch == 'I' ? INCINC : SYSINC]);
			break;

		case 'M': /* Generate dependencies for make */
			Mflag++;
			break;

		case 'P': /* Inhibit generation of line numbers */
			Pflag++;
			break;

		case 't':
			tflag = 1;
			break;

#ifdef PCC_DEBUG
		case 'V':
			dflag++;
			break;
#endif
		case 'v':
			fprintf(stderr, "PCC preprocessor version "VERSSTR"\n");
			break;

		case 'x':
			if (strcmp(optarg, "MMD") == 0) {
				MMDflag++;
			} else if (strcmp(optarg, "MP") == 0) {
				MPflag++;
			} else if (strncmp(optarg, "MT,", 3) == 0 ||
			    strncmp(optarg, "MQ,", 3) == 0) {
				int l = strlen(optarg+3) + 2;
				char *cp, *up;

				if (optarg[1] == 'Q')
					for (cp = optarg+3; *cp; cp++)
						if (*cp == '$')
							l++;
				Mxlen += l;
				Mxfile = cp = realloc(Mxfile, Mxlen);
				for (up = Mxfile; *up; up++)
					;
				if (up != Mxfile)
					*up++ = ' ';
				for (cp = optarg+3; *cp; cp++) {
					*up++ = *cp;
					if (optarg[1] == 'Q' && *cp == '$')
						*up++ = *cp;
				}
				*up = 0;
			} else
				usage();
			break;

		case '?':
		default:
			usage();
		}
	}

	argc -= optind;
	argv += optind;

	filloc = lookup((const usch *)"__FILE__", ENTER);
	linloc = lookup((const usch *)"__LINE__", ENTER);
	pragloc = lookup((const usch *)"_Pragma", ENTER);
	defloc = lookup((const usch *)"defined", ENTER);
	ctrloc = lookup((const usch *)"__COUNTER__", ENTER);
	filloc->value = linloc->value = pragloc->value =
	    ctrloc->value = (const usch *)"";
	defloc->value = defloc->namep;
	filloc->type = FILLOC;
	linloc->type = LINLOC;
	pragloc->type = PRAGLOC;
	defloc->type = DEFLOC;
	ctrloc->type = CTRLOC;

	if (Mflag && !dMflag) {
		char *c;

		if (argc < 1)
			error("-M and no infile");
		if ((c = strrchr(argv[0], '/')) == NULL)
			c = argv[0];
		else
			c++;
		Mfile = (char *)xstrdup((usch *)c);
		if (MPflag)
			MPfile = (char *)xstrdup((usch *)c);
		if (Mxfile)
			Mfile = Mxfile;
		if ((c = strrchr(Mfile, '.')) == NULL)
			error("-M and no extension: ");
		c[1] = 'o';
		c[2] = 0;
	}

	if (argc == 2) {
		close(1);
		if (open(argv[1], O_WRONLY|O_CREAT, 0600) < 0)
			error("Can't creat %s", argv[1]);
	}

	if (argc && strcmp(argv[0], "-")) {
		fn1 = fn2 = (usch *)argv[0];
	} else {
		fn1 = NULL;
		fn2 = (const usch *)"";
	}

	/* initialization defines */
	if (dMflag)
		write(1, fb->buf, fb->cptr);
	fb->buf[fb->cptr] = 0;
	memset(&bic, 0, sizeof(bic));
	bic.fname = bic.orgfn = (const usch *)"<command line>";
	bic.lineno = 1;
	bic.infil = -1;
	bic.ib = fb;
	fb->bsz = fb->cptr;
	fb->cptr = 0;
	ifiles = &bic;
	fastscan();
	bufree(fb);
	ifiles = NULL;
	/* end initial defines */

	if (pushfile(fn1, fn2, 0, NULL))
		error("cannot open %s", argv[0]);

	if (Mflag == 0)
		write(1, pb.buf, pb.cptr);
#ifdef TIMING
	(void)gettimeofday(&t2, NULL);
	t2.tv_sec -= t1.tv_sec;
	t2.tv_usec -= t1.tv_usec;
	if (t2.tv_usec < 0) {
		t2.tv_usec += 1000000;
		t2.tv_sec -= 1;
	}
	fprintf(stderr, "cpp total time: %ld s %ld us\n",
	     (long)t2.tv_sec, (long)t2.tv_usec);
#endif
	if (Eflag && warnings > 0)
		return 2;

	return 0;
}

/*
 * Write a character to an out buffer.
 */
void
putob(struct iobuf *ob, int ch)
{
	if (ob->cptr == ob->bsz) {
		int sz = ob->bsz;
		switch (ob->type) {
		case BNORMAL:
			ob->buf = xrealloc(ob->buf, sz + CPPBUF+1);
			/* ob->cptr = ob->buf + sz; */
			ob->bsz = sz + CPPBUF;
			break;
		case BMAC:
		case BINBUF:
			error("putob");
		case BUTBUF:
			if (Mflag == 0)
				(void)write(1, ob->buf, sz);
			ob->cptr = 0;
			break;
		}
	}
//	DDPRINT(("putob: iob %p pos %p ch %c (%d)\n", ob, ob->cptr, ch, ch));
	ob->buf[ob->cptr++] = ch;
}

static struct iobuf *
giob(int typ, const usch *bp, int bsz)
{
	struct iobuf *iob = xmalloc(sizeof(struct iobuf));

	if (bp == NULL)
		bp = xmalloc(bsz);
	iob->buf = (usch *)bp;
	iob->cptr = 0;
	iob->bsz = bsz;
	iob->ro = 0;
	iob->type = typ;
	return iob;
}


int nbufused;
/*
 * Write a character to an out buffer.
 */
struct iobuf *
getobuf(int type)
{
	struct iobuf *iob = 0;

	switch (type) {
	case BNORMAL:
		nbufused++;
		iob = giob(BNORMAL, NULL, CPPBUF);
		iob->bsz = CPPBUF-1; /* space for \0 */
		break;
	case BINBUF:
#if LIBVMF
		iob = giob(BINBUF, (usch *)ifiles->vseg->s_cinfo, CPPBUF);
#else
		iob = giob(BINBUF, NULL, CPPBUF);
#endif
		break;
	default:
		error("getobuf");
	}
	return iob;
}

/*
 * Create a read-only input buffer.
 */
static struct iobuf *
mkrobuf(const usch *s)
{
	struct iobuf *iob;

	nbufused++;
	iob = giob(BNORMAL, s, strlen((char *)s));
	iob->ro = 1;
	DPRINT(("mkrobuf %s\n", s));
	return iob;
}

/*
 * Copy a buffer to another buffer.
 */
struct iobuf *
buftobuf(struct iobuf *in, struct iobuf *iob)
{
	int cp;

	DPRINT(("strtobuf iob %p buf %p str %p\n", iob, iob->buf, in));
	if (iob == NULL)
		iob = getobuf(BNORMAL);
	for (cp = 0; cp < in->cptr; cp++)
		putob(iob, in->buf[cp]);
	return iob;
}

/*
 * Copy a string to a buffer.
 */
struct iobuf *
strtobuf(const usch *str, struct iobuf *iob)
{
	if (iob == NULL)
		iob = getobuf(BNORMAL);
	DPRINT(("strtobuf iob %p buf %p str %s\n", iob, iob->buf, str));
	do {
		putob(iob, *str);
	} while (*str++);
	iob->cptr--;
	return iob;
}

static usch *macbase;
static int macpos, cmbase;

static void
macsav(int ch)
{
	if (macbase == NULL)
		macbase = xmalloc(CPPBUF);
	if (macpos == CPPBUF) {
		usch *tb;
		if (cmbase == 0)
			error("macro too large");
		tb = xmalloc(CPPBUF);
		memcpy(tb, macbase+cmbase, CPPBUF-cmbase);
		macpos -= cmbase;
		cmbase = 0;
		macbase = tb;
	}
	macbase[macpos++] = ch;
}

static void                     
macstr(const usch *s)
{
	do {
		macsav(*s);
	} while (*s++ != 0);
	macpos--;
}

#define	setcmbase()	cmbase = macpos
#define	clrcmbase()	macpos = cmbase

void
bufree(struct iobuf *iob)
{
	if (iob->type == BNORMAL)
		nbufused--;
	if (iob->ro == 0)
		free(iob->buf);
	free(iob);
}

static void
addidir(char *idir, struct incs **ww)
{
	struct incs *w;
	struct stat st;

	if (stat(idir, &st) == -1 || !S_ISDIR(st.st_mode))
		return; /* ignore */
	if (*ww != NULL) {
		for (w = *ww; w->next; w = w->next) {
#ifdef _WIN32
			if (strcmp(w->dir, idir) == 0)
				return;
#else
			if (w->dev == st.st_dev && w->ino == st.st_ino)
				return;
#endif
		}
#ifdef _WIN32
		if (strcmp(w->dir, idir) == 0)
			return;
#else
		if (w->dev == st.st_dev && w->ino == st.st_ino)
			return;
#endif
		ww = &w->next;
	}
	if ((w = calloc(sizeof(struct incs), 1)) == NULL)
		error("couldn't add path %s", idir);
	w->dir = (usch *)idir;
	w->dev = st.st_dev;
	w->ino = st.st_ino;
	*ww = w;
}

void
line(void)
{
	struct iobuf *ob;
	struct symtab *nl;
	int c, n, ln;
	usch *cp, *dp;

	c = skipws(0);
	if (ISID0(c)) { /* expand macro */
		dp = readid(c);
		if ((nl = lookup(dp, FIND)) == 0 || (ob = kfind(nl)) == 0)
			goto bad;
	} else {
		ob = getobuf(BNORMAL);
		do {
			putob(ob, c);
		} while (ISDIGIT(c = cinput()));
		cunput(c);
		putob(ob, 0);
	}
	cp = ob->buf;

	n = 0;
	while (ISDIGIT(*cp))
		n = n * 10 + *cp++ - '0';
	if (*cp != 0)
		goto bad;
	bufree(ob);

	/* Can only be decimal number here between 1-2147483647 */
	if (n < 1 || n > 2147483647)
		goto bad;

	ln = n;
	ifiles->escln = 0;
	if ((c = skipws(NULL)) != '\n') {
		if (c == 'L' || c == 'U' || c == 'u') {
			n = c, c = cinput();
			if (n == 'u' && c == '8')
				c = cinput();
			if (c == '\"')
				warning("#line only allows character literals");
		}
		if (c != '\"')
			goto bad;

		ob = faststr(c, NULL);
		if (strcmp((char *)ifiles->fname, (char *)ob->buf))
			ifiles->fname = xstrdup(ob->buf);
		bufree(ob);

		c = skipws(0);
	}
	if (c != '\n')
		goto bad;

	ifiles->lineno = ln;
	prtline(1);
	ifiles->lineno--;
	cunput('\n');
	return;

bad:	error("bad #line");
}

#ifdef MACHOABI

/*
 * Example:
 * Library/Frameworks/VideoToolbox.framework/Headers/VTSession.h
 *
 * Search for framework header file.
 * Return 1 on success.
 */

static int
fsrch_macos_framework(const usch *fn, const usch *dir)
{
	struct iobuf *ob;
	usch *s = (usch *)strchr((const char*)fn, '/');
	usch *p, *q, *nm;
	int len  = s - fn;

	if (s == NULL)
		return 0;

	nm = xstrdup(dir);
	p = xstrdup(fn);
	*(p + len) = 0;

	q = (usch *)strstr((const char *)nm, (const char *)p);
	if (q != NULL) {
		*q = 0;
		return fsrch_macos_framework(fn, nm);
	}
	free(p);

	p = nm + strlen((char *)nm) - 1;
	while (*p == '/')
		p--;
	while (*p != '/')
		p--;
	++p;
	
	ob = bsheap(NULL, "%s/Frameworks/%s.framework/Headers%s", nm, fn, s);
	free(nm);
	nm = xstrdup(ob->buf);
	bufree(ob);
	if (pushfile(nm, fn, SYSINC, NULL) == 0)
		return 1;

	return 0;
}

#endif

/*
 * Search for and include next file.
 * Return 1 on success.
 */
static int
fsrch(const usch *fn, int idx, struct incs *w)
{
	int i;

	setcmbase();
	for (i = idx; i < 2; i++) {
		if (i > idx)
			w = incdir[i];
		for (; w; w = w->next) {
			macstr(w->dir); macsav('/');
			macstr(fn); macsav(0);
			if (pushfile(macbase+cmbase, fn, i, w->next) == 0)
				return 1;
			clrcmbase();
		}
	}

#ifdef MACHOABI
	/*
	 * On MacOS, we may have to do some clever stuff
	 * to resolve framework headers.
	 */
	{
		/*
		 * Dig out org filename path and try to find.
		 */
		usch *p, *dir = xstrdup(ifiles->orgfn);
		if ((p = (usch *)strrchr((char *)dir, '/')) != NULL) {
			p[1] = 0;
			if (fsrch_macos_framework(fn, dir) == 1)
				return 1;
		}
		free(dir);

		if (fsrch_macos_framework(fn,
		    (const usch *)"/Library/Frameworks/") == 1)
			return 1;

		if (fsrch_macos_framework(fn,
		    (const usch *)"/System/Library/Frameworks/") == 1)
			return 1;
	}
#endif

	return 0;
}

static void
prem(void)
{
	error("premature EOF");
}

static struct iobuf *
incfn(void)
{
	struct iobuf *ob;
	struct symtab *nl;
	usch *dp;
	int c;

	if (spechr[c = skipws(NULL)] & C_ID0) {
		dp = readid(c);
		if ((nl = lookup(dp, FIND)) == NULL)
			return NULL;

		if ((ob = kfind(nl)) == 0)
			return NULL;
	} else {
		ob = getobuf(BNORMAL);
		putob(ob, c);
		while ((c = cinput()) && c != '\n')
			putob(ob, c);
		if (c != '\n')
			return NULL;
		cunput(c);
	}

	/* now we have an (expanded?) filename in obuf */
	while (0 < ob->cptr && ISWS(ob->buf[ob->cptr-1]))
		ob->cptr--;

	if (ob->buf[0] != '\"' && ob->buf[0] != '<')
		return NULL;
	if (ob->buf[ob->cptr-1] != '\"' && ob->buf[ob->cptr-1] != '>')
		return NULL;
	ob->buf[ob->cptr-1] = 0;
	return ob;
}

/*
 * Include a file. Include order:
 * - For <...> files, first search -I directories, then system directories.
 * - For "..." files, first search "current" dir, then as <...> files.
 */
void
include(void)
{
	struct iobuf *ob;
	usch *fn, *nm = NULL;

	if (flslvl)
		return;

	if ((ob = incfn()) == NULL) /* get include file name in obuf */
		error("bad #include");

	fn = xstrdup(ob->buf) + 1;	/* Save on string heap? */
	bufree(ob);
	/* test absolute path first */
	if (fn[0] == '/' && pushfile(fn, fn, 0, NULL) == 0)
		goto okret;
	if (fn[-1] == '\"') {
		/* nope, failed, try to create a path for it */
		if ((nm = (usch *)strrchr((char *)ifiles->orgfn, '/'))) {
			ob = strtobuf((usch *)ifiles->orgfn, NULL);
			ob->cptr = (nm - ifiles->orgfn) + 1;
			strtobuf(fn, ob);
			nm = xstrdup(ob->buf);
			bufree(ob);
		} else
			nm = xstrdup(fn);
		if (pushfile(nm, nm, 0, NULL) == 0) {
			free(fn-1);
			goto okret;
		}
	}
	if (fsrch(fn, 0, incdir[0]))
		goto okret;

	error("cannot find '%s'", fn);
	/* error() do not return */

okret:
	if (nm)
		free(nm);
	prtline(1);
}

void
include_next(void)
{
	struct iobuf *ob;
	usch *nm;

	if (flslvl)
		return;

	if ((ob = incfn()) == NULL) /* get include file name in obuf */
		error("bad #include_next");

	nm = xstrdup(ob->buf+1);
	bufree(ob);

	if (fsrch(nm, ifiles->idx, ifiles->incs) == 0)
		error("cannot find '%s'", nm);
	prtline(1);
}

/*
 * Compare two replacement lists, taking in account comments etc.
 */
static int
cmprepl(const usch *o, const usch *n)
{
	for (; *o; o++, n++) {
		/* comment skip */
		if (*o == '/' && o[1] == '*') {
			while (*o != '*' || o[1] != '/')
				o++;
			o += 2;
		}
		if (*n == '/' && n[1] == '*') {
			while (*n != '*' || n[1] != '/')
				n++;
			n += 2;
		}
		while (*o == ' ' || *o == '\t')
			o++;
		while (*n == ' ' || *n == '\t')
			n++;
		if (*o != *n)
			return 1;
	}
	return 0;
}

static int
isell(void)
{
	if (cinput() != '.' || cinput() != '.')
		return 0;
	return 1;
}

static int
skipwscmnt(struct iobuf *ib)
{
	/* XXX comment */
	return skipws(ib);
}

static int
findarg(usch *s, struct iobuf *ab, int *arg, int narg)
{
	int i;

	for (i = 0; i < narg; i++)
		if (strcmp((char *)s, (char *)ab->buf + arg[i]) == 0)
			return i;
	return -1;
}

/*
 * gcc extensions:
 * #define e(a...) f(s, a) ->  a works as __VA_ARGS__
 * #define e(fmt, ...) f(s, fmt , ##__VA_ARGS__) -> remove , if no args
 */
void
define(void)
{
	struct iobuf *ib, *ab;
	struct symtab *np;
	usch cc[2], *vararg, *dp;
	int arg[MAXARGS+1];
	int c, i, redef, oCflag, t;
	int type, narg;
	int wascon;

	if (flslvl)
		return;

	oCflag = Cflag, Cflag = 0; /* Ignore comments here */
	if (!ISID0(c = skipws(0)))
		goto bad;

	dp = readid(c);
	np = lookup(dp, ENTER);
	if (np->value) {
		redef = 1;
	} else {
		np->namep = xstrdup(dp);
		redef = 0;
	}

	type = OBJCT;
	narg = 0;
	ab = getobuf(BNORMAL);
	vararg = NULL;
	if ((c = cinput()) == '(') {
		type = 0;
		/* function-like macros, deal with identifiers */
		c = skipws(0);
		for (;;) {
			switch (c) {
			case ')':
				break;
			case '.':
				if (isell() == 0 || (c = skipws(0)) != ')')
					goto bad;
				vararg = (usch *)"__VA_ARGS__";
				break;
			default:
				if (!ISID0(c))
					goto bad;

				dp = bufid(c, ab);
				/* make sure there is no arg of same name */
				if (findarg(dp, ab, arg, narg) >= 0)
					error("Duplicate parameter \"%s\"", dp);
				if (narg == MAXARGS)
					error("Too many macro args");
				putob(ab, 0);
				arg[narg++] = dp - ab->buf;
				switch ((c = skipws(0))) {
				case ',': break;
				case ')': continue;
				case '.':
					if (isell() == 0 || skipws(0) != ')')
						goto bad;
					vararg = ab->buf + arg[--narg];
					c = ')';
					continue;
				default:
					goto bad;
				}
				c = skipws(0);
			}
			if (c == ')')
				break;
		}
		c = skipws(0);
	} else if (c == '\n') {
		/* #define foo */
		;
	} else if (c == 0) {
		prem();
	} else if (!ISWS(c))
		goto bad;

	Cflag = oCflag; /* Enable comments again */

	setcmbase();
	if (ISWS(c))
		c = skipwscmnt(0);

#define	DELEWS() while ((macpos > cmbase) && \
	ISWS(macbase[macpos-1])) macpos--

	/* parse replacement-list, substituting arguments */
	wascon = 0;
	while (c != '\n') {
		cc[0] = c, cc[1] = inc2();
		t = getyp(cc);
		cunput(cc[1]);

		switch (t) {
		case ' ':
		case '\t':
			macsav(' '); /* save only one space */
			while ((c = cinput()) == ' ' || c == '\t')
				;
			continue;

		case '#':
			if (cc[1] == '#') {
				/* concat op */
				(void)cinput(); /* eat # */
				DELEWS();
				macsav(CONC);
				if (ISID0(c = skipws(0)) && type == 0)
					wascon = 1;
				if (c == '\n')
					goto bad; /* 6.10.3.3 p1 */
				continue;
			}

			if (type == OBJCT) {
				/* no meaning in object-type macro */
				macsav('#');
				break;
			}

			/* remove spaces between # and arg */
			macsav(SNUFF);
			c = skipws(0); /* whitespace, ignore */
			if (!ISID0(c))
				goto bad;
			dp = readid(c);
			if (vararg && strcmp((char *)dp, (char *)vararg) == 0) {
				macsav(WARN);
				macsav(VARG);
				macsav(SNUFF);
				break;
				
			}
			if ((i = findarg(dp, ab, arg, narg)) < 0)
				goto bad;
			macsav(WARN);
			macsav(i);
			macsav(SNUFF);
			break;

		case CMNT:
			Ccmnt(macsav);
			break;

		case NUMBER: 
			ib = getobuf(BNORMAL);
			c = fastnum(c, ib);
			for (dp = ib->buf; dp < ib->buf + ib->cptr; dp++)
				macsav(*dp);
			bufree(ib);
			continue;

		case STRING:
			if (c == 'L' || c == 'u' || c == 'U') {
				macsav(c);
				if ((c = cinput()) == '8') {
					macsav(c);
					c = cinput();
				}
			}
			if (tflag)
				macsav(c);
			else {
				ib = faststr(c, NULL);
				for (dp = ib->buf; *dp ; dp++)
					macsav(*dp);
				bufree(ib);
			}
			break;

		case IDENT:
			dp = readid(c);
			if (type == OBJCT) {
				macstr(dp);
				break; /* keep on heap */
			}
			if (vararg && strcmp((char *)dp, (char *)vararg) == 0) {
				macsav(WARN);
				macsav(wascon ? GCCARG : VARG);
				break;
			}

			/* check if its an argument */
			if ((i = findarg(dp, ab, arg, narg)) < 0) {
				macstr(dp);
				break;
			}
			macsav(WARN);
			macsav(i);
			break;

		case 0:
			goto bad;
			
		default:
			macsav(c);
			break;
		}
		wascon = 0;
		c = cinput();
	}
	cunput(c);
	/* remove trailing whitespace */
	DELEWS();

	macsav(0);
	if (vararg)
		type = VARG;

	if (macbase[cmbase] == CONC)
		goto bad; /* 6.10.3.3 p1 */

	if (redef && ifiles->idx != SYSINC) {
		if (cmprepl(np->value, macbase+cmbase) || 
		    np->type != type || np->narg != narg) { /* not equal */
			np->value = macbase+cmbase;
			warning("%s redefined (previously defined at \"%s\" line %d)",
			    np->namep, np->file, np->line);
		} else
			macpos = cmbase;  /* forget this space */
	} else
		np->value = macbase+cmbase;
	np->type = type;
	np->narg = narg;

#ifdef PCC_DEBUG
	if (dflag) {
		const usch *w = np->value;

		printf("!define %s: ", np->namep);
		if (type == OBJCT)
			printf("[object]");
		else if (type == VARG)
			printf("[VARG%d]", narg);
		else
			printf("[%d]", narg);
		putchar('\'');
		prrep(w);
		printf("\'\n");
	}
#endif
	bufree(ab);
	return;

bad:	error("bad #define");
}

void
warning(const char *fmt, ...)
{
	va_list ap;

	if (ifiles != NULL)
		fprintf(stderr, "%s:%d: warning: ",
		    ifiles->fname, ifiles->lineno);

	va_start(ap,fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);

	warnings++;
}

void
error(const char *fmt, ...)
{
	va_list ap;

	if (ifiles != NULL)
		fprintf(stderr, "%s:%d: error: ",
		    ifiles->fname, ifiles->lineno);

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(1);
}

static int
pragwin(struct iobuf *ib)
{
	return ib ? ib->buf[ib->cptr++] : cinput();
}

static int
skipws(struct iobuf *ib)
{
	int t;

	while ((t = pragwin(ib)) == ' ' || t == '\t')
		;
	return t;
}

/*
 * convert _Pragma() to #pragma for output.
 * Syntax is already correct.
 */
static void
pragoper(struct iobuf *ib)
{
	int t;
	struct iobuf *ob = getobuf(BNORMAL);

	if (skipws(ib) != '(' || ((t = skipws(ib)) != '\"' && t != 'L'))
		goto err;
	if (t == 'L' && (t = pragwin(ib)) != '\"')
		goto err;
	strtobuf((usch *)"\n#pragma ", ob);
	while ((t = pragwin(ib)) != '\"') {
		if (t == BLKID) {
			pragwin(ib);
			continue;
		}
		if (t == '\"')
			continue;
		if (t == '\\') {
			if ((t = pragwin(ib)) != '\"' && t != '\\')
				putob(ob, '\\');
		}
		putob(ob, t);
	}
	bsheap(ob, "\n# %d \"%s\"\n", ifiles->lineno, ifiles->fname);
	putstr(ob->buf);
	bufree(ob);
	if (skipws(ib) == ')')
		return;

err:	error("_Pragma() syntax error");
}

static int
expok(struct symtab *sp, int l)
{
	struct blocker *w;

	if (l == 0)
		return 1;
#ifdef PCC_DEBUG
if (dflag) { printf("expok blocked: "); for (w = blkidx[l]; w; w = w->next) printf("%s ", w->sp->namep); printf("\n"); }
#endif
	w = blkidx[l];
	while (w) {
		if (w->sp == sp)
			return 0;
		w = w->next;
	}
	return 1;
}

static int
expokb(struct symtab *sp, struct blocker *bl)
{
	struct blocker *w;

	if (bl == 0)
		return 1;
#ifdef PCC_DEBUG
if (dflag) { printf("expokb blocked: "); for (w = bl; w; w = w->next) printf("%s ", w->sp->namep); printf("\n"); }
#endif
	w = bl;
	while (w) {
		if (w->sp == sp)
			return 0;
		w = w->next;
	}
	return 1;
}

static struct blocker *
blkget(struct symtab *sp, struct blocker *obl)
{
	struct blocker *bl = calloc(sizeof(*obl), 1);

	bl->sp = sp;
	bl->next = obl;
	return bl;
}

static int
blkix(struct blocker *obl)
{
	if (blkidp > 1 && blkidx[blkidp-1] == obl)
		return blkidp-1;
	if (blkidp == RECMAX)
		error("blkix");
	blkidx[blkidp] = obl;
	return blkidp++;
}

static struct blocker *
mergeadd(struct blocker *bl, int m)
{
	struct blocker *w, *ww;

	DPRINT(("mergeadd: %p %d\n", bl, m));
	if (bl == 0)
		return blkidx[m];
	if (m == 0)
		return bl;

	blkidx[blkidp] = bl;
	for (w = blkidx[m]; w; w = w->next) {
		ww = calloc(sizeof(*w), 1);
		ww->sp = w->sp;
		ww->next = blkidx[blkidp];
		blkidx[blkidp] = ww;
	}
	DPRINT(("mergeadd return: %d ", blkidp));
#ifdef PCC_DEBUG
	if (dflag) {
		for (w = blkidx[blkidp]; w; w = w->next)
			printf("%s ", w->sp->namep);
		printf("\n");
	}
#endif
	return blkidx[blkidp++];
}

static void
storeblk(int l, struct iobuf *ob)
{
	DPRINT(("storeblk: %d\n", l));
	putob(ob, BLKID);
	putob(ob, l);
}

/*
 * Save filename on heap (with escaped chars).
 */
static struct iobuf *
unfname(void)
{
	struct iobuf *ob = getobuf(BNORMAL);
	const usch *bp = ifiles->fname;

	putob(ob, '\"');
	for (; *bp; bp++) {
		if (*bp == '\"' || *bp == '\'' || *bp == '\\')
			putob(ob, '\\');
		putob(ob, *bp);
	}
	putob(ob, '\"');
	return ob;
}

/*
 * Version of fastnum that reads from a string and saves in ob.
 * We know that it is a number before calling this routine.
 */
static void
fstrnum(struct iobuf *ib, struct iobuf *ob)  
{	
	usch *s = ib->buf+ib->cptr;

	if (*s == '.') {
		/* not digit, dot.  Next will be digit */
		putob(ob, *s++);
	}
	for (;;) {
		putob(ob, *s++);
		if ((spechr[*s] & C_EP)) {
			if (s[1] != '-' && s[1] != '+')
				break;
			putob(ob, *s++);
		} else if ((*s != '.') && ((spechr[*s] & C_ID) == 0))
			break;
	}
	ib->cptr = s - ib->buf;
}

/*
 * get a string or character constant.
 * similar to faststr.
 */
static void
fstrstr(struct iobuf *ib, struct iobuf *ob)
{
	usch *s = ib->buf+ib->cptr;
	int ch;

	if (*s == 'L' || *s == 'U' || *s == 'u')
		putob(ob, *s++);
	if (*s == '8')
		putob(ob, *s++);
	ch = *s;
	putob(ob, *s++);
	while (*s != ch) {
		if (*s == '\\')
			putob(ob, *s++);
		putob(ob, *s++);
	}
	putob(ob, *s++);
	ib->cptr = s - ib->buf;
}

/*
 * Save standard comments if found.
 */
static void
fcmnt(struct iobuf *ib, struct iobuf *ob)
{
	usch *s = ib->buf+ib->cptr;

	putob(ob, *s++); /* / */
	putob(ob, *s++); /* * */
	for (;;s++) {
		putob(ob, *s);
		if (s[-1] == '*' && *s == '/')
			break;
	}
	ib->cptr = s - ib->buf + 1;
}

static int
getyp(usch *s)
{

	if (ISID0(*s)) return IDENT;
	if ((*s == 'L' || *s == 'U' || *s == 'u') &&
	    (s[1] == '\'' || s[1] == '\"')) return STRING;
	if (s[0] == 'u' && s[1] == 'U' && s[2] == '\"') return STRING;
	if (s[0] == '\'' || s[0] == '\"') return STRING;
	if (spechr[*s] & C_DIGIT) return NUMBER;
	if (*s == '.' && (spechr[s[1]] & C_DIGIT)) return NUMBER;
	if (*s == '/' && (s[1] == '/' || s[1] == '*')) return CMNT;
	return *s;
	
}

/*
 * Check ib and print out the symbols there.
 * If expandable symbols found recurse and expand them.
 * If last identifier on the input list is expandable return it.
 * Expect ib to be zero-terminated.
 */
static struct symtab *
loopover(struct iobuf *ib, struct iobuf *ob)
{
	struct iobuf *xb, *xob;
	struct symtab *sp;
	usch *cp;
	int l, c, t, cn;

	ib->cptr = 0; /* start from beginning */
#ifdef PCC_DEBUG
	if (dflag) {
		printf("loopover: '");
		prline(ib->buf+ib->cptr);
		printf("'\n");
	}
#endif

	xb = getobuf(BNORMAL);
	while ((c = ib->buf[ib->cptr])) {
		switch (t = getyp(ib->buf+ib->cptr)) {
		case CMNT:
			fcmnt(ib, ob);
			continue;
		case NUMBER:
			fstrnum(ib, ob);
			continue;
		case STRING:
			xb->cptr = 0;
			fstrstr(ib, xb);
			xb->buf[xb->cptr] = 0;
			for (cp = xb->buf; *cp; cp++) {
				if (*cp <= BLKID) {
					if (*cp == BLKID)
						cp++;
					continue;
				}
				putob(ob, *cp);
			}
			continue;
		case BLKID:
			l = ib->buf[ib->cptr+1];
			ib->cptr+=2;
			/* FALLTHROUGH */
		case IDENT:
			if (t != BLKID)
				l = 0;
			/*
			 * Tricky: if this is the last identifier
			 * in the expanded list, and it is defined
			 * as a function-like macro, then push it
			 * back on the input stream and let fastscan
			 * handle it as a new macro.
			 * BUT: if this macro is blocked then this
			 * should not be done.
			 */
			for (cn = ib->cptr;
			    ISID(ib->buf[ib->cptr]); ib->cptr++)
				;
			if ((sp = lookup(cn+ib->buf, FIND)) == NULL) {
sstr:				for (; cn < ib->cptr; cn++)
					putob(ob, ib->buf[cn]);
				continue;
			}
			if (expok(sp, l) == 0) {
				/* blocked */
				goto sstr;
			} else {
				if (sp->type != OBJCT) {
					cn = ib->cptr;
					while (ISWS(ib->buf[ib->cptr]))
						ib->cptr++;
					if (ib->buf[ib->cptr] == 0) {
						bufree(xb);
						return sp;
					}
					ib->cptr = cn;
				}
newmac:				if ((xob = submac(sp, 1, ib, NULL)) == NULL) {
					strtobuf((usch *)sp->namep, ob);
				} else {
					sp = loopover(xob, ob);
					bufree(xob);
					if (sp != NULL)
						goto newmac;
				}
			}
			continue;
		default:
			putob(ob, c);
		}

		ib->cptr++;
	}

	bufree(xb);
	DPRINT(("loopover return 0\n"));
	return 0;
}

/*
 * Handle defined macro keywords found on input stream.
 * When finished print out the full expanded line.
 * Input here is from the lex buffer.
 * Return 1 if success, 0 otherwise.
 * Scanned data is stored on heap.  Last scan prints out the buffer.
 */
struct iobuf *
kfind(struct symtab *sp)
{
	extern int inexpr;
	struct blocker *bl;
	struct iobuf *ib, *ob, *outb, *ab;
	const usch *argary[MAXARGS+1];
	int c, n = 0;

	blkidp = 1;
	outb = NULL;
	DPRINT(("%d:enter kfind(%s)\n",0,sp->namep));
	DPRINT(("%d:enter kfind2(%s)\n",0,sp->value));
	switch ((unsigned int)sp->type) {
	case FILLOC:
		ob = unfname();
		return ob;

	case LINLOC:
		return bsheap(NULL, "%d", ifiles->lineno);

	case PRAGLOC:
		pragoper(NULL);
		return getobuf(BNORMAL);

	case DEFLOC:
	case OBJCT:
		bl = blkget(sp, NULL);
		ib = mkrobuf(sp->value);
		ob = getobuf(BNORMAL);
		ob = exparg(1, ib, ob, bl);
		bufree(ib);
		break;

	case CTRLOC:
		return bsheap(NULL, "%d", counter++);

	default:
		/* Search for '(' */
		while (ISWSNL(c = cinput()))
			if (c == '\n')
				n++;
		if (c != '(') {
			if (inexpr == 0)
				putstr(sp->namep);
			if (n == 0)
				putch(' ');
			else for (ifiles->lineno += n; n; n--)
				putch('\n');
			cunput(c);
			return 0; /* Failed */
		}

		/* fetch arguments */
again:		if ((ab = readargs1(sp, argary)) == 0)
			error("readargs");

		bl = blkget(sp, NULL);
		ib = subarg(sp, argary, 1, bl);
		bufree(ab);
		ob = getobuf(BNORMAL);
		ob = exparg(1, ib, ob, bl);
		bufree(ib);
		break;
	}

	/*
	 * Loop over ob, output the data and remove remaining  
	 * directives.  Start with extracting the last keyword (if any).
	 */
	putob(ob, 0); /* XXX needed? */

	if (outb == NULL)
		outb = getobuf(BNORMAL);

	if ((sp = loopover(ob, outb))) {
		/* Search for '(' */
		while (ISWSNL(c = cinput()))
			if (c == '\n')
				n++;
		if (c == '(') {
			bufree(ob);
			goto again;
		}
		cunput(c);
		strtobuf((usch *)sp->namep, outb);
	}
	bufree(ob);

	for (ifiles->lineno += n; n; n--)
		putob(outb, '\n');
	if (nbufused != 1)
		error("lost buffer");
	return outb;
}

/*
 * Replace and push-back on input stream the eventual replaced macro.
 * The check for whether it can expand or not should already have been done.
 * Blocks for this identifier will be added via insblock() after expansion.
 * The same as kfind but read a string.
 */
struct iobuf *
submac(struct symtab *sp, int lvl, struct iobuf *ib, struct blocker *obl)
{
	struct blocker *bl;
	struct iobuf *ob, *ab;
	const usch *argary[MAXARGS+1];
	int cn;

	DPRINT(("%d:submac: trying '%s'\n", lvl, sp->namep));
	switch ((unsigned int)sp->type) {
	case FILLOC:
		ob = unfname();
		break;
	case LINLOC:
		ob = bsheap(NULL, "%d", ifiles->lineno);
		break;
	case PRAGLOC:
		pragoper(ib);
		ob = getobuf(BNORMAL);
		break;
	case OBJCT:
		bl = blkget(sp, obl);
		ib = mkrobuf(sp->value);
		ob = getobuf(BNORMAL);
		DPRINT(("%d:submac: calling exparg\n", lvl));
		ob = exparg(lvl+1, ib, ob, bl);
		bufree(ib);
		DPRINT(("%d:submac: return exparg\n", lvl));
		break;
	case CTRLOC:
		ob = bsheap(NULL, "%d", counter++);
		break;
	default:
		cn = ib->cptr;
		while (ISWSNL(ib->buf[ib->cptr]))
			ib->cptr++;
		if (ib->buf[ib->cptr] != '(') {
			ib->cptr = cn;
			return 0;
		}
		cn = ib->cptr++;
		if ((ab = readargs2(ib, sp, argary)) == 0) {
			/* Bailed out in the middle of arg list */
			ib->cptr = cn; /* XXX */
			return 0;
		}
		bl = blkget(sp, obl);
		ib = subarg(sp, argary, lvl+1, bl);
		bufree(ab);

		ob = getobuf(BNORMAL);
		DPRINT(("%d:submac(: calling exparg\n", lvl));
		ob = exparg(lvl+1, ib, ob, bl);
		bufree(ib);
		DPRINT(("%d:submac(: return exparg\n", lvl));
		break;
	}
	putob(ob, 0);
	ob->cptr--;

	return ob;
}

static int
isdir(void)
{
	usch ch;

	while ((ch = cinput()) == ' ' || ch == '\t')
		;
	if (ch == '#')
		return 1;
	cunput(ch);
	return 0;
}

/*
 * Deal with directives inside a macro.
 * Doing so is really ugly but gcc allows it, so...
 */
static void
chkdir(void)
{
	usch ch;

	for (;;) {
		if (isdir()) {
#ifndef GCC_COMPAT
			warning("conditionals inside macro arg list");
#endif
			ppdir();
		}
		if (flslvl == 0)
			return;
		while ((ch = cinput()) != '\n')
			;
		ifiles->lineno++;
		putch('\n');
	}
}

static int
ra1_wsnl(void)
{
	int c;

	while (ISWSNL(c = cinput())) {
		if (c == '\n') {
			putch('\n');
			chkdir();
			ifiles->lineno++;
		}
	}
	return c;
}

/*
 * Read arguments and put in argument array.
 * If EOF is encountered return 1, otherwise 0.
 */
struct iobuf *
readargs1(struct symtab *sp, const usch **args)
{
	struct iobuf *ab;
	const usch *vp = sp->value;
	int c, i, j, plev, narg, ellips = 0;
	int argary[MAXARGS+1];

	DPRINT(("readargs1\n"));
	narg = sp->narg;
	ellips = sp->type == VARG;

#ifdef PCC_DEBUG
	if (dflag > 1) {
		printf("narg %d varg %d: ", narg, ellips);
		prrep(vp);
		printf("\n");
	}
#endif

	/*
	 * read arguments and store them on heap.
	 */
	ab = getobuf(BNORMAL);
	c = '(';
	for (i = 0; i < narg && c != ')'; i++) {
		argary[i] = ab->cptr;
		plev = 0;

		c = ra1_wsnl();
		for (;;) {
			if (plev == 0 && (c == ')' || c == ','))
				break;
			if (c == '(') plev++;
			if (c == ')') plev--;
			if (c == 0)
				error("eof in macro");
			else if (c == '/') {
				int mp = macpos;
				Ccmnt(macsav);
				macsav(0);
				strtobuf(macbase+mp, ab);
				macpos = mp;
			} else if (c == '\"' || c == '\'') {
				faststr(c, ab);
			} else if (ISID0(c)) {
				int mp = ab->cptr;
				bufid(c, ab);
				if ((sp = lookup(ab->buf+mp, FIND)) != NULL &&
				    (sp == linloc || sp == ctrloc)) {
					ab->cptr = mp;
					bsheap(ab,"%d", (sp == linloc ?
					ifiles->lineno : counter++));
				}
			} else
				putob(ab, c);
			if ((c = cinput()) == '\n') {
				chkdir();
				ifiles->lineno++, putch(c), c = ' ';
			}
		}

		while (argary[i] < ab->cptr && ISWSNL(ab->buf[ab->cptr-1]))
			ab->cptr--;
		putob(ab, '\0');
#ifdef PCC_DEBUG
		if (dflag) {
			printf("readargs: save arg %d '", i);
			prline(ab->buf+argary[i]);
			printf("'\n");
		}
#endif
	}

	/* Handle varargs readin */
	argary[i] = ab->cptr;
	putob(ab, 0);
	ab->cptr--;
	if (ellips && c != ')') {
		plev = 0;
		c = ra1_wsnl();
		for (;;) {
			if (plev == 0 && c == ')')
				break;
			if (c == '(') plev++;
			if (c == ')') plev--;
			if (c == '\"' || c == '\'') {
				faststr(c, ab);
			} else
				putob(ab, c);
			if ((c = cinput()) == '\n')
				ifiles->lineno++, c = ' ';
		}
		while (argary[i] < ab->cptr && ISWSNL(ab->buf[ab->cptr-1]))
			ab->cptr--;
		putob(ab, '\0');
#ifdef PCC_DEBUG
		if (dflag) {
			printf("readargs: vararg arg %d '", i);
			prline(ab->buf+argary[i]);
			printf("'\n");
		}
#endif
	}
	if (ellips)
		i++;
	if (narg == 0 && ellips == 0)
		c = ra1_wsnl();

	if (c != ')' || (i != narg && ellips == 0) || (i < narg && ellips == 1))
		error("wrong arg count");
	for (j = 0; j < i; j++)
		args[j] = ab->buf + argary[j];
	return ab;
}

static struct iobuf *rabuf;
static int
raread(void)
{
	int rv;

	if (rabuf) {
		if ((rv = rabuf->buf[rabuf->cptr]))
			rabuf->cptr++;
	} else
		rv = cinput();
	return rv;
}

/*
 * Read arguments and put in argument array.
 * If EOF is encountered return 1, otherwise 0.
 */
struct iobuf *
readargs2(struct iobuf *in, struct symtab *sp, const usch **args)
{
	struct iobuf *ab;
	const usch *vp = sp->value;
	int argary[MAXARGS+1];
	int c, i, j, plev, narg, ellips = 0;

	DPRINT(("readargs2 %s '", sp->namep));
#ifdef PCC_DEBUG
	if (dflag) {
		prline(in->buf+in->cptr);
		printf("'\n");
	}
#endif
	rabuf = in;
	narg = sp->narg;
	ellips = sp->type == VARG;

#ifdef PCC_DEBUG
	if (dflag > 1) {
		prrep(vp);
		printf("\n");
	}
#endif

	/*
	 * read arguments and store them on heap.
	 */
	ab = getobuf(BNORMAL);
	c = '(';
	for (i = 0; i < narg && c != ')'; i++) {
		argary[i] = ab->cptr;
		plev = 0;

		while ((c = raread()) == ' ' || c == '\t')
			;
		for (;;) {
			if (plev == 0 && (c == ')' || c == ','))
				break;
			if (c == '(') plev++;
			if (c == ')') plev--;
			if (c == 0) {
				if (rabuf) {
					rabuf = 0;
				} else
					error("eof in macro");
			} else if (c == BLKID) {
				putob(ab, c), putob(ab, raread());
			} else if (c == '/') {
				if ((c = raread()) == '*')
					error("FIXME ccmnt");
				putob(ab, '/');
				continue;
			} else if (c == '\"' || c == '\'') {
				if (rabuf) {
					rabuf->cptr--;
					fstrstr(rabuf, ab);
				} else {
					faststr(c, ab);
				}
			} else if (ISID0(c)) {
				int mp = ab->cptr;
				do {
					putob(ab, c);
				} while (ISID(c = raread()));
				ab->buf[ab->cptr] = 0;
				if ((sp = lookup(ab->buf+mp, FIND)) &&
				    (sp == linloc)) {
					ab->cptr = mp;
					bsheap(ab, "%d", ifiles->lineno);
				}
				continue;
			} else
				putob(ab, c);
			c = raread();
		}

		while (argary[i] < ab->cptr && ISWSNL(ab->buf[ab->cptr-1]))
			ab->cptr--;
		putob(ab, '\0');
#ifdef PCC_DEBUG
		if (dflag) {
			printf("readargs2: save arg %d '", i);
			prline(ab->buf+argary[i]);
			printf("'\n");
		}
#endif
	}

	/* Handle varargs readin */
	if (ellips)
		args[i] = (const usch *)"";
	if (ellips && c != ')') {
		argary[i] = ab->cptr;
		plev = 0;
		while ((c = raread()) == ' ' || c == '\t')
			;
		for (;;) {
			if (plev == 0 && c == ')')
				break;
			if (c == '(') plev++;
			if (c == ')') plev--;
			if (c == '\"' || c == '\'') {
				if (rabuf) {
					rabuf->cptr--;
					fstrstr(rabuf, ab);
				} else {
					faststr(c, ab);
				}
			} else
				putob(ab, c);
			c = raread();
		}
		while (argary[i] < ab->cptr && ISWSNL(ab->buf[ab->cptr-1]))
			ab->cptr--;
		putob(ab, '\0');
		i++;
	}
	if (narg == 0 && ellips == 0) {
		while ((c = raread()) == ' ' || c == '\t')
			;
	}

	if (c != ')' || (i != narg && ellips == 0) || (i < narg && ellips == 1))
		error("wrong arg count");
	for (j = 0; j < i; j++)
		args[j] = ab->buf + argary[j];
	return ab;
}

/*
 * expand a function-like macro.
 * vp points to end of replacement-list
 * reads function arguments from input stream.
 * result is pushed-back for more scanning.
 */
struct iobuf *
subarg(struct symtab *nl, const usch **args, int lvl, struct blocker *bl)
{
	struct blocker *w;
	struct iobuf *ob, *cb, *nb;
	int narg, instr, snuff;
	const usch *sp, *bp, *ap, *vp, *tp;

	DPRINT(("%d:subarg '%s'\n", lvl, nl->namep));
	ob = getobuf(BNORMAL);
	vp = nl->value;
	narg = nl->narg;

	sp = vp;
	instr = snuff = 0;
#ifdef PCC_DEBUG
	if (dflag>1) {
		printf("%d:subarg ARGlist for %s: '", lvl, nl->namep);
		prrep(vp);
		printf("' ");
		for (w = bl; w; w = w->next)
			printf("%s ", w->sp->namep);
		printf("\n");
	}
#endif

	/*
	 * walk forward over replacement-list while replacing
	 * arguments.  Arguments are macro-expanded if required.
	 */
	while (*sp) {
		if (*sp == SNUFF)
			putob(ob, '\"'), snuff ^= 1;
		else if (*sp == CONC)
			;
		else if (*sp == WARN) {

			if (sp[1] == VARG) {
				bp = ap = args[narg];
				sp++;
#ifdef GCC_COMPAT
			} else if (sp[1] == GCCARG) {
				/* XXX remove last , not add 0 */
				ap = args[narg];
				if (ap[0] == 0)
					ap = (const usch *)"0";
				bp = ap;
				sp++;
#endif
			} else
				bp = ap = args[(int)*++sp];
#ifdef PCC_DEBUG
			if (dflag>1){
				printf("%d:subarg GOTwarn; arglist '", lvl);
				prline(bp);
				printf("'\n");
			}
#endif
			if (sp[-2] != CONC && !snuff && sp[1] != CONC) {
				/*
				 * Expand an argument; 6.10.3.1:
				 * "A parameter in the replacement list,
				 *  is replaced by the corresponding argument
				 *  after all macros contained therein have
				 *  been expanded.".
				 */
				w = bl ? bl->next : NULL;
				cb = mkrobuf(bp);
				nb = getobuf(BNORMAL);
				DPRINT(("%d:subarg: calling exparg\n", lvl));
				nb = exparg(lvl+1, cb, nb, w);
				DPRINT(("%d:subarg: return exparg\n", lvl));
				bufree(cb);
				strtobuf(nb->buf, ob);
				bufree(nb);
			} else {
				while (*bp) {
					if (snuff && !instr && ISWS(*bp)) {
						while (ISWS(*bp))
							bp++;
						putob(ob, ' ');
					}

					if (snuff &&
					    (*bp == '\'' || *bp == '"')) {
						instr ^= 1;
						for (tp = bp - 1; *tp == '\\'; tp--)
							instr ^= 1;
						if (*bp == '"')
							putob(ob, '\\');
					} 
					if (snuff && instr && *bp == '\\')
						putob(ob, '\\');
					putob(ob, *bp);
					bp++;
				}
			}
		} else if (ISID0(*sp)) {
			if (lookup(sp, FIND))
				storeblk(blkix(bl), ob);
			while (ISID(*sp))
				putob(ob, *sp++);
			sp--;
		} else
			putob(ob, *sp);
		sp++;
	}
	putob(ob, 0);
	ob->cptr = 0;
	DPRINT(("%d:subarg retline %s\n", lvl, ob->buf));
	return ob;
}

/*
 * Do a (correct) expansion of a WARN-terminated buffer of tokens.
 * Data is read from the lex buffer, result on lex buffer, WARN-terminated.
 * Expansion blocking is not altered here unless when tokens are
 * concatenated, in which case they are removed.
 */
struct iobuf *
exparg(int lvl, struct iobuf *ib, struct iobuf *ob, struct blocker *bl)
{
	extern int inexpr;
	struct iobuf *nob, *tb;
	struct symtab *nl;
	int c, m;
	usch *cp;

	DPRINT(("%d:exparg: entry ib %s\n", lvl, ib->buf+ib->cptr));
#ifdef PCC_DEBUG
	if (dflag > 1) {
		printf("exparg entry: full ");
		prline(ib->buf+ib->cptr);
		printf("\n");
	}
#endif

	while ((c = getyp(ib->buf+ib->cptr)) != 0) {
		ib->cptr++;

		switch (c) {

		case CMNT:
			ib->cptr--;
			fcmnt(ib, ob);
			break;
		case NUMBER:
			ib->cptr--;
			fstrnum(ib, ob);
			break;
		case STRING:
			ib->cptr--;
			fstrstr(ib, ob);
			break;
		case BLKID:
			m = ib->buf[ib->cptr++];
			ib->cptr++;
			/* FALLTHROUGH */
		case IDENT:
			if (c != BLKID)
				m = 0;
			tb = getobuf(BNORMAL);
			cp = ib->buf+ib->cptr-1;
			for (; ISID(*cp) || *cp == BLKID; cp++) {
				if (*cp == BLKID) {
					/* XXX add to block list */
					cp++;
				} else
					putob(tb, *cp);
			}
			tb->buf[tb->cptr] = 0;
			ib->cptr = cp - ib->buf;

			/* Any match? */
			if ((nl = lookup(tb->buf, FIND)) == NULL) {
				buftobuf(tb, ob);
			} else if (inexpr && nl->type == DEFLOC) {
				/* Used in #if stmts */
				int gotlp = 0;

				cp = ib->buf+ib->cptr;
				while (ISWS(*cp)) cp++;
				if (*cp == '(')
					gotlp++, cp++;
				while (ISWS(*cp)) cp++;
				if (!ISID0(*cp))
					error("bad defined");
				putob(ob, lookup(cp, FIND) ? '1' : '0');
				while (ISID(*cp)) cp++;
				while (ISWS(*cp)) cp++;
				if (gotlp && *cp != ')')
					error("bad defined");
				cp++;
				ib->cptr = cp - ib->buf;
			} else if (expokb(nl, bl) && expok(nl, m) &&
			    (nob = submac(nl, lvl+1, ib, bl))) {
				if (nob->buf[0] == '-' || nob->buf[0] == '+')
					putob(ob, ' ');
				strtobuf(nob->buf, ob);
				if (ob->cptr > 0 &&
				    (ob->buf[ob->cptr-1] == '-' ||
				     ob->buf[ob->cptr-1] == '+'))
					putob(ob, ' ');
				bufree(nob);
			} else {
				storeblk(blkix(mergeadd(bl, m)), ob);
				buftobuf(tb, ob);
			}
			bufree(tb);
			break;

		default:
			putob(ob, c);
			break;
		}
	}
	putob(ob, 0);
	ob->cptr--;
	DPRINT(("%d:exparg return: ob %s\n", lvl, ob->buf));
#ifdef PCC_DEBUG
	if (dflag > 1) {
		printf("%d:exparg: full ", lvl);
		prline(ob->buf);
		printf("\n");
	}
#endif
	return ob;
}

#ifdef PCC_DEBUG

static void
prrep(const usch *s)
{
	while (*s) {
		switch (*s) {
		case WARN:
			if (s[1] == VARG) printf("<VARG>");
			else if (s[1] == GCCARG) printf("<GCCARG>");
			else printf("<ARG(%d)>", s[1]);
			s++;
			break;
		case CONC: printf("<CONC>"); break;
		case SNUFF: printf("<SNUFF>"); break;
		case BLKID: printf("<BLKID(%d)>",s[1]); s++; break;
		default: printf("%c", *s); break;
		}
		s++;
	}
}

static void
prline(const usch *s)
{
	while (*s) {
		switch (*s) {
		case BLKID: printf("<BLKID(%d)>", *++s); break;
		case WARN: printf("<WARN>"); break;
		case CONC: printf("<CONC>"); break;
		case SNUFF: printf("<SNUFF>"); break;
		case '\n': printf("<NL>"); break;
		default: 
			if (*s > 0x7f)
				printf("<0x%x>", *s);
			else
				printf("%c", *s);
			break;
		}
		s++;
	}
}
#endif

void
putch(int ch)
{
	putob(&pb, ch);
}

void
putstr(const usch *s)
{
	strtobuf(s, &pb);
}

/*
 * convert a number to an ascii string. Store it on the heap.
 */
static void
num2str(struct iobuf *ob, int num)
{
	static usch buf[12];
	usch *b = buf;
	int m = 0;

	if (num < 0)
		num = -num, m = 1;
	do {
		*b++ = (usch)(num % 10 + '0');
		num /= 10;
	} while (num);
	if (m)
		*b++ = '-';
	while (b > buf)
		putob(ob, *--b);
}

/*
 * similar to sprintf, but only handles %c, %s and %d.
 * saves result on heap.
 */
static void
vsheap(struct iobuf *ob, const char *fmt, va_list ap)
{
	for (; *fmt; fmt++) {
		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
			case 's':
				strtobuf(va_arg(ap, usch *), ob);
				break;
			case 'd':
				num2str(ob, va_arg(ap, int));
				break;
			case 'c':
				putob(ob, va_arg(ap, int));
				break;
			default:
				error("bad sheap");
			}
		} else
			putob(ob, *fmt);
	}
	putob(ob, 0);
	ob->cptr--;
}

struct iobuf *
bsheap(struct iobuf *ob, const char *fmt, ...)
{
	va_list ap;

	if (ob == NULL)
		ob = getobuf(BNORMAL);

	va_start(ap, fmt);
	vsheap(ob, fmt, ap);
	va_end(ap);

	return ob;
}

static void
usage(void)
{
	error("Usage: cpp [-Cdt] [-Dvar=val] [-Uvar] [-Ipath] [-Spath]");
}

#ifdef notyet
/*
 * Symbol table stuff.
 * The data structure used is a patricia tree implementation using only
 * bytes to store offsets.
 * The information stored is (lower address to higher):
 *
 *	unsigned char bitno[2]; bit number in the string
 *	unsigned char left[3];	offset from base to left element
 *	unsigned char right[3];	offset from base to right element
 */
#endif

/*
 * This patricia implementation is more-or-less the same as
 * used in ccom for string matching.
 */
struct tree {
	int bitno;
	struct tree *lr[2];
};

#define BITNO(x)		((x) & ~(LEFT_IS_LEAF|RIGHT_IS_LEAF))
#define LEFT_IS_LEAF		0x80000000
#define RIGHT_IS_LEAF		0x40000000
#define IS_LEFT_LEAF(x)		(((x) & LEFT_IS_LEAF) != 0)
#define IS_RIGHT_LEAF(x)	(((x) & RIGHT_IS_LEAF) != 0)
#define P_BIT(key, bit)		(key[bit >> 3] >> (bit & 7)) & 1
#define CHECKBITS		8

static struct tree *sympole;
static int numsyms;

static struct tree *
gtree(void)
{
	static int ntrees;
	static struct tree *tp;

	if (ntrees == 0) {
		tp = xmalloc(CPPBUF);
		ntrees = CPPBUF/sizeof(*tp);
	}
	return &tp[--ntrees];
}

/*
 * Allocate a symtab struct and store the string.
 */
static struct symtab *
getsymtab(const usch *str)
{
	static int nsyms;
	static struct symtab *spp;
	struct symtab *sp;

	if (nsyms == 0) {
		spp = xmalloc(CPPBUF);
		nsyms = CPPBUF/sizeof(*sp);
	}
	sp = &spp[--nsyms];

	sp->namep = str;
	sp->value = NULL;
	sp->file = ifiles ? ifiles->orgfn : (const usch *)"<initial>";
	sp->line = ifiles ? ifiles->lineno : 0;
	return sp;
}

/*
 * Do symbol lookup in a patricia tree.
 * Only do full string matching, no pointer optimisations.
 */
struct symtab *
lookup(const usch *key, int enterf)
{
	struct symtab *sp;
	struct tree *w, *new, *last;
	int len, cix, bit, fbit, svbit, ix, bitno;
	const usch *k, *m;

	/* Count full string length */
	for (k = key, len = 0; ISID(*k) & C_ID; k++, len++)
		;

	switch (numsyms) {
	case 0: /* no symbols yet */
		if (enterf != ENTER)
			return NULL;
		sympole = (struct tree *)getsymtab(key);
		numsyms++;
		return (struct symtab *)sympole;

	case 1:
		w = sympole;
		svbit = 0; /* XXX gcc */
		break;

	default:
		w = sympole;
		bitno = len * CHECKBITS;
		for (;;) {
			bit = BITNO(w->bitno);
			fbit = bit >= bitno ? 0 : P_BIT(key, bit);
			svbit = fbit ? IS_RIGHT_LEAF(w->bitno) :
			    IS_LEFT_LEAF(w->bitno);
			w = w->lr[fbit];
			if (svbit)
				break;
		}
	}

	sp = (struct symtab *)w;

	m = sp->namep;
	k = key;

	/* Check for correct string and return */
	for (cix = 0; *m && ISID(*k) && *m == *k; m++, k++, cix += CHECKBITS)
		;
	if (*m == 0 && ISID(*k) == 0) {
		if (enterf != ENTER && sp->value == NULL)
			return NULL;
		return sp;
	}

	if (enterf != ENTER)
		return NULL; /* no string found and do not enter */

	ix = *m ^ *k;
	while ((ix & 1) == 0)
		ix >>= 1, cix++;

	/* Create new node */
	new = gtree();
	bit = P_BIT(key, cix);
	new->bitno = cix | (bit ? RIGHT_IS_LEAF : LEFT_IS_LEAF);
	new->lr[bit] = (struct tree *)getsymtab(key);

	if (numsyms++ == 1) {
		new->lr[!bit] = sympole;
		new->bitno |= (bit ? LEFT_IS_LEAF : RIGHT_IS_LEAF);
		sympole = new;
		return (struct symtab *)new->lr[bit];
	}

	w = sympole;
	last = NULL;
	for (;;) {
		fbit = w->bitno;
		bitno = BITNO(w->bitno);
		if (bitno == cix)
			error("bitno == cix");
		if (bitno > cix)
			break;
		svbit = P_BIT(key, bitno);
		last = w;
		w = w->lr[svbit];
		if (fbit & (svbit ? RIGHT_IS_LEAF : LEFT_IS_LEAF))
			break;
	}

	new->lr[!bit] = w;
	if (last == NULL) {
		sympole = new;
	} else {
		last->lr[svbit] = new;
		last->bitno &= ~(svbit ? RIGHT_IS_LEAF : LEFT_IS_LEAF);
	}
	if (bitno < cix)
		new->bitno |= (bit ? LEFT_IS_LEAF : RIGHT_IS_LEAF);
	return (struct symtab *)new->lr[bit];
}

void *
xmalloc(int sz)
{
	usch *rv;

	if ((rv = (void *)malloc(sz)) == NULL)
		error("xmalloc: out of mem");
	return rv;
}

void *
xrealloc(void *p, int sz)
{
	usch *rv;

	if ((rv = (void *)realloc(p, sz)) == NULL)
		error("xrealloc: out of mem");
	return rv;
}

static usch *
xstrdup(const usch *str)
{
	usch *rv;

	if ((rv = (usch *)strdup((const char *)str)) == NULL)
		error("xstrdup: out of mem");
	return rv;
}


