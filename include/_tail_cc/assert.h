/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	 A S S E R T I O N    M A C R O   D E F I N I T I O N		*/

#ifndef _ASSERT_H
#define _ASSERT_H

#ifndef	NDEBUG
/*	Note: this macro uses parameter substitution inside strings */
#define	assert(exp)	(exp || (fprintf(stderr,"Assertion \"%s\" failed: file %s, line %d\n", "exp", __FILE__, __LINE__), exit(1)))
#define	_assert(exp)	(exp || (fprintf(stderr,"Assertion \"%s\" failed: file %s, line %d\n", "exp", __FILE__, __LINE__), exit(1)))
#else
#define	assert(exp)	(1)
#define _assert(exp)	(1)
#endif	NDEBUG
#endif /* _ASSERT_H */
