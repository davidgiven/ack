/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

/******** A R G U M E N T   T Y P E   C H E C K I N G ********/

#include	"private.h"
#include	"inpdef.h"

extern char *strcpy();

#define	streq(s1,s2)	(strcmp(s1, s2) == 0)

/* a format is developed into a normal parameter definition */
PRIVATE int is_formatargs;		/* present or not */
PRIVATE char formatargs[1000];		/* the definitions */

PRIVATE chk_argtps();
PRIVATE char *next_argtype();
PRIVATE int type_match();
PRIVATE form_type();
PRIVATE conv_format();

int
type_equal(act, form)
	char *act, *form;
{
	return	streq(act, form)
	||	streq(act, "erroneous")
	||	streq(form, "erroneous");
}

chk_args(id, def)
	struct inpdef *id, *def;
{
	char *act_tp = id->id_argtps;
	int nrargs = 0;			/* number of args */

	/* void is a special case */
	if (	/* the definition has one void argument */
		def->id_nrargs == 1 && streq(def->id_argtps, "void:")
	&&	/* the referent has no argumants */
		id->id_nrargs == 0
	)	/* we have a prefect match */
		return;

	/* clear format */
	is_formatargs = 0;

	/* check normal arguments */
	chk_argtps(id, def, &nrargs, act_tp, def->id_argtps);

	if (is_formatargs) {
		/* there was a format */
		register int i;

		/* skip over the actuals already covered */
		for (i = 0; i < nrargs; i++) {
			act_tp = next_argtype(act_tp);
		}

		/* and check the format arguments */
		chk_argtps(id, (struct inpdef *)0, &nrargs,
						act_tp, &formatargs[0]);
	}
}

PRIVATE chk_argtps(id, def, nrargs, act_tp, form_tp)
	struct inpdef *id;		/* the actual call */
	struct inpdef *def;		/* 0 for format-derived definition */
	int *nrargs;			/* in-out parameter, counting */
	char *act_tp;			/* actual types */
	char *form_tp;			/* formal type definitions */
{
	while (*act_tp && *form_tp && *form_tp != '.') {
		register char *act_start = act_tp;
		register char *form_start = form_tp;

		/* isolate actual argument type */
		act_tp = next_argtype(act_tp);
		act_tp[-1] = '\0';

		/* isolate formal argument type */
		form_tp = next_argtype(form_tp);
		form_tp[-1] = '\0';

		(*nrargs)++;
		if (!type_match(id, act_start, form_start)) {
			char act_form[100];
			char form_form[100];

			form_type(act_form, act_start);
			form_type(form_form, form_start);
			report("%L: arg %d of %s (%s) differs from that in %L (%s)",
				id, *nrargs, id->id_name,
				act_form, def, form_form);
		}
		act_tp[-1] = ':';
		form_tp[-1] = ':';
	}

	if (*form_tp == '.')	/* ellipsis */
		return;

	if (*form_tp) {
		/* formal type definitions not exhausted */
		report("%L: %s has fewer arguments than in %L",
			id, id->id_name, def);
	}
	if (*act_tp) {
		/* actual types not exhausted */
		if (def && def->id_nrargs < 0) {
			/* the function had VARARGS */
		}
		else {
			report("%L: %s has more arguments than in %L",
				id, id->id_name, def);
		}
	}
}

PRIVATE char *
next_argtype(tp)
	char *tp;
{
	while (*tp && *tp != ':') {
		tp++;
	}
	if (*tp == ':') {
		tp++;
	}
	return tp;
}

int
PRIVATE type_match(id, act, form)
	struct inpdef *id;
	char *act, *form;
{
	if (form[0] == '"' && act[0] == '"') {
		conv_format(id, act, form);
		return 1;
	}

	if (	(form[0] == '"' && streq(act, "char*"))
	||	(act[0] == '"' && streq(form, "char*"))
	) {
		return 1;
	}

	if (type_equal(act, form))
		return 1;

	if (act[0] == '+') {
		/* a non-negative constant */
		/* might be signed or unsigned */
		if (type_equal(&act[1], form))
			return 1;
		if (	strncmp(form, "unsigned ", strlen("unsigned ")) == 0
		&&	type_equal(&act[1], &form[strlen("unsigned ")])
		) {
			return 1;
		}
	}
	return 0;
}

PRIVATE conv_format(id, act, form)
	struct inpdef *id;
	char *act, *form;
{
	/*	convert the actual format into a def-list, using the
		formal format (form) as a map to convert from %X to type
	*/
	register char *fmt = &formatargs[0];

	is_formatargs = 1;
	while (*act) {
		register char *map;

		/* find next conversion specification */
		while (*act && *act != '%') {
			act++;
		}
		if (*act++ != '%')
			break;
		if (*act == '%') {
			/* %% */
			act++;
			continue;
		}

		/* process options */
		if (*act && *act == '-') {
			act++;
		}
		while (*act && ('0' <= *act && *act <= '9')) {
			act++;
		}
		if (*act == '*') {
			act++;
			strcpy(fmt, "int:");
			fmt += 4;
		}
		if (*act && *act == '.') {
			act++;
		}
		while (*act && ('0' <= *act && *act <= '9')) {
			act++;
		}
		if (*act == '*') {
			act++;
			strcpy(fmt, "int:");
			fmt += 4;
		}

		map = form;
		while (*map) {
			register char *cs = act;

			/* find next conversion mapping */
			while (*map && *map != '%') {
				map++;
			}
			if (*map++ != '%') {
				/* we ran off the map */
				report("%L: unknown conversion specification in format",
					id);
				break;
			}

			while (*map && *map != '=') {
				register int match = 0;

				if (*map == '[') {
					while (*map && *map != ']') {
						if (*map == *cs) {
							match = 1;
						}
						map++;
					}
				}
				else {
					match = (*map == *cs);
				}

				if (match) {
					map++, cs++;
				}
				else	break;
			}
			if (*map++ == '=') {
				/* found the type belonging to %*cs */
				while (*map && *map != '%' && *map != '"') {
					*fmt++ = *map++;
				}
				*fmt++ = ':';
				act = cs;
				break;
			}
		}
	}
	*fmt++ = '\0';
}

PRIVATE form_type(buff, tp)
	char buff[];
	char *tp;
{	/*	store a formatted version of tp in buff
	*/
	if (tp[0] == '"') {
		strcpy(buff, "char*");
	}
	else if (tp[0] == '+') {
		sprintf(buff, "[unsigned] %s", &tp[1]);
	}
	else {
		strcpy(buff, tp);
	}
}

