/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	Lint status checking	*/

#include	"parameters.h"

#ifdef	LINT

#include	<assert.h>
#include	<alloc.h>	/* for st_free */
#include	"interface.h"
#ifdef ANSI
#include	<flt_arith.h>
#endif /* ANSI */
#include	"arith.h"
#include	"label.h"
#include	"expr.h"
#include	"def.h"
#include	"code.h"	/* RVAL etc */
#include	"LLlex.h"
#include	"Lpars.h"
#include	"stack.h"
#include	"type.h"
#include	"level.h"
#include	"l_lint.h"
#include	"l_brace.h"
#include	"l_state.h"
#include	"l_comment.h"
#include	"l_outdef.h"

#ifdef	DEBUG
#define	dbg_lint_stack(m)	/*print_lint_stack(m)	/* or not */
#else
#define	dbg_lint_stack(m)
#endif	/* DEBUG */

extern char *symbol2str();
extern char *func_name;
extern struct type *func_type;
extern int func_notypegiven;
extern char loptions[];
extern struct stack_level *local_level;

/* global variables for the lint_stack */
PRIVATE struct lint_stack_entry *top_ls;

/* global variables for the brace stack */
PRIVATE int brace_count;
PRIVATE struct brace *top_br;

/* global variables for the function return */
PRIVATE int valreturned;		/* see l_lint.h */
PRIVATE int return_warned;

PRIVATE end_brace();
PRIVATE lint_1_local();
PRIVATE lint_1_global();
PRIVATE start_loop_stmt();
PRIVATE check_autos();
PRIVATE struct auto_def *copy_auto_list();
PRIVATE free_auto_list();
PRIVATE struct state *copy_state();
PRIVATE Free_state();
PRIVATE remove_settings();
PRIVATE struct auto_def *merge_autos();
PRIVATE merge_states();
PRIVATE struct lint_stack_entry *find_wdf(), *find_wdfc(), *find_cs();
PRIVATE cont_merge();
PRIVATE break_merge();
PRIVATE struct lint_stack_entry *mk_lint_stack_entry();
PRIVATE lint_push();
PRIVATE lint_pop();

lint_init_stack()
{
/*	Allocate memory for the global lint-stack elements.
*/
	top_ls = new_lint_stack_entry();
	top_ls->ls_current = new_state();
}

lint_start_local()
{
	register struct brace *br = new_brace();

	dbg_lint_stack("lint_start_local");
	brace_count++;
	br->br_count = brace_count;
	br->br_level = level;
	br->next = top_br;
	top_br = br;
}	

lint_end_local(stl)
	struct stack_level *stl;
{

	dbg_lint_stack("lint_end_local");
	if (s_NOTREACHED) {
		top_ls->ls_current->st_notreached = 1;
		top_ls->ls_current->st_warned = 0;
		s_NOTREACHED = 0;
	}

	if (top_ls->ls_class == CASE && level == top_ls->ls_level) {
		/* supply missing  break;  at end of switch */
		lint_break_stmt();
	}

	check_autos();
	end_brace(stl);
}

PRIVATE
end_brace(stl)
	struct stack_level *stl;
{
	/*	Check if static variables and labels are used and/or set.
		Automatic vars have already been checked by check_autos().
	*/
	register struct stack_entry *se = stl->sl_entry;
	register struct brace *br;

	while (se) {
		register struct idf *idf = se->se_idf;

		if (idf->id_def) {
			lint_1_local(idf, idf->id_def);
		}
		if (stl->sl_level == L_LOCAL && idf->id_label) {
			lint_1_local(idf, idf->id_label);
		}
		se = se->next;
	}

	/* remove entry from brace stack */
	br = top_br;
	top_br = br->next;
	free_brace(br);
}

PRIVATE
lint_1_local(idf, def)
	struct idf *idf;
	struct def *def;
{
	register int sc = def->df_sc;

	if (	(sc == STATIC || sc == LABEL)
	&&	!def->df_used
	&&	!is_anon_idf(idf)
	) {
		def_warning(def, "%s %s not applied anywhere in function %s",
			symbol2str(sc), idf->id_text, func_name);
	}

	if (	loptions['h']
	&&	sc == AUTO
	&&	!def->df_initialized
	&&	def->df_firstbrace != 0
	&&	def->df_minlevel != level
	&&	!is_anon_idf(idf)
	) {
		register int diff = def->df_minlevel - level;

		def_warning(def,
			"local %s could be declared %d level%s deeper",
			idf->id_text, diff, (diff == 1 ? "" : "s")
		);
	}
}

lint_end_global(stl)
	struct stack_level *stl;
{
	register struct stack_entry *se = stl->sl_entry;

	dbg_lint_stack("lint_end_global");
	assert(level == L_GLOBAL);
	while (se) {
		register struct idf *idf = se->se_idf;
		register struct def *def = idf->id_def;

		if (def) {
			lint_1_global(idf, def);
		}
		se = se->next;
	}
}

PRIVATE
lint_1_global(idf, def)
	struct idf *idf;
	struct def *def;
{
	register int sc = def->df_sc;
	register struct type *tp = def->df_type;
	register int fund = tp->tp_fund;

	switch (sc) {
	case STATIC:
	case EXTERN:
	case GLOBAL:
#ifdef	IMPLICIT
	case IMPLICIT:
#endif	/* IMPLICIT */
		if (fund == ERRONEOUS)
			break;

		if (fund == FUNCTION && sc != STATIC) {
			output_proto(idf, def);
		}

		if (def->df_set || def->df_used) {
			/* Output a line to the intermediate file for
			 * used external variables (including functions)
			 */
			output_use(idf);
		}

		if (sc == STATIC && !def->df_used) {
			if (def->df_set) {
				if (!is_anon_idf(idf) && fund != ERRONEOUS) {
					def_warning(def,
						"%s %s %s set but not used",
						symbol2str(sc),
						symbol2str(fund),
						idf->id_text);
				}
			}
			else {
				if (!is_anon_idf(idf) && fund != ERRONEOUS) {
					def_warning(def,
						"%s %s %s not used anywhere",
						symbol2str(sc),
						symbol2str(fund),
						idf->id_text);
				}
			}
		}
		if (loptions['x']) {
			register char *fn = def->df_file;

			if (	(sc == EXTERN || sc == GLOBAL)
			&&	def->df_alloc == 0
			&&	!def->df_set
			&&	!def->df_initialized
			&&	!def->df_used
			&&	strcmp(&fn[strlen(fn)-2], ".c") == 0
			&&	!is_anon_idf(idf)
			&&	fund != ERRONEOUS
			) {
				def_warning(def,
					"%s %s %s not used anywhere",
					symbol2str(sc),
					symbol2str(fund),
					idf->id_text);
			}
		}
		break;
	}
}

change_state(idf, to_state)
	struct idf *idf;
	int to_state;			/* SET or USED */
{
/* Changes the state of the variable identified by idf in the current state
 * on top of the stack.
 * The fields in the def-descriptor are set too.
 */
	register struct def *def = idf->id_def;
	register struct auto_def *a = top_ls->ls_current->st_auto_list;

	assert(def);

	switch (to_state) {
	case SET:
		def->df_set = 1;
		break;
	case USED:
		def->df_used = 1;
		break;
	}

	/* adjust minimum required brace level */
	if (def->df_firstbrace == 0) {
		def->df_firstbrace = brace_count;
		def->df_minlevel = level;
	}
	else {
		register struct brace *br = top_br;

		/*	find the smallest brace range from which
			firstbrace is visible
		*/
		while (br && br->br_count > def->df_firstbrace) {
			br = br->next;
		}
		assert(br && def->df_minlevel >= br->br_level);
		def->df_minlevel = br->br_level;
	}

	/* search auto_list */
	while(a && a->ad_idf != idf)
		a = a->next;
	if (a == 0)	/* identifier not in list, global definition */
		return;

	switch (to_state) {
	case SET:
		a->ad_maybe_set = 0;
		a->ad_set = 1;
		break;
	case USED:
		if (!a->ad_set) {
			if (!is_anon_idf(idf)) {
				warning("variable %s%s uninitialized",
					idf->id_text,
					(a->ad_maybe_set ? " possibly" : "")
				);
			}
			a->ad_maybe_set = 0;
			a->ad_set = 1;	/* one warning */
		}
		a->ad_used = 1;
		break;
	}
}

add_auto(idf)	/* to current state on top of lint_stack */
	struct idf *idf;
{
/* Check if idf's definition is really an auto (or register).
 * It could be a static or extern too.
 * Watch out for formal parameters.
 */
	register struct def *def = idf->id_def;

	assert(def);

	switch (def->df_sc) {
		register struct auto_def *a;
	case AUTO:
	case REGISTER:
		if (def->df_level < L_LOCAL)
			return;		/* a formal */

		a = new_auto_def();

		a->ad_idf = idf;
		a->ad_def = def;
		a->ad_used = def->df_used;
		a->ad_set = def->df_set;

		a->next = top_ls->ls_current->st_auto_list;
		top_ls->ls_current->st_auto_list = a;
	}
}

PRIVATE
check_autos()
{
/* Before leaving a block, remove the auto_defs of the automatic
 * variables on this level and check if they are used
 */
	register struct auto_def *a = top_ls->ls_current->st_auto_list;

	assert(!(a && a->ad_def->df_level > level));
	while (a && a->ad_def->df_level == level) {
		struct idf *idf = a->ad_idf;
		struct def *def = idf->id_def;

		if (!def->df_used && !is_anon_idf(idf)) {
			if (def->df_set || a->ad_maybe_set) {
				def_warning(def,
					"%s set but not used in function %s",
					idf->id_text, func_name);
			}
			else {
				def_warning(def,
					"%s not used anywhere in function %s",
					idf->id_text, func_name);
			}
		}

		{	/* free a */
			register struct auto_def *aux = a;
			a = a->next;
			free_auto_def(aux);
		}
	}
	top_ls->ls_current->st_auto_list = a;
}

lint_end_formals()
{
	register struct stack_entry *se = local_level->sl_entry;

	dbg_lint_stack("lint_end_formals");
	assert(level == L_FORMAL1);
	while (se) {
		register struct def *def = se->se_idf->id_def;

		if (	(def && !def->df_used)
		&&	!(f_ARGSUSED || LINTLIB)
		&&	!is_anon_idf(se->se_idf)
		) {
			def_warning(def, "argument %s not used in function %s",
					se->se_idf->id_text, func_name);
		}
		se = se->next;
	}
}

PRIVATE struct auto_def *
copy_auto_list(from_al, lvl)
	struct auto_def *from_al;
	int lvl;
{
	struct auto_def *start = 0;
	register struct auto_def **hook = &start;

	/* skip too high levels */
	while (from_al && from_al->ad_def->df_level > lvl) {
		from_al = from_al->next;
	}

	while (from_al) {
		register struct auto_def *a = new_auto_def();

		*hook = a;
		*a = *from_al;
		hook = &a->next;
		from_al = from_al->next;
	}

	return start;
}

PRIVATE
free_auto_list(a)
	register struct auto_def *a;
{
	while (a) {
		register struct auto_def *aux = a;
		a = a->next;
		free_auto_def(aux);
	}
}

PRIVATE struct state *
copy_state(from_st, lvl)
	struct state *from_st;
	int lvl;
{
/* Memory for the struct state and the struct auto_defs is allocated
 * by this function
 */
	register struct state *st = new_state();

	st->st_auto_list = copy_auto_list(from_st->st_auto_list, lvl);
	st->st_notreached = from_st->st_notreached;
	st->st_warned = from_st->st_warned;
	return st;
}

PRIVATE
Free_state(stp)
	struct state **stp;
{
/* This function also frees the list of auto_defs
 */
	free_auto_list((*stp)->st_auto_list);
	free_state(*stp);
	*stp = 0;
}

PRIVATE
remove_settings(st, lvl)
	struct state *st;
	int lvl;
{
/* The states of all variables on this level are set to 'not set' and
 * 'not maybe set'. (I think you have to read this twice.)
 */
	register struct auto_def *a = st->st_auto_list;

	while (a && a->ad_def->df_level == lvl) {
		a->ad_set = a->ad_maybe_set = 0;
		a = a->next;
	}
}


/******** M E R G E ********/

/* modes for merging */
#define	NORMAL		0
#define	CASE_BREAK	1
#define	USE_ONLY	2

PRIVATE
merge_states(st1, st2, lvl, mode)
	struct state *st1, *st2;
	int lvl;
	int mode;			/* NORMAL or CASE_BREAK */
{
/* st2 becomes the result.
 * st1 is left unchanged.
 * The resulting state is the state the program gets in if st1 OR st2
 * becomes the state. (E.g. the states at the end of an if-part and an
 * end-part are merged by this function.)
 */
	if (st1->st_notreached) {
		if (mode == NORMAL || st2->st_notreached) {
			st2->st_auto_list =
				merge_autos(st1->st_auto_list,
					st2->st_auto_list, lvl, USE_ONLY);
		}
	}
	else
	if (st2->st_notreached) {
		register struct auto_def *tmp = st2->st_auto_list;

		st2->st_auto_list = copy_auto_list(st1->st_auto_list, lvl);
		st2->st_notreached = 0;
		st2->st_warned = 0;
		st2->st_auto_list = merge_autos(tmp, st2->st_auto_list,
							lvl, USE_ONLY);
		free_auto_list(tmp);
	}
	else {
		/* both st1 and st2 reached */
		st2->st_auto_list =
			merge_autos(st1->st_auto_list, st2->st_auto_list,
				lvl, mode);
	}
}

PRIVATE struct auto_def *
merge_autos(a1, a2, lvl, mode)
	struct auto_def *a1, *a2;
	int lvl;
	int mode;
{
/* Returns a pointer to the result.
 * a1 is left unchanged.
 * a2 is used to create this result.
 * The fields are set as follows:
 *	a1_set + a2_set		-> set
 *		+ a?_maybe_set	-> maybe set
 *		ELSE		-> NOT set && NOT maybe set
 *	*	+ a?_used	-> used
 *
 * For mode == CASE_BREAK:
 * First a2 is taken as the result, then
 * variables NOT set in a2 and set or maybe set in a1 become 'maybe set'
 *
 * For mode == USE_ONLY:
 * Start with a2 as the result.
 * Variables used in a1 become used in a2.
 * The rest of the result is not changed.
 */
	register struct auto_def *a;

	/* skip too local entries */
	while (a1 && a1->ad_def->df_level > lvl) {
		a1 = a1->next;
	}

	/* discard too local entries */
	while (a2 && a2->ad_def->df_level > lvl) {
		register struct auto_def *aux = a2;
		a2 = a2->next;
		free_auto_def(aux);
	}

	a = a2;	/* pointer to the result */
	while (a1) {
		assert(a2);

		/* merge the auto_defs for one idf */
		assert(a1->ad_idf == a2->ad_idf);
		if (a1->ad_used)
			a2->ad_used = 1;

		if (mode != USE_ONLY) {
			if (	(	!a2->ad_set
				&&	(a1->ad_set || a1->ad_maybe_set)
				)
			||	(	mode == NORMAL
				&&	!a1->ad_set
				&&	(a2->ad_set || a2->ad_maybe_set)
				)
			) {
				a2->ad_set = 0;
				a2->ad_maybe_set = 1;
			}
		}

		a1 = a1->next;
		a2 = a2->next;
	}
	assert(!a2);
	return a;
}


/******** L I N T   S T A C K   S E A R C H I N G ********/

/* The next four find-functions search the lint_stack for an entry.
 * The letters mean : w: WHILE; d: DO; f: FOR; s: SWITCH; c: CASE.
 */

PRIVATE struct lint_stack_entry *
find_wdf()
{
	register struct lint_stack_entry *lse = top_ls;

	while (lse) {
		switch (lse->ls_class) {
		case WHILE:
		case DO:
		case FOR:
			return lse;
		}
		lse = lse->ls_previous;
	}
	return 0;
}

PRIVATE struct lint_stack_entry *
find_wdfc()
{
	register struct lint_stack_entry *lse = top_ls;

	while (lse) {
		switch (lse->ls_class) {
		case WHILE:
		case DO:
		case FOR:
		case CASE:
			return lse;
		}
		lse = lse->ls_previous;
	}
	return 0;
}

PRIVATE struct lint_stack_entry *
find_cs()
{
	register struct lint_stack_entry *lse = top_ls;

	while (lse) {
		switch (lse->ls_class) {
		case CASE:
		case SWITCH:
			return lse;
		}
		lse = lse->ls_previous;
	}
	return 0;
}

/******** A C T I O N S : I F ********/

start_if_part(cst)
{
	register struct lint_stack_entry *new = mk_lint_stack_entry(IF);

	dbg_lint_stack("start_if_part");
	if (cst)
		hwarning("condition in if statement is constant");

	lint_push(new);
/*	ls_current:	the state at the start of the if-part
*/
}

start_else_part()
{
/*	ls_current:	the state at the end of the if-part
	ls_previous->ls_current:	the state before the if-part
*/

	dbg_lint_stack("start_else_part");
	if (s_NOTREACHED) {
		top_ls->ls_current->st_notreached = 1;
		top_ls->ls_current->st_warned = 0;
		s_NOTREACHED = 0;
	}
	top_ls->LS_IF = top_ls->ls_current;
	/* this is the reason why ls_current is a pointer */
	top_ls->ls_current =
		copy_state(top_ls->ls_previous->ls_current, level);
	top_ls->ls_level = level;
/*	ls_current:	the state before the if-part and the else-part
	LS_IF:		the state at the end of the if-part
*/
}

end_if_else_stmt()
{
/*	ls_current:	state at the end of the else-part
	LS_IF:		state at the end of the if-part
*/

	dbg_lint_stack("end_if_else_stmt");
	merge_states(top_ls->LS_IF, top_ls->ls_current,
					top_ls->ls_level, NORMAL);
	Free_state(&top_ls->LS_IF);
	Free_state(&top_ls->ls_previous->ls_current);
	top_ls->ls_previous->ls_current = top_ls->ls_current;
	lint_pop();
}

end_if_stmt()
{
/*	No else-part met.
	ls_current:	state at the end of the if-part
*/

	dbg_lint_stack("end_if_stmt");
	merge_states(top_ls->ls_current, top_ls->ls_previous->ls_current,
						top_ls->ls_level, NORMAL);
	Free_state(&top_ls->ls_current);
	lint_pop();
}

/******** A C T I O N S : L O O P S ********/

start_while_stmt(expr)
	struct expr *expr;
{
	if (is_cp_cst(expr))	{
		start_loop_stmt(WHILE, 1, expr->VL_VALUE != (arith)0);
	}
	else	{
		start_loop_stmt(WHILE, 0, 0);
	}
}

start_do_stmt()
{
	start_loop_stmt(DO, 1, 1);
}

start_for_stmt(expr)
	struct expr *expr;
{
	if (!expr)	{
		start_loop_stmt(FOR, 1, 1);
	}
	else
	if (is_cp_cst(expr))	{
		start_loop_stmt(FOR, 1, expr->VL_VALUE != (arith)0);
	}
	else	{
		start_loop_stmt(FOR, 0, 0);
	}
}

PRIVATE
start_loop_stmt(looptype, cst, cond)
{
/*	If cst, the condition is a constant and its value is cond
*/
	register struct lint_stack_entry *new = mk_lint_stack_entry(looptype);

	dbg_lint_stack("start_loop_stmt");
	if (cst && !cond) {
		/* while (0) | for (;0;) */
		hwarning("condition in %s statement is always false",
						symbol2str(looptype));
		new->ls_current->st_notreached = 1;
	}
	if (cst && cond) {
		/* while (1) | for (;;) | do */
		/*	omitting the copy for LS_LOOP will force this loop
			to be treated as a do loop
		*/
		top_ls->ls_current->st_notreached = 1;
		top_ls->ls_current->st_warned = 0;
	}
	else {
		new->LS_LOOP = copy_state(top_ls->ls_current, level);
	}
	new->LS_TEST = (!cst ? TEST_VAR : cond ? TEST_TRUE : TEST_FALSE);
	lint_push(new);

/*	ls_current:	the state at the start of the body
	LS_TEST:	info about the loop test
	LS_BODY:	0, the state at the end of the body
	LS_LOOP:	the state at the end of the loop, or 0 if the loop
			does not end
*/
}

end_loop_body()
{
	register struct lint_stack_entry *lse = find_wdf();

	dbg_lint_stack("end_loop_body");
	assert(lse == top_ls);
	if (!lse->ls_current->st_notreached)
		cont_merge(lse);
}

end_loop_stmt()
{
	register struct lint_stack_entry *lse = find_wdf();

	dbg_lint_stack("end_loop_stmt");
	assert(lse == top_ls);
	if (lse->LS_TEST != TEST_TRUE)
		break_merge(lse);

	dbg_lint_stack("end_loop_stmt after break_merge");
	if (!top_ls->LS_LOOP) {
		/* no break met; this is really an endless loop */
		hwarning("endless %s loop", symbol2str(top_ls->ls_class));
		Free_state(&top_ls->ls_current);
	}
	else {
		Free_state(&top_ls->ls_current);
		Free_state(&top_ls->ls_previous->ls_current);
		top_ls->ls_previous->ls_current = top_ls->LS_LOOP;
	}
	lint_pop();
}

end_do_stmt(cst, cond)
{
	register struct lint_stack_entry *lse = find_wdf();

	dbg_lint_stack("end_do_stmt");
	if (cst && !cond) {
		/* do ... while (0) */
		hwarning("condition in do statement is always false");
	}
	lse->LS_TEST = (!cst ? TEST_VAR : cond ? TEST_TRUE : TEST_FALSE);
	end_loop_stmt();

}

lint_continue_stmt()
{
	register struct lint_stack_entry *lse = find_wdf();

	dbg_lint_stack("lint_continue_stmt");
	if (!lse)
		return;		/* not inside a loop statement */

	cont_merge(lse);
	top_ls->ls_current->st_notreached = 1;
	top_ls->ls_current->st_warned = 0;
}

/******** A C T I O N S : S W I T C H ********/

start_switch_part(cst)
{
/* ls_current of a SWITCH entry has different meaning from ls_current of
 * other entries. It keeps track of which variables are used in all
 * following case parts. (Needed for variables declared in a compound
 * switch-block.)
 */
	register struct lint_stack_entry *new = mk_lint_stack_entry(SWITCH);

	dbg_lint_stack("start_switch_part");
	if (cst)
		hwarning("value in switch statement is constant");

	new->LS_CASE = copy_state(top_ls->ls_current, level);
	new->ls_current->st_notreached = 1;
	new->ls_current->st_warned = 0;
	top_ls->ls_current->st_notreached = 1;
	top_ls->ls_current->st_warned = 0;
	lint_push(new);
}

end_switch_stmt()
{

	dbg_lint_stack("end_switch_stmt");
	if (top_ls->ls_class == CASE) {
		/* no break after last case or default */
		lint_break_stmt();	/* introduce break */
	}

	if (!top_ls->LS_DEFAULT_MET) {
		top_ls->ls_current->st_notreached = 0;
		if (top_ls->LS_BREAK) {
			merge_states(top_ls->ls_current, top_ls->LS_BREAK,
						top_ls->ls_level, NORMAL);
			Free_state(&top_ls->ls_current);
		}
		else {
			top_ls->LS_BREAK = top_ls->ls_current;
		}
	}
	else {
		Free_state(&top_ls->ls_current);
	}

	if (top_ls->LS_BREAK) {
		merge_states(top_ls->LS_CASE, top_ls->LS_BREAK,
						top_ls->ls_level, CASE_BREAK);
		Free_state(&top_ls->LS_CASE);
	}
	else {
		top_ls->LS_BREAK = top_ls->LS_CASE;
	}

	top_ls->LS_BREAK->st_notreached =
			top_ls->ls_previous->ls_current->st_notreached;
				/* yack */
	Free_state(&top_ls->ls_previous->ls_current);

	if (!top_ls->LS_DEFAULT_MET)
		top_ls->LS_BREAK->st_notreached = 0;
	top_ls->ls_previous->ls_current = top_ls->LS_BREAK;

	lint_pop();
}

lint_case_stmt(dflt)
{
/* A default statement is just a special case statement */
	register struct lint_stack_entry *cs_entry = find_cs();

	dbg_lint_stack("lint_case_stmt");
	if (!cs_entry)
		return;		/* not inside switch */

	if (cs_entry != top_ls) {
		warning("%s statement in strange context",
			dflt ? "default" : "case");
		return;
	}

	switch (cs_entry->ls_class) {
		register struct lint_stack_entry *new;

	case SWITCH:
		if (dflt) {
			cs_entry->LS_DEFAULT_MET = 1;
		}

		new = mk_lint_stack_entry(CASE);
		remove_settings(new->ls_current, level);
		lint_push(new);
		break;

	case CASE:
		assert(top_ls->ls_previous->ls_class == SWITCH);
		if (dflt) {
			cs_entry->ls_previous->LS_DEFAULT_MET = 1;
		}
		merge_states(top_ls->ls_current, top_ls->ls_previous->LS_CASE,
				top_ls->ls_previous->ls_level, NORMAL);
		merge_states(top_ls->ls_current,
				top_ls->ls_previous->ls_current,
				top_ls->ls_previous->ls_level, NORMAL);
		Free_state(&top_ls->ls_current);
		top_ls->ls_current =
			copy_state(top_ls->ls_previous->ls_current,
					top_ls->ls_previous->ls_level);
		remove_settings(top_ls->ls_current, top_ls->ls_level);
		break;

	default:
		NOTREACHED();
		/*NOTREACHED*/
	}
}

lint_break_stmt()
{
	register struct lint_stack_entry *lse = find_wdfc();

	dbg_lint_stack("lint_break_stmt");
	if (!lse)
		return;

	switch (lse->ls_class) {
	case WHILE:
	case FOR:
	case DO:
		/* loop break */
		lse->ls_previous->ls_current->st_notreached = 0;
		break_merge(lse);
		break;

	case CASE:
		/* case break */
		if (!top_ls->ls_current->st_notreached) {
			lse->ls_previous->ls_previous->ls_current->st_notreached = 0;
		}
		merge_states(lse->ls_current, lse->ls_previous->ls_current,
					lse->ls_previous->ls_level, NORMAL);
		if (lse->ls_previous->LS_BREAK) {
			merge_states(top_ls->ls_current,
				lse->ls_previous->LS_BREAK,
				lse->ls_previous->ls_level, NORMAL);
		}
		else {
			lse->ls_previous->LS_BREAK =
				copy_state(top_ls->ls_current,
					 lse->ls_previous->ls_level);
		}
		if (lse == top_ls) {
			Free_state(&lse->ls_current);
			lint_pop();
		}
		break;

	default:
		NOTREACHED();
		/*NOTREACHED*/
	}
	top_ls->ls_current->st_notreached = 1;
	top_ls->ls_current->st_warned = 0;
}

PRIVATE
cont_merge(lse)
	struct lint_stack_entry *lse;
{
	/* merge for continue statements */
	if (lse->LS_BODY) {
		merge_states(top_ls->ls_current, lse->LS_BODY,
						lse->ls_level, NORMAL);
	}
	else {
		lse->LS_BODY = copy_state(top_ls->ls_current, lse->ls_level);
	}
}

PRIVATE
break_merge(lse)
	struct lint_stack_entry *lse;
{
	/* merge for break statements */
	if (lse->LS_LOOP) {
		merge_states(top_ls->ls_current, lse->LS_LOOP,
						lse->ls_level, NORMAL);
	}
	else {
		lse->LS_LOOP = copy_state(top_ls->ls_current, lse->ls_level);
	}
}

/******** A C T I O N S : R E T U R N ********/

lint_start_function()
{

	dbg_lint_stack("lint_start_function");
	valreturned = NORETURN;		/* initialization */
	return_warned = 0;
	lint_comment_function();
}

lint_end_function()
{

	dbg_lint_stack("lint_end_function");
	/* write the function definition record */
	outdef();

	/* At this stage it is possible that top_ls->ls_current is
	 * pointing to a state with a list of auto_defs.
	 * These auto_defs must be freed and the state must be filled
	 * with zeros.
	 */
	assert(!top_ls->ls_previous);
	free_auto_list(top_ls->ls_current->st_auto_list);
	top_ls->ls_current->st_auto_list = 0;
	top_ls->ls_current->st_notreached = 0;
	top_ls->ls_current->st_warned = 0;
}

lint_implicit_return()
{

	dbg_lint_stack("lint_implicit_return");
	if (!top_ls->ls_current->st_notreached) {
		lint_return_stmt(NOVALRETURNED);
	}
}

lint_return_stmt(e)
	int e;
{

	dbg_lint_stack("lint_return_stmt");
	if (valreturned == NORETURN) {
		/* first return met */
		register int fund = func_type->tp_fund;

		if (	e == NOVALRETURNED
		&&	!func_notypegiven
		&&	fund != VOID
		&&	fund != ERRONEOUS
		) {
			warning("function %s declared %s%s but no value returned",
				func_name,
				(func_type->tp_unsigned && fund != POINTER) ?
					"unsigned " : "",
				 symbol2str(fund)
			);
			/* adjust */
			e = VALRETURNED;
		}
		valreturned = e;
	}
	else
	if (valreturned != e && !return_warned) {
		warning("function %s does not always return a value",
			func_name);
		return_warned = 1;
	}

	if (!top_ls->ls_current->st_notreached) {
		set_od_valreturned(valreturned);
	}
	top_ls->ls_current->st_notreached = 1;
	top_ls->ls_current->st_warned = 0;
}

/******** A C T I O N S : J U M P ********/

lint_jump_stmt(idf)
	struct idf *idf;
{

	dbg_lint_stack("lint_jump_stmt");
	top_ls->ls_current->st_notreached = 1;
	top_ls->ls_current->st_warned = 0;
	if (idf->id_label)
		idf->id_label->df_used = 1;
}

lint_label()
{
/*	When meeting a label, we should take the intersection of all
	settings at all goto's leading this way, but this cannot reasonably
	be done.  So we assume that the user knows what he is doing and set
	all automatic variables to set.
*/
	register struct auto_def *a = top_ls->ls_current->st_auto_list;

	dbg_lint_stack("lint_label");
	while (a) {
		a->ad_maybe_set = 0;
		a->ad_set = 1;
		a = a->next;
	}
}

/******** A C T I O N S : S T A T E M E N T ********/

lint_statement()
{
/*	Check if this statement can be reached
*/

	dbg_lint_stack("lint_statement");
	if (s_NOTREACHED) {
		top_ls->ls_current->st_notreached = 1;
		top_ls->ls_current->st_warned = 0;
		s_NOTREACHED = 0;
	}
	if (DOT == '{' || DOT == ';')
		return;
	if (top_ls->ls_current->st_warned)
		return;
	if (top_ls->ls_current->st_notreached) {
		if (DOT != CASE && DOT != DEFAULT && AHEAD != ':') {
			if (DOT != BREAK || loptions['b'])
				warning("statement cannot be reached");
			top_ls->ls_current->st_warned = 1;
		}
		else {
			top_ls->ls_current->st_notreached = 0;
			top_ls->ls_current->st_warned = 0;
		}
	}
}

PRIVATE struct lint_stack_entry *
mk_lint_stack_entry(cl)
	int cl;
{
/*	Prepare a new stack entry for the lint_stack with class cl.
	Copy the top ls_current to this entry and set its level.
*/
	register struct lint_stack_entry *new = new_lint_stack_entry();
	
	new->ls_class = cl;
	new->ls_current = copy_state(top_ls->ls_current, level);
	new->ls_level = level;

	return new;
}

PRIVATE
lint_push(lse)
	struct lint_stack_entry *lse;
{
	lse->ls_previous = top_ls;
	top_ls->next = lse;
	top_ls = lse;
}

PRIVATE
lint_pop()
{
	top_ls = top_ls->ls_previous;
	free_lint_stack_entry(top_ls->next);
}

#ifdef	DEBUG
/* FOR DEBUGGING */

PRIVATE
print_autos(a)
	struct auto_def *a;
{
	while (a) {
		struct idf *idf = a->ad_idf;
		struct def *def = idf->id_def;

		print("%s", idf->id_text);
		print("(lvl=%d)", a->ad_def->df_level);
		print("(u%ds%dm%d U%dS%d) ",
			a->ad_used, a->ad_set, a->ad_maybe_set,
			def->df_used, def->df_set
		);
		a = a->next;
	}
}

PRIVATE
pr_lint_state(nm, st)
	char *nm;
	struct state *st;
{
	print("%s: ", nm);
	if (st) {
		print("notreached == %d ", st->st_notreached);
		print_autos(st->st_auto_list);
	}
	else {
		print("NULL");
	}
	print("\n");
}

print_lint_stack(msg)
	char *msg;
{
	register struct lint_stack_entry *lse = top_ls;

	print("Lint stack: %s(level=%d)\n", msg, level);
	while (lse) {
		print("  |-------------- level %d ------------\n",
					lse->ls_level);
		pr_lint_state("  |current", lse->ls_current);

		print("  |class == %s\n",
			lse->ls_class ? symbol2str(lse->ls_class) : "{");

		switch (lse->ls_class) {
		case SWITCH:
			pr_lint_state("   |LS_BREAK", lse->LS_BREAK);
			pr_lint_state("   |LS_CASE", lse->LS_CASE);
			break;

		case DO:
		case WHILE:
		case FOR:
			print("   |LS_TEST == %s\n",
				lse->LS_TEST == TEST_VAR ? "TEST_VAR" :
				lse->LS_TEST == TEST_TRUE ? "TEST_TRUE" :
				lse->LS_TEST == TEST_FALSE ? "TEST_FALSE" :
				"<<BAD VALUE>>"
			);
			pr_lint_state("   |LS_BODY", lse->LS_BODY);
			pr_lint_state("   |LS_LOOP", lse->LS_LOOP);
			break;

		case IF:
			pr_lint_state("   |LS_IF", lse->LS_IF);
			break;

		default:
			break;
		}
		lse = lse->ls_previous;
	}
	print("  |--------------\n\n");
}

#endif	/* DEBUG */

#endif	/* LINT */
