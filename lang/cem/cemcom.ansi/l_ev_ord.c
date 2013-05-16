/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	Lint evaluation order checking	*/

#include	"parameters.h"

#ifdef	LINT

#include	<alloc.h>	/* for st_free */
#include	"interface.h"
#include	"assert.h"
#ifdef ANSI
#include	<flt_arith.h>
#endif /* ANSI */
#include	"arith.h"	/* definition arith */
#include	"label.h"	/* definition label */
#include	"expr.h"
#include	"def.h"
#include	"code.h"	/* RVAL etc */
#include	"LLlex.h"
#include	"Lpars.h"
#include	"stack.h"
#include	"type.h"
#include	"level.h"
#include	"l_lint.h"
#include	"l_state.h"

extern char *symbol2str();

PRIVATE check_ev_order();

check_and_merge(expr, espp, esp)
	struct expr *expr;
	struct expr_state **espp, *esp;
{
/* Checks for undefined evaluation orders in case of a non-sequencing operator.
 * In addition the sets of used and set variables of both expressions are
 * united.
 * *espp will be pointing to this new list. esp is used for this list.
 */
	register struct expr_state **pp, *p1, *p2;
	int oper = expr->OP_OPER;
	int is_sequencer =
		(oper == '?' || oper == OR || oper == AND || oper ==',');

	for (p1 = *espp; p1; p1 = p1->next) {
		/* scan the list esp for the same variable */
		p2 = esp;
		pp = &esp;
		while (p2) {
			if (	/* p1 and p2 refer to the same location */
				p1->es_idf == p2->es_idf
			&&	p1->es_offset == p2->es_offset
			) {
				/* check */
				if (!is_sequencer)
					check_ev_order(p1, p2, expr);

				/* merge the info */
				p1->es_used |= p2->es_used;
				p1->es_referred |= p2->es_referred;
				p1->es_set |= p2->es_set;

				/* and remove the entry from esp */
				*pp = p2->next;
				free_expr_state(p2);
				p2 = *pp;
			}
			else {
				/* skip over the entry in esp */
				pp = &p2->next;
				p2 = p2->next;
			}
		}
	}
	/*	If there is anything left in the list esp, this is put in
		front of the list *espp is now pointing to, and *espp will be
		left pointing to this new list.
	*/
	if (!esp)
		return;
	p1 = *espp;
	*espp = esp;
	while (esp->next)
		esp = esp->next;
	esp->next = p1;
}

PRIVATE
check_ev_order(esp1, esp2, expr)
	struct expr_state *esp1, *esp2;
	struct expr *expr;
{
	if (	(esp1->es_used && esp2->es_set)
	||	(esp1->es_set && esp2->es_used)
	||	(esp1->es_set && esp2->es_set)
	) {
		expr_warning(expr,
			"result of %s depends on evaluation order on %s",
			symbol2str(expr->OP_OPER),
			esp1->es_idf->id_text);
	}
}

#endif	/* LINT */
