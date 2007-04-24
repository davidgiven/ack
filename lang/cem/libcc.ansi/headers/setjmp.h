/*
 * setjmp.h - save/restore calling environment
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#if	!defined(_SETJMP_H)
#define	_SETJMP_H

/* In a jmp_buf, there is room for:  1 mask (long), 1 flag (int) and 3
 * pointers (stack-pointer, local base and program-counter). This may be
 * too big, but that doesn't matter. It could also be too small, when
 * sigset_t is larger than a long.  The fields is in the structure have no
 * meaning, they just get the size right.
 * The identifier __setjmp has a special meaning to the compiler.
 */

typedef struct {
	long __mask;
	int __flag;
	void (*__pc)(void);
	void *__sp;
	void *__lb;
} jmp_buf[1];

extern int __setjmp(jmp_buf _env, int _savemask);

#define	setjmp(env)	__setjmp(env, 0)
extern void longjmp(jmp_buf _env, int _val);

#endif	/* _SETJMP_H */
