/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* C A S E   S T A T E M E N T   C O D E   G E N E R A T I O N */

/* $Header$ */

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
#include	"chk_expr.h"
#include	"def.h"

#include	"density.h"

struct switch_hdr	{
	label sh_break;			/* label of statement after this one */
	label sh_default;		/* label of ELSE part, or 0 */
	int sh_nrofentries;		/* number of cases */
	t_type *sh_type;		/* type of case expression */
	arith sh_lowerbd;		/* lowest case label */
	arith sh_upperbd;		/* highest case label */
	struct case_entry *sh_entries;	/* the cases with their generated
					   labels
					*/
};

/* STATICALLOCDEF "switch_hdr" 5 */

struct case_entry	{
	struct case_entry *ce_next;	/* next in list */
	label ce_label;			/* generated label */
	arith ce_low, ce_up;		/* lower and upper bound of range */
};

/* STATICALLOCDEF "case_entry" 20 */

/* The constant DENSITY determines when CSA and when CSB instructions
   are generated. Reasonable values are: 2, 3, 4.
   On machines that have lots of address space and memory, higher values
   might also be reasonable. On these machines the density of jump tables
   may be lower.
*/

compact(nr, low, up)
	arith low, up;
{
	/*	Careful! up - low might not fit in an arith. And then,
		the test "up-low < 0" might also not work to detect this
		situation! Or is this just a bug in the M68020/M68000?
	*/
	arith diff = up - low;

	return (nr != 0 && diff >= 0 && fit(diff, (int) word_size) &&
		diff / nr <= (DENSITY - 1));
}
#define nd_lab	nd_symb

int
CaseCode(nd, exitlabel, end_reached)
	t_node *nd;
	label exitlabel;
{
	/*	Check the expression, stack a new case header and
		fill in the necessary fields.
		"exitlabel" is the exit-label of the closest enclosing
		LOOP-statement, or 0.
	*/
	register struct switch_hdr *sh = new_switch_hdr();
	register t_node *pnode = nd;
	register struct case_entry *ce;
	register arith val;
	label CaseDescrLab;
	int rval;

	assert(pnode->nd_class == Stat && pnode->nd_symb == CASE);

	if (ChkExpression(&(pnode->nd_LEFT))) {
		MkCoercion(&(pnode->nd_LEFT),BaseType(pnode->nd_LEFT->nd_type));
		CodePExpr(pnode->nd_LEFT);
	}
	sh->sh_type = pnode->nd_LEFT->nd_type;
	sh->sh_break = ++text_label;

	/* Now, create case label list
	*/
	while (pnode = pnode->nd_RIGHT) {
		if (pnode->nd_class == Link && pnode->nd_symb == '|') {
			if (pnode->nd_LEFT) {
				/* non-empty case
				*/
				pnode->nd_LEFT->nd_lab = ++text_label;
				AddCases(sh, /* to descriptor */
					 pnode->nd_LEFT->nd_LEFT,
					     /* of case labels */
					 (label) pnode->nd_LEFT->nd_lab
					     /* and code label */
					      );
			}
		}
		else {
			/* Else part
			*/

			sh->sh_default = ++text_label;
			break;
		}
	}

	if (!sh->sh_nrofentries) {
		/* There were no cases, so we have to check the case-expression
		   here
		*/
		if (! (sh->sh_type->tp_fund & T_DISCRETE)) {
			node_error(nd, "illegal type in CASE-expression");
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
		int gen = 1;

		ce = sh->sh_entries;
		while (! ce->ce_label) ce = ce->ce_next;
		C_rom_cst((arith) 0);
		C_rom_cst(sh->sh_upperbd - sh->sh_lowerbd);
		for (val = sh->sh_lowerbd; val <= sh->sh_upperbd; val++) {
			assert(ce);
			if (gen || val == ce->ce_low) {
				gen = 1;
				C_rom_ilb(ce->ce_label);
				if (val == ce->ce_up) {
					gen = 0;
					ce = ce->ce_next;
					while (ce && ! ce->ce_label) ce = ce->ce_next;
				}
			}
			else if (sh->sh_default) C_rom_ilb(sh->sh_default);
			else C_rom_ucon("0", pointer_size);
		}
		C_loc(sh->sh_lowerbd);
		C_sbu(word_size);
		c_lae_dlb(CaseDescrLab);	/* perform the switch */
		C_csa(word_size);
	}
	else	{ 
		/* CSB
		*/
		C_rom_cst((arith)sh->sh_nrofentries);
		for (ce = sh->sh_entries; ce; ce = ce->ce_next)	{
			/* generate the entries: value + prog.label
			*/
			if (! ce->ce_label) continue;
			val = ce->ce_low;
			do {
				C_rom_cst(val);
				C_rom_ilb(ce->ce_label);
			} while (val++ != ce->ce_up);
		}
		c_lae_dlb(CaseDescrLab);	/* perform the switch */
		C_csb(word_size);
	}

	/* Now generate code for the cases
	*/
	pnode = nd;
	rval = 0;
	while (pnode = pnode->nd_RIGHT) {
		if (pnode->nd_class == Link && pnode->nd_symb == '|') {
			if (pnode->nd_LEFT) {
				rval |= LblWalkNode((label) pnode->nd_LEFT->nd_lab,
					    pnode->nd_LEFT->nd_RIGHT,
					    exitlabel, end_reached);
				c_bra(sh->sh_break);
			}
		}
		else {
			/* Else part
			*/
			assert(sh->sh_default != 0);

			rval |= LblWalkNode(sh->sh_default,
					pnode, exitlabel, end_reached);
			break;
		}
	}

	def_ilb(sh->sh_break);
	FreeSh(sh);
	return rval;
}

FreeSh(sh)
	register struct switch_hdr *sh;
{
	/*	 free the allocated switch structure	
	*/
	register struct case_entry *ce;

	ce = sh->sh_entries;
	while (ce)	{
		struct case_entry *tmp = ce->ce_next;

		free_case_entry(ce);
		ce = tmp;
	}

	free_switch_hdr(sh);
}

AddCases(sh, node, lbl)
	struct switch_hdr *sh;
	register t_node *node;
	label lbl;
{
	/*	Add case labels to the case label list
	*/

	if (node->nd_class == Link) {
		if (node->nd_symb == UPTO) {
			assert(node->nd_LEFT->nd_class == Value);
			assert(node->nd_RIGHT->nd_class == Value);

			AddOneCase(sh, node->nd_LEFT, node->nd_RIGHT, lbl);
			return;
		}

		assert(node->nd_symb == ',');
		AddCases(sh, node->nd_LEFT, lbl);
		AddCases(sh, node->nd_RIGHT, lbl);
		return;
	}

	assert(node->nd_class == Value);
	AddOneCase(sh, node, node, lbl);
}

AddOneCase(sh, lnode, rnode, lbl)
	register struct switch_hdr *sh;
	t_node *lnode, *rnode;
	label lbl;
{
	register struct case_entry *ce = new_case_entry();
	register struct case_entry *c1 = sh->sh_entries, *c2 = 0;
	int fund = sh->sh_type->tp_fund;
	arith diff;

	if (! ChkCompat(&lnode, sh->sh_type, "case") ||
	    ! ChkCompat(&rnode, sh->sh_type, "case")) {
	}
	ce->ce_label = lbl;
	ce->ce_low = lnode->nd_INT;
	ce->ce_up = rnode->nd_INT;
	diff = rnode->nd_INT - lnode->nd_INT;
#define MAXRANGE	100
	if (diff < 0 || diff > MAXRANGE) {
		/* This is a bit of a hack, but it prevents the compiler
		   from crashing on things like
		   CASE a OF
		     10 .. MAX(CARDINAL): ....
		   
		   If the range covers more than MAXRANGE cases, this case
		   is dealt with separately.
		*/
		label cont = ++text_label;

		C_dup(int_size);
		C_loc(lnode->nd_INT);
		if (fund == T_INTEGER) {
			C_blt(cont);
		}
		else {
			C_cmu(int_size);
			C_zlt(cont);
		}
		C_dup(int_size);
		C_loc(rnode->nd_INT);
		if (fund == T_INTEGER) {
			C_bgt(cont);
		}
		else {
			C_cmu(int_size);
			C_zgt(cont);
		}
		C_asp(int_size);
		c_bra(lbl);
		C_df_ilb(cont);
		ce->ce_label = 0;
	}
	if (sh->sh_entries == 0)	{
		/* first case entry
		*/
		sh->sh_entries = ce;
		if (ce->ce_label) {
			sh->sh_lowerbd = ce->ce_low;
			sh->sh_upperbd = ce->ce_up;
		}
	}
	else	{
		/* second etc. case entry
		   find the proper place to put ce into the list
		*/
		
		while (c1 && chk_bounds(c1->ce_low, ce->ce_low, fund)) {
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
		if (c2) {
			if ( chk_bounds(ce->ce_low, c2->ce_up, fund)) {
node_error(rnode, "multiple case entry for value %ld", (long)(ce->ce_low));
				free_case_entry(ce);
				return;
			}
		}
		if (c1)	{
			if ( chk_bounds(c1->ce_low, ce->ce_up, fund)) {
node_error(rnode, "multiple case entry for value %ld", (long)(ce->ce_up));
				free_case_entry(ce);
				return;
			}
			if (c2)	{
				ce->ce_next = c2->ce_next;
				c2->ce_next = ce;
			}
			else	{
				ce->ce_next = sh->sh_entries;
				sh->sh_entries = ce;
			}
		}
		else	{
			assert(c2);

			c2->ce_next = ce;
		}
		if (ce->ce_label) {
			if (! chk_bounds(sh->sh_lowerbd, ce->ce_low, fund)) {
				sh->sh_lowerbd = ce->ce_low;
			}
			if (! chk_bounds(ce->ce_up, sh->sh_upperbd, fund)) {
				sh->sh_upperbd = ce->ce_up;
			}
		}
	}
	if (ce->ce_label) sh->sh_nrofentries += ce->ce_up - ce->ce_low + 1;
}
