/* $Header$ */

/* This include file exists so that <sys/types.h> can still safely be
   included. On a SUN, this would cause problems because sometimes time_t
   and/or size_t were defined twice.
*/

#ifndef	__STDTYPES_H__
#define	__STDTYPES_H__

typedef long	sigset_t;	/* for <signal.h> on SUN's */
typedef	long	time_t;
typedef	long	size_t;

#endif	/* __STDTYPES_H__ */
