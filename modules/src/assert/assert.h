/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	 A S S E R T I O N    M A C R O   D E F I N I T I O N		*/

/* This 'assert' definition can be used in a ,-expression. */

#ifndef	NDEBUG
#if __STDC__
int _BadAssertion(char *, int, char *);
#define	assert(exp)	((void)((exp) || _BadAssertion(__FILE__, __LINE__, #exp)))
#else
/*	Note: this macro uses parameter substitution inside strings */
#define	assert(exp)	((exp) || _BadAssertion(__FILE__, __LINE__, "exp"))
#endif
#else
#if __STDC__
#define	assert(exp)	((void)0)
#else
#define	assert(exp)	(0)
#endif
#endif	/* NDEBUG */
