/*
 * loc_incl.h - local include file for stdio library
 */
/* $Header$ */

#include	<stdio.h>

#define	fileno(p)		((p)->_fd)
#define	io_testflag(p,x)	((p)->_flags & (x))

#include	<stdarg.h>
int _doprnt(const char *format, va_list ap, FILE *stream);
int _doscan(FILE * stream, const char *format, va_list ap);
char *_i_compute(unsigned long val, int base, char *s, int nrdigits);
void __cleanup(void);

FILE *popen(const char *command, const char *type);
FILE *fdopen(int fd, const char *mode);

#ifndef	NOFLOAT
char *_pfloat(long double r, register char *s, int n, int flags);
char *_pscien(long double r, register char *s, int n, int flags);
char *ecvt(long double value, int ndigit, int *decpt, int *sign);
char *fcvt(long double value, int ndigit, int *decpt, int *sign);
char *gcvt(long double value, int ndigit, char *s, int flags);
#endif	/* NOFLOAT */

#define	FL_LJUST	0x0001		/* left-justify field */
#define	FL_SIGN		0x0002		/* sign in signed conversions */
#define	FL_SPACE	0x0004		/* space in signed conversions */
#define	FL_ALT		0x0008		/* alternate form */
#define	FL_ZEROFILL	0x0010		/* fill with zero's */
#define	FL_SHORT	0x0020		/* optional h */
#define	FL_LONG		0x0040		/* optional l */
#define	FL_LONGDOUBLE	0x0080		/* optional L */
#define	FL_WIDTHSPEC	0x0100		/* field width is specified */
#define	FL_PRECSPEC	0x0200		/* precision is specified */
#define FL_SIGNEDCONV	0x0400		/* may contain a sign */
#define	FL_NOASSIGN	0x0800		/* do not assign (in scanf) */
#define	FL_NOMORE	0x1000		/* all flags collected */
