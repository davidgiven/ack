/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define PRIVATE	static		/* or not */
#define IMPORT	extern
#define EXPORT

/* Here to avoid creating another header "notreached.h" */
#ifndef NDEBUG
#define	NOTREACHED()	crash("in %s, %u: unreachable statement reached", \
				__FILE__, __LINE__)
#else
#define NOTREACHED()
#endif	/* NDEBUG */
