/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	Lint miscellaneous routines	*/

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
#include	"l_state.h"

extern char *symbol2str();
extern struct type *func_type;

lint_conversion(oper, from_type, to_type)
	struct type *from_type, *to_type;
{
	register int from = from_type->tp_fund;
	register int to = to_type->tp_fund;

	switch (oper) {
	case RETURN:	/* not really an oper, but it works */
	case INT2INT:
	case '=':
	case PLUSAB:
	case MINAB:
	case TIMESAB:
	case DIVAB:
	case MODAB:
	case LEFTAB:
	case RIGHTAB:
	case ANDAB:
	case XORAB:
	case ORAB:
		if (	(from == LONG && to != LONG)
		||	(from == DOUBLE && to != DOUBLE)
		) {
			awarning("conversion from %s to %s may lose accuracy",
				symbol2str(from), symbol2str(to));
		}
	}
}

lint_ret_conv(from_type)
	struct type *from_type;
{
	lint_conversion(RETURN, from_type, func_type);
}

lint_ptr_conv(from, to)
	short from, to;
{
/* X -> X ok			-- this includes struct -> struct, of any size
 * X -> CHAR ok
 * DOUBLE -> X ok
 * FLOAT -> LONG -> INT -> SHORT  ok
 */
	if (from == to)
		return;

	if (to == CHAR)
		return;

	if (from == DOUBLE)
		return;

	switch (from) {
	case FLOAT:
		switch (to) {
		case LONG:
		case INT:
		case SHORT:
			return;
		}
		break;
	case LONG:
		switch (to) {
		case INT:
		case SHORT:
			return;
		}
		break;
	case INT:
		switch (to) {
		case SHORT:
			return;
		}
		break;
	}

	if (from == CHAR) {
		hwarning("pointer to char may not align correctly for a %s",
			symbol2str(to));
	}
	else {
		warning("pointer to %s may not align correctly for a %s",
			symbol2str(from), symbol2str(to));
	}
}

lint_relop(left, right, oper)
	struct expr *left, *right;
	int oper;	/* '<', '>', LESSEQ, GREATEREQ, EQUAL, NOTEQUAL */
{
	/* <unsigned> <relop> <neg-const|0> is doubtful */
	if (	left->ex_type->tp_unsigned
	&&	right->ex_class == Value
	&&	right->VL_CLASS == Const
	) {
		if (right->VL_VALUE < 0) {
			warning("unsigned compared to negative constant");
		}
		if (right->VL_VALUE == 0) {
			switch (oper) {
			case '<':
				warning("unsigned < 0 will always fail");
				break;

			case LESSEQ:
				warning("unsigned <= 0 is probably wrong");
				break;

			case GREATEREQ:
				warning("unsigned >= 0 will always succeed");
				break;
			}
		}
	}

	/* <char> <relop> <neg-const> is undefined */
	if (	left->ex_type->tp_fund == CHAR
	&&	right->ex_class == Value
	&&	right->VL_CLASS == Const
	&&	(right->VL_VALUE < 0 || right->VL_VALUE > 127)
	) {
		warning("character compared to negative constant");
	}
}

#endif	LINT
