/*
 * ctype.h - character handling
 */
/* $Header$ */

#ifndef	_CTYPE_HEADER_
#define	_CTYPE_HEADER_

int	isalnum(int c);		/* alpha numeric character */
int	isalpha(int c);		/* alpha character */
int 	iscntrl(int c);		/* control character */
int	isdigit(int c);		/* digit character */
int 	isgraph(int c);		/* graphical character */
int	islower(int c);		/* lower case character */
int	isprint(int c);		/* printable character */
int	ispunct(int c);		/* punctuaction character */
int	isspace(int c);		/* space character */
int	isupper(int c);		/* upper case character */
int	isxdigit(int c);	/* hexdecimal digit character */

int	tolower(int c);		/* convert to lower case character */
int	toupper(int c);		/* convert to upper case character */

#define	isdigit(c)	((unsigned)((c)-'0') < 10)
#define	islower(c)	((unsigned)((c)-'a') < 26)
#define	isupper(c)	((unsigned)((c)-'A') < 26)
#define	isprint(c)	((unsigned)((c)-' ') < 95)
#define	isascii(c)	((unsigned)(c) < 128)

#endif	/* _CTYPE_HEADER_ */
