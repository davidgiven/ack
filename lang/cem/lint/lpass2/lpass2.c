/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include	<alloc.h>

#include	"private.h"
#include	"l_lint.h"
#include	"l_class.h"
#include	"class.h"
#include	"inpdef.h"

extern char *strcpy();

#define	streq(s1,s2)	(strcmp(s1, s2) == 0)

static char cur_name[NAMESIZE];
static struct inpdef *dot, *lib, *proto, *ext, *sta;

static one_name();
static chk_def();
static ext_decls();
static proto_defs();
static chk_proto();
static ext_def();
static get_dot();
static init();
static lib_def();
static one_ext_decl();
static one_func_call();
static one_var_usage();
static stat_def();
static statics();
static usage();

#define	same_name()	(dot && streq(cur_name, dot->id_name))
#define	same_obj(stnr)	(same_name() && dot->id_statnr == stnr)

#define	def_or_dec(id)	(is_class(id, CL_DEF) ? "defined" : "declared")
#define	fun_or_var(id)	(is_class(id, CL_FUNC) ? "function" : "variable")


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
		if (proto) {
			free_inpdef(proto);
			proto = 0;
		}
		if (ext) {
			free_inpdef(ext);
			ext = 0;
		}
		one_name();
	}
}

char loptions[128];
static char *table[] = {0};

static init(argc, argv)
	char *argv[];
{
/*
 * Get command line options
 * Prepare standard input for reading using the input-package
 */
	char *result;

	init_class();

	while (argc > 1 && argv[1][0] == '-') {
		char *arg = &argv[1][1];
		char ch;

		while (ch = *arg++) {
			switch (ch) {
			case 'u':
				/*	don't report situations like
					"not used anywhere"
				*/
			case 'X':	/* ??? prints incoming inpdefs */
			default:	/* and any other */
				loptions[ch] = 1;
				break;
			}
		}
		argc--, argv++;
	}

	if (!InsertFile((char *)0, table, &result)) {
		panic("InsertFile() fails");
	}
}

static get_dot()
{
	if (!get_id(dot)) {
		free_inpdef(dot);
		dot = 0;
		cur_name[0] = '\0';
	}
	if (loptions['X']) {
		print_id("get_dot", dot);
	}
}

static one_name()
{
	strcpy(cur_name, dot->id_name);
	lib_def();
	proto_defs();
	ext_def();
	ext_decls();
	usage(0);
	if (proto) {
		chk_def(proto);
	}
	else
	if (ext) {
		chk_def(ext);
	}
	statics();
	if (same_name()) {
		/* there are lines for this name that have not been absorbed */
		panic("sequence error in intermediate file");
	}
}

/******** L I B R A R Y ********/

static lib_def()
{
	if (same_obj(0) && is_class(dot, CL_LIB)) {
		lib = dot;
		dot = new_inpdef();
		get_dot();
		while (same_obj(0) && is_class(dot, CL_LIB)) {
			report(">%L: multiple definition of %s in library",
				dot, dot->id_name);
			get_dot();
		}
	}
}


/******** P R O T O T Y P E S ********/
static proto_defs()
{
	if (same_obj(0) && dot->id_class == PFDF) {
		if (lib) {
			report("%L: function %s also defined in %L",
				dot, dot->id_name, lib);
		}
		proto = dot;
		dot = new_inpdef();
		get_dot();
		while (same_obj(0) && dot->id_class == PFDF) {
			chk_proto(dot);
			get_dot();
		}
	}
}

static chk_proto(def)
	struct inpdef *def;
{
	if (proto->id_args) {
		chk_args(def, proto);
	}
	if (!type_equal(def->id_type, proto->id_type)) {
		report("%L: return type of function %s declared differently at %L",
			def, def->id_name, proto);
	}
}


/******** E X T E R N ********/

static ext_def()
{
	if (same_obj(0) && is_class(dot, CL_EXT|CL_DEF)) {
		if (lib && !proto) {
			report("%L: %s %s also defined in %L",
				dot, fun_or_var(dot), dot->id_name, lib);
		}
		if (proto) {
			chk_proto(dot);
		}
		ext = dot;
		dot = new_inpdef();
		get_dot();
		while (same_obj(0) && is_class(dot, CL_EXT|CL_DEF)) {
			report("%L: %s %s also defined at %L",
				dot, fun_or_var(dot), dot->id_name, ext);
			get_dot();
		}
	}
}

static ext_decls()
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

static one_ext_decl(kind, other_kind, other_class)
	char *kind;
	char *other_kind;
	int other_class;
{
	struct inpdef *def = (proto ? proto : ext ? ext : lib ? lib : 0);

	if (!def) {
		/* the declaration will have to serve */
		ext = dot;
		dot = new_inpdef();
		get_dot();
		return;
	}

	if (is_class(def, other_class)) {
		/* e.g.: function FFF declared as variable at ... */
		report("%L: %s %s %s as %s at %L",
			dot, kind, dot->id_name,
			def_or_dec(def), other_kind, def
		);
		/* no further testing possible */
		get_dot();
		return;
	}

	if (!type_equal(dot->id_type, def->id_type)) {
		/* e.g.: type of variable VVV defined differently at ... */
		report("%L: type of %s %s %s differently at %L",
			dot, kind, dot->id_name, def_or_dec(def), def);

		/* no further testing needed */
		get_dot();
		return;
	}

	get_dot();
}


/******** U S A G E ********/

static usage(stnr)
	int stnr;
{
	struct inpdef *def =
		(stnr ? sta : proto ? proto : ext ? ext : lib ? lib : 0);
	int VU_count = 0;
	int VU_samefile = 0;

	while (same_obj(stnr) && dot->id_class == FC) {
		one_func_call(def);
	}

	while (same_obj(stnr) && dot->id_class == VU) {
		VU_count++;
		if (def && streq(def->id_file, dot->id_file)) {
			VU_samefile++;
		}
		one_var_usage(def);
	}

	if (def && loptions['h']) {
		char *fn = def->id_file;

		if (	stnr == 0
		&&	VU_count == 1
		&&	VU_samefile == 1
		&&	(	def == proto
			||	(def == ext && !is_class(ext, CL_IMPL))
			)
		&&	streq(&fn[strlen(fn)-2], ".c")
		) {
			report("%L: extern %s could be declared static",
				def, def->id_name);
		}
	}
}

static one_func_call(def)
	struct inpdef *def;
{
	if (!def) {
		if (!loptions['u']) {
			report("%L: function %s used but not defined",
				dot, dot->id_name);
		}
		get_dot();
		return;
	}

	def->id_called = 1;

	if (def->id_args) {
		chk_args(dot, def);
		if (	dot->id_valused == USED
		&&	def->id_valreturned == NOVALRETURNED
		) {
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
		panic("invalid dot->id_valused in one_func_call()");
		break;
	}

	get_dot();
}

static one_var_usage(def)
	struct inpdef *def;
{
	if (!def) {
		if (!loptions['u']) {
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

static statics()
{
	while (same_name()) {
		int stnr = dot->id_statnr;

		if (stnr == 0) {
			panic("sequence error in intermediate file: externals after statics");
		}

		if (sta) {
			free_inpdef(sta);
			sta = 0;
		}

		stat_def(stnr);
		usage(stnr);

		if (sta) {
			chk_def(sta);
		}

		if (same_obj(stnr)) {
			panic("sequence error in intermediate file: statics out of order");
		}
	}
}

static stat_def(stnr)
	int stnr;
{
	if (same_obj(stnr) && is_class(dot, CL_STAT|CL_DEF)) {
		if (lib) {
			report("%L: %s %s also defined in %L",
				dot, fun_or_var(dot), dot->id_name, lib);
		}
		if (proto || ext) {
			struct inpdef *def = (proto ? proto : ext);

			if (!streq(dot->id_file, def->id_file)) {
				report("%L: %s %s also %s at %L",
					dot, fun_or_var(dot), dot->id_name,
					def_or_dec(def), def
				);
			}
		}
		sta = dot;
		dot = new_inpdef();
		get_dot();
		while (same_obj(stnr) && is_class(dot, CL_STAT|CL_DEF)) {
			report("%L: %s %s also defined at %L",
				dot, fun_or_var(dot), dot->id_name, sta);
			get_dot();
		}
	}
}

static chk_def(def)
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
			if (!loptions['u']) {
				report("%L: %s %s not used anywhere",
					def, fun_or_var(def), def->id_name);
			}
		}
	}

	if (is_class(def, CL_DEF|CL_FUNC)) {
		if (	def->id_valreturned == VALRETURNED
		&&	def->id_called
		&&	def->id_ignored
		) {
			report("%L: %s returns value which is %s ignored",
				def, def->id_name,
				(def->id_used || def->id_voided) ?
						"sometimes" : "always");
		}
	}
}


/******** D E B U G G I N G ********/

print_id(name, id)
	char *name;
	struct inpdef *id;
{
	if (!id) {
		printf("%s: <NO_INPDEF>\n", name);
		return;
	}

	printf("%s: %s, %s, %04d, \"%s\", %d, %s", name,
		id->id_class == LFDF ? "LFDF" :
		id->id_class == LVDF ? "LVDF" :
		id->id_class == PFDF ? "PFDF" :
		id->id_class == EFDF ? "EFDF" :
		id->id_class == EVDF ? "EVDF" :
		id->id_class == EFDC ? "EFDC" :
		id->id_class == EVDC ? "EVDC" :
		id->id_class == IFDC ? "IFDC" :
		id->id_class == SFDF ? "SFDF" :
		id->id_class == SVDF ? "SVDF" :
		id->id_class == FC ? "FC" :
		id->id_class == VU ? "VU" : "<BADCLASS>",
		id->id_name,
		id->id_statnr,
		id->id_file,
		id->id_line,
		id->id_type
	);
	if (is_class(id, CL_FUNC|CL_DEF) || is_class(id, CL_FUNC|CL_USAGE)) {
		printf(", %d, %s, %s",
			id->id_nrargs,
			(id->id_nrargs == 0 ? "" : id->id_argtps),
			(	id->id_class == FC
			?	(	id->id_valused == USED ? "USED" :
					id->id_valused == IGNORED ? "IGNORED" :
					id->id_valused == VOIDED ? "VOIDED" :
					"<BAD VALUSED>"
				)
			:	(	id->id_valreturned == NOVALRETURNED
				?	"NOVALRETURNED"
				:	id->id_valreturned == VALRETURNED
				?	"VALRETURNED"
				:	id->id_valreturned == NORETURN
				?	"NORETURN"
				:	"<BAD VALRETURNED>"
				)
			)
		);
	}
	printf("\n");
}

