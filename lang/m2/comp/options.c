/* U S E R   O P T I O N - H A N D L I N G */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

#include	"idfsize.h"
#include	"ndir.h"

#include	<em_arith.h>
#include	<em_label.h>

#include	"type.h"
#include	"main.h"

extern int	idfsize;
static int	ndirs;

DoOption(text)
	char *text;
{
	switch(*text++)	{

	default:
		options[text[-1]] = 1;	/* flags, debug options etc.	*/
		break;

	case 'L' :	/* don't generate fil/lin */
		options['L'] = 1;
		break;

	case 'M':	/* maximum identifier length */
		idfsize = txt2int(&text);
		if (*text || idfsize <= 0)
			fatal("malformed -M option");
		if (idfsize > IDFSIZE)
			fatal("maximum identifier length is %d", IDFSIZE);
		break;

	case 'p' :	/* generate profiling code procentry/procexit ???? */
		options['p'] = 1;
		break;

	case 'I' :
		if (++ndirs >= NDIRS) {
			fatal("Too many -I options");
		}
		DEFPATH[ndirs] = text;
		break;

	case 'V' :	/* set object sizes and alignment requirements	*/
	{
		arith size;
		int align;
		char c;

		while (c = *text++)	{
			size = txt2int(&text);
			align = 0;
			if (*text == '.')	{
				text++;
				align = txt2int(&text);
			}
			switch (c)	{

			case 'w':	/* word		*/
				if (size != (arith)0) {
					word_size = size;
					dword_size = 2 * size;
				}
				if (align != 0) word_align = align;
				break;
			case 'i':	/* int		*/
				if (size != (arith)0) int_size = size;
				if (align != 0) int_align = align;
				break;
			case 'l':	/* longint	*/
				if (size != (arith)0) long_size = size;
				if (align != 0) long_align = align;
				break;
			case 'f':	/* real		*/
				if (size != (arith)0) float_size = size;
				if (align != 0) float_align = align;
				break;
			case 'd':	/* longreal	*/
				if (size != (arith)0) double_size = size;
				if (align != 0) double_align = align;
				break;
			case 'p':	/* pointer	*/
				if (size != (arith)0) pointer_size = size;
				if (align != 0) pointer_align = align;
				break;
			case 'S':	/* initial record alignment	*/
				if (align != (arith)0) struct_align = align;
				break;
			default:
				error("-V: bad type indicator %c\n", c);
			}
		}
		break;
	}

	case 'n':
		options['n'] = 1;	/* use no registers	*/
		break;

	case 'w':
		options['w'] = 1;	/* no warnings will be given	*/
		break;
	}
}

int
txt2int(tp)
	char **tp;
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
