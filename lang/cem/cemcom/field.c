/* $Header$ */
/*	BITFIELD EXPRESSION EVALUATOR	*/

#include	"nobitfield.h"

#ifndef NOBITFIELD
#include	<em.h>
#include	"debug.h"
#include	"arith.h"
#include	"type.h"
#include	"idf.h"
#include	"label.h"
#include	"code.h"
#include	"assert.h"
#include	"expr.h"
#include	"sizes.h"
#include	"Lpars.h"
#include	"field.h"

arith tmp_pointer_var();	/* eval.c	*/
char *symbol2str();		/* symbol2str.c	*/

/*	Eval_field() evaluates expressions involving bit fields.
	The various instructions are not yet optimised in the expression
	tree and are therefore dealt with in this function.
	The actions taken at any operation are described clearly by the
	code for this actions.
	Notes
	[1]	the bitfields are packed in target machine integers!
	[2]	op is either an assignment operator or an increment/
		decrement operator
	[3]	atype: the type in which the bitfield arithmetic is done;
		and in which bitfields are stored!
*/
eval_field(expr, code)
	struct expr *expr;
	int code;
{
	int op = expr->OP_OPER;
	register struct expr *leftop = expr->OP_LEFT;
	register struct expr *rightop = expr->OP_RIGHT;
	register struct field *fd = leftop->ex_type->tp_field;
	struct type *tp = leftop->ex_type->tp_up;
	arith old_offset, tmpvar;
	struct type *atype = tp->tp_unsigned ? uword_type : word_type;
	arith asize = atype->tp_size;

	/* First some assertions to be sure that the rest is legal */
	ASSERT(asize == word_size);	/* make sure that C_loc() is legal */
	ASSERT(leftop->ex_type->tp_fund == FIELD);
	leftop->ex_type = atype;	/* this is cheating but it works... */
	if (op == '=') {
		/* F = E: f = ((E & mask)<<shift) | (~(mask<<shift) & f) */
		ASSERT(tp == rightop->ex_type);
		EVAL(rightop, RVAL, TRUE, NO_LABEL, NO_LABEL);
		conversion(tp, atype);
		C_loc(fd->fd_mask);
		C_and(asize);
		if (code == TRUE)
			C_dup(asize);
		C_loc((arith)fd->fd_shift);
		if (atype->tp_unsigned)
			C_slu(asize);
		else
			C_sli(asize);
		C_loc(~((fd->fd_mask << fd->fd_shift) | (~0 << (8 * asize))));
		if (leftop->ex_depth == 0)	{	/* simple case	*/
			load_val(leftop, RVAL);
			C_and(asize);
			C_ior(asize);
			store_val(&(leftop->ex_object.ex_value), atype);
		}
		else	{			/* complex case	*/
			tmpvar = tmp_pointer_var(&old_offset);
			EVAL(leftop, LVAL, TRUE, NO_LABEL, NO_LABEL);
			C_dup(pointer_size);
			C_lal(tmpvar);
			C_sti(pointer_size);
			C_loi(asize);
			C_and(asize);
			C_ior(asize);
			C_lal(tmpvar);
			C_loi(pointer_size);
			C_sti(asize);
			free_tmp_var(old_offset);
		}
	}
	else {		/* treat ++F as F += 1 and --F as F -= 1	*/
		/*	F op= e: f = (((((f>>shift)&mask) op e)&mask)<<shift)|
					(f&~(mask<<shift))
		*/
		if (leftop->ex_depth == 0)	/* simple case	*/
			load_val(leftop, RVAL);
		else	{			/* complex case	*/
			tmpvar = tmp_pointer_var(&old_offset);
			EVAL(leftop, LVAL, TRUE, NO_LABEL, NO_LABEL);
			C_dup(pointer_size);
			C_lal(tmpvar);
			C_sti(pointer_size);
			C_loi(asize);
		}
		C_loc((arith)fd->fd_shift);
		if (atype->tp_unsigned)
			C_sru(asize);
		else
			C_sri(asize);
		C_loc(fd->fd_mask);
		C_and(asize);
		if (code == TRUE && (op == POSTINCR || op == POSTDECR))
			C_dup(asize);
		conversion(atype, rightop->ex_type);
		EVAL(rightop, RVAL, TRUE, NO_LABEL, NO_LABEL);
		/* the 'op' operation: */
		if (op == PLUSPLUS || op == POSTINCR)
			assop(rightop->ex_type, PLUSAB);
		else
		if (op == MINMIN || op == POSTDECR)
			assop(rightop->ex_type, MINAB);
		else
			assop(rightop->ex_type, op);
		conversion(rightop->ex_type, atype);
		C_loc(fd->fd_mask);
		C_and(asize);
		if (code == TRUE && op != POSTINCR && op != POSTDECR)
			C_dup(asize);
		C_loc((arith)fd->fd_shift);
		if (atype->tp_unsigned)
			C_slu(asize);
		else
			C_sli(asize);
		C_loc(~((fd->fd_mask << fd->fd_shift) | (~0 << (8 * asize))));
		if (leftop->ex_depth == 0)	{
			load_val(leftop, RVAL);
			C_and(asize);
			C_ior(asize);
			store_val(&(leftop->ex_object.ex_value), atype);
		}
		else	{
			C_lal(tmpvar);
			C_loi(pointer_size);
			C_loi(asize);
			C_and(asize);
			C_ior(asize);
			C_lal(tmpvar);
			C_loi(pointer_size);
			C_sti(asize);
			free_tmp_var(old_offset);
		}
	}
	if (code == TRUE) {
		/*	Take care that the effective value stored in
			the bit field (i.e. the value that is got on
			retrieval) is on top of stack.
		*/
		if (atype->tp_unsigned == 0) {	/* sign extension */
			register arith shift = asize * 8 - fd->fd_width;

			C_loc(shift);
			C_sli(asize);
			C_loc(shift);
			C_sri(asize);
		}
		conversion(atype, expr->ex_type);
	}
}
#endif NOBITFIELD
