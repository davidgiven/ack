/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* U S E R   O P T I O N - H A N D L I N G */

/* $Id$ */

#include "parameters.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<alloc.h>

#include	"type.h"
#include	"main.h"
#include	"warning.h"
#include	"class.h"
#include	"error.h"

#define	MINIDFSIZE	14

#if MINIDFSIZE < 14
You fouled up! MINIDFSIZE has to be at least 14 or the compiler will not
recognize some keywords!
#endif

extern int	idfsize;
static int	ndirs = 1;
int		warning_classes = W_INITIAL;
int		gdb_flag;

#if (!SQUEEZE) | (!NOCROSS)
static int txt2int(register char **tp)
{
	/*	the integer pointed to by *tp is read, while increasing
		*tp; the resulting value is yielded.
	*/
	register int val = 0;
	register int ch;

	while (ch = **tp, ch >= '0' && ch <= '9')	{
		val = val * 10 + ch - '0';
		(*tp)++;
	}
	return val;
}
#endif


void DoOption(register char *text)
{
	switch(*text++)	{

	case '-':
		options[(unsigned int)*text]++;	/* debug options etc.	*/
		break;

	case 'U':	/* allow underscores in identifiers */
		inidf['_'] = 1;
		break;
	case 'L':	/* no fil/lin */
	case 'R':	/* no range checks */
	case 'A':	/* extra array bound checks, for machines that do not
			   implement it in AAR/LAR/SAR
			*/
	case 'n':	/* no register messages */
	case 'x':	/* every name global */
	case 's':	/* symmetric: MIN(INTEGER) = -MAX(INTEGER) */
	case '3':	/* strict 3rd edition Modula-2 */
	case 'l':	/* local additions enabled */
		options[(unsigned int)text[-1]]++;
		break;

#ifdef DBSYMTAB
	case 'g':	/* generate symbol table for debugger */
		options['g']++;
		if (*text == 'd') {
			/* Assume -gdb. */
			gdb_flag = 1;
		}
		options['n']++;	/* no register vars ??? */
		break;
#endif /* DBSYMTAB */

	case 'w':
		if (*text) {
			while (*text) {
				switch(*text++) {
#ifndef STRICT_3RD_ED
				case 'O':
					warning_classes &= ~W_OLDFASHIONED;
					break;
#endif
#ifndef NOSTRICT
				case 'R':
					warning_classes &= ~W_STRICT;
					break;
#endif
				case 'W':
					warning_classes &= ~W_ORDINARY;
					break;
				}
			}
		}
		else warning_classes = W_ALWAYS;
		break;

	case 'W':
		if (*text) {
			while (*text) {
				switch(*text++) {
#ifndef STRICT_3RD_ED
				case 'O':
					warning_classes |= W_OLDFASHIONED;
					break;
#endif
#ifndef NOSTRICT
				case 'R':
					warning_classes |= W_STRICT;
					break;
#endif
				case 'W':
					warning_classes |= W_ORDINARY;
					break;
				}
			}
		}
		else warning_classes = W_ALL;
		break;

	case 'M': {	/* maximum identifier length */
#ifndef SQUEEZE
		char *t = text;		/* because &text is illegal */

		idfsize = txt2int(&t);
		if (*t || idfsize <= 0)
			fatal("malformed -M option");
		if (idfsize > IDFSIZE) {
			idfsize = IDFSIZE;
			warning(W_ORDINARY,"maximum identifier length is %d", IDFSIZE);
		}
		if (idfsize < MINIDFSIZE) {
			warning(W_ORDINARY, "minimum identifier length is %d", MINIDFSIZE);
			idfsize = MINIDFSIZE;
		}
#endif
		}
		break;

	case 'I' :
		if (*text) {
			register int i;
			register char *new = text;

			if (nDEF > mDEF) {
				DEFPATH = (char **)
				  Realloc((char *)DEFPATH,(unsigned)(mDEF+=10)*sizeof(char *));
			}

			for (i = ndirs++; i < nDEF; i++) {
				char *tmp = DEFPATH[i];
	
				DEFPATH[i] = new;
				new = tmp;
			}
			++nDEF;
		}
		else	DEFPATH[ndirs] = 0;
		break;

	case 'V' :	/* set object sizes and alignment requirements	*/
#ifndef NOCROSS
	{
		register int size;
		register int algn;
		char c;
		char *t;

		while ( (c = *text++) != 0)	{
			char *strchr();

			t = text;
			size = txt2int(&t);
			algn = 0;
			if (*(text = t) == '.')	{
				t = text + 1;
				algn = txt2int(&t);
				text = t;
			}
			if (! strchr("wislfdpS", c)) {
				error("-V: bad type indicator %c\n", c);
			}
			if (size != 0) switch (c)	{

			case 'w':	/* word		*/
				word_size = size;
				dword_size = 2 * size;
				break;
			case 'i':	/* int		*/
				int_size = size;
				break;
			case 's':	/* short (subranges) */
				short_size = size;
				break;
			case 'l':	/* longint	*/
				long_size = size;
				break;
			case 'f':	/* real		*/
				float_size = size;
				break;
			case 'd':	/* longreal	*/
				double_size = size;
				break;
			case 'p':	/* pointer	*/
				pointer_size = size;
				break;
			}
			if (algn != 0) switch (c)	{

			case 'w':	/* word		*/
				word_align = algn;
				break;
			case 'i':	/* int		*/
				int_align = algn;
				break;
			case 's':	/* short (subranges) */
				short_align = algn;
				break;
			case 'l':	/* longint	*/
				long_align = algn;
				break;
			case 'f':	/* real		*/
				float_align = algn;
				break;
			case 'd':	/* longreal	*/
				double_align = algn;
				break;
			case 'p':	/* pointer	*/
				pointer_align = algn;
				break;
			case 'S':	/* initial record alignment	*/
				struct_align = algn;
				break;
			}
		}
	}
#endif /* NOCROSS */
	break;
	}
}

