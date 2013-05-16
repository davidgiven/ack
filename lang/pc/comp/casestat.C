/* C A S E   S T A T E M E N T   C O D E   G E N E R A T I O N */
#include    "parameters.h"
#include	"debug.h"

#include	<alloc.h>
#include	<assert.h>
#include	<em.h>

#include	"LLlex.h"
#include	"Lpars.h"
#include	"chk_expr.h"
#include	"main.h"
#include	"node.h"
#include	"type.h"

struct case_hdr	{
	struct case_hdr *ch_next;		/* in the free list */
	int ch_nrofentries;		/* number of cases */
	struct type *ch_type;		/* type of case expression */
	arith ch_lowerbd;		/* lowest case label */
	arith ch_upperbd;		/* highest case label */
	struct case_entry *ch_entries;	/* the cases */
};

/* ALLOCDEF "case_hdr" 5 */

struct case_entry	{
	struct case_entry *ce_next;	/* next in list */
	arith ce_value;			/* value of case label */
	label ce_label;			/* generated label */
};

/* ALLOCDEF "case_entry" 10 */

/* The constant DENSITY determines when CSA and when CSB instructions
   are generated. Reasonable values are: 2, 3, 4.
   On machines that have lots of address space and memory, higher values
   might also be reasonable. On these machines the density of jump tables
   may be lower.
*/
#define	compact(nr, low, up)	(nr != 0 && (up - low) / nr <= DENSITY)

CaseExpr(nd)
	struct node *nd;
{
	/* Check the expression and generate code for it
	*/

	register struct node *expp = nd->nd_left;

	if( !ChkExpression(expp) ) return;
	MarkUsed(expp);

	if( !(expp->nd_type->tp_fund & T_ORDINAL) )	{
		node_error(expp, "case-expression must be ordinal");
		return;
	}

	if( !err_occurred )	{
		CodePExpr(expp);
		C_bra(nd->nd_lab);
	}
}

CaseEnd(nd, exit_label)
	struct node *nd;
	label exit_label;
{
	/*	Stack a new case header and fill in the necessary fields.
	*/
	register struct case_hdr *ch = new_case_hdr();
	register struct node *right;

	assert(nd->nd_class == Link && nd->nd_symb == CASE);

	ch->ch_type = nd->nd_left->nd_type;

	right = nd->nd_right;

	/* Now, create case label list
	*/
	while( right )	{
		assert(right->nd_class == Link && right->nd_symb == ':');

		if( !AddCases(ch, right->nd_left, right->nd_lab) )	{
				FreeCh(ch);
				return;
		}
		right = right->nd_right;
	}

	if( !err_occurred )
		CaseCode(nd->nd_lab, ch, exit_label);

	FreeCh(ch);
	FreeNode(nd);
}

FreeCh(ch)
	register struct case_hdr *ch;
{
	/*	 free the allocated case structure	
	*/
	register struct case_entry *ce;

	ce = ch->ch_entries;
	while( ce )	{
		struct case_entry *tmp = ce->ce_next;

		free_case_entry(ce);
		ce = tmp;
	}

	free_case_hdr(ch);
}

AddCases(ch, nd, CaseLabel)
	register struct case_hdr *ch;
	register struct node *nd;
	label CaseLabel;
{
	while( nd )	{
		if( !AddOneCase(ch, nd, CaseLabel) )
			return 0;
		nd = nd->nd_next;
	}
	return 1;
}

AddOneCase(ch, nd, lbl)
	register struct case_hdr *ch;
	register struct node *nd;
	label lbl;
{
	register struct case_entry *ce = new_case_entry();
	register struct case_entry *c1 = ch->ch_entries, *c2 = 0;

	ce->ce_value = nd->nd_INT;
	ce->ce_label = lbl;
	if( !TstCompat(ch->ch_type, nd->nd_type) )	{
		node_error(nd, "case-statement: type incompatibility in case");
		free_case_entry(ce);
		return 0;
	}
	if( bounded(ch->ch_type) )	{
		arith lo, hi;

		getbounds(ch->ch_type, &lo, &hi);
		if( ce->ce_value < lo || ce->ce_value > hi )
			warning("case-statement: constant out of bounds");
	}

	if( !ch->ch_entries )	{
		/* first case entry
		*/
		ce->ce_next = (struct case_entry *) 0;
		ch->ch_entries = ce;
		ch->ch_lowerbd = ch->ch_upperbd = ce->ce_value;
		ch->ch_nrofentries = 1;
	}
	else	{
		/* second etc. case entry
		   find the proper place to put ce into the list
		*/
		
		if( ce->ce_value < ch->ch_lowerbd )
			ch->ch_lowerbd = ce->ce_value;
		else if( ce->ce_value > ch->ch_upperbd )
			ch->ch_upperbd = ce->ce_value;

		while( c1 && c1->ce_value < ce->ce_value )	{
			c2 = c1;
			c1 = c1->ce_next;
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
		if( c1 )	{
			if( c1->ce_value == ce->ce_value )	{
				node_error(nd,
					"case-statement: multiple case entry");
				free_case_entry(ce);
				return 0;
			}
			if( c2 )	{
				ce->ce_next = c2->ce_next;
				c2->ce_next = ce;
			}
			else	{
				ce->ce_next = ch->ch_entries;
				ch->ch_entries = ce;
			}
		}
		else	{
			assert(c2);

			ce->ce_next = (struct case_entry *) 0;
			c2->ce_next = ce;
		}
		(ch->ch_nrofentries)++;
	}
	return 1;
}

CaseCode(lbl, ch, exit_label)
	label lbl;
	struct case_hdr *ch;
	label exit_label;
{
	label CaseDescrLab = ++data_label;	/* rom must have a label */

	register struct case_entry *ce;
	register arith val;

	C_df_dlb(CaseDescrLab);
	C_rom_icon("0", pointer_size);

	if( compact(ch->ch_nrofentries, ch->ch_lowerbd, ch->ch_upperbd) ) {
		/* CSA */
		C_rom_cst(ch->ch_lowerbd);
		C_rom_cst(ch->ch_upperbd - ch->ch_lowerbd);
		ce = ch->ch_entries;
		for( val = ch->ch_lowerbd; val <= ch->ch_upperbd; val++ ) {
			assert(ce);
			if( val == ce->ce_value )	{
				C_rom_ilb(ce->ce_label);
				ce = ce->ce_next;
			}
			else
				C_rom_icon("0", pointer_size);
		}
		C_df_ilb(lbl);
		C_lae_dlb(CaseDescrLab, (arith) 0);
		C_csa(word_size);
	}
	else	{
		/* CSB */
		C_rom_cst((arith) ch->ch_nrofentries);
		for( ce = ch->ch_entries; ce; ce = ce->ce_next )	{
				C_rom_cst(ce->ce_value);
				C_rom_ilb(ce->ce_label);
		}
		C_df_ilb(lbl);
		C_lae_dlb(CaseDescrLab, (arith) 0);
		C_csb(word_size);
	}
 
	C_df_ilb(exit_label);
}
