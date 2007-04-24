/*
 * ctype.h - character handling
 */
/* $Id$ */

#ifndef _CTYPE_H
#define	_CTYPE_H

extern char	__ctype[];	/* located in chartab.c */

#define _U		0x01	/* this bit is for upper-case letters [A-Z] */
#define _L		0x02	/* this bit is for lower-case letters [a-z] */
#define _N		0x04	/* this bit is for numbers [0-9] */
#define _S		0x08	/* this bit is for white space \t \n \f etc */
#define _P		0x10	/* this bit is for punctuation characters */
#define _C		0x20	/* this bit is for control characters */
#define _X		0x40	/* this bit is for hex digits [a-f] and [A-F]*/

extern int isalnum(int _c);		/* alphanumeric [a-z], [A-Z], [0-9] */
extern int isalpha(int _c);		/* alphabetic */
extern int iscntrl(int _c);		/* control characters */
extern int isdigit(int _c);		/* digit [0-9] */
extern int isgraph(int _c);		/* graphic character */
extern int islower(int _c);		/* lower-case letter [a-z] */
extern int isprint(int _c);		/* printable character */
extern int ispunct(int _c);		/* punctuation mark */
extern int isspace(int _c);		/* white space sp, \f, \n, \r, \t, \v */
extern int isupper(int _c);		/* upper-case letter [A-Z] */
extern int isxdigit(int _c);		/* hex digit [0-9], [a-f], [A-F] */
extern int tolower(int _c);		/* convert to lower case character */
extern int toupper(int _c);		/* convert to upper case character */

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

#endif	/* _CTYPE_H */
