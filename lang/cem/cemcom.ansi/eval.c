/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* EXPRESSION-CODE GENERATOR */

#include	"lint.h"
#ifndef	LINT

#include	<em.h>
#include	<em_reg.h>
#include	<alloc.h>
#include	"debug.h"
#include	"nobitfield.h"
#include	"dataflow.h"
#include	<flt_arith.h>
#include	"arith.h"
#include	"type.h"
#include	"idf.h"
#include	"label.h"
#include	"code.h"
#include	"assert.h"
#include	"def.h"
#include	"expr.h"
#include	"sizes.h"
#include	"Lpars.h"
#include	"level.h"
#include	"stack.h"
#include	"align.h"
#include	"mes.h"
#include	"atw.h"
#include	"specials.h"

#define	CRASH()		crash("EVAL: CRASH at line %u", __LINE__)

char *symbol2str();
char *long2str();
arith NewLocal();	/* util.c */
#define LocalPtrVar()	NewLocal(pointer_size, pointer_align, reg_pointer, REGISTER)
extern int	err_occurred; /* error.c */

/*	EVAL() is the main expression-tree evaluator, which turns
	any legal expression tree into EM code. Parameters:

	struct expr *expr
		pointer to root of the expression tree to be evaluated

	int val
		indicates whether the resulting expression is to be
		dereferenced (if val == RVAL and expr->ex_lvalue == 1)
		or not (val == LVAL).  The latter case indicates that
		the resulting expression is an lvalue expression which
		should not be dereferenced by EVAL
	
	int code
		indicates whether the expression tree must be turned
		into EM code or not. E.g. the expression statement "12;"
		delivers the expression "12" to EVAL while this should
		not result in any EM code
	
	label false_label, label true_label
		if the expression is a logical or relational expression
		and if the loop of the program depends on the resulting
		value then EVAL generates jumps to the specified program
		labels, in case they are specified (i.e. are non-zero)
*/

EVAL(expr, val, code, true_label, false_label)
	register struct expr *expr;
	int val, code;
	label true_label, false_label;
{
	int vol = (code != TRUE && recurqual(expr->ex_type, TQ_VOLATILE));
	register int gencode = code == TRUE;

	if (err_occurred) return;
	switch (expr->ex_class) {
	case Value:	/* just a simple value	*/
		if (gencode) {
			if (true_label) {
				/* can only result from ','-expressions with
				   constant right-hand sides ???
				*/
				ASSERT(is_cp_cst(expr));
				C_bra(expr->VL_VALUE == 0 ? false_label : true_label);
			}
			else load_val(expr, val);
		}
		else if (vol) {
			load_val(expr, val);
			C_asp(ATW(expr->ex_type->tp_size));
		}
		break;
	case String:	/* a string constant	*/
		if (gencode) {
			string2pointer(expr);
			C_lae_dlb(expr->VL_LBL, expr->VL_VALUE);
		}
		break;
	case Float:	/* a floating constant	*/
		if (gencode) {
			label datlab = data_label();
			char buf[FLT_STRLEN];
			
			fp_used = 1;
			flt_flt2str(&(expr->FL_ARITH), buf, FLT_STRLEN);
			C_df_dlb(datlab);
			C_rom_fcon(buf, expr->ex_type->tp_size);
			C_lae_dlb(datlab, (arith)0);
			C_loi(expr->ex_type->tp_size);
		}
		break;
	case Oper:	/* compound expression	*/
	{
		int oper = expr->OP_OPER;
		register struct expr *left = expr->OP_LEFT;
		register struct expr *right = expr->OP_RIGHT;
		register struct type *tp = expr->OP_TYPE;

		switch (oper) {
		case '+':
			/*	We have the following possibilities :
				int + int, pointer + int, pointer + long,
				long + long, double + double
			*/
			operands(expr, gencode);
			if (gencode) {
				switch (tp->tp_fund) {
				case INT:
				case LONG:
					if (tp->tp_unsigned)
						C_adu(tp->tp_size);
					else
						C_adi(tp->tp_size);
					break;
				case POINTER:
					ptr_add(right->ex_type->tp_size);
					break;
				case FLOAT:
				case DOUBLE:
				case LNGDBL:
					C_adf(tp->tp_size);
					break;
				default:
					crash("bad type +");
				}
			}
			break;
		case '-':
			if (left == 0) {	/* unary	*/
				EVAL(right, RVAL, gencode, NO_LABEL, NO_LABEL);
				if (gencode) {
					switch (tp->tp_fund) {
					case INT:
					case LONG:
					case POINTER:
						C_ngi(tp->tp_size);
						break;
					case FLOAT:
					case DOUBLE:
					case LNGDBL:
						C_ngf(tp->tp_size);
						break;
					default:
						CRASH();
					}
				}
				break;
			}
			/*	else binary; we have the following flavours:
				int - int, pointer - int, pointer - long,
				pointer - pointer, long - long, double - double
			*/
			operands(expr, gencode);
			if (!gencode)
				break;
			switch (tp->tp_fund) {
			case INT:
			case LONG:
				if (tp->tp_unsigned)
					C_sbu(tp->tp_size);
				else
					C_sbi(tp->tp_size);
				break;
			case POINTER:
				if (right->ex_type->tp_fund == POINTER)
					C_sbs(pointer_size);
				else {
					C_ngi(right->ex_type->tp_size);
					ptr_add(right->ex_type->tp_size);
				}
				break;
			case FLOAT:
			case DOUBLE:
			case LNGDBL:
				C_sbf(tp->tp_size);
				break;
			default:
				crash("bad type -");
			}
			break;
		case '*':
			if (left == 0) { /* unary */
				EVAL(right, RVAL, gencode, NO_LABEL, NO_LABEL);
				if (gencode && right->ex_class == String) {
					C_loi((arith)1);
				}
				break;
			}
			operands(expr, gencode);
			if (gencode) {
				switch (tp->tp_fund) {
				case INT:
				case LONG:
				case POINTER:
					if (tp->tp_unsigned)
						C_mlu(tp->tp_size);
					else
						C_mli(tp->tp_size);
					break;
				case FLOAT:
				case DOUBLE:
				case LNGDBL:
					C_mlf(tp->tp_size);
					break;
				default:
					crash("bad type *");
				}
			}
			break;
		case '/':
			operands(expr, gencode);
			if (gencode)
				switch (tp->tp_fund) {
				case INT:
				case LONG:
				case POINTER:
					if (tp->tp_unsigned)
						C_dvu(tp->tp_size);
					else
						C_dvi(tp->tp_size);
					break;
				case FLOAT:
				case DOUBLE:
				case LNGDBL:
					/*C_dvf(double_size);*/
					C_dvf(tp->tp_size);
					break;
				default:
					crash("bad type /");
				}
			break;
		case '%':
			operands(expr, gencode);
			ASSERT(tp->tp_fund==INT || tp->tp_fund==LONG);
			if (gencode)
				if (tp->tp_unsigned)
					C_rmu(tp->tp_size);
				else
					C_rmi(tp->tp_size);
			break;
		case LEFT:
			operands(expr, gencode);
			if (gencode)
				if (tp->tp_unsigned)
					C_slu(tp->tp_size);
				else
					C_sli(tp->tp_size);
			break;
		case RIGHT:
			operands(expr, gencode);
			if (gencode)
				if (tp->tp_unsigned)
					C_sru(tp->tp_size);
				else
					C_sri(tp->tp_size);
			break;
		case '<':
		case LESSEQ:
		case '>':
		case GREATEREQ:
		case EQUAL:
		case NOTEQUAL:
			operands(expr, gencode);
			if (gencode) {
				/* The operands have the same type */
				arith size = left->ex_type->tp_size;
				
				switch (tp->tp_fund) {
				case INT:
				case LONG:
					if (left->ex_type->tp_unsigned)
						C_cmu(size);
					else
						C_cmi(size);
					break;
				case FLOAT:
				case DOUBLE:
				case LNGDBL:
					C_cmf(size);
					break;
				case POINTER:
					C_cmp();
					break;
				case ENUM:
					C_cmi(size);
					break;
				default:
					CRASH();
				}
				if (true_label != 0) {
					compare(oper, true_label);
					C_bra(false_label);
				}
				else {
					truthvalue(oper);
				}
			}
			break;
		case '&':
		case '|':
		case '^':
			/* both operands should have type int	*/
			operands(expr, gencode);
			if (gencode) {
				arith size = tp->tp_size;

				if ((int)size < (int)word_size)
					size = word_size;
				switch (oper) {
				case '&':
					C_and(size);
					break;
				case '|':
					C_ior(size);
					break;
				case '^':
					C_xor(size);
					break;
				}
			}
			break;
		case '=':
#ifndef NOBITFIELD
			if (left->ex_type->tp_fund == FIELD) {
				eval_field(expr, gencode);
				break;
			}
#endif /* NOBITFIELD */
			if (is_struct_or_union(tp->tp_fund) && ! gencode) {
				EVAL(right, LVAL, TRUE, NO_LABEL, NO_LABEL);
				EVAL(left, LVAL, TRUE, NO_LABEL, NO_LABEL);
				copy_block(tp->tp_size, tp->tp_align);
				break;
			}
			EVAL(right, RVAL, TRUE, NO_LABEL, NO_LABEL);
			if (gencode && val == RVAL)
				C_dup(ATW(tp->tp_size));
			if (left->ex_class != Value) {
				EVAL(left, LVAL, TRUE, NO_LABEL, NO_LABEL);
				if (gencode && val == LVAL) {
					arith tmp = LocalPtrVar();
					C_dup(pointer_size);
					StoreLocal(tmp, pointer_size);
					store_block(tp->tp_size, tp->tp_align);
					LoadLocal(tmp, pointer_size);
					FreeLocal(tmp);
				}
				else store_block(tp->tp_size, tp->tp_align);
			}
			else {
				store_val(&(left->EX_VALUE), left->ex_type);
				if (gencode && val == LVAL) {
					EVAL(left, LVAL, TRUE, NO_LABEL, NO_LABEL);
				}
			}
			break;
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
		case POSTINCR:
		case POSTDECR:
		case PLUSPLUS:
		case MINMIN:
		{
			arith tmp = 0;
			int compl;	/* Complexity of left operand */
			int right_done = 0;
			int dupval;
#ifndef NOBITFIELD
			if (left->ex_type->tp_fund == FIELD) {
				eval_field(expr, gencode);
				break;
			}
#endif /* NOBITFIELD */
			if (left->ex_class == Value) {
				compl = 0; /* Value */
			}
			else if (left->ex_depth == 1 &&
			    !(left->ex_flags & EX_SIDEEFFECTS))	{
				compl = 1;
			}
			else 	compl = 2;

			/* evaluate right-hand side first when possible,
			   but not for POSTINCR or PLUSPLUS, because then
			   we might miss a chance for increment instructions.
			*/
			if (compl != 2 &&
			    tp->tp_fund != POINTER &&
			    (oper == PLUSAB || oper == TIMESAB ||
			     oper == ANDAB || oper == XORAB || oper == ORAB)) {
				right_done = 1;
				EVAL(right, RVAL, TRUE, NO_LABEL, NO_LABEL);
			}
			if (compl == 0) {
				load_val(left, RVAL);
			}
			else
			if (compl == 1) {
				EVAL(left, RVAL, TRUE, NO_LABEL, NO_LABEL);
			}
			else {
				EVAL(left, LVAL, TRUE, NO_LABEL, NO_LABEL);
				tmp = LocalPtrVar();
				C_dup(pointer_size);
				StoreLocal(tmp, pointer_size);
				C_loi(left->ex_type->tp_size);
			}
			if (gencode && (oper == POSTINCR ||
					oper == POSTDECR))
				C_dup(ATW(left->ex_type->tp_size));
			conversion(left->ex_type, tp);
			if (! right_done) {
				EVAL(right, RVAL, TRUE, NO_LABEL, NO_LABEL);
			}
			dupval = gencode && oper != POSTINCR &&
					oper != POSTDECR;
			assop(tp, oper);
			conversion(tp, left->ex_type);
			if (compl == 0) {
				store_val(&(left->EX_VALUE),
					left->ex_type);
				if (dupval) load_val(left, RVAL);
			}
			else if (compl == 1) {
				EVAL(left, LVAL, TRUE, NO_LABEL, NO_LABEL);
				C_sti(left->ex_type->tp_size);
				if (dupval) {
					EVAL(left, LVAL, TRUE, NO_LABEL, NO_LABEL);
					C_loi(left->ex_type->tp_size);
				}
			}
			else {
				LoadLocal(tmp, pointer_size);
				C_sti(left->ex_type->tp_size);
				if (dupval) {
					LoadLocal(tmp, pointer_size);
					C_loi(left->ex_type->tp_size);
				}
				FreeLocal(tmp);
			}
			break;
		}
		case '(':
		{
			register struct expr *ex;
			arith ParSize = (arith)0;
			label setjmp_label = 0;
			arith retspace = 0;

			if (ISNAME(left)) {
				if (left->VL_IDF->id_special == SP_SETJMP) {
					label addr_label = data_label();

					setjmp_label = text_label();
					C_df_dlb(addr_label);
					C_rom_ilb(setjmp_label);
					C_lae_dlb(addr_label, (arith) 0);
					C_loi(pointer_size);
					ParSize += pointer_size;
				}
			}
			if ((ex = right) != NILEXPR) {
				/* function call with parameters*/
				while (	ex->ex_class == Oper &&
					ex->OP_OPER == PARCOMMA
				) {
					EVAL(ex->OP_RIGHT, RVAL, TRUE,
							NO_LABEL, NO_LABEL);
					ParSize += ATW(ex->OP_RIGHT->ex_type->tp_size);
					ex = ex->OP_LEFT;
				}
				EVAL(ex, RVAL, TRUE, NO_LABEL, NO_LABEL);
				ParSize += ATW(ex->ex_type->tp_size);
			}
			if (is_struct_or_union(tp->tp_fund)) {
				retspace = NewLocal(tp->tp_size, tp->tp_align,
						    -1, 0);
				C_lal(retspace);
				ParSize += pointer_size;
			}
			if (ISNAME(left)) {
				/* e.g., main() { (*((int (*)())0))(); } */
				C_cal(left->VL_IDF->id_text);
				if (setjmp_label) {
					C_df_ilb(setjmp_label);
				}
#ifdef	DATAFLOW
				{	extern char options[];
					if (options['d'])
						DfaCallFunction(
							left->VL_IDF->id_text);
				}
#endif	/* DATAFLOW */
			}
			else {
				EVAL(left, LVAL, TRUE, NO_LABEL, NO_LABEL);
				C_cai();
			}
			/* remove parameters from stack	*/
			if (ParSize > (arith)0)
				C_asp(ParSize);
			if (gencode) {
				if (is_struct_or_union(tp->tp_fund)) {
					C_lfr(pointer_size);
					if (val == RVAL) {
					  load_block(tp->tp_size, (int) word_size);
					  FreeLocal(retspace);
					}
				}
				else
					C_lfr(ATW(tp->tp_size));
			}
			if (tp->tp_fund == FLOAT || tp->tp_fund == DOUBLE
			    || tp->tp_fund == LNGDBL)
				fp_used = 1;
			/* ??? set filename and line number ??? */
			break;
		}
		case '.':
		case ARROW:
			if (tp->tp_fund == FLOAT || tp->tp_fund == DOUBLE
			    || tp->tp_fund == LNGDBL)
				fp_used = 1;
			EVAL(left, oper == '.' ? LVAL : RVAL, gencode,
				NO_LABEL, NO_LABEL);
			ASSERT(is_cp_cst(right));
			if (gencode) {
				C_adp(right->VL_VALUE);
			}
			break;
		case ',':
			EVAL(left, RVAL, FALSE, NO_LABEL, NO_LABEL);
			EVAL(right, val, gencode, true_label, false_label);
			break;
		case '~':
			EVAL(right, RVAL, gencode, NO_LABEL, NO_LABEL);
			if (gencode)
				C_com(tp->tp_size);
			break;
		case '?':	/* must be followed by ':'	*/
		{
			label l_true = text_label();
			label l_false = text_label();
			label l_end = text_label();

			EVAL(left, RVAL, TRUE, l_true, l_false);
			C_df_ilb(l_true);
			EVAL(right->OP_LEFT, val, gencode, NO_LABEL, NO_LABEL);
			C_bra(l_end);
			C_df_ilb(l_false);
			EVAL(right->OP_RIGHT, val, gencode, NO_LABEL, NO_LABEL);
			C_df_ilb(l_end);
			break;
		}
		case OR:
		case AND: {
			label l_false, l_true, l_maybe;

			l_maybe = text_label();
			if (true_label) {
				l_false = false_label;
				l_true = true_label;
			}
			else {
				l_false = text_label();
				l_true = gencode ? text_label(): l_false;
			}

			EVAL(left, RVAL, TRUE, oper == AND ? l_maybe : l_true,
					       oper == AND ? l_false : l_maybe);
			C_df_ilb(l_maybe);
			EVAL(right, RVAL, gencode, l_true, l_false);
			if (gencode && !true_label) {
				label l_end = text_label();

				C_df_ilb(l_true);
				C_loc((arith)1);
				C_bra(l_end);
				C_df_ilb(l_false);
				C_loc((arith)0);
				C_df_ilb(l_end);
			}
			else {
				if (! true_label) C_df_ilb(l_false);
			}
			}
			break;
		case '!':
			if (true_label == 0) {
				EVAL(right, RVAL, gencode, NO_LABEL, NO_LABEL);
				if (gencode) {
					C_teq();
				}
			}
			else
				EVAL(right, RVAL, gencode, false_label,
								true_label);
			break;
		case INT2INT:
		case INT2FLOAT:
		case FLOAT2INT:
		case FLOAT2FLOAT:
			EVAL(right, RVAL, gencode, NO_LABEL, NO_LABEL);
			if (gencode)
				conversion(right->ex_type, left->ex_type);
			break;
		default:
			crash("(EVAL) bad operator %s\n", symbol2str(oper));
		}
		/*	If the rvalue of the expression is required but
			only its lvalue is evaluated, its rvalue is
			loaded by the following statements:
		*/
		if (gencode && val == RVAL && expr->ex_lvalue == 1) {
			if (expr->ex_type->tp_fund == FLOAT
			    || expr->ex_type->tp_fund == DOUBLE
			    || expr->ex_type->tp_fund == LNGDBL)
				fp_used = 1;
			load_block(expr->ex_type->tp_size,
				expr->ex_type->tp_align);
		}
		break;
	}
	default:
		crash("(EVAL) bad expression class");
	}
	if (val == RVAL && ((expr->ex_flags & EX_VOLATILE) || vol)) C_nop();
}

/*	compare() serves as an auxiliary function of EVAL	*/
compare(relop, lbl)
	int relop;
	label lbl;
{
	switch (relop) {
	case '<':
		C_zlt(lbl);
		break;
	case LESSEQ:
		C_zle(lbl);
		break;
	case '>':
		C_zgt(lbl);
		break;
	case GREATEREQ:
		C_zge(lbl);
		break;
	case EQUAL:
		C_zeq(lbl);
		break;
	case NOTEQUAL:
		C_zne(lbl);
		break;
	default:
		CRASH();
	}
}

/*	truthvalue() serves as an auxiliary function of EVAL	*/
truthvalue(relop)
	int relop;
{
	switch (relop)	{
	case '<':
		C_tlt();
		break;
	case LESSEQ:
		C_tle();
		break;
	case '>':
		C_tgt();
		break;
	case GREATEREQ:
		C_tge();
		break;
	case EQUAL:
		C_teq();
		break;
	case NOTEQUAL:
		C_tne();
		break;
	default:
		CRASH();
	}
}


/*	assop() generates the opcode of an assignment operators op=	*/
assop(type, oper)
	register struct type *type;
	int oper;
{
	register arith size;
	register uns = type->tp_unsigned;

	if ((int)(size = type->tp_size) < (int)word_size)
		size = word_size;
	switch (type->tp_fund) {
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case ENUM:
		switch (oper) {
		case PLUSAB:
		case PLUSPLUS:
		case POSTINCR:
			if (uns)
				C_adu(size);
			else
				C_adi(size);
			break;
		case MINAB:
		case MINMIN:
		case POSTDECR:
			if (uns)
				C_sbu(size);
			else
				C_sbi(size);
			break;
		case TIMESAB:
			if (uns)
				C_mlu(size);
			else
				C_mli(size);
			break;
		case DIVAB:
			if (uns)
				C_dvu(size);
			else
				C_dvi(size);
			break;
		case MODAB:
			if (uns)
				C_rmu(size);
			else
				C_rmi(size);
			break;
		case LEFTAB:
			if (uns)
				C_slu(size);
			else
				C_sli(size);
			break;
		case RIGHTAB:
			if (uns)
				C_sru(size);
			else
				C_sri(size);
			break;
		case ANDAB:
			C_and(size);
			break;
		case XORAB:
			C_xor(size);
			break;
		case ORAB:
			C_ior(size);
			break;
		}
		break;
	case FLOAT:
	case DOUBLE:
	case LNGDBL:
		switch (oper) {
		case PLUSAB:
		case PLUSPLUS:			/* ??? etc... */
		case POSTINCR:
			C_adf(size);
			break;
		case MINAB:
		case MINMIN:
		case POSTDECR:
			C_sbf(size);
			break;
		case TIMESAB:
			C_mlf(size);
			break;
		case DIVAB:
			C_dvf(size);
			break;
		}
		break;
	case POINTER:
		if (oper == MINAB || oper == MINMIN || oper == POSTDECR)
			C_ngi(size);
		ptr_add(size);
		break;
	default:
		crash("(assop) bad type %s\n", symbol2str(type->tp_fund));
	}
}

ptr_add(size)
	arith size;
{
	if (size != pointer_size) {
		C_loc(size);
		C_loc(pointer_size);
		C_cuu();
	}
	C_ads(pointer_size);
}

/*	store_val() generates code for a store operation.
	There are four ways of storing data:
	- into a global variable
	- into an automatic local variable
	- into a local static variable
	- absolute addressing
*/
store_val(vl, tp)
	register struct value *vl;
	register struct type *tp;
{
	register int inword = 0;
	register int indword = 0;
	arith val = vl->vl_value;

	if (vl->vl_class == Const) {	/* absolute addressing */
		load_cst(val, pointer_size);
		store_block(tp->tp_size, tp->tp_align);
		return;
	}
	if (tp->tp_align % word_align == 0) {
		if (tp->tp_size == word_size) inword = 1;
		else if (tp->tp_size == dword_size) indword = 1;
	}
	if (vl->vl_class == Name) {
		register struct idf *id = vl->vl_data.vl_idf;
		register struct def *df = id->id_def;

		/* if (df->df_level == L_GLOBAL) { /* } ??? re-examine */
		if (df->df_sc == GLOBAL
		    || df->df_sc == EXTERN
		    || df->df_sc == STATIC) {
			if (inword)
				C_ste_dnam(id->id_text, val);
			else
			if (indword)
				C_sde_dnam(id->id_text, val);
			else {
				C_lae_dnam(id->id_text, val);
				store_block(tp->tp_size, tp->tp_align);
			}
		}
		else {
			ASSERT(df->df_sc != STATIC);
			if (inword || indword)
				StoreLocal(df->df_address + val, tp->tp_size);
			else {
				AddrLocal(df->df_address + val);
				store_block(tp->tp_size, tp->tp_align);
			}
		}
	}
	else {	
		label dlb = vl->vl_data.vl_lbl;

		ASSERT(vl->vl_class == Label);
		if (inword)
			C_ste_dlb(dlb, val);
		else
		if (indword)
			C_sde_dlb(dlb, val);
		else {
			C_lae_dlb(dlb, val);
			store_block(tp->tp_size, tp->tp_align);
		}
	}
}


/*	load_val() generates code for stacking a certain value (from ex),
	which can be obtained in one of the following ways:
	- value from absolute addressed memory
	- constant value
	- function result
	- global variable
	- static variable
	- local variable
*/
load_val(expr, rlval)
	register struct expr *expr; /* expression containing the value	*/
	int rlval;		/* generate either LVAL or RVAL		*/
{
	register struct type *tp = expr->ex_type;
	int rvalue = (rlval == RVAL && expr->ex_lvalue != 0);
	register int inword = 0, indword = 0;
	register arith val = expr->VL_VALUE;

	if (expr->ex_type->tp_fund == FLOAT
	    || expr->ex_type->tp_fund == DOUBLE
	    || expr->ex_type->tp_fund == LNGDBL)
		fp_used = 1;
	if (expr->VL_CLASS == Const) {
		if (rvalue) { /* absolute addressing */
			load_cst(val, pointer_size);
			load_block(tp->tp_size, tp->tp_align);
		}
		else	/* integer, unsigned, long, enum etc	*/
			load_cst(val, tp->tp_size);
		return;
	}
	if (rvalue) {
		if (tp->tp_align % word_align == 0) {
			if (tp->tp_size == word_size) inword = 1;
			else if (tp->tp_size == dword_size) indword = 1;
		}
	}
	if (expr->VL_CLASS == Label) {
		if (rvalue) {
			if (inword)
				C_loe_dlb(expr->VL_LBL, val);
			else
			if (indword)
				C_lde_dlb(expr->VL_LBL, val);
			else {
				C_lae_dlb(expr->VL_LBL, val);
				load_block(tp->tp_size, tp->tp_align);
			}

		}
		else {
			C_lae_dlb(expr->VL_LBL, (arith)0);
			C_adp(val);
		}
	}
	else {
		register struct idf *id = expr->VL_IDF;
		register struct def *df = id->id_def;
		int fund = df->df_type->tp_fund;

		ASSERT(ISNAME(expr));
		if (fund == FUNCTION) {
			/*	the previous statement tried to catch a function
				identifier, which may be cast to a pointer to a
				function.
				ASSERT(!(rvalue)); ???
			*/
			C_lpi(id->id_text);
		}
		else
		/* if (df->df_level == L_GLOBAL) { /* } ??? re-examine */
		if ( df->df_sc == GLOBAL
		    || df->df_sc == STATIC
		    || df->df_sc == EXTERN) {
			if (rvalue) {
				if (inword)
					C_loe_dnam(id->id_text, val);
				else
				if (indword)
					C_lde_dnam(id->id_text, val);
				else {
					C_lae_dnam(id->id_text, val);
					load_block(tp->tp_size, tp->tp_align);
				}
			}
			else {
				C_lae_dnam(id->id_text, (arith)0);
				C_adp(val);
			}
		}
		else {
			/* ASSERT(df->df_sc != STATIC); */
			if (rvalue) {
				if (inword || indword)
					LoadLocal(df->df_address + val, tp->tp_size);
				else {
					AddrLocal(df->df_address + val);
					load_block(tp->tp_size, tp->tp_align);
				}
			}
			else {
				AddrLocal(df->df_address);
				C_adp(val);
			}
		}
	}
}

load_cst(val, siz)
	arith val, siz;
{
	if ((int)siz <= (int)word_size)
		C_loc(val);
	else
	if ((int)siz == (int)dword_size)
		C_ldc(val);
	else {
		label datlab;

		C_df_dlb(datlab = data_label());
		C_rom_icon(long2str((long)val, 10), siz);
		C_lae_dlb(datlab, (arith)0);
		C_loi(siz);
	}
}

operands(expr, gencode)
	register struct expr *expr;
{
	EVAL(expr->OP_LEFT, RVAL, gencode, NO_LABEL, NO_LABEL);
	EVAL(expr->OP_RIGHT, RVAL, gencode, NO_LABEL, NO_LABEL);
}
#endif	/* LINT */

