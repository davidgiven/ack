/* $Header$ */
/*	U S E R   O P T I O N - H A N D L I N G		*/

#include	"botch_free.h"
#include	<alloc.h>
#include	"nofloat.h"
#include	"nopp.h"
#include	"idfsize.h"
#include	"maxincl.h"
#include	"nobitfield.h"
#include	"class.h"
#include	"macro.h"
#include	"idf.h"
#include	"arith.h"
#include	"sizes.h"
#include	"align.h"
#include	"use_tmp.h"
#include	"dataflow.h"

#ifndef NOPP
extern char *inctable[MAXINCL];
extern int inc_pos;
#endif NOPP

extern char options[];
extern int idfsize;
#ifdef USE_TMP
extern char *tmpfdir;	/* main.c */
#endif USE_TMP

int txt2int();

do_option(text)
	char *text;
{
	switch(*text++)	{

	default:
		fatal("illegal option: %c", *--text);

	case '-':
		options[*text] = 1;	/* flags, debug options etc.	*/
		break;

#ifdef DATAFLOW
	case 'd':
#endif DATAFLOW
	case 'p':			/* procentry/procexit */
	case 'L' :			/* no fil/lin */
	case 'n':			/* use no registers */
	case 'w':			/* no warnings will be given */
	case 'R':			/* strict version */
		options[*(text-1)] = 1;
		break;

#ifdef ___XXX___
deleted, is now a debug-flag
	case 'C' :	/* E option + comment output		*/
#ifndef NOPP
		options['E'] = 1;
		warning("-C: comment is not output");
#else NOPP
		warning("-C option ignored");
#endif NOPP
		break;
#endif ___XXX___

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
#else NOPP
		warning("-D option ignored");
#endif NOPP
		break;
	}

#ifdef ___XXX___
	case 'E' :	/* run preprocessor only, with #<int>	*/
#ifndef NOPP
		options['E'] = 1;
#else NOPP
		warning("-E option ignored");
#endif NOPP
		break;
#endif ___XXX___

	case 'I' :	/* -Ipath : insert "path" into include list	*/
#ifndef NOPP
		if (*text)	{
			register int i = inc_pos++;
			register char *new = text;
			
			while (new)	{
				register char *tmp = inctable[i];
				
				inctable[i++] = new;
				if (i == MAXINCL)
					fatal("too many -I options");
				new = tmp;
			}
		}
		else inctable[inc_pos] = 0;
#else NOPP
		warning("-I option ignored");
#endif NOPP
		break;

	case 'M':	/* maximum identifier length */
		idfsize = txt2int(&text);
		if (*text || idfsize <= 0)
			fatal("malformed -M option");
		if (idfsize > IDFSIZE)
			fatal("maximum identifier length is %d", IDFSIZE);
		break;

	case 'N' :
#ifdef USE_TMP
		options['N'] = 1;
#else USE_TMP
		warning("-N option ignored");
#endif USE_TMP
		break;

#ifdef ___XXX___
	case 'P' :	/* run preprocessor stand-alone, without #'s	*/
#ifndef NOPP
		options['E'] = 1;
		options['P'] = 1;
#else NOPP
		warning("-P option ignored");
#endif NOPP
		break;
#endif ___XXX___

#ifdef USE_TMP
	case 'T' :
		if (*text)
			tmpfdir = text;
		else
			tmpfdir = ".";
#else USE_TMP
		warning("-T option ignored");
#endif USE_TMP
		break;
		
	case 'U' :	{	/* -Uname :	undefine predefined	*/
#ifndef NOPP
		struct idf *idef;

		if (*text)	{
			if ((idef = str2idf(text))->id_macro) {
				free_macro(idef->id_macro);
				idef->id_macro = (struct macro *) 0;
			}
		}
#else NOPP
		warning("-U option ignored");
#endif NOPP
		break;
	}

	case 'V' :	/* set object sizes and alignment requirements	*/
	{
		arith size, align;
		char c;

		while (c = *text++)	{
			size = txt2int(&text);
			align = 0;
			if (*text == '.')	{
				text++;
				align = txt2int(&text);
			}
			switch (c)	{
			case 's':	/* short	*/
				if (size != (arith)0)
					short_size = size;
				if (align != 0)
					short_align = align;
				break;
			case 'w':	/* word		*/
				if (size != (arith)0)
					dword_size = (word_size = size) << 1;
				if (align != 0)
					word_align = align;
				break;
			case 'i':	/* int		*/
				if (size != (arith)0)
					int_size = size;
				if (align != 0)
					int_align = align;
				break;
			case 'l':	/* long		*/
				if (size != (arith)0)
					long_size = size;
				if (align != 0)
					long_align = align;
				break;
			case 'f':	/* float	*/
#ifndef NOFLOAT
				if (size != (arith)0)
					float_size = size;
				if (align != 0)
					float_align = align;
#endif NOFLOAT
				break;
			case 'd':	/* double	*/
#ifndef NOFLOAT
				if (size != (arith)0)
					double_size = size;
				if (align != 0)
					double_align = align;
#endif NOFLOAT
				break;
			case 'p':	/* pointer	*/
				if (size != (arith)0)
					pointer_size = size;
				if (align != 0)
					pointer_align = align;
				break;
			case 'r':	/* adjust bitfields right	*/
#ifndef NOBITFIELD
				options['r'] = 1;
#else NOBITFIELD
				warning("bitfields are not implemented");
#endif NOBITFIELD
				break;
			case 'S':	/* initial struct alignment	*/
				if (size != (arith)0)
					struct_align = size;
				break;
			case 'U':	/* initial union alignment	*/
				if (size != (arith)0)
					union_align = size;
				break;
			default:
				error("-V: bad type indicator %c\n", c);
			}
		}
		break;
	}
	}
}

int
txt2int(tp)
	char **tp;
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
