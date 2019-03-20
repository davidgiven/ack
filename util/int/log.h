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

/* Initalize logging system. */
void init_log(void);
int logarg(char *str);
/* Open the log file. */
void open_log(int firsttime);
/* Close the log file. */
void close_log(void);
int check_log(char mark[]);
/* Log an entry into the logfile. */
void do_log(char *fmt, ...);
void log_eoi(void);


#else

#define	LOG(a)

#endif	/* LOGGING */

