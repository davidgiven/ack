/*
 * ctype.h - character handling
 */
/* $Header$ */

#if	!defined(_CTYPE_H)
#define	_CTYPE_H

extern char	__ctype[];	/* located in chartab.c */
extern int	__x; 		/* scratch variable */

#define _U		0x01	/* this bit is for upper-case letters [A-Z] */
#define _L		0x02	/* this bit is for lower-case letters [a-z] */
#define _N		0x04	/* this bit is for numbers [0-9] */
#define _S		0x08	/* this bit is for white space \t \n \f etc */
#define _P		0x10	/* this bit is for punctuation characters */
#define _C		0x20	/* this bit is for control characters */
#define _X		0x40	/* this bit is for hex digits [a-f] and [A-F]*/

int isalnum(int __c);		/* alphanumeric [a-z], [A-Z], [0-9] */
int isalpha(int __c);		/* alphabetic */
int iscntrl(int __c);		/* control characters */
int isdigit(int __c);		/* digit [0-9] */
int isgraph(int __c);		/* graphic character */
int islower(int __c);		/* lower-case letter [a-z] */
int isprint(int __c);		/* printable character */
int ispunct(int __c);		/* punctuation mark */
int isspace(int __c);		/* white space sp, \f, \n, \r, \t, \v */
int isupper(int __c);		/* upper-case letter [A-Z] */
int isxdigit(int __c);		/* hex digit [0-9], [a-f], [A-F] */
int tolower(int __c);		/* convert to lower case character */
int toupper(int __c);		/* convert to upper case character */

#define isalpha(c)	((__ctype+1)[c]&(_U|_L))
#define isspace(c)	((__ctype+1)[c]&_S)
#define	iscntrl(c)	((__ctype+1)[c]&_C)
#define isxdigit(c)	((__ctype+1)[c]&(_N|_X))
#define isalnum(c)	((__ctype+1)[c]&(_U|_L|_N))
#define isgraph(c)	((__ctype+1)[c]&(_P|_U|_L|_N))
#define ispunct(c)	((__ctype+1)[c]&_P)

#define isdigit(c)	((unsigned) ((c)-'0') < 10)
#define islower(c)	((unsigned) ((c)-'a') < 26)
#define isupper(c)	((unsigned) ((c)-'A') < 26)
#define	isprint(c)	((unsigned) ((c)-' ') < 95)
#define	isascii(c)	((unsigned) (c) < 128)

#define toupper(c)	(__x = (c), islower( __x) ? __x - 'a' + 'A' : __x)
#define tolower(c)	(__x = (c), isupper( __x) ? __x - 'A' + 'a' : __x)

#endif	/* _CTYPE_H */
