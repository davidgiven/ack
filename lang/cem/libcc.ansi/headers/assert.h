/*
 * assert.h - diagnostics
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

void __bad_assertion(const char *__expr, const char *__file, int __line);

#undef	assert

#if	defined(NDEBUG)
#define	assert(ignore)	((void)0)
#else
#define assert(expr)	((expr)? (void)0 : __bad_assertion( #expr, __FILE__, __LINE__))
#endif	/* NDEBUG */
