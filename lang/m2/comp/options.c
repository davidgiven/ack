/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* U S E R   O P T I O N - H A N D L I N G */

/* $Header$ */

#include	"idfsize.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<alloc.h>

#include	"type.h"
#include	"main.h"
#include	"warning.h"

#define	MINIDFSIZE	14

#if MINIDFSIZE < 14
You fouled up! MINIDFSIZE has to be at least 14 or the compiler will not
recognize some keywords!
#endif

extern int	idfsize;
static int	ndirs = 1;
int		warning_classes;

DoOption(text)
	register char *text;
{
	switch(*text++)	{

	case '-':
		options[*text]++;	/* debug options etc.	*/
		break;

	case 'L':	/* no fil/lin */
	case 'p':	/* call procentry/procexit */
	case 'n':	/* no register messages */
	case 'x':	/* every name global */
		options[text[-1]]++;
		break;

	case 'i':	/* # of bits in set */
	{
		char *t = text;
		int val;
		extern int maxset;

		val = txt2int(&t);
		if (val <= 0 || *t) {
			error("bad -i flag; use -i<num>");
		}
		else	maxset = val;
		break;
	}
	case 'w':
		if (*text) {
			while (*text) {
				switch(*text++) {
				case 'O':
					warning_classes &= ~W_OLDFASHIONED;
					break;
				case 'R':
					warning_classes &= ~W_STRICT;
					break;
				case 'W':
					warning_classes &= ~W_ORDINARY;
					break;
				}
			}
		}
		else warning_classes = 0;
		break;

	case 'W':
		while (*text) {
			switch(*text++) {
			case 'O':
				warning_classes |= W_OLDFASHIONED;
				break;
			case 'R':
				warning_classes |= W_STRICT;
				break;
			case 'W':
				warning_classes |= W_ORDINARY;
				break;
			}
		}
		break;

	case 'M': {	/* maximum identifier length */
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
		}
		break;

	case 'I' :
		if (*text) {
			register int i;
			register char *new = text;

			if (++nDEF > mDEF) {
				char **n = (char **)
				   Malloc((unsigned)((10+mDEF)*sizeof(char *)));

				for (i = 0; i < mDEF; i++) {
					n[i] = DEFPATH[i];
				}
				free((char *) DEFPATH);
				DEFPATH = n;
				mDEF += 10;
			}

			i = ndirs++;
			while (new) {
				register char *tmp = DEFPATH[i];
	
				DEFPATH[i++] = new;
				new = tmp;
			}
		}
		else	DEFPATH[ndirs] = 0;
		break;

	case 'V' :	/* set object sizes and alignment requirements	*/
	{
		arith size;
		int align;
		char c;
		char *t;

		while (c = *text++)	{
			t = text;
			size = txt2int(&t);
			align = 0;
			if (*(text = t) == '.')	{
				t = text + 1;
				align = txt2int(&t);
				text = t;
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
			case 's':	/* short (subranges) */
				if (size != 0) short_size = size;
				if (align != 0) short_align = align;
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
	}
}

int
txt2int(tp)
	register char **tp;
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
