/* $Header$ */
#include <stdio.h>
#include <ctype.h>

union ptr_union {
	char           *chr_p;
	unsigned short *ushort_p;
	unsigned int   *uint_p;
	unsigned long  *ulong_p;
#ifndef NOFLOAT
	float		*float_p;
	double		*double_p;
#endif
};

static char	Xtable[128];

/*
 * the routine that does the job 
 */

_doscanf (iop, format, argp)
register FILE	*iop;
char           *format;		/* the format control string */
union ptr_union *argp;		/* our argument list */
{
	int             done = 0;	/* number of items done */
	int             base;		/* conversion base */
	long            val;		/* an integer value */
	int             sign;		/* sign flag */
	int             do_assign;	/* assignment suppression flag */
	unsigned        width;		/* width of field */
	int             widflag;	/* width was specified */
	int             longflag;	/* true if long */
	int		shortflag;	/* true if short */
	int             done_some;	/* true if we have seen some data */
	int		reverse;	/* reverse the checking in [...] */
	int		kind;
	register int	ic;
#ifndef NOFLOAT
	extern double	atof();
	int		dotseen;
	int		expseen;
	char		buffer[128];
#endif

	ic = getc(iop);
	if (ic == EOF) {
		done = EOF;
		goto quit;
	}

	while (1) {
		if (isspace(*format)) {
			while (isspace (*format))
				++format;	/* skip whitespace */
			while (isspace (ic)) ic = getc(iop);
		}
		if (!*format)
			goto all_done;	/* end of format */
		if (ic < 0)
			goto quit;	/* seen an error */
		if (*format != '%') {
			if (ic != *format)
				goto all_done;
			++format;
			ic = getc(iop);
			continue;
		}
		++format;
		do_assign = 1;
		if (*format == '*') {
			++format;
			do_assign = 0;
		}
		if (isdigit (*format)) {
			widflag = 1;
			for (width = 0; isdigit (*format);)
				width = width * 10 + *format++ - '0';
		} else
			widflag = 0;	/* no width spec */
		if (longflag = (tolower (*format) == 'l'))
			++format;
		else if (shortflag = (tolower(*format) == 'h'))
			++format;
		if (isupper(*format)) {
			kind = tolower(*format);
			longflag = 1;
		}
		else	kind = *format;
		if (kind != 'c')
			while (isspace (ic))
				ic = getc(iop);
		done_some = 0;	/* nothing yet */
		switch (kind) {
		case 'o':
			base = 8;
			goto decimal;
		case 'u':
		case 'd':
			base = 10;
			goto decimal;
		case 'x':
			base = 16;
			if (((!widflag) || width >= 2) && ic == '0') {
				ic = getc(iop);
				if (tolower (ic) == 'x') {
					width -= 2;
					done_some = 1;
					ic = getc(iop);
				} else {
					ungetc(ic, iop);
					ic = '0';
				}
			}
	decimal:
			val = 0L;	/* our result value */
			sign = 0;	/* assume positive */
			if (!widflag)
				width = 0xffff;	/* very wide */
			if (width && ic == '+')
				ic = getc(iop);
			else if (width && ic == '-') {
				sign = 1;
				ic = getc(iop);
			}
			while (width--) {
				if (isdigit (ic) && ic - '0' < base)
					ic -= '0';
				else if (base == 16 && tolower (ic) >= 'a' && tolower (ic) <= 'f')
					ic = 10 + tolower (ic) - 'a';
				else
					break;
				val = val * base + ic;
				ic = getc(iop);
				done_some = 1;
			}
			if (do_assign) {
				if (sign)
					val = -val;
				if (longflag)
					*(argp++)->ulong_p = (unsigned long) val;
				else if (shortflag)
					*(argp++)->ushort_p = (unsigned short) val;
				else
					*(argp++)->uint_p = (unsigned) val;
			}
			if (done_some)
				if (do_assign) ++done;
			else
				goto all_done;
			break;
		case 'c':
			if (!widflag)
				width = 1;
			while (width-- && ic >= 0) {
				if (do_assign)
					*(argp)->chr_p++ = (char) ic;
				ic = getc(iop);
				done_some = 1;
			}
			if (do_assign) {
				argp++;	/* done with this one */
				if (done_some)
					++done;
			}
			break;
		case 's':
			if (!widflag)
				width = 0xffff;
			while (width-- && !isspace (ic) && ic > 0) {
				if (do_assign)
					*(argp)->chr_p++ = (char) ic;
				ic = getc(iop);
				done_some = 1;
			}
			if (do_assign)		/* terminate the string */
				*(argp++)->chr_p = '\0';	
			if (done_some)
				++done;
			else
				goto all_done;
			break;
		case '[':
			if (!widflag)
				width = 0xffff;

			if ( *(++format) == '^' ) {
				reverse = 1;
				format++;
			} else
				reverse = 0;
			
			{ register char *c;
			  for (c = Xtable; c < &Xtable[128]; c++) *c = 0;
			}
			while (*format && *format != ']') {
				Xtable[*format++] = 1;
			}
			if (!*format)
				goto quit;
			
			while (width-- && !isspace (ic) && ic > 0 &&
				(Xtable[ic] ^ reverse)) {
				if (do_assign)
					*(argp)->chr_p++ = (char) ic;
				ic = getc(iop);
				done_some = 1;
			}
			if (do_assign)		/* terminate the string */
				*(argp++)->chr_p = '\0';	
			if (done_some && do_assign)
				++done;
			else
				goto all_done;
			break;
#ifndef NOFLOAT:
		case 'e':
		case 'f': {
			register char *c = buffer;

			if (!widflag) width = 127;
			if (width >= 128) width = 127;
			if (width && (ic == '+' || ic == '-')) {
				*c++ = ic;
				width--;
				ic = getc(iop);
			}
			while (isdigit(ic) && width) {
				width--;
				*c++ = ic;
				ic = getc(iop);
			}
			if (ic == '.' && width) {
				width--;
				*c++ = ic;
				ic = getc(iop);
			}
			while (isdigit(ic) && width) {
				width--;
				*c++ = ic;
				ic = getc(iop);
			}
			if (width && (ic == 'e' || ic == 'E')) {
				width--;
				*c++ = ic;
				ic = getc(iop);
				if (width && (ic == '+' || ic == '-')) {
					width--;
					*c++ = ic;
					ic = getc(iop);
				}
			}
			while (isdigit(ic) && width) {
				width--;
				*c++ = ic;
				ic = getc(iop);
			}
			if (c == buffer) goto all_done;
			*c = 0;

			if (do_assign) {
				done++;
				if (longflag)
					*(argp++)->double_p = atof(buffer);
				else
					*(argp++)->float_p = atof(buffer);
			}
			}
			break;
#endif
		}		/* end switch */
		++format;
	}
all_done:
	if (ic >= 0)
		ungetc(ic, iop);
quit:
	return done;
}
