/*
 * setjmp.h - restore calling environment
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#if	!defined(_SETJMP_H)
#define	_SETJMP_H

/* In a jmp_buf, there is room for:  1 mask (long), 1 flag (int) and 3
 * pointers (stack-pointer, local base and program-counter). This may be
 * too big, but that doesn't matter. It could also be too small, when
 * sigset_t is larger than a long.
 * The identifier __setjmp has a special meaning to the compiler.
 */

typedef char jmp_buf[ sizeof(long) + sizeof(int) + 3 * sizeof(void *)];

int	__setjmp(jmp_buf __env, int __savemask);

#define	setjmp(env)	__setjmp(env, 0)
void	longjmp(jmp_buf __env, int __val);

#if defined(_POSIX_SOURCE)
typedef jmp_buf sigjmp_buf;
#define	sigsetjmp(env, savemask)	__setjmp(env, savemask)
int	siglongjmp(sigjmp_buf __env, int __val);
#endif

#endif	/* _SETJMP_H */
