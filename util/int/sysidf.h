/*
	Provisional arrangement for determining the system on which
	the program is being translated.
*/

/* $Header$ */

#undef		BSD4_1		/* Berkeley Software Distr. 4.1 */
#define		BSD4_2		/* Berkeley Software Distr. 4.2 */
#undef		SYS_V0		/* System V0 */

#ifdef	BSD4_1
#define	BSD_X
#endif	BSD4_1

#ifdef	BSD4_2
#define	BSD_X
#endif	BSD4_2

#ifdef	SYS_V0
#define	SYS_V
#endif	SYS_V0

