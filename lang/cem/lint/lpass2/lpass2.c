#include	<varargs.h>

#include	"../lpass1/l_lint.h"
#include	"../lpass1/l_class.h"
#include	"inpdef.h"

#include	<alloc.h>
#include	<system.h>

#define	MSGOUT		STDERR	/* filedes on which to write the messages */
#define	ERROUT		STDERR	/* filedes on which to write the panics */

#define	streq(s1,s2)	(strcmp(s1, s2) == 0)
#define	min(a,b)	((a) <= (b) ? (a) : (b))

#define	type_equal(s1,s2)	(streq(s1, s2))

char *cur_name;
struct inpdef *dot_id,
	*ext_def,
	*static_def;
struct inpdef *id_read();

#define	same_name()	streq(cur_name, dot_id->id_name)

main(argc, argv)
	char *argv[];
{
	struct inpdef *id;

	init(argc, argv);

	get_dot_id();
	while (dot_id) {
		cur_name = dot_id->id_name;
		read_defs();
		while (dot_id && same_name()) {
			id = id_read();
			check(id);
			free_inpdef(id);
		}
		check_usage();
		free_defs();
	}
}

char options[128];
static char *table[] = {0};

init(argc, argv)
	char *argv[];
{
/* Prepare standard input for reading using the input-package
 * Read first inpdef into dot_id
 * Parse options
 */

	char *result;

	while (argc > 1 && *argv[1] == '-') {
		switch (argv[1][1]) {
		case 'u':
		/* don't report situations like "used but not defined"
		 * and "defined but never used"
		 */
		case 'X':		/* ??? prints incoming inpdefs */
			options[argv[1][1]] = 1;
			break;
		default:
			/* ready to extend */
			break;
		}
		argc--, argv++;
	}

	if (!InsertFile((char *)0, table, &result))
		panic("InsertFile() fails");
}

read_defs()
{
	struct inpdef *id;

	if (ext_def || static_def)
		panic("read_defs: slate not clean");/*???*/

	while (dot_id && same_name() && is_def(dot_id)) {
		id = id_read();
		switch (id->id_class) {
		case EFDF:
		case EVDF:
			if (ext_def) {
				report("%L: %s also defined at %L",
					id, id->id_name, ext_def);
				free_inpdef(id);
			}
			else {
				ext_def = id;
			}
			break;
		case SFDF:
		case SVDF:
			if (ext_def) {
				report("%L: %s also defined at %L",
					id, id->id_name, ext_def);
				free_inpdef(id);
			}
			else {
				static_in_list(id);
			}
			break;
		case LFDF:
		case LVDF:
			if (ext_def) {
				/* Some libraries contain more than one
				 * definition
				 */
				if (is_lib_class(ext_def->id_class)) {
					report("%L: %s redefined in library %L",
						id, id->id_name, ext_def);
				}
				free_inpdef(id);
			}
			else {
				ext_def = id;
			}
			break;
		default:
			panic("invalid class (%c) in read_defs", id->id_class);
		}
	}
}

struct inpdef *
id_read()
{
/* Returns the value of dot_id if present, 0 otherwise.
 * Reads a new inpdef ahead, to which dot_id will be pointing.
 * Cur_name will be pointing to id_name of the returned inpdef.
 */
	struct inpdef *old_id;

	if (!dot_id)
		return (0);
	old_id = dot_id;
	cur_name = old_id->id_name;
	get_dot_id();
	return (old_id);
}

get_dot_id()
{
/* Allocates a new inpdef, calls it dot_id and fills it */
	dot_id = new_inpdef();
	if (!get_id(dot_id)) {
		free_inpdef(dot_id);
		cur_name = "";
		dot_id = 0;
	}
}

struct inpdef *definition();

check(id)
	struct inpdef *id;
{
/* Checks a declaration, function call or variable usage, described by id,
 * against the definitions.
 */
	struct inpdef *idef;

	idef = definition(id);
	switch (id->id_class) {
	case EFDC:
		if (!idef) {
			if (!options['u']) {
				report("%L: %s declared but never defined",
					id, id->id_name);
			}
			discard_defs();
			break;
		}
		if (idef->id_class == EVDF || idef->id_class == LVDF) {
			report("%L: function %s declared as variable at %L",
				id, id->id_name, idef);
			break;
		}
		if (!type_equal(id->id_type, idef->id_type)) {
			report("%L: value of function %s declared differently at %L",
				id, id->id_name, idef);
		}
		break;
	case EVDC:
		if (!idef) {
			if (!options['u']) {
				report("%L: %s declared but never defined",
					id, id->id_name);
			}
			discard_defs();
			break;
		}
		if (idef->id_class == EFDF || idef->id_class == LFDF) {
			report("%L: variable %s declared as function at %L",
				id, id->id_name, idef);
			break;
		}
		if (!type_equal(id->id_type, idef->id_type)) {
			report("%L: variable %s declared differently at %L",
				id, id->id_name, idef);
		}
		break;
	case IFDC:
		if (!idef)
			break;		/* used but not defined */
		if (idef->id_class == EVDF || idef->id_class == LVDF) {
			report("%L: function %s declared as variable at %L",
				id, id->id_name, idef);
			break;
		}
		if (!type_equal(id->id_type, idef->id_type)) {
			report("%L: function value of %s declared differently at %L",
				id, id->id_name, idef);
		}
		break;
	case FC:
		if (!idef) {
			if (!options['u']) {
				report("%L: function %s used but not defined",
					id, id->id_name);
			}
			discard_defs();
			break;
		}
		idef->id_called = 1;
		check_args(id, idef);
		if (id->id_returns == USED && !idef->id_returns) {
			report("%L: value of %s is used, but none is returned at %L",
				id, id->id_name, idef);
		}
		switch (id->id_returns) {
		case USED:
			idef->id_used = 1;
			break;
		case IGNORED:
			idef->id_ignored = 1;
			break;
		case VOIDED:
			idef->id_voided = 1;
			break;
		default:
			panic("invalid id->id_returns in check");
		}
		break;
	case VU:
		if (!idef) {
			if (!options['u']) {
				report("%L: variable %s used but not defined",
					id, id->id_name);
			}
			discard_defs();
			break;
		}
		idef->id_called = 1;
		break;
	case EFDF:
	case SFDF:
	case EVDF:
	case SVDF:
	case LFDF:
	case LVDF:
		panic("check() called for a definition");
		break;
	default:
		panic("invalid class (%c) in check", id->id_class);
		break;
	}
}

discard_defs()
{
/* Read until a definition having another name */

	struct inpdef *id;

	while (dot_id && same_name()) {
		id = id_read();
		free_inpdef(id);
	}
}

check_args(id, idef)
	struct inpdef *id, *idef;
{
	register char *act_tp = id->id_argtps;
	register char *def_tp = idef->id_argtps;
	register int i;
	register int nrargs;		/* # of args to be type-checked */
	register int varargs;

	/* determine nrargs */
	if (idef->id_nrargs < 0) {
		varargs = 1;
		nrargs = -idef->id_nrargs - 1;
	}
	else {
		varargs = 0;
		nrargs = idef->id_nrargs;
	}

	/* adjust nrargs, if necessary */
	if (varargs) {
		if (nrargs > id->id_nrargs) {
			report("%L: number of args to %s differs from %L",
				id, id->id_name, idef);
			nrargs = id->id_nrargs;
		}
	}
	else {
		if (nrargs != id->id_nrargs) {
			report("%L: number of args to %s differs from %L",
				id, id->id_name, idef);
			nrargs = min(nrargs, id->id_nrargs);
		}
	}

	for (i = 1; i <= nrargs; i++) {
		register char *act = act_tp;
		register char *def = def_tp;

		/* isolate actual argument type */
		while (*act_tp) {
			if (*act_tp == ':') {
				*act_tp = '\0';
				break;
			}
			act_tp++;
		}
		/* isolate formal argument type */
		while (*def_tp) {
			if (*def_tp == ':') {
				*def_tp = '\0';
				break;
			}
			def_tp++;
		}

		if (!type_match(act, def)) {
			report("%L: arg %d of %s differs from that at %L",
				id, i, id->id_name, idef);
		}
		*act_tp++ = ':';
		*def_tp++ = ':';
	}
}

int
type_match(act, def)
	char *act, *def;
{
	if (type_equal(act, def))
		return 1;
	if (act[0] == '+') {
		/* a non-negative constant */
		/* might be signed or unsigned */
		if (type_equal(&act[1], def))
			return 1;
		if (	strncmp(def, "unsigned ", 9)
		&&	type_equal(&act[1], &def[10])
		) {
			return 1;
		}
	}
	return 0;
}

check_usage()
{
/* Checks if the defined function or variable is used.
 * There can be several static definitions.
 */

	struct inpdef *sd = static_def;

	if (ext_def)
		check_def(ext_def);
	while (sd) {
		check_def(sd);
		sd = sd->next;
	}
}

check_def(id)
	struct inpdef *id;
{
	if (!id->id_called) {
		if (streq(id->id_name, "main")) {
			/* silent */
		}
		else if (ext_def && is_lib_class(ext_def->id_class)) {
			/* silent */
		}
		else {
			if (!options['u']) {
				report("%L: %s defined but never used",
					id, id->id_name);
			}
		}
	}

	if (is_fundef_class(id->id_class)) {
		if (id->id_returns && id->id_called && id->id_ignored) {
			report("%L: %s returns value which is %s ignored",
				id, id->id_name,
				(id->id_used || id->id_voided) ?
						"sometimes" : "always");
		}
	}
}

static_in_list(id)
	struct inpdef *id;
{
/* Put id in the list of static definitions.
 * static_def points to the first element.
 */
	id->next = static_def;
	static_def = id;
}

struct inpdef *
definition(id)
	struct inpdef *id;
{
/* If there is a static definition that comes from the same file, a pointer
 * to this definition will be returned.
 * Otherwise a pointer to the ext_def is returned (which may be null).
 */
	struct inpdef *sd = static_def;

	while (sd) {
		if (id->id_statnr == sd->id_statnr)
			return (sd);
		sd = sd->next;
	}
	return (ext_def);
}

free_defs()
{
/* Dispose the external definition and the static definitions. */
	struct inpdef *sd;

	if (ext_def) {
		free_inpdef(ext_def);
		ext_def = 0;
	}
	while (static_def) {
		sd = static_def;
		static_def = static_def->next;
		free_inpdef(sd);
	}
}

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

/* for DEBUGGING */
print_id(id)
	struct inpdef *id;
{
	print("inpdef: %s, %s, %04d, \"%s\", %d, %d, %s, %d, %s\n",
		id->id_class == EFDF ? "EFDF" :
		id->id_class == SFDF ? "SFDF" :
		id->id_class == EVDF ? "EVDF" :
		id->id_class == SVDF ? "SVDF" :
		id->id_class == LFDF ? "LFDF" :
		id->id_class == LVDF ? "LVDF" :
		id->id_class == EFDC ? "EFDC" :
		id->id_class == EVDC ? "EVDC" :
		id->id_class == IFDC ? "IFDC" :
		id->id_class == FC ? "FC" :
		id->id_class == VU ? "VU" :
		id->id_class == ERRCL ? "ERRCL" : "<BADCLASS>",
		id->id_name,
		id->id_statnr,
		id->id_file,
		id->id_line,
		id->id_nrargs,
		((id->id_nrargs == 0) ? "" : id->id_argtps),
		id->id_returns,
		id->id_type);
}

