/*
	Concerning the Function Return Area
*/

/* $Id$ */

#include	"logging.h"


void init_FRA(void);
void pushFRA(size);
void popFRA(size);

#ifdef	LOGGING

extern char *FRA_sh;		/* shadowbytes of Function Return Area */
#define	spoilFRA()	{ FRA_def = UNDEFINED; }

#else

#define	spoilFRA()

#endif	/* LOGGING */
