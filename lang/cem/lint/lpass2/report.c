/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<varargs.h>

#include	<system.h>
#include	"private.h"
#include	"../lpass1/l_class.h"
#include	"class.h"
#include	"inpdef.h"

#define	MSGOUT		STDERR	/* file descr. on which to write the messages */
#define	ERROUT		STDERR	/* file descr. on which to write the panics */

extern int LineNr;

PRIVATE rep_loc();

/* VARARGS */
report(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char*);
		register char *f = fmt;
		register char fc;

		/*	First see if the first arg is an inpdef with
			a global file name not ending in .c; if so,
			skip this message.
		*/
		if (f[0] == '%' && f[1] == 'L') {
			/* it is an inpdef */
			register struct inpdef *id =
				va_arg(ap, struct inpdef *);
			register char *fn = id->id_file;

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
			fprint(MSGOUT, "\"%s\", line %d",
				fn, id->id_line);
		}
		while ((fc = *f++)) {
			if (fc == '%') {
				switch (*f++) {
					register struct inpdef *id;
					register char *s;
					register int i;
				case 'L':	/* a location item */
					id = va_arg(ap, struct inpdef *);
					rep_loc(id);
					break;
				case 's':	/* a string item */
					s = va_arg(ap, char *);
					fprint(MSGOUT, "%s", s);
					break;
				case 'd':	/* an int item */
					i = va_arg(ap, int);
					fprint(MSGOUT, "%d", i);
					break;
				default:
					panic("internal error: bad format %s",
						fmt);
					break;
				}
			}
			else {
				fprint(MSGOUT, "%c", fc);
			}
		}
		fprint(MSGOUT, "\n");
	}
	va_end(ap);
}

PRIVATE
rep_loc(id)
	struct inpdef *id;
{
	/* a definition can come from a number of places */
	if (!id) {
		fprint(MSGOUT, "format");
	}
	else
	if (is_class(id, CL_LIB)) {
		fprint(MSGOUT, "library");
	}
	else {
		fprint(MSGOUT, "\"%s\", line %d",
			id->id_file, id->id_line);
	}
}

/* VARARGS */
panic(va_alist)				/* fmt, args */
	va_dcl
{
	va_list ap;

	va_start(ap);
	{
		char *fmt = va_arg(ap, char *);

		fprint(ERROUT, "PANIC, lint, pass2: line %d: ", LineNr);
		doprnt(ERROUT, fmt, ap);
		fprint(ERROUT, "\n");
	}
	va_end(ap);

	exit(1);
}

