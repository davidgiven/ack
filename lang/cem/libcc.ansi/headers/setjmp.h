/*
 * setjmp.h - restore calling environment
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#if	!defined(__SETJMP_HEADER__)
#define	__SETJMP_HEADER__

typedef char jmp_buf[256];

int	setjmp(jmp_buf __env);
void	longjmp(jmp_buf __env, int __val);

#endif	/* __SETJMP_HEADER__ */
