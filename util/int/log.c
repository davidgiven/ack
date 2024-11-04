/*
 The logging machine
 */

/* $Id$ */

#include <stdio.h>
#include <string.h>
#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include "logging.h"
#include "global.h"
#include "dump.h"
#include "linfil.h"
#include "io.h"

#ifdef LOGGING

extern long mess_id; /* from io.c */

/******** The Logging Machine Variables ********/

long inr; /* current instruction number */

int must_log; /* set if logging may be required */
long log_start; /* first instruction to be logged */
int logging; /* set as soon as logging starts */

static long stop; /* inr after which to stop */
static long gdump; /* inr at which to dump GDA */
static ptr gmin, gmax; /* GDA dump limits */
static long hdump; /* inr at which to dump the heap */
static long stdsize; /* optional size of stack dump */
static int stdrawflag; /* set if unformatted stack dump */

static char log_file[64] = "int.log"; /* Name of log file */
static long at; /* patch to set log_start */
static char* lmask; /* patch to set logmask */
static char* logvar; /* Name of LOG variable */
static int log_level[128]; /* Holds the log levels */
static FILE* log_fp; /* Filepointer of log file */

/* arguments for the logging machine */
static int argcount;
static char* arglist[20]; /* arbitrary size */

static void set_lmask(char* mask);
static char* getpar(char*);
static long longpar(char*, long);

int logarg(char* str)
{
	/*	If the string might be an interesting argument for the
	 logging machine, it is stored in the arglist, and logarg
	 succeeds.  Otherwise it fails.

	 The string is interesting if it contains a '='.
	 */
	char* arg = str;
	char ch;

	while ((ch = *arg) && (ch != '='))
	{
		arg++;
	}
	if (ch == '=')
	{
		if (argcount == (sizeof arglist / sizeof arglist[0]))
			fatal("too many logging arguments on command line");
		arglist[argcount++] = str;
		return 1;
	}
	return 0;
}

void init_log(void)
{
	/* setting the logging machine */

	stop = longpar("STOP", 0L);
	gdump = longpar("GDA", 0L);
	if (gdump)
	{
		gmin = i2p(longpar("GMIN", 0L));
		gmax = i2p(longpar("GMAX", 0L));
		set_lmask("+1");
	}
	hdump = longpar("HEAP", 0L);
	if (hdump)
	{
		set_lmask("*1");
	}
	stdsize = longpar("STDSIZE", 0L);
	stdrawflag = longpar("RAWSTACK", 0L);

	if (getpar("LOGFILE"))
	{
		strcpy(log_file, getpar("LOGFILE"));
	}

	if ((at = longpar("AT", 0L)))
	{
		/* abbreviation for: */
		stop = at + 1; /* stop AFTER at + 1 */
		/*	Note: the setting of log_start is deferred to
		 init_ofiles(1), for implementation reasons. The
		 AT-variable presently only works for the top
		 level.
		 */
	}

	if ((lmask = getpar("L")))
	{
		/* abbreviation for: */
		log_start = 0;
		must_log = 1;
	}

	inr = 0;
}

/********  The log file  ********/

void open_log(int firsttime)
{
	if (!firsttime)
	{
		sprintf(logvar, "%s%ld", logvar, mess_id);
		if (log_fp)
		{
			fclose(log_fp);
			log_fp = 0;
		}
		logging = 0;
		if ((must_log = getpar(logvar) != 0))
		{
			sprintf(log_file, "%s%ld", log_file, mess_id);
			log_start = atol(getpar(logvar));
		}
	}
	else
	{
		/* first time, top level */
		logvar = "LOG\0            ";

		if (at)
		{ /* patch */
			must_log = 1;
			log_start = at - 1;
		}
		else if (!must_log && (must_log = getpar(logvar) != 0))
		{
			log_start = atoi(getpar(logvar));
		}

		set_lmask(lmask ? lmask : getpar("LOGMASK") ? getpar("LOGMASK") : "A-Z9d2twx9");
	}

	/* Create logfile if needed */
	if (must_log)
	{
		if ((log_fp = fcreat_high(log_file)) == NULL)
			fatal("Cannot create logfile '%s'", log_file);
	}

	if (must_log && inr >= log_start)
	{
		logging = 1;
	}
}

void close_log(void)
{
	if (log_fp)
	{
		fclose(log_fp);
		log_fp = 0;
	}
}

/******** The logmask ********/

#define inrange(c, l, h) (l <= c && c <= h)
#define layout(c) (c == ' ' || c == '\t' || c == ',')

static void set_lmask(char* mask)
{
	char* mp = mask;

	while (*mp != 0)
	{
		char* lvp;
		int lev;

		while (layout(*mp))
		{
			mp++;
		}
		/* find level */
		lvp = mp;
		while (*lvp != 0 && !inrange(*lvp, '0', '9'))
		{
			lvp++;
		}
		lev = *lvp - '0';
		/* find classes */
		while (mp != lvp)
		{
			int mc = *mp;

			if (inrange(mc, 'a', 'z') || inrange(mc, 'A', 'Z') || mc == '+' || mc == '*')
			{
				log_level[mc] = lev;
				mp++;
			}
			else if (mc == '-')
			{
				char c;

				for (c = *(mp - 1) + 1; c <= *(mp + 1); c++)
				{
					log_level[(unsigned char)c] = lev;
				}
				mp += 2;
			}
			else if (layout(mc))
			{
				mp++;
			}
			else
				fatal("Bad logmask initialization string");
		}
		mp = lvp + 1;
	}
}

/******** The logging ********/

int check_log(char mark[])
{
	/*	mark must be of the form ".CL...", C is class letter,
	 L is level digit.
	 */
	if (!logging)
		return 0;

	return ((mark[2] - '0') <= log_level[(unsigned char)mark[1]]);
}

#if __STDC__
/*VARARGS*/
void do_log(char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{

#else
/*VARARGS*/
do_log(va_alist) va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char* fmt = va_arg(ap, char*);

#endif
		if (!check_log(fmt))
			return;

		if (fmt[0] == '@')
		{
			/* include position */
			fprintf(log_fp, "%.4s%s, ", fmt, position());
			vfprintf(log_fp, &fmt[4], ap);
		}
		else
		{
			vfprintf(log_fp, &fmt[0], ap);
		}
	}
	va_end(ap);

	putc('\n', log_fp);
}

void log_eoi(void)
{
	/* Logging to be done at end of instruction */
	if (logging)
	{
		if (inr == gdump)
			gdad_all(gmin, gmax);
		if (inr == hdump)
			hpd_all();
		std_all(stdsize, stdrawflag);
	}

	if (inr == stop)
	{
		message("program stopped on request");
		close_down(0);
	}
}

/******** Service routines ********/

static char* getpar(char* var)
{
	/*	Looks up the name in the argument list.
	 */
	int count;
	int ln = strlen(var);

	for (count = 0; count < argcount; count++)
	{
		char* arg = arglist[count];

		if (strncmp(var, arg, ln) == 0 && arg[ln] == '=')
		{
			return &arg[ln + 1];
		}
	}

	return 0;
}

static long longpar(
    char* var, /* name of the variable */
    long def /* default value */
)
{
	char* res = getpar(var);

	return (res ? atol(res) : def);
}

#endif /* LOGGING */
