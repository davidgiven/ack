/* $Header$ */
/*	 A S S E R T I O N    M A C R O   D E F I N I T I O N		*/

#ifdef	DEBUG
/*	Note: this macro uses parameter substitution inside strings */
#define	assert(exp)	(exp || _BadAssertion(__FILE__, __LINE__, "exp"))
#else
#define	assert(exp)	(1)
#endif	DEBUG
