/*
	Provisional arrangement for determining the system on which
	the program is being translated.
*/

/* $Id$ */

#include <local.h>

#ifdef	BSD4_1
#define	BSD_X
#endif	/* BSD4_1 */

#ifdef	BSD4_2
#define	BSD_X
#endif	/* BSD4_2 */

#ifdef	SYS_5
#define	SYS_V
#endif	/* SYS_5 */

/* FIXME: A lot of modern systems, including Linux, don't have struct sgtty
 * and associated ioctls. If you *do* have it, uncomment this. All this
 * code should probably be rewritten one day to use termios instead, which
 * seems to be the standard that won. --- dtrg */
 
//#define WANT_SGTTY

/* FIXME: Uncomment to enable these calls in moncalls.c */

//#define WANT_MOUNT_UMOUNT
//#define WANT_STIME
