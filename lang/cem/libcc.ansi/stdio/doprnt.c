/*
 * doprnt.c - print formatted output
 */
/* $Header$ */

#include	<ctype.h>
#include	<stdio.h>
#include	<stdarg.h>
#include	<string.h>
#include	"loc_incl.h"

static char *
gnum(register const char *f, int *ip, va_list *app)
{
	register int    i, c;

	if (*f == '*') {
		*ip = va_arg((*app), int);
		f++;
	} else {
		i = 0;
		while ((c = *f - '0') >= 0 && c <= 9) {
			i = i*10 + c;
			f++;
		}
		*ip = i;
	}
	return(f);
}

#if	EM_WSIZE == EM_PSIZE
#define set_pointer(flags)				/* nothing */
#elif	EM_LSIZE == EM_PSIZE
#define set_pointer(flags)	(flags |= FL_LONG)
#else
#define set_pointer(flags)				/* nothing */
#error garbage pointer size
#endif

char *
i_compute(unsigned long val, int base, char *s, int nrdigits)
{
	int c;

	c= val % base ;
	val /= base ;
	if (val || nrdigits > 0)
		s = i_compute(val, base, s, nrdigits - 1);
	*s++ = (c>9 ? c-10+'a' : c+'0');
	return(s);
}

/* print an ordinal number */
static char *
o_print(va_list *ap, int flags, char *s, char c, int precision, int is_signed)
{
	long signed_val;
	unsigned long unsigned_val;
	char *old_s = s;
	int i, base, is_zero = 0, is_neg = 0;

	switch (flags & (FL_SHORT | FL_LONG)) {
	case FL_SHORT:
		if (is_signed) {
			signed_val = (short) va_arg(*ap, int);
		} else {
			unsigned_val = (unsigned short) va_arg(*ap, unsigned);
		}
		break;
	case FL_LONG:
		if (is_signed) {
			signed_val = va_arg(*ap, long);
		} else {
			unsigned_val = va_arg(*ap, unsigned long);
		}
		break;
	default:
		if (is_signed) {
			signed_val = va_arg(*ap, int);
		} else {
			unsigned_val = va_arg(*ap, unsigned int);
		}
		break;
	}

	if (is_signed) {
		if (signed_val < 0) {
			*s++ = '-';
			signed_val = -signed_val;
		} else if (flags & FL_SIGN) *s++ = '+';
		else if (flags & FL_SPACE) *s++ = ' ';
		unsigned_val = signed_val;
	}
	if (!unsigned_val) {
		if (precision != 0)
			*s++ = '0';
		return s;
	}
	if (((flags & FL_ALT) && (c == 'x' || c == 'X' || c == 'o'))
	    || c == 'p') {
		*s++ = '0';
		if (c != 'o')
			*s++ = (c == 'X') ? 'X' : 'x';
	}

	switch (c) {
	case 'b':	base = 2;	break;
	case 'o':	base = 8;	break;
	case 'd':
	case 'i':
	case 'u':	base = 10;	break;
	case 'x':
	case 'X':
	case 'p':	base = 16;	break;
	}

	s = i_compute(unsigned_val, base, s, precision - 1);

	if (c == 'X')
		while (old_s != s) {
			*old_s = toupper(*old_s);
			old_s++;
		}

	return s;
}

#ifndef	NOFLOAT
static char *
f_print(va_list *ap, int flags, char *s, char c, int precision)
{
	register char *old_s = s;
	double d_val;

#if	EM_DSIZE != EM_LDSIZE
	long double ld_val;

	if (flags & FL_LONGDOUBLE) ld_val = va_arg(*ap, long double);
	else
#endif
	    d_val = va_arg(*ap, double);

	switch(c) {
	case 'f':
#if	EM_DSIZE != EM_LDSIZE
		if (flags & FL_LONGDOUBLE)
			s = _pfloat_ldbl(ld_val, s, precision, flags);
		else
#endif
		    s = _pfloat(d_val, s, precision, flags);
		break;
	case 'e':
	case 'E':
#if	EM_DSIZE != EM_LDSIZE
		if (flags & FL_LONGDOUBLE)
			s = _pscien_ldbl(ld_val, s, precision, flags);
		else
#endif
		    s = _pscien(d_val, s, precision, flags);
		break;
	case 'g':
	case 'G':
#if	EM_DSIZE != EM_LDSIZE
		if (flags & FL_LONGDOUBLE)
			s = gcvt_ldbl(ld_val, precision, s, flags) + strlen(s);
		else
#endif
		    s = gcvt(d_val, precision, s, flags) + strlen(s);
		break;
	}
	if ( c == 'E' || c == 'G') {
		while (*old_s && *old_s != 'e') old_s++;
		if (*old_s == 'e') *old_s = 'E';
	}
	return s;
}
#endif	/* NOFLOAT */

int
_doprnt(register const char *fmt, va_list ap, FILE *stream)
{
	register char   *s;
	unsigned int    uint;
	register int    j;
	int             i, c, width, precision, zfill, flags, between_fill;
	int		nrchars=0;
	char            *oldfmt, *s1, buf[1025];

	while (c = *fmt++) {
		if (c != '%') {
#ifdef  CPM
			if (c == '\n') {
				putc('\r', stream);
				nrchars++;
			}
#endif
			putc(c, stream);
			nrchars++;
			continue;
		}
		flags = 0;
		do {
			switch(*fmt) {
			case '-':	flags |= FL_LJUST;	break;
			case '+':	flags |= FL_SIGN;	break;
			case ' ':	flags |= FL_SPACE;	break;
			case '#':	flags |= FL_ALT;	break;
			case '0':	flags |= FL_ZEROFILL;	break;
			default:	flags |= FL_NOMORE;	continue;
			}
			fmt++;
		} while(!(flags & FL_NOMORE));

		oldfmt = fmt;
		fmt = gnum(fmt, &width, &ap);
		if (fmt != oldfmt) flags |= FL_WIDTHSPEC;

		if (*fmt == '.') {
			fmt++; oldfmt = fmt;
			fmt = gnum(fmt, &precision, &ap);
			if (precision >= 0) flags |= FL_PRECSPEC;
		}

		if ((flags & FL_WIDTHSPEC) && width < 0) {
			width = -width;
			flags |= FL_LJUST;
		}
		if (!(flags & FL_WIDTHSPEC)) width = 1;

		if (flags & FL_SIGN)
			flags &= ~FL_SPACE;

		if (flags & (FL_LJUST | FL_PRECSPEC))
			flags &= ~FL_ZEROFILL;

		zfill = ' ';
		if (flags & FL_ZEROFILL)
			zfill = '0';

		s = s1 = buf;

		switch (*fmt) {
		case 'h':	flags |= FL_SHORT; fmt++; break;
		case 'l':	flags |= FL_LONG; fmt++; break;
		case 'L':	flags |= FL_LONGDOUBLE; fmt++; break;
		}

		switch (c = *fmt++) {
		default:
#ifdef  CPM
			if (c == '\n') {
				putc('\r', stream);
				nrchars++;
			}
#endif
			putc(c, stream);
			nrchars++;
			continue;
		case 'n':
			if (flags & FL_SHORT)
				*va_arg(ap, short *) = (short) nrchars;
			else if (flags & FL_LONG)
				*va_arg(ap, long *) = (long) nrchars;
			else
				*va_arg(ap, int *) = (int) nrchars;
			continue;
		case 's':
			s1 = va_arg(ap, char *);
			if (s1 == NULL)
				s1 = "(null)";
			s = s1;
			do {
				if (*s ==  '\0')
					break;
				s++;
			} while (!(flags & FL_PRECSPEC) || --precision);
			break;
		case 'p':
			set_pointer(flags);
			/* fallthrough */
		case 'b':
		case 'o':
		case 'u':
		case 'x':
		case 'X':
			if (!(flags & FL_PRECSPEC)) precision = 1;
			s = o_print(&ap, flags, s, c, precision, 0);
			break;
		case 'd':
		case 'i':
			flags |= FL_SIGNEDCONV;
			if (!(flags & FL_PRECSPEC)) precision = 1;
			s = o_print(&ap, flags, s, c, precision, 1);
			break;
		case 'c':
			uint = va_arg(ap, unsigned int);
			for ( i= sizeof(uint) -1  ; i>=0 ; i-- ) {
				if ( *s = uint%256 ) s++;
				uint/= 256 ;
			}
			break;
#ifndef NOFLOAT
		case 'G':
		case 'g':
			if ((flags & FL_PRECSPEC) && (precision == 0))
				precision = 1;
		case 'f':
		case 'E':
		case 'e':
			if (!(flags & FL_PRECSPEC)) 
				precision = 6;

			if (precision >= sizeof(buf))
				precision = sizeof(buf) - 1;

			flags |= FL_SIGNEDCONV;
			s = f_print(&ap, flags, s, c, precision);
			break;
#endif	/* NOFLOAT */
		case 'r':
			ap = va_arg(ap, va_list);
			fmt = va_arg(ap, char *);
			continue;
		}
		j = s - s1;

		/* between_fill is true under the following conditions:
		 * 1- the fill character is '0'
		 * and
		 * 2a- the number is of the form 0x... or 0X...
		 * or
		 * 2b- the number contains a sign or space
		 */
		between_fill = 0;
		if ((flags & FL_ZEROFILL)
		    && (((c == 'x' || c == 'X') && (flags & FL_ALT))
			|| (c == 'p')
			|| ((flags & FL_SIGNEDCONV)
			    && ( *s1 == '+' || *s1 == '-' || *s1 == ' '))))
			between_fill++;

		if ((i = width - j) > 0)
			if (!(flags & FL_LJUST)) {	/* right justify */
				nrchars += i;
				if (between_fill) {
					if (flags & FL_SIGNEDCONV) {
						j--; nrchars++;
						putc(*s1++, stream);
					} else {
						j -= 2; nrchars += 2;
						putc(*s1++, stream);
						putc(*s1++, stream);
					}
				}
				do putc(zfill, stream);
				while (--i);
			}

		nrchars += j;
		while (--j >= 0)
			putc(*s1++, stream);

		if (i > 0) nrchars += i;
		while (--i >= 0)
			putc(zfill, stream);
	}
	return nrchars;
}
