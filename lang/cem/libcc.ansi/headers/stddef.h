/*
 * stddef.h - common definitions
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#if	!defined(_STDDEF_H)
#define	_STDDEF_H

#define	NULL		((void *)0)

#define	offsetof(type, ident)	((size_t) (unsigned long) &((type *)0)->ident)

#if	_EM_PSIZE == _EM_WSIZE
typedef int	ptrdiff_t;	/* result of substracting two pointers */
#elif	_EM_PSIZE == _EM_LSIZE
typedef long	ptrdiff_t;	/* result of substracting two pointers */
#else
#error garbage pointer size
#endif	/* _EM_PSIZE */

#if	!defined(_SIZE_T)
#define	_SIZE_T
typedef unsigned int	size_t;		/* type returned by sizeof */
#endif	/* _SIZE_T */

#if	!defined(_WCHAR_T)
#define	_WCHAR_T
typedef char	wchar_t;		/* type expanded character set */
#endif	/* _WCHAR_T */

#endif	/* _STDDEF_H */
