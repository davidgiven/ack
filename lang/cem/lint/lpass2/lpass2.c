#include "../lpass1/lint.h"
#include "../lpass1/manifest.h"
#include "inpdef.h"
#include <ctype.h>	/* for efficient definition of isdigit */

#define INP_NPUSHBACK 2

#include <inp_pkg.spec>
#include <inp_pkg.body>

#define min(a, b) ((a) <= (b) ? (a) : (b))
#define same_name() !strcmp(cur_name, next_id->id_name)

#define ReadUnsigned ReadInt	/* for the time being ??? */

char *cur_name;
struct inpdef *next_id,
	*ext_def,
	*static_def;
struct inpdef *id_read();
static int LineNr = 1;

/* Two dangerous macro's. They replace a single statement by
 * two statements
 */
#define loadchar(ch) LoadChar(ch); if (ch=='\n') LineNr++
#define pushback(ch) PushBack(); if (ch=='\n') LineNr--


main(argc, argv)
	char *argv[];
{
	struct inpdef *id;

	init(argc, argv);
	while (next_id) {
		cur_name = next_id->id_name;
		read_defs();
		while (next_id && same_name()) {
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
 * Read first inpdef into next_id
 * Parse options
 */

	char *result;

	if (!InsertFile((char *)0, table, &result))
		panic("InsertFile() fails");
	next_id = new_inpdef();
	if (get_id(next_id) == EOI) {
		free_inpdef(next_id);
		next_id = 0;
		return;
	}
	for (;argc > 1 && *argv[1] == '-'; argc--, argv++)
		switch (argv[1][1]) {
		case 'u':
		/* don't give warnings like "used but not defined"
		 * and "defined but never used"
		 */
			options[argv[1][1]] = 1;
			break;
		default:
			/* ready to extend */
			break;
		}
}

read_defs()
{
	struct inpdef *id;

	while (next_id && same_name() && next_id->id_class <= LVDF) {
		id = id_read();
		switch (id->id_class) {
		case EFDF:
		case EVDF:
			if (ext_def) {
				warning("%s multiply defined", id->id_name);
				places(id, ext_def);
				free_inpdef(id);
			}
			else {
				ext_def = id;
			}
			break;
		case SFDF:
		case SVDF:
			if (ext_def) {
				warning("%s multiply defined", id->id_name);
				places(id, ext_def);
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
				if (	ext_def->id_class != LFDF
				&&	ext_def->id_class != LVDF
				) {
					warning("%s also defined in library",
						id->id_name);
					places(id, ext_def);
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
/* Returns the value of next_id if present, 0 otherwise.
 * Reads a new inpdef ahead, to which next_id will be pointing.
 * Cur_name will be pointing to id_name of the returned inpdef.
 */
	struct inpdef *old_id;

	if (!next_id)
		return (0);
	old_id = next_id;
	cur_name = old_id->id_name;
	next_id = new_inpdef();
	if (get_id(next_id) == EOI) {
		free_inpdef(next_id);
		cur_name = "";
		next_id = 0;
	}
	return (old_id);
}

int
get_id(id)
	struct inpdef *id;
{
/* A low-level function which just reads a definition */

	int eoi;

	if (ReadString(id->id_name, ':', NAMESIZE) == EOI)
		return (EOI);
	loadchar(id->id_class);
	if (id->id_class == EOI)
		return (EOI);
	SkipChar(':');
	if (ReadString(id->id_file, ':', FNAMESIZE) == EOI)
		return (EOI);
	if (ReadUnsigned(&id->id_line) == EOI)
		return (EOI);
	SkipChar(':');
	switch (id->id_class) {
	case EFDF:
	case SFDF:
	case LFDF:
	case FC:
		if (ReadInt(&id->id_nrargs) == EOI)
			return (EOI);
		SkipChar(':');
		eoi = args_read(
			(id->id_nrargs < 0 ? -id->id_nrargs-1 : id->id_nrargs),
			id->id_argtps);
		if (eoi == EOI)
			return (EOI);
		if (ReadInt(&id->id_returns) == EOI)
			return (EOI);
		SkipChar(':');
		break;
	}
	return (ReadString(id->id_type, '\n', TYPESIZE));
}

int
ReadString(buf, delim, maxsize)
	char *buf;
{
/* Reads a string until 'delim' is encountered.
 * Delim is discarded.
 * If 'maxsize-1' is exeded, "string too long" is written by panic().
 * A '\0' is appended to the string.
 * At EOI EOI is returned, else the length of the string (including
 * the appended '\0') is returned.
 */
	int ch;
	int nread = 0;

	while (nread < maxsize - 1) {
		loadchar(ch);
		if (ch == EOI)
			return (EOI);
		if (ch == delim)
			break;
		buf[nread++] = (char)ch;
	}
	if (ch != delim) {
		buf[nread] = '\0';
		panic("line %d: string too long: %s", LineNr, buf);
	}
	buf[nread++] = '\0';
	return (nread);
}

int
ReadInt(ip)
	int *ip;
{
/* Reads a decimal integer until a character which is not
 * a digit is encountered.
 * Non-digits except minus-sign in front of the number are discarded.
 * Doesn't check on overflow.
 * Just a minus-sign is interpreted as 0. (To prevent a look-ahead.)
 * At EOI EOI is returned.
 */
	int ch;
	int negative = 0;
	int i = 0;

	do {
		loadchar(ch);
	}	/* {} needed because of the loadchar-macro; yack */
	while (!isdigit(ch) && ch != '-');
	if (ch == EOI)
		return (EOI);
	if (ch == '-')
		negative = 1;
	else
		i = ch - '0';
	loadchar(ch);
	while (isdigit(ch)) {
		i = 10*i + ch - '0';
		loadchar(ch);
	}
	pushback(ch);
	*ip = negative ? -i : i;
	return (!EOI);
}

SkipChar(ch)
{
	int c;

	loadchar(c);
	if (c != ch)
		panic("line %d: bad format, '%c' expected; '%c' read",
				LineNr, ch, c);
}

int
args_read(nrargs, buf)
char *buf;
{
/* Reads a string into buf with format <type1>:<type2>: ... :<typen>: */

	int i;
	int charcount = 1;
	int n;

	*buf = '\0';
	for (i = 0; i < nrargs; i++) {
		if ((n = ReadString(buf, ':', ARGTPSSIZE-charcount-1)) == EOI)
			return (EOI);
		charcount += n;
		buf += n - 1;
		*buf++ = ':';
	}
	*buf = '\0';
	return (!EOI);
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
			uwarning("%s declared but never defined", id->id_name);
			unewline();
			discard_defs();
			break;
		}
		if (idef->id_class == EVDF || idef->id_class == LVDF) {
			warning("%s value declared inconsistently",
				id->id_name);
			places(id, idef);
			break;
		}
		if (strcmp(id->id_type, idef->id_type)) {
			warning("%s value declared inconsistently",
				id->id_name);
			places(id, idef);
		}
		break;
	case EVDC:
		if (!idef) {
			uwarning("%s declared but never defined", id->id_name);
			unewline();
			discard_defs();
			break;
		}
		if (idef->id_class == EFDF || idef->id_class == LFDF) {
			warning("%s value declared inconsistently",
				id->id_name);
			places(id, idef);
			break;
		}
		if (strcmp(id->id_type, idef->id_type)) {
			warning("%s value declared inconsistently",
				id->id_name);
			places(id, idef);
		}
		break;
	case IFDC:
		if (!idef)
			break;		/* used but not defined */
		if (idef->id_class == EVDF || idef->id_class == LVDF) {
			warning("%s value declared inconsistently",
				id->id_name);
			places(id, idef);
			break;
		}
		if (strcmp(id->id_type, idef->id_type)) {
			warning("%s implicitly declared inconsistently",
				id->id_name);
			places(id, idef);
		}
		break;
	case FC:
	case VU:
		if (!idef) {
			uwarning("%s used but not defined", id->id_name);
			unewline();
			discard_defs();
			break;
		}
		idef->id_called = 1;
		if (id->id_class == VU)
			break;
		check_args(id, idef);
		if (id->id_returns == USED && !idef->id_returns) {
			warning("%s value is used, but none is returned",
				id->id_name);
			places(id, idef);
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
	default:
		panic("invalid class (%c) in check", id->id_class);
		break;
	}
}

discard_defs()
{
/* Read until a definition having another name */

	struct inpdef *id;

	while (next_id && same_name()) {
		id = id_read();
		free_inpdef(id);
	}
}

check_args(id, idef)
	struct inpdef *id, *idef;
{
	register char *argtps1 = id->id_argtps;
	register char *argtps2 = idef->id_argtps;
	int i, n, varargs = 0;

	if (idef->id_nrargs < 0) {
		varargs = 1;
		n = -idef->id_nrargs - 1;
	}
	else {
		n = idef->id_nrargs;
	}
	if (varargs) {
		if (n > id->id_nrargs) {
			warning("%s variable # of args", id->id_name);
			places(id, idef);
			n = id->id_nrargs;
		}
	}
	else {
		if (n != id->id_nrargs) {
			warning("%s variable # of args", id->id_name);
			places(id, idef);
			n = min(n, id->id_nrargs);
		}
	}
	for (i = 1; i <= n; i++) {
		while (*argtps1 == *argtps2 && *argtps1 != ':') {
			argtps1++;
			argtps2++;
		}
		if (*argtps1 != *argtps2) {
			warning("%s, arg %d used inconsistently",
				id->id_name, i);
			places(id, idef);
		}
		while (*argtps1++ != ':') ;
		while (*argtps2++ != ':') ;
	}
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
	if (	!id->id_called && strcmp(id->id_name, "main")
	&&	ext_def->id_class != LFDF
	&&	ext_def->id_class != LVDF
	) {
		uwarning("%s defined (%s(%u)) but never used",
			id->id_name, id->id_file, id->id_line);
		unewline();
	}
	if (	id->id_class == EFDF
	||	id->id_class == SFDF
	||	id->id_class == LFDF
	) {
		if (id->id_returns && id->id_called && id->id_ignored) {
			warning("%s returns value which is %s ignored",
				id->id_name,
				id->id_used || id->id_voided ?
				"sometimes" : "always");
			newline();
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
		if (!strcmp(id->id_file, sd->id_file))
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

#include "../lpass1/errout.h"	/* for definition of ERROUT */

places(id1, id2)
	struct inpdef *id1, *id2;
{
	fprint(ERROUT, "\t%s(%u) :: %s(%u)\n",
		id1->id_file, id1->id_line, id2->id_file, id2->id_line);
}

/* VARARGS1 */
warning(fmt, args)
	char *fmt;
{
	doprnt(ERROUT, fmt, &args);
}

/* VARARGS1 */
uwarning(fmt, args)
	char *fmt;
{
	if (!options['u'])
		doprnt(ERROUT, fmt, &args);
}

newline()
{
	fprint(ERROUT, "\n");
}

unewline()
{
	if (!options['u'])
		fprint(ERROUT, "\n");
}

/* VARARGS1 */
panic(fmt, args)
	char *fmt;
{
	fprint(ERROUT, "PANIC: ");
	doprnt(ERROUT, fmt, &args);
	fprint(ERROUT, "\n");
	exit(1);
}

/* for DEBUGGING */
print_id(id)
	struct inpdef *id;
{
	print("%c, %s, %s, %u, %d, %s, %d, %s\n",
		id->id_class,
		id->id_name,
		id->id_file,
		id->id_line,
		id->id_nrargs,
		((id->id_nrargs == 0) ? "" : id->id_argtps),
		id->id_returns,
		id->id_type);
}
