/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	Lint status checking	*/

#include	"lint.h"

#ifdef	LINT

#include	<alloc.h>	/* for st_free */
#include	"debug.h"
#include	"interface.h"
#include	"assert.h"
#include	<flt_arith.h>
#include	"arith.h"	/* definition arith */
#include	"label.h"	/* definition label */
#include	"expr.h"
#include	"idf.h"
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

#define min(a, b) ((a) < (b) ? (a) : (b))

extern char *symbol2str();
extern char *func_name;
extern struct type *func_type;
extern int func_notypegiven;
extern char loptions[];

/* global variables for the lint_stack */
PRIVATE struct lint_stack_entry stack_bottom;
PRIVATE struct lint_stack_entry *top_ls = &stack_bottom;

/* global variables for the brace stack */
PRIVATE int brace_count;
PRIVATE struct brace brace_bottom;
PRIVATE struct brace *top_br = &brace_bottom;

PRIVATE end_brace();
PRIVATE lint_1_local();
PRIVATE lint_1_global();
PRIVATE check_autos();
PRIVATE struct auto_def *copy_st_auto_list();
PRIVATE free_st_auto_list();
PRIVATE struct state *copy_state();
PRIVATE Free_state();
PRIVATE remove_settings();
PRIVATE struct auto_def *merge_autos();
PRIVATE merge_states();
PRIVATE struct lint_stack_entry *find_wdf(), *find_wdfc(), *find_cs();
PRIVATE cont_break_merge();
PRIVATE lint_push();
PRIVATE lint_pop();

lint_init_stack()
{
/* Allocate some memory for the global stack_bottom
 */
	stack_bottom.ls_current = new_state();
}

lint_start_local()
{
	register struct brace *br = new_brace();

	brace_count++;
	br->br_count = brace_count;
	br->br_level = level;
	br->next = top_br;
	top_br = br;
}	

lint_local_level(stl)
	struct stack_level *stl;
{
	if (s_NOTREACHED) {
		top_ls->ls_current->st_notreached = 1;
		s_NOTREACHED = 0;
	}

	if (top_ls->ls_class == CASE && level == top_ls->ls_level) {
		lint_break_stmt();
			/* To prevent a warning for the case
			 *	switch (cond) {
			 *	int i;
			 *	case 0:
			 *		i = 0;
			 *		use(i);
			 *	}
			 */
	}

	check_autos();
	end_brace(stl);
}

PRIVATE
end_brace(stl)
	struct stack_level *stl;
{
	/*	Check if static variables and labels are used and/or set.
	*/
	register struct stack_entry *se = stl->sl_entry;
	register struct brace *br;

	while (se) {
		register struct idf *idf = se->se_idf;
		register struct def *def = idf->id_def;

		if (def) {
			lint_1_local(idf, def);
		}
		se = se->next;
	}

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
	) {
		def_warning(def, "%s %s declared but not used in function %s",
			symbol2str(sc), idf->id_text, func_name);
	}

	if (	loptions['h']
	&&	sc == AUTO
	&&	!def->df_initialized
	&&	def->df_firstbrace != 0
	&&	def->df_minlevel != level
	) {
		register int diff = def->df_minlevel - level;

		def_warning(def,
			"local %s could be declared %d level%s deeper",
			idf->id_text, diff, (diff == 1 ? "" : "s")
		);
	}
}

lint_global_level(stl)
	struct stack_level *stl;
{
	register struct stack_entry *se = stl->sl_entry;

	ASSERT(level == L_GLOBAL);
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
	register int fund = def->df_type->tp_fund;

	switch (sc) {
	case STATIC:
	case EXTERN:
	case GLOBAL:
	case IMPLICIT:
		if (fund == ERRONEOUS)
			break;

		if (def->df_set || def->df_used) {
			/* Output a line to the intermediate file for
			 * used external variables (including functions)
			 */
			output_use(idf);
		}
		else {
			if (sc == STATIC) {
				if (def->df_set) {
					def_warning(def,
						"%s %s %s set but not used",
						symbol2str(sc),
						symbol2str(fund),
						idf->id_text);
				}
				else {
					def_warning(def,
						"%s %s %s not used anywhere",
						symbol2str(sc),
						symbol2str(fund),
						idf->id_text);
				}
			}
			if (loptions['x']) {
				register char *fn = def->df_file;

				if (	(sc == EXTERN || sc == GLOBAL)
				&&	def->df_alloc == 0
				&&	!def->df_set
				&&	!def->df_initialized
				&&	strcmp(&fn[strlen(fn)-2], ".c") == 0
				) {
					def_warning(def,
						"%s %s %s not used anywhere",
						symbol2str(sc),
						symbol2str(fund),
						idf->id_text);
				}
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
 * For non-automatic variables, the fields in the def-descriptor are set too.
 */
	register struct def *def = idf->id_def;
	register struct auto_def *a = top_ls->ls_current->st_auto_list;

	if (def) {
		switch (to_state) {
		case SET:
			def->df_set = 1;
			break;
		case USED:
			def->df_used = 1;
			break;
		}

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
			ASSERT(br && def->df_minlevel >= br->br_level);
			def->df_minlevel = br->br_level;
		}
	}

	while(a && a->ad_idf != idf)
		a = a->next;
	if (a == 0)	/* identifier not in list */
		return;

	switch (to_state) {
	case SET:
		a->ad_maybe_set = 0;
		a->ad_set = 1;
		break;
	case USED:
		if (!a->ad_set) {
			warning("%s%s uninitialized", idf->id_text,
				(a->ad_maybe_set ? " possibly" : "")
			);
			a->ad_maybe_set = 0;
			a->ad_set = 1;	/* one warning */
		}
		a->ad_used = 1;
		break;
	}
}

extern struct stack_level *local_level;

add_auto(idf)	/* to current state on top of lint_stack */
	struct idf *idf;
{
/* Check if idf's definition is really an auto (or register).
 * It could be a static or extern too.
 * Watch out for register formal parameters.
 */
	register struct def *def = idf->id_def;
	register struct auto_def *a;

	if (!def)
		return;
	switch (def->df_sc) {
	case AUTO:
	case REGISTER:
		if (def->df_level < L_LOCAL)
			return;		/* a register formal */
		a = new_auto_def();
		a->ad_idf = idf;
		a->ad_def = idf->id_def;
		a->ad_used = def->df_used;
		a->ad_set = def->df_set;
		a->next = top_ls->ls_current->st_auto_list;
		top_ls->ls_current->st_auto_list = a;
	}
}

PRIVATE
check_autos()
{
/* Before leaving a block remove the auto_defs of the automatic
 * variables on this level and check if they are used
 */
	register struct auto_def *a1 = top_ls->ls_current->st_auto_list;
	register struct auto_def *a2;

	ASSERT(!(a1 && a1->ad_def->df_level > level));
	while (a1 && a1->ad_def->df_level == level) {
		a2 = a1;
		a1 = a1->next;
		if (!a2->ad_used) {
			if (a2->ad_set || a2->ad_maybe_set) {
				def_warning(a2->ad_def,
					"%s set but not used in function %s",
					a2->ad_idf->id_text, func_name);
			}
			else {
				def_warning(a2->ad_def,
					"%s neither set nor used in function %s",
					a2->ad_idf->id_text, func_name);
			}
		}
		free_auto_def(a2);
	}
	top_ls->ls_current->st_auto_list = a1;
}

check_args_used()
{
	register struct stack_entry *se = local_level->sl_entry;

	ASSERT(level == L_FORMAL1);
	while (se) {
		register struct def *def = se->se_idf->id_def;

		if (	(def && !def->df_used)
		&&	!(f_ARGSUSED || LINTLIB)
		) {
			def_warning(def, "argument %s not used in function %s",
					se->se_idf->id_text, func_name);
		}
		se = se->next;
	}
}

PRIVATE struct auto_def *
copy_st_auto_list(from_al, lvl)
	struct auto_def *from_al;
{
	struct auto_def *start = 0;
	register struct auto_def **hook = &start;

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
free_st_auto_list(au)
	register struct auto_def *au;
{
	register struct auto_def *a;

	while (au) {
		a = au;
		au = au->next;
		free_auto_def(a);
	}
}

PRIVATE struct state *
copy_state(from_st, lvl)
	struct state *from_st;
{
/* Memory for the struct state and the struct auto_defs is allocated
 * by this function
 */
	register struct state *st = new_state();

	st->st_auto_list = copy_st_auto_list(from_st->st_auto_list, lvl);
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
	free_st_auto_list((*stp)->st_auto_list);
	free_state(*stp);
	*stp = 0;
}

PRIVATE
remove_settings(state, lvl)
	struct state *state;
{
/* The state of all variables on this level are set to 'not set' and
 * 'not maybe set'. (I think you have to read this twice.)
 */
	register struct auto_def *a = state->st_auto_list;

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

PRIVATE struct auto_def *
merge_autos(a1, a2, lvl, mode)
	struct auto_def *a1, *a2;
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

	while (a1 && a1->ad_def->df_level > lvl) {
		a1 = a1->next;
	}
	while (a2 && a2->ad_def->df_level > lvl) {
		a = a2;
		a2 = a2->next;
		free_auto_def(a);
	}
	a = a2;	/* pointer to the result */
	while (a1) {
		ASSERT(a2);
		ASSERT(a1->ad_idf == a2->ad_idf);
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
	ASSERT(!a2);
	return a;
}

PRIVATE
merge_states(st1, st2, lvl, mode)
	struct state *st1, *st2;
	int mode;
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
	else if (st2->st_notreached) {
		register struct auto_def *tmp = st2->st_auto_list;

		st2->st_auto_list = copy_st_auto_list(st1->st_auto_list, lvl);
		st2->st_notreached = 0;
		st2->st_warned = 0;
		st2->st_auto_list = merge_autos(tmp, st2->st_auto_list,
							lvl, USE_ONLY);
		free_st_auto_list(tmp);
	}
	else {
		st2->st_auto_list =
			merge_autos(st1->st_auto_list, st2->st_auto_list,
				lvl, mode);
	}
}


/******** L I N T   S T A C K   S E A R C H I N G ********/

/* The next four find-functions search the lint_stack for an entry.
 * The letters mean : w: WHILE; d: DO; f: FOR; s: SWITCH; c: CASE.
 */

PRIVATE struct lint_stack_entry *
find_wdf()
{
	register struct lint_stack_entry *lse = top_ls;

	while (lse != &stack_bottom) {
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

	while (lse != &stack_bottom) {
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

	while (lse != &stack_bottom) {
		switch (lse->ls_class) {
		case CASE:
		case SWITCH:
			return lse;
		}
		lse = lse->ls_previous;
	}
	return 0;
}

/******** A C T I O N S ********/

start_if_part(const)
{
/* Push a new stack entry on the lint_stack with class == IF
 * copy the ls_current to the top of this stack
 */
	register struct lint_stack_entry *lse = new_lint_stack_entry();

	if (const)
		hwarning("condition in if statement is constant");

	lse->ls_class = IF;
	lse->ls_current = copy_state(top_ls->ls_current, level);
	lse->ls_level = level;
	lint_push(lse);
}

start_else_part()
{
/* Move ls_current to LS_IF_STATE
 * ls_current of the stack entry one below is copied to ls_current.
 */
	if (s_NOTREACHED) {
		top_ls->ls_current->st_notreached = 1;
		s_NOTREACHED = 0;
	}
	top_ls->LS_IF_STATE = top_ls->ls_current;
	/* this is the reason why ls_current is a pointer */
	top_ls->ls_current = copy_state(top_ls->ls_previous->ls_current,
								level);
	top_ls->ls_level = level;
}

end_if_else_stmt()
{
	Free_state(&top_ls->ls_previous->ls_current);
	merge_states(top_ls->LS_IF_STATE, top_ls->ls_current,
					top_ls->ls_level, NORMAL);
	Free_state(&top_ls->LS_IF_STATE);
	top_ls->ls_previous->ls_current = top_ls->ls_current;
	lint_pop();
}

end_if_stmt()
{
/* No else-part met; merge ls_current with ls_current of previous
 * stack entry
 */
	merge_states(top_ls->ls_current, top_ls->ls_previous->ls_current,
						top_ls->ls_level, NORMAL);
	Free_state(&top_ls->ls_current);
	lint_pop();
}

start_loop_stmt(looptype, const, cond)
{
/* If const, the condition is constant and given in cond */
	register struct lint_stack_entry *lse = new_lint_stack_entry();

	lse->ls_class = looptype;
	lse->ls_current = copy_state(top_ls->ls_current, level);
	lse->ls_level = level;
	if (const && !cond) {
		/* while (0) | for (;0;) */
		hwarning("condition in %s statement is constant",
						symbol2str(looptype));
		lse->ls_current->st_notreached = 1;
	}
	if (const && cond) {
		/* while (1) | for (;;) | do */
		/*	omitting the copy for LS_END will force this loop
			to be treated as a do loop
		*/
		top_ls->ls_current->st_notreached = 1;
	}
	else {
		lse->LS_END = copy_state(top_ls->ls_current, level);
	}
	lint_push(lse);
}

end_loop_stmt()
{
	register struct lint_stack_entry *prev_ls = top_ls->ls_previous;

	lint_continue_stmt();
	top_ls->LS_END->st_notreached = prev_ls->ls_current->st_notreached;
	top_ls->LS_END->st_warned = prev_ls->ls_current->st_warned;
	Free_state(&top_ls->ls_current);
	Free_state(&prev_ls->ls_current);
	prev_ls->ls_current = top_ls->LS_END;
	lint_pop();
}

end_do_stmt(const, cond)
{
	end_loop_stmt();
	if (const)
		hwarning("condition in do-while statement is constant");
	if (const && cond && top_ls->ls_current->st_notreached) {
		/* no break met; this is really an endless loop */
	}
	else {
		top_ls->ls_current->st_notreached = 0;
	}
}

PRIVATE
cont_break_merge(lse)
	struct lint_stack_entry *lse;
{
	/* merge for continue and break statements */
	if (lse->LS_END) {
		merge_states(top_ls->ls_current, lse->LS_END,
						lse->ls_level, NORMAL);
	}
	else {
		lse->LS_END = copy_state(top_ls->ls_current, lse->ls_level);
	}
}

lint_continue_stmt()
{
	register struct lint_stack_entry *lse = find_wdf();

	if (!lse)
		return;		/* not inside a loop statement */

	cont_break_merge(lse);
	top_ls->ls_current->st_notreached = 1;
}

start_switch_part(expr)
	struct expr *expr;
{
/* ls_current of a SWITCH entry has different meaning from ls_current of
 * other entries. It keeps track of which variables are used in all
 * following case parts. (Needed for variables declared in a compound
 * switch-block.)
 */
	register struct lint_stack_entry *lse = new_lint_stack_entry();

	if (is_cp_cst(expr))
		hwarning("value in switch statement is constant");

	lse->ls_class = SWITCH;
	lse->ls_current = copy_state(top_ls->ls_current, level);
	lse->ls_level = level;
	lse->LS_CASE = copy_state(top_ls->ls_current, level);
	lse->ls_current->st_notreached = 1;
	top_ls->ls_current->st_notreached = 1;
	lint_push(lse);
}

end_switch_stmt()
{
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

	register struct lint_stack_entry *lse;
	register struct lint_stack_entry *cs_entry = find_cs();

	if (!cs_entry)
		return;		/* not inside switch */
	if (cs_entry != top_ls) {
		warning("%s statement in strange context",
			dflt ? "default" : "case");
		return;
	}
	if (cs_entry->ls_class == SWITCH) {
		if (dflt) {
			cs_entry->LS_DEFAULT_MET = 1;
		}
		lse = new_lint_stack_entry();
		lse->ls_class = CASE;
		lse->ls_current = copy_state(top_ls->ls_current, level);
		remove_settings(lse->ls_current, level);
		lse->ls_level = level;
		lint_push(lse);
	}
	else {
		ASSERT(cs_entry->ls_class == CASE);
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
	}
}

lint_break_stmt()
{
	register struct lint_stack_entry *lse = find_wdfc();

	if (!lse)
		return;

	switch (lse->ls_class) {
	case WHILE:
	case FOR:
	case DO:
		/* loop break */
		lse->ls_previous->ls_current->st_notreached = 0;
		cont_break_merge(lse);
		break;

	case CASE:
		/* case break */
		if (!top_ls->ls_current->st_notreached) {
			lse->ls_previous->ls_previous->ls_current->st_notreached = 0;
		}
		merge_states(lse->ls_current, lse->ls_previous->ls_current,
					lse->ls_previous->ls_level, NORMAL);
		if (lse->ls_previous->LS_BREAK) {
			merge_states(top_ls->ls_current, lse->ls_previous->LS_BREAK,
					lse->ls_previous->ls_level, NORMAL);
		}
		else {
			lse->ls_previous->LS_BREAK = copy_state(top_ls->ls_current,
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
}

lint_start_function()
{
	lint_return_stmt(-1);	/* initialization */
	lint_comment_function();
}

lint_end_function()
{
	extern struct outdef OutDef;
	register int fund = func_type->tp_fund;

	if (	OutDef.od_valreturned == NOVALRETURNED
	&&	!func_notypegiven
	&&	fund != VOID
	) {
		warning("function %s declared %s%s but no value returned",
			func_name,
			(func_type->tp_unsigned && fund != POINTER) ?
				"unsigned " : "",
			 symbol2str(fund)
		);
	}
	/* write the function definition record */
	outdef();

	/* At this stage it is possible that stack_bottom.ls_current is
	 * pointing to a state with a list of auto_defs.
	 * These auto_defs must be freed and the state must be filled
	 * with zeros.
	 */
	ASSERT(top_ls == &stack_bottom);
	if (top_ls->ls_current->st_auto_list != 0)
		free_st_auto_list(top_ls->ls_current->st_auto_list);
	top_ls->ls_current->st_auto_list = 0;
	top_ls->ls_current->st_notreached = 0;
	top_ls->ls_current->st_warned = 0;
}

lint_return_stmt(e)
	int e;
{
/* The statics of this function are initialized by calling it with e = -1. */

	static int ret_e;
				/*-1	no return met yet
				 * 0	return; met
				 * 1	return with expression met
				 */
	static int warned;

	switch (e) {
	case -1:
		ret_e = -1;
		warned = 0;
		return;
	case 0:
		if (top_ls->ls_current->st_notreached)
			break;
		if (ret_e == 1 && !warned) {
			warning("function %s does not always return a value",
				func_name);
			warned = 1;
		}
		else
			ret_e = 0;
		break;
	case 1:
		if (top_ls->ls_current->st_notreached)
			break;
		if (ret_e == 0 && !warned) {
			warning("function %s does not always return a value",
				func_name);
			warned = 1;
		}
		else
			ret_e = 1;
		break;
	}
	if (!top_ls->ls_current->st_notreached)
		set_od_valreturned(e);
	top_ls->ls_current->st_notreached = 1;
}

lint_jump_stmt(idf)
	struct idf *idf;
{
	top_ls->ls_current->st_notreached = 1;
	if (!idf->id_def)
		return;
	idf->id_def->df_used = 1;
}

lint_label()
{
/*	When meeting a label, we should take the intersection of all
	settings at all goto's leading this way, but this cannot reasonably
	be done.  So we assume that the user knows what he is doing and set
	all automatic variables to set.
*/
	register struct auto_def *a = top_ls->ls_current->st_auto_list;

	while (a) {
		a->ad_maybe_set = 0;
		a->ad_set = 1;
		a = a->next;
	}
}

lint_statement()
{
/* Check if this statement can be reached
 */
	if (s_NOTREACHED) {
		top_ls->ls_current->st_notreached = 1;
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

print_lint_stack()
{
	register struct lint_stack_entry *lse = top_ls;

	while (lse) {
		print("  |-------------- level %d ------------\n",
					lse->ls_level);
		print("  |cur: ");
		if (lse->ls_current) {
			print_autos(lse->ls_current->st_auto_list);
			print("  |st_notreached == %d\n",
				lse->ls_current->st_notreached);
		}
		else
			print("\n");
		print("  |class == %s\n",
			lse->ls_class ? symbol2str(lse->ls_class) : "{");
		switch (lse->ls_class) {
		case SWITCH:
			print("  |LS_BREAK: ");
			if (lse->LS_BREAK) {
				print_autos(lse->LS_BREAK->st_auto_list);
				print("  |st_notreached == %d\n",
					lse->LS_BREAK->st_notreached);
			}
			else
				print("\n");
			print("  |LS_CASE:  ");
			if (lse->LS_CASE) {
				print_autos(lse->LS_CASE->st_auto_list);
				print("  |st_notreached == %d\n",
					lse->LS_CASE->st_notreached);
			}
			else
				print("\n");
			break;
		case DO:
		case WHILE:
		case FOR:
			print("  |LS_END:  ");
			if (lse->LS_END) {
				print_autos(lse->LS_END->st_auto_list);
				print("  |st_notreached == %d\n",
					lse->LS_END->st_notreached);
			}
			else
				print("\n");
			break;
		case IF:
			print("  |LS_IF_STATE: ");
			if (lse->LS_IF_STATE) {
				print_autos(lse->LS_IF_STATE->st_auto_list);
				print("  |st_notreached == %d\n",
					lse->LS_IF_STATE->st_notreached);
			}
			else
				print("\n");
			break;
		default:
			break;
		}
		lse = lse->ls_previous;
	}
	print("  |--------------\n\n");
}

print_autos(a)
	register struct auto_def *a;
{
	while (a) {
		print("%s", a->ad_idf->id_text);
		print("(l=%d)", a->ad_def->df_level);
		print("(U%dS%dM%d) ", a->ad_used, a->ad_set, a->ad_maybe_set);
		a = a->next;
	}
	print("\n");
}
#endif	DEBUG

#endif	LINT
