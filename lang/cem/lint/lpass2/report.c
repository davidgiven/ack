#include	<varargs.h>

#include	<system.h>
#include	"inpdef.h"

#define	MSGOUT		STDERR	/* filedes on which to write the messages */
#define	ERROUT		STDERR	/* filedes on which to write the panics */

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
			a global file name; if so, skip this message.
		*/
		if (f[0] == '%' && f[1] == 'L') {
			/* it is an inpdef */
			register struct inpdef *id =
				va_arg(ap, struct inpdef *);

			f += 2;
			/* is the file name global? */
			if (id->id_file[0] == '/')
				return;
			/*	if no, we have used up the argument,
				so print it here
			*/
			fprint(MSGOUT, "\"%s\", line %d",
				id->id_file, id->id_line);
		}
		while ((fc = *f++)) {
			if (fc == '%') {
				switch (*f++) {
					register struct inpdef *id;
					register char *s;
					register int i;
				case 'L':	/* a location item */
					id = va_arg(ap, struct inpdef *);
					fprint(MSGOUT, "\"%s\", line %d",
						id->id_file, id->id_line);
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
					panic("bad format %s", fmt);
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

/* VARARGS1 */
panic(fmt, args)
	char *fmt;
{
	fprint(ERROUT, "PANIC, lint, pass2: ");
	doprnt(ERROUT, fmt, &args);
	fprint(ERROUT, "\n");
	exit(1);
}

