/*
 * setjmp.h - restore calling environment
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#ifndef	_SETJMP_HEADER_
#define	_SETJMP_HEADER_

typedef char jmp_buf[256];

int	setjmp(jmp_buf env);
void	longjmp(jmp_buf env, int val);

#endif	/* _SETJMP_HEADER_ */
