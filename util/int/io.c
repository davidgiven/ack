/*
	In and output, error messages, etc.
*/

/* $Id$ */

#include	<stdio.h>
#include	<varargs.h>

#include	"logging.h"
#include	"global.h"
#include	"mem.h"
#include	"linfil.h"

extern int running;			/* from main.c */
extern char *prog_name;			/* from main.c */
extern char *load_name;			/* from init.c */

/********  The message file  ********/

extern char mess_file[64];		/* from main.c */
long mess_id;				/* Id, to determine unique mess file */
FILE *mess_fp;				/* Filepointer of message file */

PRIVATE do_fatal();

incr_mess_id()
{	/* for a new child */
	mess_id++;
}

#ifdef	LOGGING
extern long inr;			/* from log.c */
#endif	/* LOGGING */

/********  General file handling  ********/

PRIVATE int highestfd();

int fd_limit = 100;			/* first non-available file descriptor */

FILE *fcreat_high(fn)
	char *fn;
{
	/*	Creates an unbuffered FILE with name  fn  on the highest
		possible file descriptor.
	*/
	register int fd;
	register FILE *fp;
	
	if ((fd = creat(fn, 0644)) == -1)
		return NULL;
	fd = highestfd(fd);
	if ((fp = fdopen(fd, "w")) == NULL)
		return NULL;
	setbuf(fp, (char *) 0);		/* unbuffered! */
	fd_limit = fd;
	return fp;
}

PRIVATE int highestfd(fd)
	int fd;
{
	/*	Moves the (open) file descriptor  fd  to the highest available
		position and returns the new fd.  Does this without knowing
		how many fd-s are available.
	*/
	register int newfd, higherfd;

	/* try to get a better fd */
	newfd = dup(fd);
	if (newfd < 0) {
		return fd;
	}
	if (newfd > 99) {
		/* for systems with an unlimited supply of file descriptors */
		close(newfd);
		return fd;
	}

	/* occupying the new fd, try to do even better */
	higherfd = highestfd(newfd);
	close(fd);
	return higherfd;		/* this is a deep one */
}

init_ofiles(firsttime)
	int firsttime;
{
	if (!firsttime) {
		fclose(mess_fp);	/* old message file */
		mess_fp = 0;
		sprintf(mess_file, "%s_%ld", mess_file, mess_id);
	}

	/* Create messagefile */
	if ((mess_fp = fcreat_high(mess_file)) == NULL)
		fatal("Cannot create messagefile '%s'", mess_file);
	init_wmsg();

	mess_id = 1;			/* ID of next child */

#ifdef	LOGGING
	open_log(firsttime);
#endif	/* LOGGING */
}

/*VARARGS0*/
fatal(va_alist)
	va_dcl
{
	va_list ap;

	fprintf(stderr, "%s: ", prog_name);

	va_start(ap);
	{
		register char *fmt = va_arg(ap, char *);
		do_fatal(stderr, fmt, ap);
	}
	va_end(ap);

	if (mess_fp) {
		va_start(ap);
		{
			register char *fmt = va_arg(ap, char *);
			do_fatal(mess_fp, fmt, ap);
		}
		va_end(ap);
	}

	if (running)
		core_dump();
	
	close_down(1);
}

close_down(rc)
	int rc;
{
	/* all exits should go through here */
	if (mess_fp) {
		fclose(mess_fp);
		mess_fp = 0;
	}

#ifdef	LOGGING
	close_log();
#endif	/* LOGGING */

	exit(rc);
}

PRIVATE do_fatal(fp, fmt, ap)
	FILE *fp;
	char *fmt;
	va_list ap;
{
	fprintf(fp, "(Fatal error) ");
	if (load_name)
		fprintf(fp, "%s: ", load_name);
	vfprintf(fp, fmt, ap);
	fputc('\n', fp);
}

/*VARARGS0*/
message(va_alist)
	va_dcl
{
	va_list ap;

	fprintf(mess_fp, "(Message): ");

	va_start(ap);
	{
		register char *fmt = va_arg(ap, char *);
		vfprintf(mess_fp, fmt, ap);
	}
	va_end(ap);

	fprintf(mess_fp, " at %s\n", position());
}

char *position()			/* transient */
{
	static char buff[300];
	register char *fn = dt_fname(getFIL());
	
#ifdef	LOGGING
	sprintf(buff, "\"%s\", line %ld, INR = %ld", fn, getLIN(), inr);
#else	/* LOGGING */
	sprintf(buff, "\"%s\", line %ld", fn, getLIN());
#endif	/* LOGGING */
	return buff;
}

char *dt_fname(p)
	ptr p;
{
	return (p ? &data_loc(p) : "<unknown>");
}

