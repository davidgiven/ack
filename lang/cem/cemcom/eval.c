/* $Header$ */
/* EXPRESSION-CODE GENERATOR */

/*	main functions :
		EVAL()			-- expression tree evaluator
		tmp_pointer_var()	-- deliver temporary pointer variable
		free_tmp_var()		-- return the pointer var
		store_val()		-- store primary expression
		load_val()		-- load primary expression
	auxiliary functions:
		assop()
		compare()
*/

#include	"nofloat.h"
#include	<em.h>
#include	"debug.h"
#include	"nobitfield.h"
#include	"dataflow.h"
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

#define	CRASH()		crash("EVAL: CRASH at line %u", __LINE__)
#define	toword(n)	((n) < word_size ? word_size : (n))

char *symbol2str();
char *long2str();
arith tmp_pointer_var();

/*	EVAL() serves as the main expression tree evaluator, which turns
	any legal expression tree into legal EM code.
	The parameters describe how EVAL should treat the expression tree:

	struct expr *expr:	pointer to root of the expression tree to
				be evaluated

	int val:		indicates whether the resulting expression
				is to be dereferenced (if val == RVAL and
				expr->ex_lvalue == 1) or not (val == LVAL).
				The latter case indicates that the resulting
				expression is an lvalue expression which should
				not be dereferenced by EVAL
	
	int code:		indicates whether the expression tree must be
				turned into EM code or not. E.g. the expression
				statement "12;" delivers the expression "12" to
				EVAL while this should not result in any EM
				code
	
	label false_label:
	label true_label:	if the expression is a logical or relational
				expression and if the loop of the program
				depends on the resulting value then EVAL
				generates jumps to the specified program
				labels, in case they are specified
				(i.e. are non-zero)
*/

EVAL(expr, val, code, true_label, false_label)
	register struct expr *expr;	/* the expression tree itself	*/
	int val;		/* either RVAL or LVAL			*/
	int code;		/* generate explicit code or not	*/
	label true_label;
	label false_label;	/* labels to jump to in logical expr's	*/
{
	register int gencode = (code == TRUE);

	switch (expr->ex_class)	{
	case Value:	/* just a simple value	*/
		if (gencode)
			load_val(expr, val);
		break;
	case String:	/* a string constant	*/
		expr_warning(expr, "(DEBUG) value-class 'String' seen");
		if (gencode) {
			struct expr *ex = expr;
			string2pointer(&ex);
			expr = ex;
			C_lae_dlb(expr->VL_LBL, expr->VL_VALUE);
		}
		break;
#ifndef NOFLOAT
	case Float:	/* a floating constant	*/
		if (gencode) {
			label datlab = data_label();
			
			C_df_dlb(datlab);
			C_rom_fcon(expr->FL_VALUE, expr->ex_type->tp_size);
			C_lae_dlb(datlab, (arith)0);
			C_loi(expr->ex_type->tp_size);
		}
		break;
#endif NOFLOAT
	case Oper:	/* compound expression	*/
	{
		int oper = expr->OP_OPER;
		register struct expr *leftop = expr->OP_LEFT;
		register struct expr *rightop = expr->OP_RIGHT;
		register struct type *tp = expr->OP_TYPE;

		if (tp->tp_fund == ERRONEOUS)	/* stop immediately */
			break;
		switch (oper)	{
		case '+':
			/*	We have the following possibilities :
				int + int, pointer + int, pointer + long,
				long + long, double + double
			*/
			EVAL(leftop, RVAL, code, NO_LABEL, NO_LABEL);
			EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);

			if (gencode) {
				switch (tp->tp_fund)	{
				case INT:
				case LONG:
					if (tp->tp_unsigned)
						C_adu(tp->tp_size);
					else
						C_adi(tp->tp_size);
					break;
				case POINTER:
					C_ads(rightop->ex_type->tp_size);
					break;
#ifndef NOFLOAT
				case DOUBLE:
					C_adf(tp->tp_size);
					break;
#endif NOFLOAT
				default:
					crash("bad type +");
				}
			}
			break;
		case '-':
			if (leftop == 0)	{	/* unary	*/
				EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);
				if (gencode) {
					switch (tp->tp_fund)	{
					case INT:
					case LONG:
					case POINTER:
						C_ngi(tp->tp_size);
						break;
#ifndef NOFLOAT
					case DOUBLE:
						C_ngf(tp->tp_size);
						break;
#endif NOFLOAT
					default:
						CRASH();
					}
				}
				break;
			}
			/*	Binary: we have the following flavours:
				int - int, pointer - int, pointer - long,
				pointer - pointer, long - long, double - double
			*/
			EVAL(leftop, RVAL, code, NO_LABEL, NO_LABEL);
			EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);

			if (!gencode)
				break;
			switch (tp->tp_fund)	{
			case INT:
			case LONG:
				if (tp->tp_unsigned)
					C_sbu(tp->tp_size);
				else
					C_sbi(tp->tp_size);
				break;
			case POINTER:
				if (rightop->ex_type->tp_fund == POINTER)
					C_sbs(pointer_size);
				else	{
					C_ngi(rightop->ex_type->tp_size);
					C_ads(rightop->ex_type->tp_size);
				}
				break;
#ifndef NOFLOAT
			case DOUBLE:
				C_sbf(tp->tp_size);
				break;
#endif NOFLOAT
			default:
				crash("bad type -");
			}
			break;
		case '*':
			if (leftop == 0)	/* unary	*/
				EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);
			else	{		/* binary	*/
				EVAL(leftop, RVAL, code, NO_LABEL, NO_LABEL);
				EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);
				if (gencode)
					switch (tp->tp_fund)	{
					case INT:
					case LONG:
					case POINTER:
						if (tp->tp_unsigned)
							C_mlu(tp->tp_size);
						else
							C_mli(tp->tp_size);
						break;
#ifndef NOFLOAT
					case DOUBLE:
						C_mlf(double_size);
						break;
#endif NOFLOAT
					default:
						crash("bad type *");
					}
			}
			break;
		case '/':
			EVAL(leftop, RVAL, code, NO_LABEL, NO_LABEL);
			EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);
			if (gencode)
				switch (tp->tp_fund)	{
				case INT:
				case LONG:
				case POINTER:
					if (tp->tp_unsigned)
						C_dvu(tp->tp_size);
					else
						C_dvi(tp->tp_size);
					break;
#ifndef NOFLOAT
				case DOUBLE:
					C_dvf(double_size);
					break;
#endif NOFLOAT
				default:
					crash("bad type /");
				}
			break;
		case '%':
			EVAL(leftop, RVAL, code, NO_LABEL, NO_LABEL);
			EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);
			ASSERT(tp->tp_fund==INT || tp->tp_fund==LONG);
			if (gencode)
				if (tp->tp_unsigned)
					C_rmu(tp->tp_size);
				else
					C_rmi(tp->tp_size);
			break;
		case LEFT:
			EVAL(leftop, RVAL, code, NO_LABEL, NO_LABEL);
			EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);
			if (gencode)
				if (tp->tp_unsigned)
					C_slu(tp->tp_size);
				else
					C_sli(tp->tp_size);
			break;
		case RIGHT:
			EVAL(leftop, RVAL, code, NO_LABEL, NO_LABEL);
			EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);
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
			EVAL(leftop, RVAL, code, NO_LABEL, NO_LABEL);
			EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);
			if (gencode) {
				/* The operands have the same type */
				arith size = leftop->ex_type->tp_size;
				
				switch (tp->tp_fund)	{
				case INT:
				case LONG:
					if (leftop->ex_type->tp_unsigned)
						C_cmu(size);
					else
						C_cmi(size);
					break;
#ifndef NOFLOAT
				case FLOAT: /* thought they were converted??? */
				case DOUBLE:
					C_cmf(size);
					break;
#endif NOFLOAT
				case POINTER:
					C_cmp();
					break;
				case ENUM:
					C_cmi(size);
					break;
				default:
					CRASH();
				}
				if (true_label != 0)	{
					compare(oper, true_label);
					C_bra(false_label);
				}
				else	{
					label l_true = text_label();
					label l_end = text_label();

					compare(oper, l_true);
					C_loc((arith)0);
					C_bra(l_end);
					C_df_ilb(l_true);
					C_loc((arith)1);
					C_df_ilb(l_end);
				}
			}
			break;
		case '&':
		case '|':
		case '^':
			/* both operands should have type int	*/
			EVAL(leftop, RVAL, code, NO_LABEL, NO_LABEL);
			EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);
			if (gencode) {
				arith size = tp->tp_size;

				if (size < word_size)
					size = word_size;
				switch (oper)	{
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
			if (leftop->ex_type->tp_fund == FIELD)	{
				/*	assignment to bitfield variable
				*/
				eval_field(expr, code);
				break;
			}
#endif NOBITFIELD
			EVAL(rightop, RVAL, TRUE, NO_LABEL, NO_LABEL);
			if (gencode)
				C_dup(ATW(tp->tp_size));

			if (leftop->ex_class != Value) {
				EVAL(leftop, LVAL, TRUE, NO_LABEL, NO_LABEL);
				store_block(tp->tp_size, tp->tp_align);
			}
			else
				store_val(
					&(leftop->ex_object.ex_value),
					leftop->ex_type
				);
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
		{
			arith old_offset;
			arith tmpvar = tmp_pointer_var(&old_offset);

#ifndef NOBITFIELD
			if (leftop->ex_type->tp_fund == FIELD)	{
				eval_field(expr, code);
				break;
			}
#endif NOBITFIELD
			if (leftop->ex_class != Value) {
				EVAL(leftop, LVAL, TRUE, NO_LABEL, NO_LABEL);
				C_lal(tmpvar);
				C_sti(pointer_size);
				C_lal(tmpvar);
				C_loi(pointer_size);
				C_loi(leftop->ex_type->tp_size);
			}
			else	{
				load_val(leftop, RVAL);
			}
			conversion(leftop->ex_type, tp);
			EVAL(rightop, RVAL, TRUE, NO_LABEL, NO_LABEL);
			assop(tp, oper);
			conversion(tp, leftop->ex_type);
			if (gencode)
				C_dup(toword(leftop->ex_type->tp_size));
			if (leftop->ex_class != Value) {
				C_lal(tmpvar);
				C_loi(pointer_size);
				C_sti(leftop->ex_type->tp_size);
				free_tmp_var(old_offset);
			}
			else {
				store_val(
					&(leftop->ex_object.ex_value),
					leftop->ex_type
				);
			}
			if (gencode)
				conversion(leftop->ex_type, expr->ex_type);
			break;
		}
		case '(':
		{
			register struct expr *ex;
			arith ParSize = (arith)0;

			if ((ex = rightop) != NILEXPR)	{
				/* function call with parameters*/
				while (	ex->ex_class == Oper &&
					ex->OP_OPER == PARCOMMA
				) {
					EVAL(ex->OP_RIGHT, RVAL, TRUE,
							NO_LABEL, NO_LABEL);
					ParSize += ATW(ex->ex_type->tp_size);
					ex = ex->OP_LEFT;
				}
				EVAL(ex, RVAL, TRUE, NO_LABEL, NO_LABEL);
				ParSize += ATW(ex->ex_type->tp_size);
			}
			if (	leftop->ex_class == Value
			&&	leftop->VL_CLASS == Name
			) {
				/* just an example:
					main() { (*((int (*)())0))(); }
				*/
				C_cal(leftop->VL_IDF->id_text);
#ifdef	DATAFLOW
				{	extern char options[];
					if (options['d'])
						DfaCallFunction(
							leftop->VL_IDF->id_text
						);
				}
#endif	DATAFLOW
			}
			else	{
				EVAL(leftop, LVAL, TRUE, NO_LABEL, NO_LABEL);
				C_cai();
			}
			/* remove parameters from stack	*/
			if (ParSize > (arith)0)
				C_asp(ParSize);
			if (gencode) {
				if (is_struct_or_union(tp->tp_fund)) {
					C_lfr(pointer_size);
					load_block(tp->tp_size, tp->tp_align);
				}
				else
					C_lfr(ATW(tp->tp_size));
			}
			break;
		}
		case '.':
			EVAL(leftop, LVAL, code, NO_LABEL, NO_LABEL);
			ASSERT(is_cp_cst(rightop));
			if (gencode)
				C_adp(rightop->VL_VALUE);
			break;
		case ARROW:
			EVAL(leftop, RVAL, code, NO_LABEL, NO_LABEL);
			ASSERT(is_cp_cst(rightop));
			if (gencode)
				C_adp(rightop->VL_VALUE);
			break;
		case ',':
			EVAL(leftop, RVAL, FALSE, NO_LABEL, NO_LABEL);
			EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);
			break;
		case '~':
			EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);
			if (gencode)
				C_com(tp->tp_size);
			break;
		case POSTINCR:
		case POSTDECR:
		case PLUSPLUS:
		case MINMIN:
		{
			arith old_offset, tmp;
			arith esize = tp->tp_size;
			int compl;	/* Complexity of left operand */
#ifndef NOBITFIELD
			if (leftop->ex_type->tp_fund == FIELD)	{
				eval_field(expr, code);
				break;
			}
#endif NOBITFIELD
			if (leftop->ex_class == Value) {
				compl = 0; /* Value */
				load_val(leftop, RVAL);
			}
			else
			if (leftop->ex_depth == 1 && leftop->OP_OPER == ARROW) {
				compl = 1; /* Value->sel */
				ASSERT(leftop->OP_LEFT->ex_class == Value);
				EVAL(leftop, RVAL, TRUE, NO_LABEL, NO_LABEL);
			}
			else {
				compl = 2; /* otherwise */
				tmp = tmp_pointer_var(&old_offset);
				EVAL(leftop, LVAL, TRUE, NO_LABEL, NO_LABEL);
				C_dup(pointer_size);
				C_lal(tmp);
				C_sti(pointer_size);
				C_loi(esize);
			}

			/*	We made the choice to put this stuff here
				and not to put the conversion in the expression
				tree because this conversion is EM dependent
				and not described in C
			*/
			if (esize < word_size)	{
				conversion(tp, word_type);
				esize = word_size;
			}

			if (gencode && (oper == POSTINCR || oper == POSTDECR))
				C_dup(esize);
			EVAL(rightop, RVAL, TRUE, NO_LABEL, NO_LABEL);
			assop(tp, oper);
			if (gencode && (oper == PLUSPLUS || oper == MINMIN))
				C_dup(esize);
			if (tp->tp_size < word_size)
				conversion(word_type, tp);
			if (compl == 0) {
				store_val(
					&(leftop->ex_object.ex_value),
					leftop->ex_type
				);
			}
			else
			if (compl == 1) {
				EVAL(leftop, LVAL, TRUE, NO_LABEL, NO_LABEL);
				C_sti(tp->tp_size);
			}
			else {
				C_lal(tmp);	/* always init'd */
				C_loi(pointer_size);
				C_sti(tp->tp_size);
				free_tmp_var(old_offset);
			}
			break;
		}
		case '?':	/* must be followed by ':'	*/
		{
			label l_true = text_label();
			label l_false = text_label();
			label l_end = text_label();

			EVAL(leftop, RVAL, TRUE, l_true, l_false);
			C_df_ilb(l_true);
			EVAL(rightop->OP_LEFT, RVAL, code, NO_LABEL, NO_LABEL);
			C_bra(l_end);
			C_df_ilb(l_false);
			EVAL(rightop->OP_RIGHT, RVAL, code, NO_LABEL, NO_LABEL);
			C_df_ilb(l_end);
			break;
		}
		case AND:
			if (true_label == 0)	{
				label l_true = text_label();
				label l_false = text_label();
				label l_maybe = text_label();
				label l_end = text_label();

				EVAL(leftop, RVAL, TRUE, l_maybe, l_false);
				C_df_ilb(l_maybe);
				if (gencode)	{
					EVAL(rightop, RVAL, TRUE,
							l_true, l_false);
					C_df_ilb(l_true);
					C_loc((arith)1);
					C_bra(l_end);
					C_df_ilb(l_false);
					C_loc((arith)0);
					C_df_ilb(l_end);
				}
				else {
					EVAL(rightop, RVAL, FALSE, l_false,
						l_false);
					C_df_ilb(l_false);
				}
			}
			else	{
				label l_maybe = text_label();

				EVAL(leftop, RVAL, TRUE, l_maybe, false_label);
				C_df_ilb(l_maybe);
				EVAL(rightop, RVAL, code, true_label,
					false_label);
			}
			break;
		case OR:
			if (true_label == 0)	{
				label l_true = text_label();
				label l_false = text_label();
				label l_maybe = text_label();
				label l_end = text_label();

				EVAL(leftop, RVAL, TRUE, l_true, l_maybe);
				C_df_ilb(l_maybe);
				if (gencode)	{
					EVAL(rightop, RVAL, TRUE,
							l_true, l_false);
					C_df_ilb(l_false);
					C_loc((arith)0);
					C_bra(l_end);
					C_df_ilb(l_true);
					C_loc((arith)1);
					C_df_ilb(l_end);
				}
				else	{
					EVAL(rightop, RVAL, FALSE, l_true,
						l_true);
					C_df_ilb(l_true);
				}
			}
			else	{
				label l_maybe = text_label();

				EVAL(leftop, RVAL, TRUE, true_label, l_maybe);
				C_df_ilb(l_maybe);
				EVAL(rightop, RVAL, code, true_label,
					false_label);
			}
			break;
		case '!':
			if (true_label == 0)	{
				if (gencode)	{
					label l_true = text_label();
					label l_false = text_label();
					label l_end = text_label();

					EVAL(rightop, RVAL, TRUE,
							l_false, l_true);
					C_df_ilb(l_false);
					C_loc((arith)0);
					C_bra(l_end);
					C_df_ilb(l_true);
					C_loc((arith)1);
					C_df_ilb(l_end);
				}
				else
					EVAL(rightop, RVAL, FALSE,
							NO_LABEL, NO_LABEL);
			}
			else
				EVAL(rightop, RVAL, code, false_label,
								true_label);
			break;
		case INT2INT:
#ifndef NOFLOAT
		case INT2FLOAT:
		case FLOAT2INT:
		case FLOAT2FLOAT:
#endif NOFLOAT
			EVAL(rightop, RVAL, code, NO_LABEL, NO_LABEL);
			if (gencode)
				conversion(rightop->ex_type, leftop->ex_type);
			break;
		default:
			crash("(EVAL) bad operator %s\n", symbol2str(oper));
		}

		/*	If the rvalue of the expression is required but
			only its lvalue is evaluated, its rvalue is
			loaded by the following statements:
		*/
		if (gencode && val == RVAL && expr->ex_lvalue == 1)
			load_block(expr->ex_type->tp_size,
				expr->ex_type->tp_align);
		break;
	}
	default:
		crash("(EVAL) bad expression class");
	}
}

/*	compare() serves as an auxiliary function of EVAL	*/
compare(relop, lbl)
	int relop;
	label lbl;
{
	switch (relop)	{
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

/*	assop() generates the opcode of an assignment operators op=	*/
assop(type, oper)
	register struct type *type;
	int oper;
{
	register arith size;
	register uns = type->tp_unsigned;

	if ((size = type->tp_size) < word_size)
		size = word_size;
	switch (type->tp_fund)	{
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case ENUM:
		switch (oper)	{
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
#ifndef NOFLOAT
	case FLOAT:
	case DOUBLE:
		switch (oper)	{
		case PLUSAB:
		case PLUSPLUS:
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
#endif NOFLOAT
	case POINTER:
		if (oper == MINAB || oper == MINMIN || oper == POSTDECR)
			C_ngi(size);
		C_ads(size);
		break;
	case ERRONEOUS:
		break;
	default:
		crash("(assop) bad type %s\n", symbol2str(type->tp_fund));
	}
}

/*	tmp_pointer_var() returns the EM address of a new temporary
	pointer variable needed at increment, decrement and assignment
	operations to store the address of some variable or lvalue-expression.
*/
arith
tmp_pointer_var(oldoffset)
	arith *oldoffset;	/* previous allocated address	*/
{
	register struct stack_level *stl = local_level;

	*oldoffset = stl->sl_local_offset;
	stl->sl_local_offset =
		- align(-stl->sl_local_offset + pointer_size, pointer_align);
	if (stl->sl_local_offset < stl->sl_max_block)
		stl->sl_max_block = stl->sl_local_offset;
	return stl->sl_local_offset;
}

/*	free_tmp_var() returns the address allocated by tmp_pointer_var()
	and resets the last allocated address.
*/
free_tmp_var(oldoffset)
	arith oldoffset;
{
	local_level->sl_local_offset = oldoffset;
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
	struct type *tp;
{
	arith size = tp->tp_size;
	int tpalign = tp->tp_align;
	int al_on_word;
	register int inword;
	register int indword;
	arith val = vl->vl_value;

	if (vl->vl_class == Const)	{	/* absolute addressing */
		load_cst(val, pointer_size);
		store_block(size, tpalign);
		return;
	}

	al_on_word = (tpalign % word_align == 0);
	if (!(inword = (size == word_size && al_on_word)))
		indword = (size == dword_size && al_on_word);

	if (vl->vl_class == Name)	{
		register struct idf *id = vl->vl_data.vl_idf;
		register struct def *df = id->id_def;

		if (df->df_level == L_GLOBAL)	{
			if (inword)
				C_ste_dnam(id->id_text, val);
			else
			if (indword)
				C_sde_dnam(id->id_text, val);
			else {
				C_lae_dnam(id->id_text, val);
				store_block(size, tpalign);
			}
		}
		else {
			ASSERT(df->df_sc != STATIC);
			if (inword)
				C_stl(df->df_address + val);
			else
			if (indword)
				C_sdl(df->df_address + val);
			else	{
				C_lal(df->df_address + val);
				store_block(size, tpalign);
				df->df_register = REG_NONE;
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
			store_block(size, tpalign);
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
load_val(expr, val)
	register struct expr *expr; /* expression containing the value	*/
	int val;		/* generate either LVAL or RVAL		*/
{
	register struct type *tp = expr->ex_type;
	int rvalue = (val == RVAL && expr->ex_lvalue != 0);
	arith size = tp->tp_size;
	int tpalign = tp->tp_align;
	int al_on_word;
	register int inword, indword;
	arith val = expr->VL_VALUE;

	if (expr->VL_CLASS == Const)	{
		if (rvalue)	{ /* absolute addressing */
			load_cst(val, pointer_size);
			load_block(size, tpalign);
		}
		else	/* integer, unsigned, long, enum etc	*/
			load_cst(val, size);
		return;
	}
	if (rvalue) {
		al_on_word = (tpalign % word_align == 0);
		if (!(inword = (size == word_size && al_on_word)))
			indword = (size == dword_size && al_on_word);
	}
	if (expr->VL_CLASS == Label) {
		if (rvalue)	{
			if (inword)
				C_loe_dlb(expr->VL_LBL, val);
			else
			if (indword)
				C_lde_dlb(expr->VL_LBL, val);
			else {
				C_lae_dlb(expr->VL_LBL, val);
				load_block(size, tpalign);
			}

		}
		else	{
			C_lae_dlb(expr->VL_LBL, (arith)0);
			C_adp(val);
		}
	}
	else {
		register struct idf *id = expr->VL_IDF;
		register struct def *df;

		ASSERT(expr->VL_CLASS == Name);
		if ((df = id->id_def)->df_type->tp_fund == FUNCTION)
			/*	the previous statement tried to catch a function
				identifier, which may be cast to a pointer to a
				function.
				ASSERT(!(rvalue)); ???
			*/
			C_lpi(id->id_text);
		else
		if (df->df_level == L_GLOBAL)	{
			if (rvalue)	{
				if (inword)
					C_loe_dnam(id->id_text, val);
				else
				if (indword)
					C_lde_dnam(id->id_text, val);
				else {
					C_lae_dnam(id->id_text, val);
					load_block(size, tpalign);
				}
			}
			else	{
				C_lae_dnam(id->id_text, (arith)0);
				C_adp(val);
			}
		}
		else	{
			ASSERT(df->df_sc != STATIC);
			if (rvalue)	{
				if (inword)
					C_lol(df->df_address + val);
				else
				if (indword)
					C_ldl(df->df_address + val);
				else {
					C_lal(df->df_address + val);
					load_block(size, tpalign);
					df->df_register = REG_NONE;
				}
			}
			else	{
				C_lal(df->df_address);
				C_adp(val);
				df->df_register = REG_NONE;
			}
		}
	}
}

load_cst(val, siz)
	arith val, siz;
{
	if (siz <= word_size)
		C_loc(val);
	else
	if (siz == dword_size)
		C_ldc(val);
	else {
		label datlab;

		C_df_dlb(datlab = data_label());
		C_rom_icon(long2str((long)val, 10), siz);
		C_lae_dlb(datlab, (arith)0);
		C_loi(siz);
	}
}
