/*
 * stddef.h - common definitions
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#ifndef	_STDDEF_HEADER_
#define	_STDDEF_HEADER_

#ifndef	NULL
#define	NULL		0
#endif	/* NULL */

/* ??? cast to char pointer necessary? */
#define	offsetof(type, ident) \
	(size_t) &(((type *)0)->ident)

#ifndef	_TYPE_PTRDIFF_
#define	_TYPE_PTRDIFF_
typedef int	ptrdiff_t;	/* result of substracting two pointers */
#endif	/* _TYPE_PTRDIFF_ */

#ifndef	_TYPE_SIZE_
#define	_TYPE_SIZE_
typedef unsigned int	size_t;		/* type returned by sizeof */
#endif	/* _TYPE_SIZE_ */

#ifndef	_TYPE_WCHAR_
#define	_TYPE_WCHAR_
typedef char	wchar_t;		/* type expanded character set */
#endif	/* _TYPE_WCHAR_ */

#endif	/* _STDEF_HEADER_ */
