/*
	Defines and externs for the logging machine
*/

/* $Id$ */

#include	"logging.h"

/********* Logging control ********/

#ifdef	LOGGING

extern int must_log;			/* set if logging must occur */
extern long log_start;			/* inr at start of logging */
extern int logging;			/* set if logging in progress */

#define	LOG(a)		{ if (logging) do_log a; }

#else

#define	LOG(a)

#endif	/* LOGGING */

