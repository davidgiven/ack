/* $Header$ */
/*	C O D E - G E N E R A T I N G   R O U T I N E S		*/

#include	"dataflow.h"
#include	"use_tmp.h"
#include	"botch_free.h"

#include	"arith.h"
#include	"type.h"
#include	"idf.h"
#include	"label.h"
#include	"code.h"
#include	"alloc.h"
#include	"def.h"
#include	"expr.h"
#include	"sizes.h"
#include	"stack.h"
#include	"em.h"
#include	"level.h"
#include	"decspecs.h"
#include	"declar.h"
#include	"Lpars.h"
#include	"mes.h"
#include	"LLlex.h"
#include	"specials.h"
#include	"storage.h"
#include	"atw.h"
#include	"assert.h"

static struct stat_block *stat_sp, *stat_head;

char *symbol2str();
int fp_used;
label lab_count = 1;
label datlab_count = 1;

extern char options[];

/*	init_code() initialises the output file on which the compact
	EM code is written
*/
init_code(dst_file)
	char *dst_file;
{
	if (C_open(dst_file) == 0)
		fatal("cannot write to %s\n", dst_file);
#ifndef	USE_TMP
	famous_first_words();
#endif	USE_TMP
	stat_sp = stat_head = new_stat_block();
	clear((char *)stat_sp, sizeof(struct stat_block));
}

famous_first_words()
{
	C_magic();
	C_ms_emx(word_size, pointer_size);
}

end_code()
{
	/*	end_code() performs the actions to be taken when closing
		the output stream.
	*/
	C_ms_src((arith)(LineNumber - 2), FileName);
	C_close();
}

#ifdef	USE_TMP
prepend_scopes(dst_file)
	char *dst_file;
{
	/*	prepend_scopes() runs down the list of global idf's
		and generates those exa's, exp's, ina's and inp's
		that superior hindsight has provided, on the file dst_file.
	*/
	struct stack_entry *se = local_level->sl_entry;

	if (C_open(dst_file) == 0)
		fatal("cannot create file %s", dst_file);
	famous_first_words();
	while (se != 0)	{
		struct idf *idf = se->se_idf;
		struct def *def = idf->id_def;
		
		if (def &&
			(	def->df_initialized ||
				def->df_used ||
				def->df_alloc
			)
		)
			code_scope(idf->id_text, def);
		se = se->next;
	}
	C_close();
}
#endif	USE_TMP

code_scope(text, def)
	char *text;
	struct def *def;
{
	/*	generates code for one name, text, of the storage class
		as given by def, if meaningful.
	*/
	int fund = def->df_type->tp_fund;
	
	switch (def->df_sc)	{
	case EXTERN:
	case GLOBAL:
	case IMPLICIT:
		if (fund == FUNCTION)
			C_exp(text);
		else
			C_exa(text);
		break;
	case STATIC:
		if (fund == FUNCTION)
			C_inp(text);
		else
			C_ina(text);
		break;
	}
}

static label return_label;
static char return_expr_occurred;
static struct type *func_tp;
static label func_res_label;
static char *last_fn_given = "";
static label file_name_label;

/*	begin_proc() is called at the entrance of a new function
	and performs the necessary code generation:
	-	a scope indicator (if needed) exp/inp
	-	the procedure entry pro $name
	-	reserves some space if the result of the function
		does not fit in the return area
	-	a fil pseudo instruction
*/
begin_proc(name, def)	/* to be called when entering a procedure	*/
	char *name;
	struct def *def;
{
	arith size;

#ifndef	USE_TMP
	code_scope(name, def);
#endif	USE_TMP
#ifdef	DATAFLOW
	if (options['d'])
		DfaStartFunction(name);
#endif	DATAFLOW

	func_tp = def->df_type->tp_up;
	size = ATW(func_tp->tp_size);
	C_pro_narg(name);
	if (is_struct_or_union(func_tp->tp_fund))	{
		C_df_dlb(func_res_label = data_label());
		C_bss_cst(size, (arith)0, 1);
	}
	else
		func_res_label = 0;

	/*	Special arrangements if the function result doesn't fit in
		the function return area of the EM machine.  The size of
		the function return area is implementation dependent.
	*/
	lab_count = (label) 1;
	return_label = text_label();
	return_expr_occurred = 0;

	if (options['p'])	{	/* profiling */
		if (strcmp(last_fn_given, FileName) != 0)	{
			/* previous function came from other file */
			C_df_dlb(file_name_label = data_label());
			C_con_begin();
			C_scon(last_fn_given = FileName, (arith)0);
			C_con_end();
		}
		/* enable debug trace of EM source */
		C_fil_dlb(file_name_label, (arith)0);
		C_lin((arith)LineNumber);
	}
}

/*	end_proc() deals with the code to be generated at the end of
	a function, as there is:
	-	the EM ret instruction: "ret 0"
	-	loading of the function result in the function result area
		if there has been a return <expr> in the function body
		(see do_return_expr())
	-	indication of the use of floating points
	-	indication of the number of bytes used for formal parameters
	-	use of special identifiers such as "setjmp"
	-	"end" + number of bytes used for local variables
*/
end_proc(fbytes, nbytes)
	arith fbytes, nbytes;
{
	static int mes_flt_given = 0;	/* once for the whole program */

#ifdef	DATAFLOW
	if (options['d'])
		DfaEndFunction();
#endif	DATAFLOW
	C_ret((arith)0);
	if (return_expr_occurred != 0)	{
		C_df_ilb(return_label);
		if (func_res_label != 0)	{
			C_lae_dlb(func_res_label, (arith)0);
			store_block(func_tp->tp_size, func_tp->tp_align);
			C_lae_dlb(func_res_label, (arith)0);
			C_ret(pointer_size);
		}
		else
			C_ret(ATW(func_tp->tp_size));
	}
	if (fp_used && mes_flt_given == 0)	{
		/* floating point used	*/
		C_ms_flt();
		mes_flt_given++;
	}
	C_ms_par(fbytes);		/* # bytes for formals		*/
	if (sp_occurred[SP_SETJMP]) {	/* indicate use of "setjmp"	*/
		C_ms_gto();
		sp_occurred[SP_SETJMP] = 0;
	}
	C_end(ATW(nbytes));
}

do_return_expr(expr)
	struct expr *expr;
{
	/*	do_return_expr() generates the expression and the jump for
		a return statement with an expression.
	*/
	ch7cast(&expr, RETURN, func_tp);
	code_expr(expr, RVAL, TRUE, NO_LABEL, NO_LABEL);
	C_bra(return_label);
	return_expr_occurred = 1;
}

code_declaration(idf, expr, lvl, sc)
	struct idf *idf;	/* idf to be declared	*/
	struct expr *expr;	/* initialisation; NULL if absent	*/
	int lvl;		/* declaration level	*/
	int sc;			/* storage class, as in the declaration */
{
	/*	code_declaration() does the actual declaration of the
		variable indicated by "idf" on declaration level "lvl".
		If the variable is initialised, the expression is given
		in "expr".
		There are some cases to be considered:
		-	filter out typedefs, they don't correspond to code;
		-	global variables, coded only if initialized;
		-	local static variables;
		-	local automatic variables;
		If there is a storage class indication (EXTERN/STATIC),
		code_declaration() will generate an exa or ina.
		The sc is the actual storage class, as given in the
		declaration.  This is to allow:
			extern int a;
			int a = 5;
		while at the same time forbidding
			extern int a = 5;
	*/
	char *text = idf->id_text;
	struct def *def = idf->id_def;
	arith size = def->df_type->tp_size;
	int def_sc = def->df_sc;
	
	if (def_sc == TYPEDEF)	/* no code for typedefs		*/
		return;
	if (sc == EXTERN && expr && !is_anon_idf(idf))
		error("%s is extern; cannot initialize", text);
	if (lvl == L_GLOBAL)	{	/* global variable	*/
		/* is this an allocating declaration? */
		if (	(sc == 0 || sc == STATIC)
			&& def->df_type->tp_fund != FUNCTION
			&& size >= 0
		)
			def->df_alloc = ALLOC_SEEN;
		if (expr) {	/* code only if initialized */
#ifndef	USE_TMP
			code_scope(text, def);
#endif	USE_TMP
			def->df_alloc = ALLOC_DONE;
			C_df_dnam(text);
			do_ival(&(def->df_type), expr);
		}
	}
	else
	if (lvl >= L_LOCAL)	{	/* local variable	*/
		/* they are STATIC, EXTERN, GLOBAL, IMPLICIT, AUTO or
		   REGISTER
		*/
		switch (def_sc)	{
		case STATIC:
			/*	they are handled on the spot and get an
				integer label in EM.
			*/
			C_df_dlb((label)def->df_address);
			if (expr) /* there is an initialisation	*/
				do_ival(&(def->df_type), expr);
			else {	/* produce blank space */
				if (size <= 0) {
					error("size of \"%s\" unknown", text);
					size = (arith)0;
				}
				C_bss_cst(align(size, word_align), (arith)0, 1);
			}
			break;
		case EXTERN:
		case GLOBAL:
		case IMPLICIT:
			/* we are sure there is no expression */
#ifndef	USE_TMP
			code_scope(text, def);
#endif	USE_TMP
			break;
		case AUTO:
		case REGISTER:
			if (expr)
				loc_init(expr, idf);
			break;
		default:
			crash("bad local storage class");
			break;
		}
	}
}

loc_init(expr, id)
	struct expr *expr;
	struct idf *id;
{
	/*	loc_init() generates code for the assignment of
		expression expr to the local variable described by id.
	*/
	register struct type *tp = id->id_def->df_type;
	
	/* automatic aggregates cannot be initialised. */
	switch (tp->tp_fund)	{
	case ARRAY:
	case STRUCT:
	case UNION:
		error("no automatic aggregate initialisation");
		return;
	}
	
	if (ISCOMMA(expr))	{	/* embraced: int i = {12};	*/
		if (options['R'])	{
			if (ISCOMMA(expr->OP_LEFT)) /* int i = {{1}} */
				expr_error(expr, "extra braces not allowed");
			else
			if (expr->OP_RIGHT != 0) /* int i = {1 , 2} */
				expr_error(expr, "too many initializers");
		}
		while (expr)	{
			loc_init(expr->OP_LEFT, id);
			expr = expr->OP_RIGHT;
		}
	}
	else	{	/* not embraced	*/
		ch7cast(&expr, '=', tp);
		EVAL(expr, RVAL, TRUE, NO_LABEL, NO_LABEL);
		store_val(id, tp, (arith) 0);
	}
}

/*	bss() allocates bss space for the global idf.
*/
bss(idf)
	struct idf *idf;
{
	register struct def *def = idf->id_def;
	arith size = def->df_type->tp_size;
	
#ifndef	USE_TMP
	code_scope(idf->id_text, def);
#endif	USE_TMP
	/*	Since bss() is only called if df_alloc is non-zero, and
		since df_alloc is only non-zero if size >= 0, we have:
	*/
	if (options['R'] && size == 0)
		warning("actual array of size 0");
	C_df_dnam(idf->id_text);
	C_bss_cst(align(size, word_align), (arith)0, 1);
}

formal_cvt(def)
	struct def *def;
{
	/*	formal_cvt() converts a formal parameter of type char or
		short from int to that type.
	*/
	register struct type* tp = def->df_type;

	if (tp->tp_size != int_size)
		if (tp->tp_fund == CHAR || tp->tp_fund == SHORT) {
			C_lol(def->df_address);
			conversion(int_type, def->df_type);
			C_lal(def->df_address);
			C_sti(tp->tp_size);
			def->df_register = REG_NONE;
		}
}

/*	code_expr() is the parser's interface to the expression code
	generator.
	If line number trace is wanted, it generates a lin instruction.
	EVAL() is called directly.
*/
code_expr(expr, val, code, tlbl, flbl)
	struct expr *expr;
	label tlbl, flbl;
{
	if (options['p'])	/* profiling	*/
		C_lin((arith)LineNumber);
	EVAL(expr, val, code, tlbl, flbl);
}

/*	The FOR/WHILE/DO/SWITCH stacking mechanism:
	stat_stack() has to be called at the entrance of a
	for, while, do or switch statement to indicate the
	EM labels where a subsequent break or continue causes
	the program to jump to.
*/
/*	do_break() generates EM code needed at the occurrence of "break":
	it generates a branch instruction to the break label of the
	innermost statement in which break has a meaning.
	As "break" is legal in any of 'while', 'do', 'for' or 'switch',
	which are the only ones that are stacked, only the top of
	the stack is interesting.
	0 is returned if the break cannot be bound to any enclosing
	statement.
*/
int
do_break()
{
	register struct stat_block *stat_ptr = stat_sp;

	if (stat_ptr)	{
		C_bra(stat_ptr->st_break);
		return 1;
	}
	return 0;	/* break is illegal	*/
}

/*	do_continue() generates EM code needed at the occurrence of "continue":
	it generates a branch instruction to the continue label of the
	innermost statement in which continue has a meaning.
	0 is returned if the continue cannot be bound to any enclosing
	statement.
*/
int
do_continue()
{
	register struct stat_block *stat_ptr = stat_sp;

	while (stat_ptr)	{
		if (stat_ptr->st_continue)	{
			C_bra(stat_ptr->st_continue);
			return 1;
		}
		stat_ptr = stat_ptr->next;
	}
	return 0;
}

stat_stack(break_label, cont_label)
	label break_label, cont_label;
{
	register struct stat_block *newb = new_stat_block();

	newb->next = stat_sp;
	newb->st_break = break_label;
	newb->st_continue = cont_label;
	stat_sp = newb;
}

/*	stat_unstack() unstacks the data of a statement
	which may contain break or continue
*/
stat_unstack()
{
	register struct stat_block *sbp = stat_sp;
	stat_sp = stat_sp->next;
	free_stat_block(sbp);
}
