/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	 A S S E R T I O N    M A C R O   D E F I N I T I O N		*/

#ifdef	DEBUG
#ifdef __STDC__
#define	assert(exp)	((exp) || _BadAssertion(__FILE__, __LINE__, #exp))
#else
/*	Note: this macro uses parameter substitution inside strings */
#define	assert(exp)	((exp) || _BadAssertion(__FILE__, __LINE__, "exp"))
#endif
#else
#define	assert(exp)	(1)
#endif	/* DEBUG */
