/* $Id$ */
#include <stdio.h>
#include <varargs.h>

#ifndef NOFLOAT
extern char     *_pfloat();
extern char     *_pscien();
extern char     *gcvt();
#endif

# define wsize(par) ( (sizeof par) / sizeof (int) )


static char *gnum(f,ip,app) register char *f; int *ip; va_list *app; {
	register int    i,c;

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

#define signbit(par) (1L<<(sizeof par*8 -1))

static char *i_compute(val,base,s) unsigned val; char *s; {
	int c;

	c= val % base ;
	val/= base ;
	if (val)
		s = i_compute(val,base,s);
	*s++ = (c>9 ? c-10+'a' : c+'0');
	return(s);
}

#ifndef NOLONG
static char *l_compute(l1,d,s) long l1; char *s; {
	int c;
	long l2;

	if ( l1<0 ) {
		/* assumption: d is a multiple of 2 */
		c= l1&1 ;
		l2= ( (l1>>1) & ~signbit(l1) );
		l1= l2/(d>>1) ;
		c += (l2%(d>>1))<<1 ;
	} else {
		c= l1 % d ;
		l1= l1 / d ;
	}
	if (l1)
		s = l_compute(l1,d,s);
	*s++ = (c>9 ? c-10+'a' : c+'0');
	return(s);
}
#endif

_doprnt(fmt,ap,stream)
	register char *fmt; va_list ap ; FILE *stream;
{
	register char   *s;
#ifndef NOLONG
	long            l;
	int             lflag ;
#else
#define lflag 0
#endif
#ifndef NOFLOAT
	double          dbl ;
	int		capitalE = 0;
#endif
	int             inte ;
	unsigned int    uint ;
	register int    j ;
	int             i,c,rjust,width,ndigit,ndfnd,zfill;
	char            *oldfmt,*s1,buf[1025];

	while (c = *fmt++) {
		if (c != '%') {
#ifdef  CPM
			if (c == '\n') putc('\r',stream);
#endif
			putc(c,stream);
			continue;
		}
#ifndef NOLONG
		lflag = 0 ;
#endif
		j = 10 ;
		rjust = 0;
		if (*fmt == '-') {
			fmt++;
			rjust++;
		}
		zfill = ' ';
		if (*fmt == '0') {
			fmt++;
			zfill = '0';
		}
		fmt = gnum(fmt,&width,&ap);
		ndigit = 0; ndfnd = 0;
		if (*fmt == '.') {
			fmt++; oldfmt = fmt;
			fmt = gnum(fmt,&ndigit,&ap);
			ndfnd = (fmt != oldfmt);
		}
		s = s1 = buf;
#ifndef NOLONG
		if ( *fmt == 'l' || *fmt == 'L' ) {
			fmt++ ; lflag++ ;
		}
#endif
		switch (c = *fmt++) {
		default:
#ifdef  CPM
			if (c == '\n') putc('\r',stream);
#endif
			putc(c,stream);
			continue;
		case 's':
			s1 = va_arg(ap, char *);
			if (s1 == 0)
				s1 = "(null)";
			s = s1;
			do {
				if (*s == 0)
					break;
				s++;
			} while (--ndigit);
			break;
		case 'b':
			j = 2;
		case 'u':
		getu:
			if ( !lflag ) {
				inte = va_arg(ap, int);
				goto i_unsignd ;
			}
#ifndef NOLONG
		case 'U':
		getlu:
			l = va_arg(ap, long);
			goto l_unsignd ;
		case 'B':
			j = 2 ;
			goto getlu ;
		case 'X':
			j = 16;
			goto getlu ;
		case 'O':
			j = 8;
			goto getlu ;
		case 'D':
		    l_signed:
			l = va_arg(ap, long);
			if (l < 0) {
				*s++ = '-';
				l = -l;
			}
		    l_unsignd:
			s = l_compute(l,j,s);
			break;
#endif

		case 'x':
			j = 16;
			goto getu ;
		case 'o':
			j = 8;
			goto getu ;
		case 'd':
			if ( lflag ) goto l_signed; ;
			inte = va_arg(ap, int);
			if ( inte<0 ) {
				*s++ = '-';
				inte= -inte ;
			}
		    i_unsignd:
			s = i_compute(inte,j,s);
			break;
		case 'c':
			uint = va_arg(ap, unsigned int);
			for ( i= sizeof uint -1  ; i>=0 ; i-- ) {
				if ( *s = uint%256 ) s++;
				uint/= 256 ;
			}
			break;
#ifndef NOFLOAT
		case 'E':
			capitalE = 1;
			/* fall through */
		case 'e':
			if (ndigit >= sizeof(buf)) ndigit = sizeof(buf) - 1;
			dbl = va_arg(ap, double);
			s = _pscien(dbl,s,ndigit,ndfnd);
			break;
		case 'f':
			if (ndigit >= sizeof(buf)) ndigit = sizeof(buf) - 1;
			dbl = va_arg(ap, double);
			s = _pfloat(dbl,s,ndigit,ndfnd);
			break;
		case 'G':
			capitalE = 1;
			/* fall through */
		case 'g':
			if (ndigit >= sizeof(buf)) ndigit = sizeof(buf) - 1;
			dbl = va_arg(ap, double);
			s = gcvt(dbl, ndigit ? ndigit : 6, s) + strlen(s);
			break;
#endif
		case 'r':
			ap = va_arg(ap, va_list);
			fmt = va_arg(ap, char *);
			continue;
		}
#ifndef NOFLOAT
		if (capitalE) {
			register char *p = buf;
			capitalE=0;
			while (*p && *p != 'e') p++;
			if (*p == 'e') *p = 'E';
		}
#endif
		j = s - s1;
		if ((c = width - j) > 0)
			if (rjust == 0)
				do putc(zfill,stream);
				while (--c);
		while (--j >= 0)
			putc(*s1++,stream);
		while (--c >= 0)
			putc(zfill,stream);
	}
}
