/*
 * stddef.h - common definitions
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#if	!defined(__STDDEF_HEADER__)
#define	__STDDEF_HEADER__

#if	!defined(NULL)
#define	NULL		0
#endif	/* NULL */

#define	offsetof(type, ident)		(((size_t) &(((type *)0)->ident))

#if	!defined(__TYPE_PTRDIFF__)
#define	__TYPE_PTRDIFF__
typedef int	ptrdiff_t;	/* result of substracting two pointers */
#endif	/* __TYPE_PTRDIFF__ */

#if	!defined(__TYPE_SIZE__)
#define	__TYPE_SIZE__
typedef unsigned int	size_t;		/* type returned by sizeof */
#endif	/* __TYPE_SIZE__ */

#if	!defined(__TYPE_WCHAR__)
#define	__TYPE_WCHAR__
typedef char	wchar_t;		/* type expanded character set */
#endif	/* __TYPE_WCHAR__ */

#endif	/* __STDDEF_HEADER__ */
