/*
 * ctype.h - character handling
 */
/* $Header$ */

#if	!defined(__CTYPE_HEADER__)
#define	__CTYPE_HEADER__

int	isalnum(int __c);		/* alpha numeric character */
int	isalpha(int __c);		/* alpha character */
int 	iscntrl(int __c);		/* control character */
int	isdigit(int __c);		/* digit character */
int 	isgraph(int __c);		/* graphical character */
int	islower(int __c);		/* lower case character */
int	isprint(int __c);		/* printable character */
int	ispunct(int __c);		/* punctuaction character */
int	isspace(int __c);		/* space character */
int	isupper(int __c);		/* upper case character */
int	isxdigit(int __c);		/* hexdecimal digit character */

int	tolower(int __c);		/* convert to lower case character */
int	toupper(int __c);		/* convert to upper case character */

#define	isdigit(c)	((unsigned)((c)-'0') < 10)
#define	islower(c)	((unsigned)((c)-'a') < 26)
#define	isupper(c)	((unsigned)((c)-'A') < 26)
#define	isprint(c)	((unsigned)((c)-' ') < 95)
#define	isascii(c)	((unsigned)(c) < 128)

#endif	/* __CTYPE_HEADER__ */
