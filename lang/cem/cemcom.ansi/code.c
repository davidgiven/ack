/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	C O D E - G E N E R A T I N G   R O U T I N E S		*/

#include	"lint.h"
#include	"debug.h"
#include	"dbsymtab.h"
#ifndef	LINT
#include	<em.h>
#else
#include	"l_em.h"
#include	"l_lint.h"
#endif	LINT
#include	"botch_free.h"
#include	<alloc.h>
#include	"dataflow.h"
#include	"use_tmp.h"
#include	<flt_arith.h>
#include	"arith.h"
#include	"type.h"
#include	"idf.h"
#include	"label.h"
#include	"code.h"
#include	"stmt.h"
#include	"def.h"
#include	"expr.h"
#include	"sizes.h"
#include	"stack.h"
#include	"level.h"
#include	"decspecs.h"
#include	"declar.h"
#include	"Lpars.h"
#include	"specials.h"
#include	"atw.h"
#include	"assert.h"
#include	"LLlex.h"
#ifdef	LINT
#include	"l_lint.h"
#endif	LINT
#ifdef DBSYMTAB
#include	<stb.h>
#endif /* DBSYMTAB */

label lab_count = 1;
label datlab_count = 1;

int fp_used;
extern arith NewLocal();	/* util.c	*/

/* global function info */
char *func_name;
struct type *func_type;
int func_notypegiven;

#ifdef USE_TMP
static int	tmp_id;
static int	pro_id;
#endif USE_TMP

extern char options[];
extern char *symbol2str();
extern char *source;

#ifndef	LINT
init_code(dst_file)
	char *dst_file;
{
	/*	init_code() initialises the output file on which the
		compact EM code is written
	*/
	C_init(word_size, pointer_size); /* initialise EM module */
	if (C_open(dst_file) == 0)
		fatal("cannot write to %s\n", dst_file);
	C_magic();
	C_ms_emx(word_size, pointer_size);
#ifdef DBSYMTAB
	if (options['g']) {
		C_ms_std(source, N_SO, 0);
		stb_typedef(int_type, "int");
		stb_typedef(schar_type, "char");
		stb_typedef(long_type, "long");
		stb_typedef(short_type, "short");
		stb_typedef(uchar_type, "unsigned char");
		stb_typedef(ushort_type, "unsigned short");
		stb_typedef(ulong_type, "unsigned long");
		stb_typedef(uint_type, "unsigned int");
		stb_typedef(float_type, "float");
		stb_typedef(double_type, "double");
		stb_typedef(lngdbl_type, "long double");
		stb_typedef(void_type, "void");
	}
#endif /* DBSYMTAB */
#ifdef USE_TMP
#ifdef PREPEND_SCOPES
	C_insertpart(tmp_id = C_getid());
#endif	PREPEND_SCOPES
#endif	USE_TMP
}
#endif	LINT

struct string_cst *str_list = 0;

code_string(val, len, dlb)
	char *val;
	int len;
	label dlb;
{
	register struct string_cst *sc = new_string_cst();

	C_ina_dlb(dlb);
	sc->next = str_list;
	str_list = sc;
	sc->sc_value = val;
	sc->sc_len = len;
	sc->sc_dlb = dlb;
}

def_strings(sc)
	register struct string_cst *sc;
{
	while (sc) {
		struct string_cst *sc1 = sc;

		C_df_dlb(sc->sc_dlb);
		str_cst(sc->sc_value, sc->sc_len, 1);	/* string in rom */
		sc = sc->next;
		free(sc1->sc_value);
		free_string_cst(sc1);
	}
}

/* flush_strings() is called from program.g after each external definition */
flush_strings() {
	if (str_list) {
		def_strings(str_list);
		str_list = 0;
	}
}

#ifndef	LINT
end_code()
{
	/*	end_code() performs the actions to be taken when closing
		the output stream.
	*/
	if (fp_used) {
		/* floating point used	*/
		C_ms_flt();
	}
	C_ms_src((int)(LineNumber - 2), FileName);
	C_close();
}
#endif	LINT

#ifdef	PREPEND_SCOPES
prepend_scopes()
{
	/*	prepend_scopes() runs down the list of global idf's
		and generates those exa's, exp's, ina's and inp's
		that superior hindsight has provided.
	*/
	register struct stack_entry *se = local_level->sl_entry;

#ifdef USE_TMP
	C_beginpart(tmp_id);
#endif USE_TMP
	while (se != 0)	{
		register struct def *df = se->se_idf->id_def;
		
		if (df && (df->df_initialized || df->df_used || df->df_alloc)) {
			code_scope(se->se_idf->id_text, df);
		}
		se = se->next;
	}
#ifdef USE_TMP
	C_endpart(tmp_id);
#endif USE_TMP
}
#endif	PREPEND_SCOPES

code_scope(text, def)
	char *text;
	register struct def *def;
{
	/*	generates code for one name, text, of the storage class
		as given by def, if meaningful.
	*/
	int fund = def->df_type->tp_fund;
	
	switch (def->df_sc)	{
	case EXTERN:
	case GLOBAL:
		if (fund == FUNCTION)
			C_exp(text);
		else
			C_exa_dnam(text);
		break;
	case STATIC:
		if (fund == FUNCTION)
			C_inp(text);
		else
			C_ina_dnam(text);
		break;
	}
}

static label return_label, return2_label;
static char return_expr_occurred;
static arith func_size;
static label func_res_label;
static char *last_fn_given = (char *)0;
static label file_name_label;

begin_proc(ds, idf)		/* to be called when entering a procedure */
	struct decspecs *ds;
	struct idf *idf;
{
	/*	begin_proc() is called at the entrance of a new function
		and performs the necessary code generation:
		-	a scope indicator (if needed) exp/inp
		-	the procedure entry pro $name
		-	reserves some space if the result of the function
			does not fit in the return area
		-	a fil pseudo instruction
	*/
	register char *name = idf->id_text;
	register struct def *def = idf->id_def;

	/* idf->id_def does not indicate the right def structure
	 * when the function being defined has a parameter of the
	 * same name in an old-style function definition.
	 */
	while (def->df_level != L_GLOBAL) def = def->next;

	/* When we have a new-style function definition, the parameters
	 * are defined first, which means that it may look as if they have
	 * the greatest scope.  Correct this.
	 */
	if (idf->id_def == def && def->next && def->next->df_level == L_PROTO) {
		struct def *tmpdef = def->next;

		def->next = tmpdef->next;
		tmpdef->next = def;
		idf->id_def = tmpdef;
	}
#ifndef PREPEND_SCOPES
	code_scope(name, def);
#endif	PREPEND_SCOPES
#ifdef	DATAFLOW
	if (options['d'])
		DfaStartFunction(name);
#endif	DATAFLOW


	/* set global function info */
	func_name = name;
	if (def->df_type->tp_fund != FUNCTION) {
		error("making function body for non-function");
		func_type = error_type;
	}
	else {
		func_type = def->df_type->tp_up;
	}
	func_notypegiven = ds->ds_notypegiven;
	func_size = ATW(func_type->tp_size);
	sp_occurred[SP_SETJMP] = 0;

#ifndef USE_TMP
	C_pro_narg(name);
#else
	C_insertpart(pro_id = C_getid());
#endif
	if (is_struct_or_union(func_type->tp_fund))	{
		if (func_size <= 0) {
			error("unknown return type for function %s", name);
		} else {
			C_df_dlb(func_res_label = data_label());
			C_bss_cst(func_size, (arith)0, 1);
		}
	}
	else
		func_res_label = 0;
	/*	Special arrangements if the function result doesn't fit in
		the function return area of the EM machine.  The size of
		the function return area is implementation dependent.
	*/
	lab_count = (label) 1;
	return_label = text_label();
	return2_label = text_label();
	return_expr_occurred = 0;
	LocalInit();
	prc_entry(name);
	if (! options['L'])	{	/* profiling */
		if (!last_fn_given || strcmp(last_fn_given, FileName) != 0) {
			/* previous function came from other file */
			C_df_dlb(file_name_label = data_label());
			C_con_scon(last_fn_given = FileName,
				(arith)(strlen(FileName) + 1));
		}
		/* enable debug trace of EM source */
		C_fil_dlb(file_name_label, (arith)0);
		C_lin((arith)LineNumber);
	}
#ifdef DBSYMTAB
	if (options['g']) {
		stb_string(def, FUNCTION, name);
		if (! strcmp(name, "main")) {
			C_ms_stb_cst(name, N_MAIN, 0, (arith) 0);
		}
	}
#endif
}

end_proc(fbytes)
	arith fbytes;
{
	/*	end_proc() deals with the code to be generated at the end of
		a function, as there is:
		-	the EM ret instruction: "ret 0"
		-	loading of the function result in the function
			result area if there has been a return <expr>
			in the function body (see do_return_expr())
		-	indication of the use of floating points
		-	indication of the number of bytes used for
			formal parameters
		-	use of special identifiers such as "__setjmp"
		-	"end" + number of bytes used for local variables
	*/
	arith nbytes;
	char optionsn = options['n'];

#ifdef	DATAFLOW
	if (options['d'])
		DfaEndFunction();
#endif	DATAFLOW
	C_df_ilb(return2_label);
	if (return_expr_occurred && func_res_label == 0) {
			C_asp(-func_size);
	}
	C_df_ilb(return_label);
	prc_exit();
	if (return_expr_occurred) {
		if (func_res_label != 0)	{
			C_lae_dlb(func_res_label, (arith)0);
			C_ret(pointer_size);
		}
		else
			C_ret(func_size);
	}
	else	C_ret((arith) 0);

	/* getting the number of "local" bytes is posponed until here,
	   because copying the function result in "func_res_label" may
	   need temporaries! However, local_level is now L_FORMAL2, because
	   L_LOCAL is already unstacked. Therefore, "unstack_level" must
	   also pass "sl_max_block" to the level above L_LOCAL.
	*/
	nbytes = ATW(- local_level->sl_max_block);
#ifdef USE_TMP
	C_beginpart(pro_id);
	C_pro(func_name, nbytes);
#endif
	if (fbytes > max_int) {
		error("%s has more than %ld parameter bytes",
			func_name, (long) max_int);
	}
	C_ms_par(fbytes);		/* # bytes for formals		*/
	if (sp_occurred[SP_SETJMP]) {	/* indicate use of "__setjmp"	*/
		options['n'] = 1;
		C_ms_gto();
		sp_occurred[SP_SETJMP] = 0;
	}
#ifdef USE_TMP
	C_endpart(pro_id);
#endif
	LocalFinish();
	C_end(nbytes);
	if (nbytes > max_int) {
		error("%s has more than %ld bytes of local variables",
			func_name, (long) max_int);
	}
	options['n'] = optionsn;
}

do_return()
{
	/*	do_return handles the case of a return without expression.
		This version branches to the return label, which is
		probably smarter than generating a direct return.
		Return sequences may be expensive.
	*/
#ifdef DBSYMTAB
	if (options['g']) db_line(dot.tk_file, dot.tk_line);
#endif /* DBSYMTAB */
	C_bra(return2_label);
}

do_return_expr(expr)
	struct expr *expr;
{
	/*	do_return_expr() generates the expression and the jump for
		a return statement with an expression.
	*/
	ch3cast(&expr, RETURN, func_type);
	code_expr(expr, RVAL, TRUE, NO_LABEL, NO_LABEL);
	if (func_res_label != 0) {
		C_lae_dlb(func_res_label, (arith)0);
		store_block(func_size, func_type->tp_align);
	}
	C_bra(return_label);
	return_expr_occurred = 1;
}

code_declaration(idf, expr, lvl, sc)
	register struct idf *idf;	/* idf to be declared	*/
	struct expr *expr;	/* initialisation; NULL if absent	*/
	int lvl;		/* declaration level	*/
	int sc;			/* storage class, as in the declaration */
{
	/*	code_declaration() does the actual declaration of the
		variable indicated by "idf" on declaration level "lvl".
		If the variable is initialised, the expression is given
		in "expr", but for global and static initialisations it
		is just non-zero, as the expression is not parsed yet.
		There are some cases to be considered:
		-	filter out typedefs, they don't correspond to code;
		-	global variables, coded only if initialized;
		-	local static variables;
		-	local automatic variables;
		Since the expression may be modified in the process,
		code_declaration() frees it after use, as the caller can
		no longer do so.
		
		If there is a storage class indication (EXTERN/STATIC),
		code_declaration() will generate an exa or ina.
		The sc is the actual storage class, as given in the
		declaration.
	*/
	register struct def *def = idf->id_def;
	register arith size = def->df_type->tp_size;
	int fund = def->df_type->tp_fund;
	int def_sc = def->df_sc;
	
	if (def_sc == TYPEDEF)	{	/* no code for typedefs		*/
#ifdef DBSYMTAB
		if (options['g']) {
			stb_typedef(def->df_type, idf->id_text);
		}
#endif /* DBSYMTAB */
		return;
	}
	if (lvl == L_GLOBAL)	{	/* global variable	*/
		/* is this an allocating declaration? */
		if (	(sc == 0 || sc == STATIC)
			&& fund != FUNCTION
		)
			def->df_alloc = ALLOC_SEEN;
		if (expr && def_sc == STATIC && sc == EXTERN) {
			warning("%s has internal linkage", idf->id_text);
		}
		if (expr) {	/* code only if initialized */
#ifndef PREPEND_SCOPES
			code_scope(idf->id_text, def);
#endif PREPEND_SCOPES
			def->df_alloc = ALLOC_DONE;
			C_df_dnam(idf->id_text);
		}
	}
	else
	if (lvl >= L_LOCAL)	{	/* local variable	*/
		/* STATIC, EXTERN, GLOBAL, AUTO or REGISTER */
		switch (def_sc)	{
		case STATIC:
			if (fund == FUNCTION) {
				/* should produce "inp $function" ??? */
				break;
			}
			/*	they are handled on the spot and get an
				integer label in EM.
			*/
#ifdef DBSYMTAB
			if (options['g'] && ! expr) {
				stb_string(def, sc, idf->id_text);
			}
#endif /* DBSYMTAB */
			C_df_dlb((label)def->df_address);
			if (expr) { /* there is an initialisation */
			}
			else {	/* produce blank space */
				if (size <= 0) {
					error("size of %s unknown", idf->id_text);
					size = (arith)0;
				}
				C_bss_cst(ATW(size), (arith)0, 1);
			}
			break;
		case EXTERN:
			if (expr && !is_anon_idf(idf) && level != L_GLOBAL)
				error("cannot initialize extern in block"
						, idf->id_text);
		case GLOBAL:
			/* we are sure there is no expression */
			break;
		case AUTO:
		case REGISTER:
#ifdef DBSYMTAB
			if (options['g']) {
				stb_string(def, sc, idf->id_text);
			}
#endif /* DBSYMTAB */
			if (expr)
				loc_init(expr, idf);
			else if ((fund == ARRAY)
				    && (def->df_type->tp_size == (arith)-1)) {
				error("size for local %s unknown"
					, idf->id_text);
			}
			break;
		default:
			crash("bad local storage class");
			/*NOTREACHED*/
		}
	}
}

loc_init(expr, id)
	struct expr *expr;
	struct idf *id;
{
	/*	loc_init() generates code for the assignment of
		expression expr to the local variable described by id.
		It frees the expression afterwards.
	*/
	register struct expr *e = expr;
	register struct def *df = id->id_def;
	register struct type *tp = df->df_type;
	static arith tmpoffset = 0;
	static arith unknownsize = 0;
	
	ASSERT(df->df_sc != STATIC);
	switch (tp->tp_fund)	{
	case ARRAY:
		if (tp->tp_size == (arith) -1)
			unknownsize = 1;
	case STRUCT:
	case UNION:
		if (e != (struct expr *) 0) {
			break;		/* switch */
		} else if (!tmpoffset) {/* first time for this variable */
			tmpoffset = df->df_address;
			df->df_address = data_label();
			C_df_dlb((label)df->df_address);
		} else {
			C_lae_dlb((label)df->df_address, (arith)0);
			load_block(tp->tp_size, 1);
			if (unknownsize) {
				/* tmpoffset += tp->tp_size; */
				unknownsize = 0;

				tmpoffset = NewLocal(tp->tp_size
						    , tp->tp_align
						    , regtype(tp)
						    , df->df_sc);
			}
			C_lal(tmpoffset);
			store_block(tp->tp_size, 1);
			df->df_address = tmpoffset;
			tmpoffset = 0;
		}
#ifdef DBSYMTAB
		if (options['g']) {
			stb_string(df, AUTO, id->id_text);

		}
#endif /* DBSYMTAB */
		return;
	}
	if (ISCOMMA(e))	{	/* embraced: int i = {12};	*/
		while (e)	{
			loc_init(e->OP_LEFT, id);
			e = e->OP_RIGHT;
		}
	}
	else	{	/* not embraced	*/
		ch3cast(&expr, '=', tp);	/* may modify expr */
#ifndef	LINT
		{
			struct value vl;

			EVAL(expr, RVAL, TRUE, NO_LABEL, NO_LABEL);
			vl.vl_class = Name;
			vl.vl_data.vl_idf = id;
			vl.vl_value = (arith)0;
			store_val(&vl, tp);
		}
#else	LINT
		id->id_def->df_set = 1;
#endif	LINT
		free_expression(expr);
	}
}

bss(idf)
	register struct idf *idf;
{
	/*	bss() allocates bss space for the global idf.
	*/
	register struct def *df = idf->id_def;
	
#ifndef	PREPEND_SCOPES
	code_scope(idf->id_text, df);
#endif	PREPEND_SCOPES
#ifdef DBSYMTAB
	if (options['g']) {
		stb_string(df, df->df_sc, idf->id_text);
	}
#endif /* DBSYMTAB */
	if (df->df_type->tp_size <= 0) {
		error("size of %s unknown (\"%s\", line %d)"
			, idf->id_text, df->df_file, df->df_line);
	} else {
		C_df_dnam(idf->id_text);
		C_bss_cst(ATW(df->df_type->tp_size), (arith)0, 1);
	}
}

formal_cvt(hasproto,df)
	int hasproto;
	register struct def *df;
{
	/*	formal_cvt() converts a formal parameter of type char or
		short from int to that type. It also converts a formal
		parameter of type float from a double to a float.
	*/
	register struct type *tp = df->df_type;

	if (tp->tp_size != int_size &&
		(tp->tp_fund == CHAR || tp->tp_fund == SHORT)
	) {
		LoadLocal(df->df_address, int_size);
		/* conversion(int_type, df->df_type); ???
		   No, you can't do this on the stack! (CJ)
		*/
		StoreLocal(df->df_address, tp->tp_size);
	} else if (tp->tp_size != double_size
		    && tp->tp_fund == FLOAT
		    && !hasproto) {
		LoadLocal(df->df_address, double_size);
#ifndef	LINT
		conversion(double_type, float_type);
#endif	LINT
		StoreLocal(df->df_address, tp->tp_size);
	}
}

#ifdef	LINT
/*ARGSUSED*/
#endif	LINT
code_expr(expr, val, code, tlbl, flbl)
	struct expr *expr;
	label tlbl, flbl;
{
	/*	code_expr() is the parser's interface to the expression code
		generator.  If line number trace is wanted, it generates a
		lin instruction.  EVAL() is called directly.
	*/
#ifndef	LINT
	if (! options['L'])	/* profiling	*/
		C_lin((arith)(expr->ex_line));
#ifdef DBSYMTAB
	if (options['g']) db_line(expr->ex_file, (unsigned int)expr->ex_line);
#endif
	EVAL(expr, val, code, tlbl, flbl);
#else	LINT
	lint_expr(expr, code ? USED : IGNORED);
#endif	LINT
}

/*	The FOR/WHILE/DO/SWITCH stacking mechanism:
	stack_stmt() has to be called at the entrance of a
	for, while, do or switch statement to indicate the
	EM labels where a subsequent break or continue causes
	the program to jump to.
*/
static struct stmt_block *stmt_stack;	/* top of statement stack */

/*	code_break() generates EM code needed at the occurrence of "break":
	it generates a branch instruction to the break label of the
	innermost statement in which break has a meaning.
	As "break" is legal in any of 'while', 'do', 'for' or 'switch',
	which are the only ones that are stacked, only the top of
	the stack is interesting.
*/
code_break()
{
	register struct stmt_block *stmt_block = stmt_stack;

#ifdef DBSYMTAB
	if (options['g']) db_line(dot.tk_file, dot.tk_line);
#endif /* DBSYMTAB */
	if (stmt_block)
		C_bra(stmt_block->st_break);
	else
		error("break not inside for, while, do or switch");
}

/*	code_continue() generates EM code needed at the occurrence of
	"continue":
	it generates a branch instruction to the continue label of the
	innermost statement in which continue has a meaning.
*/
code_continue()
{
	register struct stmt_block *stmt_block = stmt_stack;

	while (stmt_block)	{
		if (stmt_block->st_continue)	{
#ifdef DBSYMTAB
			if (options['g']) db_line(dot.tk_file, dot.tk_line);
#endif /* DBSYMTAB */
			C_bra(stmt_block->st_continue);
			return;
		}
		stmt_block = stmt_block->next;
	}
	error("continue not inside for, while or do");
}

stack_stmt(break_label, cont_label)
	label break_label, cont_label;
{
	register struct stmt_block *stmt_block = new_stmt_block();

	stmt_block->next = stmt_stack;
	stmt_block->st_break = break_label;
	stmt_block->st_continue = cont_label;
	stmt_stack = stmt_block;
}

unstack_stmt()
{
	/*	unstack_stmt() unstacks the data of a statement
		which may contain break or continue
	*/
	register struct stmt_block *sbp = stmt_stack;
	stmt_stack = sbp->next;
	free_stmt_block(sbp);
}

static label prc_name;

prc_entry(name)
	char *name;
{
	if (options['p']) {
		C_df_dlb(prc_name = data_label());
		C_rom_scon(name, (arith) (strlen(name) + 1));
		C_lae_dlb(prc_name, (arith) 0);
		C_cal("procentry");
		C_asp(pointer_size);
	}
}

prc_exit()
{
	if (options['p']) {
		C_lae_dlb(prc_name, (arith) 0);
		C_cal("procexit");
		C_asp(pointer_size);
	}
}

#ifdef DBSYMTAB
db_line(file, line)
	char		*file;
	unsigned int	line;
{
	static unsigned oldline;
	static char	*oldfile;

	if (file != oldfile || line != oldline) {
		C_ms_std((char *) 0, N_SLINE, (int) line);
		oldline = line;
		oldfile = file;
	}
}
#endif /* DBSYMTAB */
