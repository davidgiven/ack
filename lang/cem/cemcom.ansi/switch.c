/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	S W I T C H - S T A T E M E N T  A D M I N I S T R A T I O N	*/

#include	"parameters.h"
#ifndef	LINT
#include	<em.h>
#else
#include	"l_em.h"
#endif	/* LINT */
#include	<alloc.h>
#include	"Lpars.h"
#include	"label.h"
#include	<flt_arith.h>
#include	"arith.h"
#include	"switch.h"
#include	"code.h"
#include	"assert.h"
#include	"expr.h"
#include	"type.h"
#include	"sizes.h"

extern char options[];

int	density = DENSITY;

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
	- the expression E in "switch(E)" shall have integral type (3.6.4.2)
	- the expression E in "case E:" is converted to the promoted type
					of the controlling expression
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
	int fund = any2arith(expp, SWITCH);
				    /* INT, LONG, FLOAT, DOUBLE or LNGDBL */
	
	switch (fund) {
	case FLOAT:
	case DOUBLE:
	case LNGDBL:
		error("floating point type in switch");
		erroneous2int(expp);
		break;
	}
	stack_stmt(l_break, NO_LABEL);
	sh->sh_break = l_break;
	/* sh->sh_default = 0; */
	sh->sh_table = l_table;
	/* sh->sh_nrofentries = 0; */
	sh->sh_type = (*expp)->ex_type;	/* the expression switched	*/
	/* sh->sh_entries = (struct case_entry *) 0; -- case-entry list	*/
	sh->sh_expr = *expp;
#ifdef LINT
	code_expr(sh->sh_expr, RVAL, TRUE, NO_LABEL, NO_LABEL);
#endif
	sh->next = switch_stack;	/* push onto switch-stack	*/
	switch_stack = sh;
	C_bra(l_table);			/* goto start of switch_table	*/
}

extern char *long2str();

code_endswitch()
{
	register struct switch_hdr *sh = switch_stack;
	register label tablabel;
	register struct case_entry *ce;
	arith size = sh->sh_type->tp_size;

	if (sh->sh_default == 0)	/* no default occurred yet */
		sh->sh_default = sh->sh_break;

#ifndef LINT
	C_bra(sh->sh_break);		/* skip the switch table now	*/
	C_df_ilb(sh->sh_table);		/* switch table entry		*/
	/* evaluate the switch expr.	*/
	code_expr(sh->sh_expr, RVAL, TRUE, NO_LABEL, NO_LABEL);
	if (sh->sh_nrofentries <= 1) {
		if (sh->sh_nrofentries) {
			load_cst(sh->sh_lowerbd, size);
			C_cms(size);
			C_zeq(sh->sh_entries->ce_label);
		}
		else	C_asp(size);
		C_bra(sh->sh_default);
	}
	else {
	    tablabel = data_label();	/* the rom must have a label	*/
	    C_df_dlb(tablabel);
	    C_rom_ilb(sh->sh_default);
	    if (compact(sh->sh_nrofentries, sh->sh_lowerbd, sh->sh_upperbd)) {
		/* CSA */
		register arith val;

		C_rom_icon(long2str((long)sh->sh_lowerbd,10), size);
		C_rom_icon(long2str((long)(sh->sh_upperbd - sh->sh_lowerbd),10),
				size);
		ce = sh->sh_entries;
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
		C_csa(size);
	    }
	    else { /* CSB */
		C_rom_icon(long2str((long)sh->sh_nrofentries,10),size);
		for (ce = sh->sh_entries; ce; ce = ce->next) {
			/* generate the entries: value + prog.label	*/
			C_rom_icon(long2str((long)ce->ce_value,10),size);
			C_rom_ilb(ce->ce_label);
		}
		C_lae_dlb(tablabel, (arith)0); /* perform the switch	*/
		C_csb(size);
	    }
	}
	C_df_ilb(sh->sh_break);
#endif

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
	ch3cast(&expr, CASE, sh->sh_type);
	ce = new_case_entry();
	C_df_ilb(ce->ce_label = text_label());
	ce->ce_value = val = expr->VL_VALUE;
	if (sh->sh_entries == 0) { /* first case entry	*/
		/* ce->next = (struct case_entry *) 0; */
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
