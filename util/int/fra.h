/*
	Concerning the Function Return Area
*/

/* $Id$ */

#include	"logging.h"

#ifdef	LOGGING

extern char *FRA_sh;		/* shadowbytes of Function Return Area */
#define	spoilFRA()	{ FRA_def = UNDEFINED; }

#else

#define	spoilFRA()

#endif	/* LOGGING */
