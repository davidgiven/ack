/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	 A S S E R T I O N    M A C R O   D E F I N I T I O N		*/

/*	At some points in the program, it must be sure that some condition
	holds true, due to further, successful, processing.  As long as
	there is no reasonable method to prove that a program is 100%
	correct, these assertions are needed in some places.
*/
#include	"debug.h"	/* UF */

#ifdef	DEBUG
/*	Note: this macro uses parameter substitution inside strings */
#define	ASSERT(exp)	(exp || crash("in %s, %u: assertion %s failed", \
				__FILE__, __LINE__, "exp"))
#define	NOTREACHED()	crash("in %s, %u: unreachable statement reached", \
				__FILE__, __LINE__)
#else
#define	ASSERT(exp)
#define	NOTREACHED()
#endif	/* DEBUG */
