#include <system.h>
#include "data.h"

int mysprint( fmt, args)
	char *fmt;
	int args;
{
	return( _myformat( fmt, &args));
}

char *long2str();

static int integral(c)
{
	switch (c) {
	case 'b':
		return -2;
	case 'd':
		return 10;
	case 'o':
		return -8;
	case 'u':
		return -10;
	case 'x':
		return -16;
	}
	return 0;
}

int _myformat( fmt, argp)
	char *fmt;
	char *argp;
{
	register char *pf = fmt, *pa = argp;
	register char *pb = string; 
	int n = 0;

	while (*pf) {
		if (*pf == '%') {
			register width, base, pad, npad, s_l;
			char *arg;
			char cbuf[2];
			char *badformat = "<bad format>";
			
			/* get padder */
			if (*++pf == '0') {
				pad = '0';
				++pf;
			}
			else
				pad = ' ';
			
			/* get width */
			width = 0;
			while (*pf >= '0' && *pf <= '9')
				width = 10 * width + *pf++ - '0';
			
			/* get text and move pa */
			if (*pf == 's') {
				arg = *(char **)pa;
				pa += sizeof(char *);
			}
			else
			if (*pf == 'c') {
				cbuf[0] = * (int *) pa;
				cbuf[1] = '\0';
				pa += sizeof(int);
				arg = &cbuf[0];
			}
			else
			if (*pf == 'l') {
				/* alignment ??? */
				if (base = integral(*++pf)) {
					arg = long2str(*(long *)pa, base);
					pa += sizeof(long);
				}
				else {
					pf--;
					arg = badformat;
				}
			}
			else
			if (base = integral(*pf)) {
				arg = long2str((long)*(int *)pa, base);
				pa += sizeof(int);
			}
			else
			if (*pf == '%')
				arg = "%";
			else
				arg = badformat;

		        s_l = strlen( arg);
			npad = width - s_l;
			if ( npad > 0) 
				s_l += npad;

			if (  n + s_l + string - string_area >= size_string) {
				mem_string();
				pb = string + n;
			}

			while (npad-- > 0)
				*pb++ = pad;
			
			while (*pb++ = *arg++);
			n += s_l;

			pb--;
			pf++;
		}
		else  {
			if ( n + ( string - string_area) >= size_string) {
				mem_string();
				pb = string + n;
			}
			n++;
			*pb++ = *pf++;
		}
	}
	return n ;
}
