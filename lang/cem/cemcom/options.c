/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	U S E R	  O P T I O N - H A N D L I N G		*/

#include	"lint.h"
#include	"botch_free.h"
#include	<alloc.h>
#include	"nofloat.h"
#include	"nopp.h"
#include	"idfsize.h"
#include	"nobitfield.h"
#include	"class.h"
#include	"macro.h"
#include	"idf.h"
#include	"arith.h"
#include	"sizes.h"
#include	"align.h"
#include	"use_tmp.h"
#include	"dataflow.h"
#include	"noRoption.h"
#include	"dbsymtab.h"

#ifndef NOPP
extern char **inctable;
extern int inc_pos;
extern int inc_max;
extern int inc_total;
int do_dependencies = 0;
char *dep_file = 0;

#endif /* NOPP */

char options[128];			/* one for every char	*/
#ifdef	LINT
char loptions[128];			/* one for every char	*/
#endif	/* LINT */

extern int idfsize;
extern int density;

static int txt2int();

do_option(text)
	char *text;
{
	register char opt;

next_option:			/* to allow combined one-char options */
	switch (opt = *text++)	{

	case 0:			/* to end the goto next_option loop */
		break;

	default:
#ifndef	LINT
		fatal("illegal option: %c", opt);
#else	/* LINT */
		warning("illegal option: %c", opt);
#endif	/* LINT */
		break;

	case '-':
		options[*text++] = 1;	/* flags, debug options etc.	*/
		goto next_option;

#ifndef	LINT
#ifdef	DATAFLOW
	case 'd':
#endif	/* DATAFLOW */
	case 'p':			/* procentry/procexit */
	case 'L' :			/* no fil/lin */
	case 'n':			/* use no registers */
	case 'w':			/* no warnings will be given */
		options[opt] = 1;
		goto next_option;
#endif	/* LINT */

#ifdef	LINT
	case 'h':	/* heuristic tests */
	case 'v':	/* no complaints about unused arguments */
	case 'a':	/* check long->int int->long conversions */
	case 'b':	/* don't report unreachable break-statements */
	case 'x':	/* complain about unused extern declared variables */
	case 'u':	/* no "used but not defined"; for pass 2 */
	case 'L':	/* lintlibrary */
		loptions[opt] = 1;
		goto next_option;
#endif	/* LINT */

#ifndef LINT
#ifndef NOPP
	case 'A' :	/* Amake dependency generation */
		do_dependencies = 1;
		if (*text) {
			dep_file = text;
		}
		break;
	case 'i':
	case 'm':
		options[opt] = 1;
		break;
#endif /* NOPP */
#endif /* LINT */
#ifdef DBSYMTAB
	case 'g':	/* symbol table for debugger */
		options['g'] = 1;
		options['n'] = 1;
		break;
#endif /* DBSYMTAB */

	case 'R':			/* strict version */
#ifndef	NOROPTION
		options[opt] = 1;
#else	/* NOROPTION */
		warning("-R option not implemented");
#endif	/* NOROPTION */
		goto next_option;

#ifdef	___XXX___
deleted, is now a debug-flag
	case 'C' :	/* E option + comment output		*/
#ifndef	NOPP
		options['E'] = 1;
		warning("-C: comment is not output");
#else /* NOPP */
		warning("-C option ignored");
#endif	/* NOPP */
		break;
#endif	/* ___XXX___ */

	case 'D' :	{	/* -Dname :	predefine name		*/
#ifndef NOPP
		register char *cp = text, *name, *mactext;

		if (class(*cp) != STIDF)	{
			error("identifier missing in -D%s", text);
			break;
		}

		name = cp;

		while (*cp && in_idf(*cp)) {
			++cp;
		}

		if (!*cp) {			/* -Dname */
			mactext = "1";
		}
		else
		if (*cp == '=')	{		/* -Dname=text	*/
			*cp++ = '\0';		/* end of name	*/
			mactext = cp;
		}
		else	{			/* -Dname?? */
			error("malformed option -D%s", text);
			break;
		}

		macro_def(str2idf(name), mactext, -1, strlen(mactext),
			NOFLAG);
#else /* NOPP */
		warning("-D option ignored");
#endif /* NOPP */
		break;
	}

#ifdef ___XXX___
deleted, is now a debug-flag
	case 'E' :	/* run preprocessor only, with #<int>	*/
#ifndef NOPP
		options['E'] = 1;
#else /* NOPP */
		warning("-E option ignored");
#endif /* NOPP */
		break;
#endif /* ___XXX___ */

	case 'I' :	/* -Ipath : insert "path" into include list	*/
#ifndef NOPP
		if (*text)	{
			int i;
			register char *new = text;

			if (++inc_total > inc_max) {
				inctable = (char **)
				  Realloc(inctable,(inc_max+=10)*sizeof(char *));
			}

			for (i = inc_pos++; i < inc_total; i++) {
				char *tmp = inctable[i];

				inctable[i] = new;
				new = tmp;
			}
		}
		else inctable[inc_pos] = 0;
#else /* NOPP */
		warning("-I option ignored");
#endif /* NOPP */
		break;

	case 'M':	/* maximum identifier length */
		idfsize = txt2int(&text);
		if (*text || idfsize <= 0)
			fatal("malformed -M option");
		if (idfsize > IDFSIZE)
			fatal("maximum identifier length is %d", IDFSIZE);
		break;

#ifdef ___XXX___
deleted, is now a debug-flag
	case 'P' :	/* run preprocessor stand-alone, without #'s	*/
#ifndef NOPP
		options['E'] = 1;
		options['P'] = 1;
#else /* NOPP */
		warning("-P option ignored");
#endif /* NOPP */
		break;
#endif /* ___XXX___ */

#ifdef	LINT
	case 'S' : {		/* -Sint :	static scope number for lint */
		extern int stat_number;
		stat_number = txt2int(&text);
		break;
	}
#endif	/* LINT */

	case 'T' : {
#ifdef USE_TMP
		extern char *C_tmpdir;
		if (*text)
			C_tmpdir = text;
		else
			C_tmpdir = ".";
#else /* USE_TMP */
		warning("-T option ignored");
#endif /* USE_TMP */
		break;
	}

	case 'U' :	{	/* -Uname :	undefine predefined	*/
#ifndef NOPP
		register struct idf *idef;

		if (*text)	{
			if ((idef = str2idf(text))->id_macro) {
				free_macro(idef->id_macro);
				idef->id_macro = (struct macro *) 0;
			}
		}
#else /* NOPP */
		warning("-U option ignored");
#endif /* NOPP */
		break;
	}

#ifndef	LINT
	case 'V' :	/* set object sizes and alignment requirements	*/
#ifdef NOCROSS
		warning("-V option ignored");
		break;
#else /* NOCROSS */
	{
		register arith sz, algn;
		char c;

		while (c = *text++)	{
			sz = txt2int(&text);
			algn = 0;
			if (*text == '.')	{
				text++;
				algn = txt2int(&text);
			}
			switch (c)	{
			case 's':	/* short	*/
				if (sz != (arith)0)
					short_size = sz;
				if (algn != 0)
					short_align = algn;
				break;
			case 'w':	/* word		*/
				if (sz != (arith)0)
					dword_size = (word_size = sz) << 1;
				if (algn != 0)
					word_align = algn;
				break;
			case 'i':	/* int		*/
				if (sz != (arith)0)
					int_size = sz;
				if (algn != 0)
					int_align = algn;
				break;
			case 'l':	/* long		*/
				if (sz != (arith)0)
					long_size = sz;
				if (algn != 0)
					long_align = algn;
				break;
			case 'f':	/* float	*/
#ifndef NOFLOAT
				if (sz != (arith)0)
					float_size = sz;
				if (algn != 0)
					float_align = algn;
#endif /* NOFLOAT */
				break;
			case 'd':	/* double	*/
#ifndef NOFLOAT
				if (sz != (arith)0)
					double_size = sz;
				if (algn != 0)
					double_align = algn;
#endif /* NOFLOAT */
				break;
			case 'p':	/* pointer	*/
				if (sz != (arith)0)
					pointer_size = sz;
				if (algn != 0)
					pointer_align = algn;
				break;
			case 'r':	/* adjust bitfields right	*/
#ifndef NOBITFIELD
				options['r'] = 1;
#else /* NOBITFIELD */
				warning("bitfields are not implemented");
#endif /* NOBITFIELD */
				break;
			case 'S':	/* initial struct alignment	*/
				if (sz != (arith)0)
					struct_align = sz;
				break;
			case 'U':	/* initial union alignment	*/
				if (sz != (arith)0)
					union_align = sz;
				break;
			default:
				error("-V: bad type indicator %c\n", c);
			}
		}
		break;
	}
#endif /* NOCROSS */
	case 'S':
		density = txt2int(&text);
		break;
#endif	/* LINT */
	}
}

static int
txt2int(tp)
	register char **tp;
{
	/*	the integer pointed to by *tp is read, while increasing
		*tp; the resulting value is yielded.
	*/
	register int val = 0, ch;

	while (ch = **tp, ch >= '0' && ch <= '9')	{
		val = val * 10 + ch - '0';
		(*tp)++;
	}
	return val;
}
