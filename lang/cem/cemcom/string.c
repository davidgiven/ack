/* $Header$ */
/* STRING MANIPULATION AND PRINT ROUTINES */

#include	<system.h>
#include	"string.h"
#include	"nopp.h"
#include	"str_params.h"
#include	"arith.h"

doprnt(fp, fmt, argp)
	File *fp;
	char *fmt;
	int argp[];
{
	char buf[SSIZE];

	sys_write(fp, buf, format(buf, fmt, (char *)argp));
}

/*VARARGS1*/
printf(fmt, args)
	char *fmt;
	char args;
{
	char buf[SSIZE];

	sys_write(STDOUT, buf, format(buf, fmt, &args));
}

/*VARARGS1*/
fprintf(fp, fmt, args)
	File *fp;
	char *fmt;
	char args;
{
	char buf[SSIZE];

	sys_write(fp, buf, format(buf, fmt, &args));
}

/*VARARGS1*/
char *
sprintf(buf, fmt, args)
	char *buf, *fmt;
	char args;
{
	buf[format(buf, fmt, &args)] = '\0';
	return buf;
}

int
format(buf, fmt, argp)
	char *buf, *fmt;
	char *argp;
{
	register char *pf = fmt, *pa = argp;
	register char *pb = buf;

	while (*pf) {
		if (*pf == '%') {
			register width, base, pad, npad;
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
				cbuf[0] = * (char *) pa;
				cbuf[1] = '\0';
				pa += sizeof(int);
				arg = &cbuf[0];
			}
			else
			if (*pf == 'l') {
				/* alignment ??? */
				if (base = integral(*++pf)) {
					arg = int_str(*(long *)pa, base);
					pa += sizeof(long);
				}
				else {
					pf--;
					arg = badformat;
				}
			}
			else
			if (base = integral(*pf)) {
				arg = int_str((long)*(int *)pa, base);
				pa += sizeof(int);
			}
			else
			if (*pf == '%')
				arg = "%";
			else
				arg = badformat;

			npad = width - strlen(arg);

			while (npad-- > 0)
				*pb++ = pad;
			
			while (*pb++ = *arg++);
			pb--;
			pf++;
		}
		else
			*pb++ = *pf++;
	}
	return pb - buf;
}

integral(c)
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

/* Integer to String translator
*/
char *
int_str(val, base)
	register long val;
	register base;
{
	/*	int_str() is a very simple integer to string converter.
		base < 0 : unsigned.
		base must be an element of [-16,-2] V [2,16].
	*/
	static char numbuf[MAXWIDTH];
	static char vec[] = "0123456789ABCDEF";
	register char *p = &numbuf[MAXWIDTH];
	int sign = (base > 0);

	*--p = '\0';		/* null-terminate string	*/
	if (val) {
		if (base > 0) {
			if (val < (arith)0) {
				if ((val = -val) < (arith)0)
					goto overflow;
			}
			else
				sign = 0;
		}
		else
		if (base < 0) {			/* unsigned */
			base = -base;
			if (val < (arith)0) {
				register mod, i;
				
			overflow:
			/* this takes a rainy Sunday afternoon to explain */
			/* ??? */
				mod = 0;
				for (i = 0; i < 8 * sizeof val; i++) {
					mod <<= 1;
					if (val < 0)
						mod++;
					val <<= 1;
					if (mod >= base) {
						mod -= base;
						val++;
					}
				}
				*--p = vec[mod];
			}
		}
			
		do {
			*--p = vec[(int) (val % base)];
			val /= base;
		} while (val != (arith)0);

		if (sign)
			*--p = '-';	/* don't forget it !!	*/
	}
	else
		*--p = '0';		/* just a simple 0	*/

	return p;
}

/*	return negative, zero or positive value if
	resp. s < t, s == t or s > t
*/
int
strcmp(s, t)
	register char *s, *t;
{
	while (*s == *t++)
		if (*s++ == '\0')
			return 0;
	return *s - *--t;
}

/* return length of s
*/
int
strlen(s)
	char *s;
{
	register char *b = s;

	while (*b++)
		;
	return b - s - 1;
}

#ifndef	NOPP
/* append t to s
*/
char *
strcat(s, t)
	register char *s, *t;
{
	register char *b = s;

	while (*s++)
		;
	s--;
	while (*s++ = *t++)
		;
	return b;
}

/* Copy t into s
*/
char *
strcpy(s, t)
	register char *s, *t;
{
	register char *b = s;

	while (*s++ = *t++)
		;
	return b;
}

char *
rindex(str, chr)
	register char *str, chr;
{
	register char *retptr = 0;

	while (*str)
		if (*str++ == chr)
			retptr = &str[-1];
	return retptr;
}
#endif	NOPP
