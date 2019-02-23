/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	U S E R   O P T I O N - H A N D L I N G		*/

#include	"parameters.h"
#include	<stdlib.h>
#include	<string.h>
#include	<alloc.h>
#include    "options.h"
#include	"class.h"
#include	"macro.h"
#include	"idf.h"
#include	"arith.h"
#include	"sizes.h"
#include	"align.h"
#include    "error.h"

char options[128];			/* one for every char	*/
#ifdef	LINT
char loptions[128];			/* one for every char	*/
#endif	/* LINT */

extern int idfsize;
extern int density;



static int txt2int(register char **);

void do_option(char *text)
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
		options[(int)*text++] = 1;	/* flags, debug options etc.	*/
		goto next_option;

#ifndef LINT
#endif /* LINT */
#ifdef DBSYMTAB
	case 'g':	/* symbol table for debugger */
		options['g'] = 1;
		options['n'] = 1;
		break;
#endif /* DBSYMTAB */

#ifndef	LINT
#ifdef	DATAFLOW
	case 'd':
#endif	/* DATAFLOW */
	case 'p':			/* procentry/procexit */
	case 'L' :			/* no fil/lin */
	case 'n':			/* use no registers */
	case 'w':			/* no warnings will be given */
	case 's':			/* no stricts will be given */
	case 'o':			/* no complaints about old-style */
		options[opt] = 1;
		goto next_option;
	case 'a':		/* suppress all but errors diagnostics */
		options['w'] = 1;	/* implies -a */
		options['s'] = 1;
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

	case 'M':	/* maximum identifier length */
		idfsize = txt2int(&text);
		if (*text || idfsize <= 0)
			fatal("malformed -M option");
		if (idfsize > IDFSIZE)
			fatal("maximum identifier length is %d", IDFSIZE);
		break;

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
		
#ifndef	LINT
#ifndef NOCROSS
	case 'V' :	/* set object sizes and alignment requirements	*/
	{
		register arith sz, algn;
		char c;

		while ( (c = *text++) !=0)	{
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
				if (sz != (arith)0)
					float_size = sz;
				if (algn != 0)
					float_align = algn;
				break;
			case 'd':	/* double	*/
				if (sz != (arith)0)
					double_size = sz;
				if (algn != 0)
					double_align = algn;
				break;
			case 'x':	/* long double	*/
				if (sz != (arith)0)
					lngdbl_size = sz;
				if (algn != 0)
					lngdbl_align = algn;
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
	case 'S':
		density = txt2int(&text);
		break;
#endif /* NOCROSS */
#endif	/* LINT */
	}
}

static int txt2int(register char **tp)
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
