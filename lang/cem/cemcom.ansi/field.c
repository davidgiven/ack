/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	BITFIELD EXPRESSION EVALUATOR	*/

#include	"lint.h"
#ifndef	LINT

#include	"nobitfield.h"

#ifndef NOBITFIELD
#include	<em.h>
#include	<em_reg.h>
#include	"debug.h"
#include	<flt_arith.h>
#include	"arith.h"
#include	"type.h"
#include	"idf.h"
#include	"label.h"
#include	"code.h"
#include	"assert.h"
#include	"expr.h"
#include	"sizes.h"
#include	"align.h"
#include	"Lpars.h"
#include	"field.h"

arith NewLocal();		/* util.c	*/
extern arith full_mask[];	/* cstoper.c	*/

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
	arith tmpvar = 0;
	struct type *atype = ( tp->tp_unsigned
				&& fd->fd_width >= 8 * (int)word_size)
				    ? uword_type
				    : word_type;

	/* First some assertions to be sure that the rest is legal */
	ASSERT(atype->tp_size == word_size);	/* make sure that C_loc() is legal */
	ASSERT(leftop->ex_type->tp_fund == FIELD);
	leftop->ex_type = atype;	/* this is cheating but it works... */
	if (op == '=') {
		/* F = E: f = ((E & mask)<<shift) | (~(mask<<shift) & f) */
		ASSERT(tp == rightop->ex_type);
		EVAL(rightop, RVAL, TRUE, NO_LABEL, NO_LABEL);
		conversion(tp, atype);
		store_field(fd, tp->tp_unsigned, code, leftop, (arith) 0);
	}
	else {		/* treat ++F as F += 1 and --F as F -= 1	*/
		/*	F op= e: f = (((((f>>shift)&mask) op e)&mask)<<shift)|
					(f&~(mask<<shift))
		*/
		if (leftop->ex_depth == 0)	/* simple case	*/
			load_val(leftop, RVAL);
		else	{			/* complex case	*/
			tmpvar = NewLocal(pointer_size, pointer_align, 
					  reg_pointer, 0);
			EVAL(leftop, LVAL, TRUE, NO_LABEL, NO_LABEL);
			C_dup(pointer_size);
			StoreLocal(tmpvar, pointer_size);
			C_loi(word_size);
		}
		if (tp->tp_unsigned) {
			C_loc((arith)fd->fd_shift);
			C_sru(word_size);
			C_loc(fd->fd_mask);
			C_and(word_size);
		}
		else {
			arith sft = (int)word_size * 8 - fd->fd_width;
			C_loc(sft - fd->fd_shift);
			C_sli(word_size);
			C_loc(sft);
			C_sri(word_size);
		}
		if (code == TRUE && (op == POSTINCR || op == POSTDECR))
			C_dup(word_size);
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
		store_field(fd, atype->tp_unsigned, 
			    code == TRUE && op != POSTINCR && op != POSTDECR,
			    leftop, tmpvar);
	}
	if (code == TRUE) {
		/*	Take care that the effective value stored in
			the bit field (i.e. the value that is got on
			retrieval) is on top of stack.
		*/
		if (tp->tp_unsigned == 0) {	/* sign extension */
			register arith shift = (int)word_size * 8 - fd->fd_width;

			C_loc(shift);
			C_sli(word_size);
			C_loc(shift);
			C_sri(word_size);
		}
		conversion(atype, expr->ex_type);
	}
}

store_field(fd, uns, code, leftop, tmpvar)
	register struct field *fd;
	int uns;
	int code;
	register struct expr *leftop;
	arith tmpvar;
{
	C_loc(fd->fd_mask);
	C_and(word_size);
	if (code == TRUE)
		C_dup(word_size);
	C_loc((arith)fd->fd_shift);
	if (uns)
		C_slu(word_size);
	else
		C_sli(word_size);
	C_loc(~((fd->fd_mask << fd->fd_shift) | ~full_mask[(int)word_size]));
	if (leftop->ex_depth == 0)	{	/* simple case	*/
		load_val(leftop, RVAL);
		C_and(word_size);
		C_ior(word_size);
		store_val(&(leftop->EX_VALUE), uns ? uword_type : word_type);
	}
	else	{			/* complex case	*/
		if (! tmpvar) {
			tmpvar = NewLocal(pointer_size, pointer_align, 
				  reg_pointer, 0);
			EVAL(leftop, LVAL, TRUE, NO_LABEL, NO_LABEL);
			StoreLocal(tmpvar, pointer_size);
		}
		LoadLocal(tmpvar, pointer_size);
		C_loi(word_size);
		C_and(word_size);
		C_ior(word_size);
		LoadLocal(tmpvar, pointer_size);
		C_sti(word_size);
		FreeLocal(tmpvar);
	}
}
#endif /* NOBITFIELD */

#endif	/* LINT */

