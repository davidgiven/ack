/*
 * assert.h - diagnostics
 *
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

void _bad_assertion(const char *expr, const char *file, int line);

#undef	assert

#ifdef	NDEBUG
#define	assert(ignore)	((void)0)
#else
#define assert(expr)							\
	{								\
		if (!(expr)) {						\
			_bad_assertion( #expr, __FILE__, __LINE);	\
			abort();					\
		}							\
	}
#endif	/* NDEBUG */
