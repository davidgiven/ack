#include	<alloc.h>

#include	"../lpass1/l_lint.h"
#include	"../lpass1/l_class.h"
#include	"class.h"
#include	"inpdef.h"

#define	streq(s1,s2)	(strcmp(s1, s2) == 0)
#define	min(a,b)	((a) <= (b) ? (a) : (b))

char cur_name[NAMESIZE];
struct inpdef *dot, *lib, *ext, *sta;

#define	same_name()	(dot && streq(cur_name, dot->id_name))
#define	same_obj(stnr)	(same_name() && dot->id_statnr == stnr)

#define	defdec(id)	(is_class(id, CL_DEF) ? "defined" : "declared")


/******** M A I N ********/

main(argc, argv)
	char *argv[];
{
	init(argc, argv);

	dot = new_inpdef();
	get_dot();
	while (dot) {
		if (lib) {
			free_inpdef(lib);
			lib = 0;
		}
		if (ext) {
			free_inpdef(ext);
			ext = 0;
		}
		strcpy(cur_name, dot->id_name);
		lib_def();
		ext_def();
		ext_decls();
		usage(0);
		if (ext)
			check_def(ext);
		statics();
		/* inpdefs of class ERRCL are never generated */
		if (same_name()) {
			/*	there are more lines for this name that have
				not been absorbed
			*/
			panic("sequence error in intermediate file");
		}
	}
}

char options[128];
static char *table[] = {0};

init(argc, argv)
	char *argv[];
{
/*
 * Parse options
 * Prepare standard input for reading using the input-package
 */
	char *result;

	init_class();

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
			/* ready to be extended */
			break;
		}
		argc--, argv++;
	}

	if (!InsertFile((char *)0, table, &result))
		panic("InsertFile() fails");
}

get_dot()
{
	if (!get_id(dot)) {
		free_inpdef(dot);
		dot = 0;
		cur_name[0] = '\0';
	}
}


/******** L I B R A R Y ********/

lib_def()
{
	if (same_obj(0) && is_class(dot, CL_LIB)) {
		lib = dot;
		dot = new_inpdef();
		get_dot();
	}

	while (same_obj(0) && is_class(dot, CL_LIB)) {
		report("%L: multiple definition of %s in library",
			dot, dot->id_name);
		get_dot();
	}
}


/******** E X T E R N ********/

ext_def()
{
	if (same_obj(0) && is_class(dot, CL_EXT|CL_DEF)) {
		if (lib) {
			report("%L: %s also defined in library %L",
				dot, dot->id_name, lib);
		}
		ext = dot;
		dot = new_inpdef();
		get_dot();
	}

	while (same_obj(0) && is_class(dot, CL_EXT|CL_DEF)) {
		report("%L: %s also defined at %L",
			dot, dot->id_name, ext);
		get_dot();
	}
}

ext_decls()
{
	while (same_obj(0) && dot->id_class == EFDC) {
		one_ext_decl("function", "variable", CL_VAR);
	}

	while (same_obj(0) && dot->id_class == EVDC) {
		one_ext_decl("variable", "function", CL_FUNC);
	}

	while (same_obj(0) && dot->id_class == IFDC) {
		one_ext_decl("function", "variable", CL_VAR);
	}
}

one_ext_decl(kind, other_kind, other_class)
	char *kind;
	char *other_kind;
	int other_class;
{
	struct inpdef *def = ext ? ext : lib ? lib : 0;

	if (!def) {
		/* the declaration will have to serve */
		if (!is_class(dot, CL_IMPL) && !options['u']) {
			report("%L: %s %s declared but never defined",
				dot, dot->id_name, kind);
		}
		ext = dot;
		dot = new_inpdef();
		get_dot();
		return;
	}

	if (is_class(def, other_class)) {
		/* e.g.: function FFF declared as variable at ... */
		report("%L: %s %s %s as %s at %L",
			dot, kind, dot->id_name, defdec(def), other_kind, def);
		/* no further testing possible */
		return;
	}

	if (!type_equal(dot->id_type, def->id_type)) {
		/* e.g.: type of variable VVV defined differently at ... */
		report("%L: type of %s %s %s differently at %L",
			dot, kind, dot->id_name, defdec(def), def);
	}

	get_dot();
}


/******** U S A G E ********/

usage(stnr)
	int stnr;
{
	struct inpdef *def = stnr ? sta : ext ? ext : lib ? lib : 0;

	while (same_obj(stnr) && dot->id_class == FC) {
		one_func_call(def);
	}

	while (same_obj(stnr) && dot->id_class == VU) {
		one_var_usage(def);
	}
}

one_func_call(def)
	struct inpdef *def;
{
	if (!def) {
		if (!options['u']) {
			report("%L: function %s used but not defined",
				dot, dot->id_name);
		}
		get_dot();
		return;
	}

	def->id_called = 1;

	if (def->id_args) {
		check_args(dot, def);
		if (dot->id_valused == USED && !def->id_valreturned) {
			report("%L: value of %s is used, but none is returned at %L",
				dot, dot->id_name, def);
		}
	}

	switch (dot->id_valused) {
	case USED:
		def->id_used = 1;
		break;
	case IGNORED:
		def->id_ignored = 1;
		break;
	case VOIDED:
		def->id_voided = 1;
		break;
	default:
		panic("invalid dot->id_valused in check");
		break;
	}

	get_dot();
}

one_var_usage(def)
	struct inpdef *def;
{
	if (!def) {
		if (!options['u']) {
			report("%L: variable %s used but not defined",
				dot, dot->id_name);
		}
		get_dot();
		return;
	}

	def->id_called = 1;

	get_dot();
}


/******** S T A T I C ********/

statics()
{
	while (same_name() && dot->id_statnr != 0) {
		one_static(dot->id_statnr);
	}
}

one_static(stnr)
	int stnr;
{
	while (same_obj(stnr)) {
		if (sta) {
			free_inpdef(sta);
			sta = 0;
		}
		stat_def(stnr);
		usage(stnr);
		if (sta)
			check_def(sta);
		get_dot();
	}
}

stat_def(stnr)
	int stnr;
{
	if (same_obj(stnr) && is_class(dot, CL_STAT|CL_DEF)) {
		if (lib) {
			report("%L: %s also defined in library %L",
				dot, dot->id_name, lib);
		}
		if (ext) {
			report("%L: %s also %s at %L",
				dot, dot->id_name, defdec(ext), ext);
		}
		sta = dot;
		dot = new_inpdef();
		get_dot();
	}

	while (same_obj(stnr) && is_class(dot, CL_STAT|CL_DEF)) {
		report("%L: %s also defined at %L",
			dot, dot->id_name, sta);
		get_dot();
	}
}

check_def(def)
	struct inpdef *def;
{
	if (!def)
		return;

	if (!def->id_called) {
		if (streq(def->id_name, "main")) {
			/* silent */
		}
		else if (ext && is_class(ext, CL_LIB)) {
			/* silent */
		}
		else {
			if (!options['u']) {
				report("%L: %s %s but never used",
					def, def->id_name, defdec(def));
			}
		}
	}

	if (is_class(def, CL_DEF|CL_FUNC)) {
		if (def->id_valreturned && def->id_called && def->id_ignored) {
			report("%L: %s returns value which is %s ignored",
				def, def->id_name,
				(def->id_used || def->id_voided) ?
						"sometimes" : "always");
		}
	}
}


/******** T Y P E   C H E C K I N G ********/

check_args(id, def)
	struct inpdef *id, *def;
{
	register char *act_tp = id->id_argtps;
	register char *def_tp = def->id_argtps;
	register int i;
	register int nrargs;		/* # of args to be type-checked */
	register int varargs;

	/* determine nrargs */
	if (def->id_nrargs < 0) {
		varargs = 1;
		nrargs = -def->id_nrargs - 1;
	}
	else {
		varargs = 0;
		nrargs = def->id_nrargs;
	}

	/* adjust nrargs, if necessary */
	if (varargs) {
		if (nrargs > id->id_nrargs) {
			report("%L: number of args to %s differs from %L",
				id, id->id_name, def);
			nrargs = id->id_nrargs;
		}
	}
	else {
		if (nrargs != id->id_nrargs) {
			report("%L: number of args to %s differs from %L",
				id, id->id_name, def);
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
				id, i, id->id_name, def);
		}
		*act_tp++ = ':';
		*def_tp++ = ':';
	}
}

int
type_equal(act, def)
	char *act, *def;
{
	return streq(act, def);
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


/******** D E B U G G I N G ********/

print_id(id)
	struct inpdef *id;
{
	print("inpdef: %s, %s, %04d, \"%s\", %d, %s",
		id->id_class == LFDF ? "LFDF" :
		id->id_class == LVDF ? "LVDF" :
		id->id_class == EFDF ? "EFDF" :
		id->id_class == EVDF ? "EVDF" :
		id->id_class == EFDC ? "EFDC" :
		id->id_class == EVDC ? "EVDC" :
		id->id_class == IFDC ? "IFDC" :
		id->id_class == SFDF ? "SFDF" :
		id->id_class == SVDF ? "SVDF" :
		id->id_class == FC ? "FC" :
		id->id_class == VU ? "VU" :
		id->id_class == ERRCL ? "ERRCL" : "<BADCLASS>",
		id->id_name,
		id->id_statnr,
		id->id_file,
		id->id_line,
		id->id_type
	);
	if (is_class(id, CL_FUNC|CL_DEF) || is_class(id, CL_FUNC|CL_USAGE)) {
		print(", %d, %s, %s",
			id->id_nrargs,
			id->id_nrargs == 0 ? "" : id->id_argtps,
			id->id_class == FC ?
				(id->id_valused == USED ? "USED" :
				id->id_valused == IGNORED ? "IGNORED" :
				id->id_valused == VOIDED ? "VOIDED" :
				"<BAD VALUSED>")
			:	(id->id_valreturned ? "VALRETURNED" :
				"NOVALRETURNED")
		);
	}
	print("\n");
}

