/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	Lint miscellaneous routines	*/

#include	"lint.h"

#ifdef	LINT

#include	<alloc.h>	/* for st_free */
#include	"interface.h"
#ifdef ANSI
#include	<flt_arith.h>
#endif /* ANSI */
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
#include	"l_state.h"

extern char *symbol2str();
extern struct type *func_type;

PRIVATE lint_enum_arith();
PRIVATE lint_conversion();
PRIVATE int numsize();

check_hiding(idf, lvl, sc)
	struct idf *idf;
	int lvl;
	int sc;
{
	/*	Checks if there is already a definition for this non-extern
		name on a more global level.
	*/
	struct def *def = idf->id_def;
	
	if (	def && def->df_level < lvl
	&&	! (	lvl == L_FORMAL2
		||	def->df_level == L_UNIVERSAL
		||	sc == GLOBAL
		||	sc == EXTERN
		)
	) {
		warning("%s is already defined as a %s",
			idf->id_text,
			def->df_level == L_GLOBAL ? "global" :
			def->df_level == L_FORMAL2 ? "formal" :
				"more global local"
		);
	}
}

lint_new_oper(expr)
	struct expr *expr;
{
	/*	Does additional checking on a newly constructed expr node
		of class Oper.

		Some code in this routine could be contracted, but since
		I am not sure we have covered the entire ground, we'll
		leave the contracting for some rainy day.
	*/
	register struct expr *left = expr->OP_LEFT;
	register struct expr *right = expr->OP_RIGHT;
	register int oper = expr->OP_OPER;
	register int l_fund =
		left == 0 ? 0 :			/* for monadics */
		left->ex_type->tp_fund;
	register int r_fund =
		right == 0 ? 0 :		/* for ( without parameters */
		right->ex_type->tp_fund;

	/*	In ch7.c, in ch7asgn(), a combined operator/assignment
		is hammered into correctness by repeated application of
		ch7bin(), which calls new_oper(), which calls lint_new_oper().
		These spurious calls understandably cause spurious error
		messages, which we don't like.  So we try to suppress these
		wierd calls here.  This refers to the code marked
			this is really $#@&*%$# !
		in ch7asgn().
	*/
	switch (oper) {
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
		/* is the left operand wierd? */
		if (	left->ex_class == Value
		&&	left->VL_CLASS == Const
		&&	left->VL_VALUE == 0
		) {
			return;
		}
	}

	switch (oper) {
	case '=':
		lint_conversion(right, l_fund);
		break;

	case PLUSAB:
		lint_conversion(right, l_fund);
	case '+':
		lint_enum_arith(l_fund, oper, r_fund);
		break;

	case MINAB:
		lint_conversion(right, l_fund);
	case '-':
		if (left == 0) {
			/* unary */
			if (r_fund == ENUM)
				warning("negating an enum");
		}
		else {
			/* binary */
			if (l_fund == ENUM && r_fund == ENUM) {
				if (left->ex_type != right->ex_type)
					warning("subtracting enums of different type");
				/* update the type, cem does not do it */
				expr->ex_type = int_type;
			}
			lint_enum_arith(l_fund, oper, r_fund);
		}
		break;

	case TIMESAB:
		lint_conversion(right, l_fund);
	case '*':
		if (left == 0) {
			/* unary */
		}
		else {
			/* binary */
			if (l_fund == ENUM || r_fund == ENUM)
				warning("multiplying enum");
		}
		break;

	case DIVAB:
		lint_conversion(right, l_fund);
	case '/':
		if (l_fund == ENUM || r_fund == ENUM)
			warning("division on enum");
		break;

	case MODAB:
		lint_conversion(right, l_fund);
	case '%':
		if (l_fund == ENUM || r_fund == ENUM)
			warning("modulo on enum");
		break;

	case '~':
		if (r_fund == ENUM || r_fund == FLOAT || r_fund == DOUBLE)
			warning("~ on %s", symbol2str(r_fund));
		break;

	case '!':
		if (r_fund == ENUM)
			warning("! on enum");
		break;

	case INT2INT:
	case INT2FLOAT:
	case FLOAT2INT:
	case FLOAT2FLOAT:
		lint_conversion(right, l_fund);
		break;

	case '<':
	case '>':
	case LESSEQ:
	case GREATEREQ:
	case EQUAL:
	case NOTEQUAL:
		if (	(l_fund == ENUM || r_fund == ENUM)
		&&	left->ex_type != right->ex_type
		) {
			warning("comparing enum with non-enum");
		}
		lint_relop(left, right, oper);
		lint_relop(right, left, 
			oper == '<' ? '>' :
			oper == '>' ? '<' :
			oper == LESSEQ ? GREATEREQ :
			oper == GREATEREQ ? LESSEQ :
			oper
		);
		break;

	case LEFTAB:
	case RIGHTAB:
		lint_conversion(right, l_fund);
	case LEFT:
	case RIGHT:
		if (l_fund == ENUM || r_fund == ENUM)
			warning("shift on enum");
		break;

	case ANDAB:
	case ORAB:
	case XORAB:
		lint_conversion(right, l_fund);
	case '&':
	case '|':
	case '^':
		if (l_fund == ENUM || r_fund == ENUM)
			warning("bit operations on enum");
		break;

	case ',':
	case '?':
	case ':':
	case AND:
	case OR:
	case POSTINCR:
	case POSTDECR:
	case PLUSPLUS:
	case MINMIN:
	case '(':
	case '.':
	case ARROW:
	default:
		/* OK with lint */
		break;
	}
}

PRIVATE
lint_enum_arith(l_fund, oper, r_fund)
	int l_fund, oper, r_fund;
{
	if (	l_fund == ENUM
	&&	r_fund != CHAR
	&&	r_fund != SHORT
	&&	r_fund != INT
	) {
		warning("%s on enum and %s",
			symbol2str(oper), symbol2str(r_fund));
	}
	else
	if (	r_fund == ENUM
	&&	l_fund != CHAR
	&&	l_fund != SHORT
	&&	l_fund != INT
	) {
		warning("%s on %s and enum",
			symbol2str(oper), symbol2str(l_fund));
	}
}

PRIVATE
lint_conversion(from_expr, to_fund)
	struct expr *from_expr;
	int to_fund;
{
	register int from_fund = from_expr->ex_type->tp_fund;

	/*	was there an attempt to reduce the type of the from_expr
		of the form
			expr & 0377
		or something like this?
	*/
	if (from_expr->ex_class == Oper && from_expr->OP_OPER == INT2INT) {
		from_expr = from_expr->OP_LEFT;
	}
	if (from_expr->ex_class == Oper && from_expr->OP_OPER == '&') {
		struct expr *bits =
			is_cp_cst(from_expr->OP_LEFT) ? from_expr->OP_LEFT :
			is_cp_cst(from_expr->OP_RIGHT) ? from_expr->OP_RIGHT :
			0;

		if (bits) {
			arith val = bits->VL_VALUE;

			if (val < 256)
				from_fund = CHAR;
			else if (val < 256)
				from_fund = SHORT;
		}
	}
	if (numsize(from_fund) > numsize(to_fund)) {
		awarning("conversion from %s to %s may lose accuracy",
			symbol2str(from_fund), symbol2str(to_fund));
	}
}

PRIVATE int
numsize(fund)
{
	switch (fund) {
	case CHAR:	return 1;
	case SHORT:	return 2;
	case INT:	return 3;
	case ENUM:	return 3;
	case LONG:	return 4;
	case FLOAT:	return 5;
	case DOUBLE:	return 6;
	default:	return 0;
	}
}

lint_ret_conv(from_expr)
	struct expr *from_expr;
{
	lint_conversion(from_expr, func_type->tp_fund);
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

	if (from == VOID) {
		/* OK any which way */
	}
	else
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
	/* left operand may be converted */
	if (	left->ex_class == Oper
	&&	left->OP_OPER == INT2INT
	) {
		left = left->OP_RIGHT;
	}

	/* <unsigned> <relop> <neg-const|0> is doubtful */
	if (	left->ex_type->tp_unsigned
	&&	right->ex_class == Value
	&&	right->VL_CLASS == Const
	) {
		if (!right->ex_type->tp_unsigned && right->VL_VALUE < 0) {
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

#endif	/* LINT */
