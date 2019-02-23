/* U S E R   O P T I O N - H A N D L I N G */

#include	<em_arith.h>
#include	<em_label.h>

#include    "parameters.h"
#include	"class.h"
#include	"const.h"
#include	"main.h"
#include    "LLlex.h"
#include    "node.h"
#include	"type.h"
#include    "options.h"
#include    "error.h"


#define	MINIDFSIZE	9

#if MINIDFSIZE < 9
You fouled up! MINIDFSIZE has to be at least 10 or the compiler will not
recognize some keywords!
#endif

extern int	idfsize;


static int txt2int(register char **tp)
{
	/*	the integer pointed to by *tp is read, while increasing
		*tp; the resulting value is yielded.
	*/
	register int val = 0;
	register int ch;

	while( ch = **tp, ch >= '0' && ch <= '9' )	{
		val = val * 10 + ch - '0';
		(*tp)++;
	}
	return val;
}

void DoOption(register char *text)
{
	switch( *text++ )	{

	default:
		options[(int)text[-1]]++;	/* flags, debug options etc.	*/
		break;
				/* recognized flags:
					-i: largest value of set of integer
					-u, -U: allow underscore in identifier
					-w: no warnings
					-R: no range checks
					-A: range checks for array references
				   and many more if DEBUG
				*/


#ifdef DBSYMTAB
	case 'g':
		options['g'] = 1;
		options['n'] = 1;
		break;
#endif
	case 'i':	{		/* largest value of set of integer */
		char *t = text;

		max_intset = txt2int(&t);
		text = t;
		if( max_intset <= (arith) 0 || *t )	{
			error("bad -i flag : use -i<num>");
			max_intset = 0;
		}
		break;
	}

	case 'M': {	/* maximum identifier length */
		char *t = text;

		idfsize = txt2int(&t);
		text = t;
		if( idfsize <= 0 || *t ) {
			fatal("malformed -M option");
			/*NOTREACHED*/
		}
		if( idfsize > IDFSIZE )	{
			idfsize = IDFSIZE;
			warning("maximum identifier length is %d", IDFSIZE);
		}
		if( idfsize < MINIDFSIZE )	{
			idfsize = MINIDFSIZE;
			warning("minimum identifier length is %d", MINIDFSIZE);
		}
		break;
	}

	/* case 'u':			*//* underscore allowed in identifiers */
		/* class('_') = STIDF;*/
		/* inidf['_'] = 1;*/
		/* break;*/


	case 'V' :	{ /* set object sizes and alignment requirements */
			  /* syntax : -V[ [w|i|l|f|p] size? [.alignment]? ]* */
#ifndef NOCROSS
		register arith size;
		register int align;
		char c, *t;

		while( (c = *text++) !=0 )	{
			char *strchr();

			t = text;
			size = txt2int(&t);
			align = 0;
			if( *(text = t) == '.' )	{
				t = text + 1;
				align = txt2int(&t);
				text = t;
			}
			if( !strchr("wilfpS", c) )
				error("-V: bad type indicator %c\n", c);
			if( size )
				switch( c )	{
				case 'w':	/* word		*/
					word_size = size;
					break;
				case 'i':	/* int		*/
					int_size = size;
					break;
				case 'l':	/* long		*/
					long_size = size;
					break;
				case 'f':	/* real		*/
					real_size = size;
					break;
				case 'p':	/* pointer	*/
					pointer_size = size;
					break;
				case 'S':	/* structure	*/
					/* discard size */
					break;
				}

			if( align )
				switch( c )	{
				case 'w':	/* word		*/
					word_align = align;
					break;
				case 'i':	/* int		*/
					int_align = align;
					break;
				case 'l':	/* long		*/
					long_align = align;
					break;
				case 'f':	/* real		*/
					real_align = align;
					break;
				case 'p':	/* pointer	*/
					pointer_align = align;
					break;
				case 'S':	/* initial record alignment */
					struct_align = align;
					break;
				}
		}
		break;
#endif /* NOCROSS */
	}
	}
}


