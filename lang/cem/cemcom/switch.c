/* $Header$ */
/*	S W I T C H - S T A T E M E N T  A D M I N I S T R A T I O N	*/

#include	"debug.h"
#include	"botch_free.h"
#include	"density.h"

#include	"Lpars.h"
#include	"idf.h"
#include	"label.h"
#include	"arith.h"
#include	"switch.h"
#include	"code.h"
#include	"storage.h"
#include	"assert.h"
#include	"expr.h"
#include	"type.h"
#include	"em.h"

extern char options[];

#define	compact(nr, low, up)	(nr != 0 && (up - low) / nr <= (DENSITY - 1))

static struct switch_hdr *switch_stack = 0;

code_startswitch(expr)
	struct expr *expr;
{
	/*	Check the expression, stack a new case header and
		fill in the necessary fields.
	*/
	register label l_table = text_label();
	register label l_break = text_label();
	register struct switch_hdr *sh = new_switch_hdr();
	int fund = any2arith(&expr, SWITCH);	/* INT, LONG or DOUBLE */
	
	switch (fund)	{
	case LONG:
		if (options['R'])
			warning("long in switch");
		break;
	case DOUBLE:
		error("float/double in switch");
		erroneous2int(&expr);
		break;
	}
	
	stat_stack(l_break, NO_LABEL);
	sh->sh_break = l_break;
	sh->sh_default = 0;
	sh->sh_table = l_table;
	sh->sh_nrofentries = 0;
	sh->sh_type = expr->ex_type;	/* the expression switched	*/
	sh->sh_lowerbd = sh->sh_upperbd = (arith)0;	/* immaterial ??? */
	sh->sh_entries = (struct case_entry *) 0; /* case-entry list	*/
	sh->next = switch_stack;	/* push onto switch-stack	*/
	switch_stack = sh;
	code_expr(expr, RVAL, TRUE, NO_LABEL, NO_LABEL);
					/* evaluate the switch expr.	*/
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
	tablabel = data_label();	/* the rom must have a label	*/
	C_df_dlb(tablabel);
	C_rom_ilb(sh->sh_default);
	if (compact(sh->sh_nrofentries, sh->sh_lowerbd, sh->sh_upperbd)) {
		/* CSA */
		register arith val;

		C_rom_cst(sh->sh_lowerbd);
		C_rom_cst(sh->sh_upperbd - sh->sh_lowerbd);
		ce = sh->sh_entries;
		for (val = sh->sh_lowerbd; val <= sh->sh_upperbd; val++) {
			ASSERT(ce);
			if (val == ce->ce_value)	{
				C_rom_ilb(ce->ce_label);
				ce = ce->next;
			}
			else
				C_rom_ilb(sh->sh_default);
		}
		C_lae_dlb(tablabel, (arith)0); /* perform the switch	*/
		C_csa(sh->sh_type->tp_size);
	}
	else	{ /* CSB */
		C_rom_cst((arith)sh->sh_nrofentries);
		for (ce = sh->sh_entries; ce; ce = ce->next)	{
			/* generate the entries: value + prog.label	*/
			C_rom_cst(ce->ce_value);
			C_rom_ilb(ce->ce_label);
		}
		C_lae_dlb(tablabel, (arith)0); /* perform the switch	*/
		C_csb(sh->sh_type->tp_size);
	}
	C_df_ilb(sh->sh_break);
	switch_stack = sh->next;	/* unstack the switch descriptor */
	
	/* free the allocated switch structure	*/
	ce = sh->sh_entries;
	while (ce)	{
		register struct case_entry *tmp = ce->next;
		free_case_entry(ce);
		ce = tmp;
	}
	free_switch_hdr(sh);
	stat_unstack();
}

code_case(expr)
	struct expr *expr;
{
	register arith val;
	register struct case_entry *ce;
	register struct switch_hdr *sh = switch_stack;
	
	if (sh == 0)	{
		error("case statement not in switch");
		return;
	}
	
	expr->ex_type = sh->sh_type;
	cut_size(expr);
	
	ce = new_case_entry();
	C_df_ilb(ce->ce_label = text_label());
	ce->ce_value = val = expr->VL_VALUE;
	
	if (sh->sh_entries == 0)	{
		/* first case entry	*/
		ce->next = (struct case_entry *) 0;
		sh->sh_entries = ce;
		sh->sh_lowerbd = sh->sh_upperbd = val;
		sh->sh_nrofentries = 1;
	}
	else	{
		/* second etc. case entry		*/
		/* find the proper place to put ce into the list	*/
		register struct case_entry *c1 = sh->sh_entries, *c2 = 0;
		
		if (val < sh->sh_lowerbd)
			sh->sh_lowerbd = val;
		else
		if (val > sh->sh_upperbd)
			sh->sh_upperbd = val;
		while (c1 && c1->ce_value < ce->ce_value)	{
			c2 = c1;
			c1 = c1->next;
		}
		/*	At this point three cases are possible:
			1: c1 != 0 && c2 != 0:
				insert ce somewhere in the middle
			2: c1 != 0 && c2 == 0:
				insert ce right after the head
			3: c1 == 0 && c2 != 0:
				append ce to last element
			The case c1 == 0 && c2 == 0 cannot occur, since
			the list is guaranteed not to be empty.
		*/
		if (c1)	{
			if (c1->ce_value == ce->ce_value)	{
				error("multiple case entry for value %ld",
					ce->ce_value);
				free_case_entry(ce);
				return;
			}
			if (c2)	{
				ce->next = c2->next;
				c2->next = ce;
			}
			else	{
				ce->next = sh->sh_entries;
				sh->sh_entries = ce;
			}
		}
		else	{
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

	if (sh == 0)	{
		error("default not in switch");
		return;
	}
	if (sh->sh_default != 0)	{
		error("multiple entry for default in switch");
		return;
	}
	C_df_ilb(sh->sh_default = text_label());
}
