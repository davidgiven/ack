/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#if __STDC__
#include	<stdarg.h>
extern panic(char *, ...);
#else
#include	<varargs.h>
#endif

#include	<system.h>
#include	"private.h"
#include	"l_class.h"
#include	"class.h"
#include	"inpdef.h"

#define	MSGOUT		stderr	/* file descr. on which to write the messages */
#define	ERROUT		stderr	/* file descr. on which to write the panics */

extern int LineNr;

static rep_loc();

#if __STDC__
/* VARARGS */
report(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
#else
/* VARARGS */
report(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char*);
#endif
		char *f = fmt;
		char fc;

		/*	First see if the first arg is an inpdef with
			a global file name not ending in .c; if so,
			skip this message.
		*/
		if (f[0] == '%' && f[1] == 'L') {
			/* it is an inpdef */
			struct inpdef *id =
				va_arg(ap, struct inpdef *);
			char *fn = id->id_file;

			f += 2;
			
			if (	/* the file name global */
				fn[0] == '/'
			&&	/* it is not a .c file */
				strcmp(&fn[strlen(fn)-2], ".c") != 0
			) {
				/* we skip this message */
				return;
			}
			/*	otherwise, we have used up the argument,
				so print it here
			*/
			fprintf(MSGOUT, "\"%s\", line %d",
				fn, id->id_line);
		}
		while ((fc = *f++)) {
			if (fc == '%') {
				switch (*f++) {
					struct inpdef *id;
					char *s;
					int i;
				case 'L':	/* a location item */
					id = va_arg(ap, struct inpdef *);
					rep_loc(id);
					break;
				case 's':	/* a string item */
					s = va_arg(ap, char *);
					fprintf(MSGOUT, "%s", s);
					break;
				case 'd':	/* an int item */
					i = va_arg(ap, int);
					fprintf(MSGOUT, "%d", i);
					break;
				default:
					panic("internal error: bad format %s",
						fmt);
					break;
				}
			}
			else {
				fprintf(MSGOUT, "%c", fc);
			}
		}
		fprintf(MSGOUT, "\n");
	}
	va_end(ap);
}

static
rep_loc(id)
	struct inpdef *id;
{
	/* a definition can come from a number of places */
	if (!id) {
		fprintf(MSGOUT, "format");
	}
	else
	if (is_class(id, CL_LIB)) {
		fprintf(MSGOUT, "library");
	}
	else {
		fprintf(MSGOUT, "\"%s\", line %d",
			id->id_file, id->id_line);
	}
}

#if __STDC__
/* VARARGS */
panic(char *fmt, ...)				/* fmt, args */
{
	va_list ap;

	va_start(ap, fmt);
	{
		fprintf(ERROUT, "PANIC, lint, pass2: line %d: ", LineNr);
		vfprintf(ERROUT, fmt, ap);
		fprintf(ERROUT, "\n");
	}
	va_end(ap);

	exit(1);
}
#else
/* VARARGS */
panic(va_alist)				/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);

		fprintf(ERROUT, "PANIC, lint, pass2: line %d: ", LineNr);
		vfprintf(ERROUT, fmt, ap);
		fprintf(ERROUT, "\n");
	}
	va_end(ap);

	exit(1);
}
#endif
