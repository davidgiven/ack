/* C A S E   S T A T E M E N T   C O D E   G E N E R A T I O N */

/*	Generation of case statements is done by first creating a
	description structure for the statement, build a list of the
	case-labels, then generating a case description in the code,
	and generating either CSA or CSB, and then generating code for the
	cases themselves.
*/

#include	"debug.h"

#include	<em_label.h>
#include	<em_arith.h>
#include	<em_code.h>
#include	<alloc.h>
#include	<assert.h>

#include	"Lpars.h"
#include	"type.h"
#include	"LLlex.h"
#include	"node.h"
#include	"desig.h"
#include	"walk.h"

#include	"density.h"

struct switch_hdr	{
	struct switch_hdr *next;	/* in the free list */
	label sh_break;			/* label of statement after this one */
	label sh_default;		/* label of ELSE part, or 0 */
	int sh_nrofentries;		/* number of cases */
	struct type *sh_type;		/* type of case expression */
	arith sh_lowerbd;		/* lowest case label */
	arith sh_upperbd;		/* highest case label */
	struct case_entry *sh_entries;	/* the cases with their generated
					   labels
					*/
};

/* STATICALLOCDEF "switch_hdr" 5 */

struct case_entry	{
	struct case_entry *next;	/* next in list */
	label ce_label;			/* generated label */
	arith ce_value;			/* value of case label */
};

/* STATICALLOCDEF "case_entry" 20 */

/* The constant DENSITY determines when CSA and when CSB instructions
   are generated. Reasonable values are: 2, 3, 4.
   On machines that have lots of address space and memory, higher values
   might also be reasonable. On these machines the density of jump tables
   may be lower.
*/
#define	compact(nr, low, up)	(nr != 0 && (up - low) / nr <= DENSITY)

CaseCode(nd, exitlabel)
	struct node *nd;
	label exitlabel;
{
	/*	Check the expression, stack a new case header and
		fill in the necessary fields.
		"exitlabel" is the exit-label of the closest enclosing
		LOOP-statement, or 0.
	*/
	register struct switch_hdr *sh = new_switch_hdr();
	register struct node *pnode = nd;
	register struct case_entry *ce;
	register arith val;
	label CaseDescrLab;

	assert(pnode->nd_class == Stat && pnode->nd_symb == CASE);

	WalkExpr(pnode->nd_left);	/* evaluate case expression */
	sh->sh_type = pnode->nd_left->nd_type;
	sh->sh_break = ++text_label;

	/* Now, create case label list
	*/
	while (pnode->nd_right) {
		pnode = pnode->nd_right;
		if (pnode->nd_class == Link && pnode->nd_symb == '|') {
			if (pnode->nd_left) {
				/* non-empty case
				*/
				pnode->nd_lab = ++text_label;
				if (! AddCases(sh, /* to descriptor */
					       pnode->nd_left->nd_left,
						   /* of case labels */
					       pnode->nd_lab
						   /* and code label */
					      )) {
					FreeSh(sh);
					return;
				}
			}
		}
		else {
			/* Else part
			*/

			sh->sh_default = ++text_label;
			break;
		}
	}

	/* Now generate code for the switch itself
	   First the part that CSA and CSB descriptions have in common.
	*/
	CaseDescrLab = ++data_label;	/* the rom must have a label	*/
	C_df_dlb(CaseDescrLab);
	if (sh->sh_default) C_rom_ilb(sh->sh_default);
	else C_rom_ucon("0", pointer_size);
	if (compact(sh->sh_nrofentries, sh->sh_lowerbd, sh->sh_upperbd)) {
		/* CSA
		*/
		C_rom_cst(sh->sh_lowerbd);
		C_rom_cst(sh->sh_upperbd - sh->sh_lowerbd);
		ce = sh->sh_entries;
		for (val = sh->sh_lowerbd; val <= sh->sh_upperbd; val++) {
			assert(ce);
			if (val == ce->ce_value)	{
				C_rom_ilb(ce->ce_label);
				ce = ce->next;
			}
			else if (sh->sh_default) C_rom_ilb(sh->sh_default);
			else C_rom_ucon("0", pointer_size);
		}
		C_lae_dlb(CaseDescrLab, (arith)0);	/* perform the switch */
		C_csa(word_size);
	}
	else	{ 
		/* CSB
		*/
		C_rom_cst((arith)sh->sh_nrofentries);
		for (ce = sh->sh_entries; ce; ce = ce->next)	{
			/* generate the entries: value + prog.label
			*/
			C_rom_cst(ce->ce_value);
			C_rom_ilb(ce->ce_label);
		}
		C_lae_dlb(CaseDescrLab, (arith)0);	/* perform the switch */
		C_csb(word_size);
	}

	/* Now generate code for the cases
	*/
	pnode = nd;
	while (pnode->nd_right) {
		pnode = pnode->nd_right;
		if (pnode->nd_class == Link && pnode->nd_symb == '|') {
			if (pnode->nd_left) {
				C_df_ilb(pnode->nd_lab);
				WalkNode(pnode->nd_left->nd_right, exitlabel);
				C_bra(sh->sh_break);
			}
		}
		else {
			/* Else part
			*/
			assert(sh->sh_default != 0);

			C_df_ilb(sh->sh_default);
			WalkNode(pnode, exitlabel);
			break;
		}
	}

	C_df_ilb(sh->sh_break);
	FreeSh(sh);
}

FreeSh(sh)
	register struct switch_hdr *sh;
{
	/*	 free the allocated switch structure	
	*/
	register struct case_entry *ce;

	ce = sh->sh_entries;
	while (ce)	{
		struct case_entry *tmp = ce->next;

		free_case_entry(ce);
		ce = tmp;
	}

	free_switch_hdr(sh);
}

AddCases(sh, node, lbl)
	struct switch_hdr *sh;
	register struct node *node;
	label lbl;
{
	/*	Add case labels to the case label list
	*/
	register arith v1, v2;

	if (node->nd_class == Link) {
		if (node->nd_symb == UPTO) {
			assert(node->nd_left->nd_class == Value);
			assert(node->nd_right->nd_class == Value);

			v2 = node->nd_right->nd_INT;
			node->nd_type = node->nd_left->nd_type;
			for (v1 = node->nd_left->nd_INT; v1 <= v2; v1++) {
				node->nd_INT = v1;
				if (! AddOneCase(sh, node, lbl)) return 0;
			}
			return 1;
		}

		assert(node->nd_symb == ',');
		return	AddCases(sh, node->nd_left, lbl) &&
		    	AddCases(sh, node->nd_right, lbl);
	}

	assert(node->nd_class == Value);
	return AddOneCase(sh, node, lbl);
}

AddOneCase(sh, node, lbl)
	register struct switch_hdr *sh;
	register struct node *node;
	label lbl;
{
	register struct case_entry *ce = new_case_entry();
	register struct case_entry *c1 = sh->sh_entries, *c2 = 0;

	ce->ce_label = lbl;
	ce->ce_value = node->nd_INT;
	if (! TstCompat(sh->sh_type, node->nd_type)) {
		node_error(node, "Type incompatibility in case");
		free_case_entry(ce);
		return 0;
	}
	if (sh->sh_entries == 0)	{
		/* first case entry
		*/
		ce->next = (struct case_entry *) 0;
		sh->sh_entries = ce;
		sh->sh_lowerbd = sh->sh_upperbd = ce->ce_value;
		sh->sh_nrofentries = 1;
	}
	else	{
		/* second etc. case entry
		   find the proper place to put ce into the list
		*/
		
		if (ce->ce_value < sh->sh_lowerbd) {
			sh->sh_lowerbd = ce->ce_value;
		}
		else if (ce->ce_value > sh->sh_upperbd) {
			sh->sh_upperbd = ce->ce_value;
		}
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
node_error(node, "multiple case entry for value %ld", ce->ce_value);
				free_case_entry(ce);
				return 0;
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
			assert(c2);

			ce->next = (struct case_entry *) 0;
			c2->next = ce;
		}
		(sh->sh_nrofentries)++;
	}
	return 1;
}
