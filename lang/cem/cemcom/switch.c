/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	S W I T C H - S T A T E M E N T  A D M I N I S T R A T I O N	*/

#include	"lint.h"
#include	"nofloat.h"
#ifndef	LINT
#include	<em.h>
#else
#include	"l_em.h"
#endif	/* LINT */
#include	"debug.h"
#include	"botch_free.h"
#include	<alloc.h>
#include	"density.h"
#include	"Lpars.h"
#include	"idf.h"
#include	"label.h"
#include	"arith.h"
#include	"switch.h"
#include	"code.h"
#include	"assert.h"
#include	"expr.h"
#include	"type.h"
#include	"noRoption.h"

extern char options[];
int density = DENSITY;

compact(nr, low, up)
	arith low, up;
{
	/*	Careful! up - low might not fit in an arith. And then,
		the test "up-low < 0" might also not work to detect this
		situation! Or is this just a bug in the M68020/M68000?
	*/
	arith diff = up - low;

	return (nr == 0 || (diff >= 0 && diff / nr <= (density - 1)));
}

static struct switch_hdr *switch_stack = 0;

/* (EB 86.05.20) The following rules hold for switch statements:
	- the expression E in "switch(E)" is cast to 'int' (RM 9.7)
	- the expression E in "case E:" must be 'int' (RM 9.7)
	- the values in the CSA/CSB tables are words (EM 7.4)
	For simplicity, we suppose int_size == word_size.
*/

code_startswitch(expp)
	struct expr **expp;
{
	/*	Check the expression, stack a new case header and
		fill in the necessary fields.
	*/
	register label l_table = text_label();
	register label l_break = text_label();
	register struct switch_hdr *sh = new_switch_hdr();
	int fund = any2arith(expp, SWITCH);	/* INT, LONG or DOUBLE */
	
	switch (fund) {
	case LONG:
#ifndef NOROPTION
		if (options['R'])
			warning("long in switch (cast to int)");
#endif
		int2int(expp, int_type);
		break;
#ifndef NOFLOAT
	case DOUBLE:
		error("float/double in switch");
		erroneous2int(expp);
		break;
#endif /* NOFLOAT */
	}
	stack_stmt(l_break, NO_LABEL);
	sh->sh_break = l_break;
	sh->sh_default = 0;
	sh->sh_table = l_table;
	sh->sh_nrofentries = 0;
	sh->sh_type = (*expp)->ex_type;	/* the expression switched	*/
	sh->sh_lowerbd = sh->sh_upperbd = (arith)0;	/* immaterial ??? */
	sh->sh_entries = (struct case_entry *) 0; /* case-entry list	*/
	sh->sh_expr = *expp;
#ifdef LINT
	code_expr(sh->sh_expr, RVAL, TRUE, NO_LABEL, NO_LABEL);
#endif
	sh->next = switch_stack;	/* push onto switch-stack	*/
	switch_stack = sh;
	C_bra(l_table);			/* goto start of switch_table	*/
}

code_endswitch()
{
	register struct switch_hdr *sh = switch_stack;
	register label tablabel;
	register struct case_entry *ce;

	if (sh->sh_default == 0)	/* no default occurred yet */
		sh->sh_default = sh->sh_break;
	C_bra(sh->sh_break);		/* skip the switch table now	*/
	C_df_ilb(sh->sh_table);		/* switch table entry		*/
	/* evaluate the switch expr.	*/
#ifndef LINT
	code_expr(sh->sh_expr, RVAL, TRUE, NO_LABEL, NO_LABEL);
#endif
	tablabel = data_label();	/* the rom must have a label	*/
	C_df_dlb(tablabel);
	C_rom_ilb(sh->sh_default);
	if (compact(sh->sh_nrofentries, sh->sh_lowerbd, sh->sh_upperbd)) {
		/* CSA */
		register arith val;

		C_rom_cst(sh->sh_lowerbd);
		C_rom_cst(sh->sh_upperbd - sh->sh_lowerbd);
		ce = sh->sh_entries;
		if (sh->sh_nrofentries)
		    for (val = sh->sh_lowerbd; val <= sh->sh_upperbd; val++) {
			ASSERT(ce);
			if (val == ce->ce_value) {
				C_rom_ilb(ce->ce_label);
				ce = ce->next;
			}
			else
				C_rom_ilb(sh->sh_default);
		}
		C_lae_dlb(tablabel, (arith)0); /* perform the switch	*/
		C_csa(sh->sh_type->tp_size);
	}
	else { /* CSB */
		C_rom_cst((arith)sh->sh_nrofentries);
		for (ce = sh->sh_entries; ce; ce = ce->next) {
			/* generate the entries: value + prog.label	*/
			C_rom_cst(ce->ce_value);
			C_rom_ilb(ce->ce_label);
		}
		C_lae_dlb(tablabel, (arith)0); /* perform the switch	*/
		C_csb(sh->sh_type->tp_size);
	}
	C_df_ilb(sh->sh_break);
	switch_stack = sh->next;	/* unstack the switch descriptor */
	for (ce = sh->sh_entries; ce;) { /* free allocated switch structure */
		register struct case_entry *tmp = ce->next;

		free_case_entry(ce);
		ce = tmp;
	}
	free_switch_hdr(sh);
	unstack_stmt();
}

code_case(expr)
	struct expr *expr;
{
	register arith val;
	register struct case_entry *ce;
	register struct switch_hdr *sh = switch_stack;
	
	ASSERT(is_cp_cst(expr));
	if (sh == 0) {
		error("case statement not in switch");
		return;
	}
	if (expr->ex_flags & EX_ERROR) /* is probably 0 anyway */
		return;
	ch7cast(&expr, CASE, sh->sh_type);
	ce = new_case_entry();
	C_df_ilb(ce->ce_label = text_label());
	ce->ce_value = val = expr->VL_VALUE;
	if (sh->sh_entries == 0) { /* first case entry	*/
		ce->next = (struct case_entry *) 0;
		sh->sh_entries = ce;
		sh->sh_lowerbd = sh->sh_upperbd = val;
		sh->sh_nrofentries = 1;
	}
	else { /* second etc. case entry; put ce into proper place */
		register struct case_entry *c1 = sh->sh_entries, *c2 = 0;
		
		if (val < sh->sh_lowerbd)
			sh->sh_lowerbd = val;
		else
		if (val > sh->sh_upperbd)
			sh->sh_upperbd = val;
		while (c1 && c1->ce_value < ce->ce_value) {
			c2 = c1;
			c1 = c1->next;
		}
		/*	At this point three cases are possible:
			1: c1 != 0 && c2 != 0: insert ce somewhere in the middle
			2: c1 != 0 && c2 == 0: insert ce right after the head
			3: c1 == 0 && c2 != 0: append ce to last element
			The case c1 == 0 && c2 == 0 cannot occur, since
			the list is guaranteed to be non-empty.
		*/
		if (c1) {
			if (c1->ce_value == ce->ce_value) {
				error("multiple case entry for value %ld",
					ce->ce_value);
				free_case_entry(ce);
				return;
			}
			if (c2) {
				ce->next = c2->next;
				c2->next = ce;
			}
			else {
				ce->next = sh->sh_entries;
				sh->sh_entries = ce;
			}
		}
		else {
			ASSERT(c2);
			ce->next = (struct case_entry *) 0;
			c2->next = ce;
		}
		(sh->sh_nrofentries)++;
	}
}

code_default()
{
	register struct switch_hdr *sh = switch_stack;

	if (sh == 0) {
		error("default statement not in switch");
		return;
	}
	if (sh->sh_default != 0) {
		error("multiple entry for default in switch");
		return;
	}

	C_df_ilb(sh->sh_default = text_label());
}
