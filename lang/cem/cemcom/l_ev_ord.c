/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	Lint evaluation order checking	*/

#include	"lint.h"

#ifdef	LINT

#include	<alloc.h>	/* for st_free */
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
#include	"nofloat.h"
#include	"l_lint.h"
#include	"l_state.h"

extern char *symbol2str();

check_and_merge(espp, esp, com_oper)
	struct expr_state **espp, *esp;
{
/* Checks for undefined evaluation orders in case of a commutative operator.
 * In addition the sets of used and set variables of both expressions are
 * united.
 * *espp will be pointing to this new list. esp is used for this list.
 */
	register struct expr_state **pp, *p1, *p2;

	for (p1 = *espp; p1; p1 = p1->next) {
		p2 = esp;
		pp = &esp;
		while (p2) {
			if (	/* p1 and p2 are the same */
				p1->es_idf == p2->es_idf
			&&	p1->es_offset == p2->es_offset
			) {
				if (com_oper)
					check_ev_order(p1, p2, com_oper);
				p1->es_used |= p2->es_used;
				p1->es_set |= p2->es_set;
				*pp = p2->next;
				free_expr_state(p2);
				p2 = *pp;
			}
			else {
				pp = &p2->next;
				p2 = p2->next;
			}
		}
	}
	/* The rest of the list esp is pointing to, is put in front of the list
	 * *espp is now pointing to.
	 * *espp will be pointing to this new list.
	 */
	if (!esp)
		return;
	p1 = *espp;
	*espp = esp;
	while (esp->next)
		esp = esp->next;
	esp->next = p1;
}

check_ev_order(esp1, esp2, com_oper)
	struct expr_state *esp1, *esp2;
{
	if (	(esp1->es_used && esp2->es_set)
	||	(esp1->es_set && esp2->es_used)
	||	(esp1->es_set && esp2->es_set)
	) {
		warning("result of %s depends on evaluation order on %s",
			symbol2str(com_oper),
			esp1->es_idf->id_text);
	}
}

add_expr_state(value, to_state, espp)
	struct value value;
	struct expr_state **espp;
{
	register struct expr_state *esp = *espp;

	if (value.vl_class != Name) {
		crash("(add_expr_state) invalid vl_class");
		/*NOTREACHED*/
	}
	while (	esp
	&&	!(	esp->es_idf == value.vl_data.vl_idf
		&&	esp->es_offset == value.vl_value
		)
	) {
		esp = esp->next;
	}
	if (!esp) {	/* create new expr_state */
		esp = new_expr_state();
		esp->es_idf = value.vl_data.vl_idf;
		esp->es_offset = value.vl_value;
		esp->next = *espp;
		*espp = esp;
	}
	if (to_state == SET)
		esp->es_set = 1;
	else	/* USED */
		esp->es_used = 1;
}

#endif	LINT
