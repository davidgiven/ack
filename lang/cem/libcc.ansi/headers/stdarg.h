/*
 * stdarg.h - variable arguments
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#if	!defined(__STDARG_HEADER__)
#define	__STDARG_HEADER__

typedef	void	*va_list;

#define _vasz(x)		((sizeof(x)+sizeof(int)-1) & ~(sizeof(int) -1))

#define va_start(ap, parmN)	(ap = (va_list)&parmN + _vasz(parmN))
#define va_arg(ap, type)	(*((type *)((ap += _vasz(type)) - _vasz(type))))
#define va_end(ap)

#endif	/* __STDARG_HEADER__ */
