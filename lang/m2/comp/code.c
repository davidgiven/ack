/* C O D E   G E N E R A T I O N   R O U T I N E S */

/*	Code generation for expressions and coercions
*/

#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include	<assert.h>

#include	"type.h"
#include	"def.h"
#include	"scope.h"
#include	"desig.h"
#include	"LLlex.h"
#include	"node.h"
#include	"Lpars.h"
#include	"standards.h"
#include	"walk.h"

extern char	*long2str();
extern char	*symbol2str();
extern int	proclevel;
int		fp_used;

CodeConst(cst, size)
	arith cst, size;
{
	/*	Generate code to push constant "cst" with size "size"
	*/

	if (size <= word_size) {
		C_loc(cst);
	}
	else if (size == dword_size) {
		C_ldc(cst);
	}
	else {
		crash("(CodeConst)");
/*
		label dlab = ++data_label;

		C_df_dlb(dlab);
		C_rom_icon(long2str((long) cst), size);
		C_lae_dlb(dlab, (arith) 0);
		C_loi(size);
*/
	}
}

CodeString(nd)
	register struct node *nd;
{
	if (nd->nd_type->tp_fund != T_STRING) {
		C_loc(nd->nd_INT);
	}
	else {
		label lab = ++data_label;

		C_df_dlb(lab);
		C_rom_scon(nd->nd_STR, WA(nd->nd_SLE + 1));
		C_lae_dlb(lab, (arith) 0);
	}
}

STATIC
CodePadString(nd, sz)
	register struct node *nd;
	arith sz;
{
	/*	Generate code to push the string indicated by "nd".
		Make it null-padded to "sz" bytes
	*/
	register arith sizearg = WA(nd->nd_type->tp_size);

	assert(nd->nd_type->tp_fund == T_STRING);

	if (sizearg != sz) {
		/* null padding required */
		assert(sizearg < sz);
		C_zer(sz - sizearg);
	}
	CodeString(nd);		/* push address of string */
	C_loi(sizearg);
}

CodeExpr(nd, ds, true_label, false_label)
	register struct node *nd;
	register struct desig *ds;
	label true_label, false_label;
{
	register struct type *tp = nd->nd_type;

	if (tp->tp_fund == T_REAL) fp_used = 1;
	switch(nd->nd_class) {
	case Def:
		if (nd->nd_def->df_kind & (D_PROCEDURE|D_PROCHEAD)) {
			C_lpi(NameOfProc(nd->nd_def));
			ds->dsg_kind = DSG_LOADED;
			break;
		}
		/* Fall through */

	case Link:
	case LinkDef:
	case Arrsel:
	case Arrow:
		CodeDesig(nd, ds);
		break;

	case Oper:
		CodeOper(nd, true_label, false_label);
		if (true_label == 0) ds->dsg_kind = DSG_LOADED;
		else {
			ds->dsg_kind = DSG_INIT;
			true_label = 0;
		}
		break;

	case Uoper:
		CodeUoper(nd);
		ds->dsg_kind = DSG_LOADED;
		break;

	case Value:
		switch(nd->nd_symb) {
		case REAL: {
			label lab = ++data_label;

			C_df_dlb(lab);
			C_rom_fcon(nd->nd_REL, nd->nd_type->tp_size);
			C_lae_dlb(lab, (arith) 0);
			C_loi(nd->nd_type->tp_size);
			}
			break;
		case STRING:
			CodeString(nd);
			break;
		case INTEGER:
			CodeConst(nd->nd_INT, tp->tp_size);
			break;
		default:
			crash("Value error");
		}
		ds->dsg_kind = DSG_LOADED;
		break;

	case Call:
		CodeCall(nd);
		ds->dsg_kind = DSG_LOADED;
		break;

	case Set: {
		register arith *st = nd->nd_set;
		register int i;

		st = nd->nd_set;
		ds->dsg_kind = DSG_LOADED;
		if (!st) {
			C_zer(tp->tp_size);
			break;
		}
		for (i = tp->tp_size / word_size, st += i; i > 0; i--) { 
			C_loc(*--st);
		}
		}
		break;

	case Xset:
		CodeSet(nd);
		ds->dsg_kind = DSG_LOADED;
		break;
		
	default:
		crash("(CodeExpr) bad node type");
	}

	if (true_label != 0) {
		/* Only for boolean expressions
		*/
		CodeValue(ds, tp->tp_size);
		*ds = InitDesig;
		C_zne(true_label);
		C_bra(false_label);
	}
}

CodeCoercion(t1, t2)
	register struct type *t1, *t2;
{
	register int fund1, fund2;

	t1 = BaseType(t1);
	t2 = BaseType(t2);
	if (t1 == t2) return;
	if ((fund1 = t1->tp_fund) == T_WORD) fund1 = T_INTEGER;
	if ((fund2 = t2->tp_fund) == T_WORD) fund2 = T_INTEGER;
	switch(fund1) {
	case T_INTEGER:
		switch(fund2) {
		case T_INTEGER:
			if (t2->tp_size != t1->tp_size) {
				C_loc(t1->tp_size);
				C_loc(t2->tp_size);
				C_cii();
			}
			break;
		case T_ENUMERATION:
		case T_CHAR:
		case T_CARDINAL:
			if (t1->tp_size != word_size) {
				C_loc(t1->tp_size);
				C_loc(word_size);
				C_ciu();
			}
			break;
		case T_REAL:
			C_loc(t1->tp_size);
			C_loc(t2->tp_size);
			C_cif();
			break;
		default:
			crash("Funny integer conversion");
		}
		break;

	case T_CHAR:
	case T_ENUMERATION:
	case T_CARDINAL:
	case T_INTORCARD:
		switch(fund2) {
		case T_ENUMERATION:
		case T_CHAR:
		case T_CARDINAL:
		case T_POINTER:
		case T_EQUAL:
		case T_INTORCARD:
			if (t2->tp_size > word_size) {
				C_loc(word_size);
				C_loc(t2->tp_size);
				C_cuu();
			}
			break;
		case T_INTEGER:
			C_loc(word_size);
			C_loc(t2->tp_size);
			C_cui();
			break;
		case T_REAL:
			C_loc(word_size);
			C_loc(t2->tp_size);
			C_cuf();
			break;
		default:
			crash("Funny cardinal conversion");
		}
		break;

	case T_REAL:
		switch(fund2) {
		case T_REAL:
			if (t2->tp_size != t1->tp_size) {
				C_loc(t1->tp_size);
				C_loc(t2->tp_size);
				C_cff();
			}
			break;
		case T_INTEGER:
			C_loc(t1->tp_size);
			C_loc(t2->tp_size);
			C_cfi();
			break;
		case T_CARDINAL:
			C_loc(t1->tp_size);
			C_loc(t2->tp_size);
			C_cfu();
			break;
		default:
			crash("Funny REAL conversion");
		}
		break;
	}
}

CodeCall(nd)
	register struct node *nd;
{
	/*	Generate code for a procedure call. Checking of parameters
		and result is already done.
	*/
	register struct node *left = nd->nd_left;
	register struct node *right = nd->nd_right;
	register struct type *result_tp;

	if (left->nd_type == std_type) {
		CodeStd(nd);
		return;
	}	

	if (IsCast(left)) {
		/* it was just a cast. Simply ignore it
		*/
		CodePExpr(right->nd_left);
		*nd = *(right->nd_left);
		nd->nd_type = left->nd_def->df_type;
		return;
	}

	assert(IsProcCall(left));

	if (right) {
		CodeParameters(ParamList(left->nd_type), right);
	}

	switch(left->nd_class) {
	case Def: {
		register struct def *df = left->nd_def;

		if (df->df_kind == D_PROCEDURE) {
			arith level = df->df_scope->sc_level;

			if (level > 0) {
				C_lxl((arith) proclevel - level);
			}
			C_cal(NameOfProc(df));
			break;
		}
		else if (df->df_kind == D_PROCHEAD) {
			C_cal(df->for_name);
			break;
		}}
		/* Fall through */
	default:
		CodePExpr(left);
		C_cai();
	}
	if (left->nd_type->prc_nbpar) C_asp(left->nd_type->prc_nbpar);
	if (result_tp = ResultType(left->nd_type)) {
		if (IsConstructed(result_tp)) {
			C_lfr(pointer_size);
			C_loi(result_tp->tp_size);
		}
		else	C_lfr(WA(result_tp->tp_size));
	}
}

CodeParameters(param, arg)
	struct paramlist *param;
	struct node *arg;
{
	register struct type *tp;
	register struct node *left;
	register struct type *left_type;

	assert(param != 0 && arg != 0);

	if (param->next) {
		CodeParameters(param->next, arg->nd_right);
	}

	tp = TypeOfParam(param);
	left = arg->nd_left;
	left_type = left->nd_type;
	if (IsConformantArray(tp)) {
		register struct type *elem = tp->arr_elem;

		C_loc(tp->arr_elsize);
		if (IsConformantArray(left_type)) {
			DoHIGH(left);
			if (elem->tp_size != left_type->arr_elem->tp_size) {
				/* This can only happen if the formal type is
				   ARRAY OF (WORD|BYTE)
				*/
				C_loc(left_type->arr_elem->tp_size);
				C_mli(word_size);
				if (elem == word_type) {
					C_loc(word_size - 1);
					C_adi(word_size);
					C_loc(word_size);
					C_dvi(word_size);
				}
				else {
					assert(elem == byte_type);
				}
			}
		}
		else if (left->nd_symb == STRING) {
			C_loc(left->nd_SLE - 1);
		}
		else if (elem == word_type) {
			C_loc((left_type->tp_size+word_size-1) / word_size - 1);
		}
		else if (elem == byte_type) {
			C_loc(left_type->tp_size - 1);
		}
		else {
			arith lb, ub;
			getbounds(IndexType(left_type), &lb, &ub);
			C_loc(ub - lb);
		}
		C_loc((arith) 0);
		if (left->nd_symb == STRING) {
			CodeString(left);
		}
		else if (left->nd_class == Call) {
			/* ouch! forgot about this one! */
			arith tmp, TmpSpace();

			CodePExpr(left);
			tmp = TmpSpace(left->nd_type->tp_size, left->nd_type->tp_align);
			C_lal(tmp);
			C_sti(WA(left->nd_type->tp_size));
			C_lal(tmp);
		}
		else	CodeDAddress(left);
		return;
	}
	if (IsVarParam(param)) {
		CodeDAddress(left);
		return;
	}
	if (left_type->tp_fund == T_STRING) {
		CodePadString(left, tp->tp_size);
		return;
	}
	CodePExpr(left);
	RangeCheck(tp, left_type);
	CodeCoercion(left_type, tp);
}

CodeStd(nd)
	struct node *nd;
{
	register struct node *arg = nd->nd_right;
	register struct node *left = 0;
	register struct type *tp;
	int std = nd->nd_left->nd_def->df_value.df_stdname;

	if (arg) {
		left = arg->nd_left;
		tp = BaseType(left->nd_type);
		arg = arg->nd_right;
	}

	switch(std) {
	case S_ABS:
		CodePExpr(left);
		if (tp->tp_fund == T_INTEGER) {
			if (tp->tp_size == int_size) C_cal("_absi");
			else	C_cal("_absl");
		}
		else if (tp->tp_fund == T_REAL) {
			if (tp->tp_size == float_size) C_cal("_absf");
			else	C_cal("_absd");
		}
		C_asp(tp->tp_size);
		C_lfr(tp->tp_size);
		break;

	case S_CAP:
		CodePExpr(left);
		C_loc((arith) 0137);	/* ASCII assumed */
		C_and(word_size);
		break;

	case S_CHR:
		CodePExpr(left);
		RangeCheck(char_type, tp);
		break;

	case S_FLOAT:
		CodePExpr(left);
		CodeCoercion(tp, real_type);
		break;

	case S_HIGH:
		assert(IsConformantArray(tp));
		DoHIGH(left);
		break;

	case S_ODD:
		CodePExpr(left);
		if (tp->tp_size == word_size) {
			C_loc((arith) 1);
			C_and(word_size);
		}
		else {
			assert(tp->tp_size == dword_size);
			C_ldc((arith) 1);
			C_and(dword_size);
			C_ior(word_size);
		}
		break;

	case S_ORD:
		CodePExpr(left);
		break;

	case S_TRUNC:
		CodePExpr(left);
		CodeCoercion(tp, card_type);
		break;

	case S_VAL:
		CodePExpr(left);
		RangeCheck(nd->nd_type, tp);
		break;

	case S_ADR:
		CodeDAddress(left);
		break;

	case S_DEC:
	case S_INC: {
		register arith size = tp->tp_size;

		if (size < word_size) size = word_size;
		CodePExpr(left);
		if (arg) CodePExpr(arg->nd_left);
		else	C_loc((arith) 1);
		if (std == S_DEC) {
			if (tp->tp_fund == T_INTEGER) C_sbi(size);
			else	C_sbu(size);
		}
		else {
			if (tp->tp_fund == T_INTEGER) C_adi(size);
			else	C_adu(size);
		}
		if (size == word_size) RangeCheck(tp, int_type);
		CodeDStore(left);
		break;
		}

	case S_HALT:
		C_cal("_halt");
		break;

	case S_INCL:
	case S_EXCL:
		CodePExpr(left);
		CodePExpr(arg->nd_left);
		C_set(tp->tp_size);
		if (std == S_INCL) {
			C_ior(tp->tp_size);
		}
		else {
			C_com(tp->tp_size);
			C_and(tp->tp_size);
		}
		CodeDStore(left);
		break;

	default:
		crash("(CodeStd)");
	}
}

RangeCheck(tpl, tpr)
	register struct type *tpl, *tpr;
{
	/*	Generate a range check if neccessary
	*/

	arith llo, lhi, rlo, rhi;

	if (bounded(tpl)) {
		/* in this case we might need a range check */
		if (!bounded(tpr)) {
			/* yes, we need one */
			genrck(tpl);
		}
		else {
			/* both types are restricted. check the bounds
			   to see wether we need a range check.
			   We don't need one if the range of values of the
			   right hand side is a subset of the range of values
			   of the left hand side.
			*/
			getbounds(tpl, &llo, &lhi);
			getbounds(tpr, &rlo, &rhi);
			if (llo > rlo || lhi < rhi) {
				genrck(tpl);
			}
		}
	}
}

Operands(leftop, rightop, tp)
	register struct node *leftop, *rightop;
	struct type *tp;
{

	CodePExpr(leftop);
	CodeCoercion(leftop->nd_type, tp);
	CodePExpr(rightop);
	CodeCoercion(rightop->nd_type, tp);
}

CodeOper(expr, true_label, false_label)
	register struct node *expr;	/* the expression tree itself	*/
	label true_label;
	label false_label;	/* labels to jump to in logical expr's	*/
{
	register struct node *leftop = expr->nd_left;
	register struct node *rightop = expr->nd_right;
	register struct type *tp = expr->nd_type;

	switch (expr->nd_symb)	{
	case '+':
		Operands(leftop, rightop, tp);
		switch (tp->tp_fund)	{
		case T_INTEGER:
			C_adi(tp->tp_size);
			break;
		case T_REAL:
			C_adf(tp->tp_size);
			break;
		case T_POINTER:
		case T_EQUAL:
		case T_CARDINAL:
		case T_INTORCARD:
			C_adu(tp->tp_size);
			break;
		case T_SET:
			C_ior(tp->tp_size);
			break;
		default:
			crash("bad type +");
		}
		break;
	case '-':
		Operands(leftop, rightop, tp);
		switch (tp->tp_fund)	{
		case T_INTEGER:
			C_sbi(tp->tp_size);
			break;
		case T_REAL:
			C_sbf(tp->tp_size);
			break;
		case T_POINTER:
		case T_EQUAL:
		case T_CARDINAL:
		case T_INTORCARD:
			C_sbu(tp->tp_size);
			break;
		case T_SET:
			C_com(tp->tp_size);
			C_and(tp->tp_size);
			break;
		default:
			crash("bad type -");
		}
		break;
	case '*':
		Operands(leftop, rightop, tp);
		switch (tp->tp_fund)	{
		case T_INTEGER:
			C_mli(tp->tp_size);
			break;
		case T_POINTER:
		case T_EQUAL:
		case T_CARDINAL:
		case T_INTORCARD:
			C_mlu(tp->tp_size);
			break;
		case T_REAL:
			C_mlf(tp->tp_size);
			break;
		case T_SET:
			C_and(tp->tp_size);
			break;
		default:
			crash("bad type *");
		}
		break;
	case '/':
		Operands(leftop, rightop, tp);
		switch (tp->tp_fund)	{
		case T_REAL:
			C_dvf(tp->tp_size);
			break;
		case T_SET:
			C_xor(tp->tp_size);
			break;
		default:
			crash("bad type /");
		}
		break;
	case DIV:
		Operands(leftop, rightop, tp);
		switch(tp->tp_fund)	{
		case T_INTEGER:
			C_dvi(tp->tp_size);
			break;
		case T_POINTER:
		case T_EQUAL:
		case T_CARDINAL:
		case T_INTORCARD:
			C_dvu(tp->tp_size);
			break;
		default:
			crash("bad type DIV");
		}
		break;
	case MOD:
		Operands(leftop, rightop, tp);
		switch(tp->tp_fund)	{
		case T_INTEGER:
			C_rmi(tp->tp_size);
			break;
		case T_POINTER:
		case T_EQUAL:
		case T_CARDINAL:
		case T_INTORCARD:
			C_rmu(tp->tp_size);
			break;
		default:
			crash("bad type MOD");
		}
		break;
	case '<':
	case LESSEQUAL:
	case '>':
	case GREATEREQUAL:
	case '=':
	case '#':
		tp = BaseType(leftop->nd_type);
		if (tp == intorcard_type) tp = BaseType(rightop->nd_type);
		Operands(leftop, rightop, tp);
		switch (tp->tp_fund)	{
		case T_INTEGER:
			C_cmi(tp->tp_size);
			break;
		case T_POINTER:
		case T_EQUAL:
		case T_HIDDEN:
		case T_CARDINAL:
		case T_INTORCARD:
			C_cmu(tp->tp_size);
			break;
		case T_ENUMERATION:
		case T_CHAR:
			C_cmu(word_size);
			break;
		case T_REAL:
			C_cmf(tp->tp_size);
			break;
		case T_SET:
			if (expr->nd_symb == GREATEREQUAL) {
				/* A >= B is the same as A equals A + B
				*/
				C_dup(2*tp->tp_size);
				C_asp(tp->tp_size);
				C_ior(tp->tp_size);
			}
			else if (expr->nd_symb == LESSEQUAL) {
				/* A <= B is the same as A - B = {}
				*/
				C_com(tp->tp_size);
				C_and(tp->tp_size);
				C_zer(tp->tp_size);
			}
			C_cms(tp->tp_size);
			break;
		default:
			crash("bad type COMPARE");
		}
		if (true_label != 0)	{
			compare(expr->nd_symb, true_label);
			C_bra(false_label);
		}
		else	{
			truthvalue(expr->nd_symb);
		}
		break;
	case IN:
		/* In this case, evaluate right hand side first! The
		   INN instruction expects the bit number on top of the
		   stack
		*/
		CodePExpr(rightop);
		CodePExpr(leftop);
		CodeCoercion(leftop->nd_type, word_type);
		C_inn(rightop->nd_type->tp_size);
		if (true_label != 0) {
			C_zne(true_label);
			C_bra(false_label);
		}
		break;
	case OR:
	case AND:
	case '&': {
		label  l_maybe = ++text_label, l_end;
		struct desig Des;
		int genlabels = 0;

		if (true_label == 0)	{
			genlabels = 1;
			true_label = ++text_label;
			false_label = ++text_label;
			l_end = ++text_label;
		}

		Des = InitDesig;
		if (expr->nd_symb == OR) {
			CodeExpr(leftop, &Des, true_label, l_maybe);
		}
		else	CodeExpr(leftop, &Des, l_maybe, false_label);
		C_df_ilb(l_maybe);
		Des = InitDesig;
		CodeExpr(rightop, &Des, true_label, false_label);
		if (genlabels) {
			C_df_ilb(true_label);
			C_loc((arith)1);
			C_bra(l_end);
			C_df_ilb(false_label);
			C_loc((arith)0);
			C_df_ilb(l_end);
		}
		break;
		}
	default:
		crash("(CodeOper) Bad operator %s\n",symbol2str(expr->nd_symb));
	}
}

/*	compare() serves as an auxiliary function of CodeOper	*/
compare(relop, lbl)
	int relop;
	register label lbl;
{
	switch (relop)	{
	case '<':
		C_zlt(lbl);
		break;
	case LESSEQUAL:
		C_zle(lbl);
		break;
	case '>':
		C_zgt(lbl);
		break;
	case GREATEREQUAL:
		C_zge(lbl);
		break;
	case '=':
		C_zeq(lbl);
		break;
	case '#':
		C_zne(lbl);
		break;
	default:
		crash("(compare)");
	}
}

/*	truthvalue() serves as an auxiliary function of CodeOper	*/
truthvalue(relop)
	int relop;
{
	switch (relop)	{
	case '<':
		C_tlt();
		break;
	case LESSEQUAL:
		C_tle();
		break;
	case '>':
		C_tgt();
		break;
	case GREATEREQUAL:
		C_tge();
		break;
	case '=':
		C_teq();
		break;
	case '#':
		C_tne();
		break;
	default:
		crash("(truthvalue)");
	}
}

CodeUoper(nd)
	register struct node *nd;
{
	register struct type *tp = nd->nd_type;

	CodePExpr(nd->nd_right);
	switch(nd->nd_symb) {
	case '~':
	case NOT:
		C_teq();
		break;
	case '-':
		switch(tp->tp_fund) {
		case T_INTEGER:
		case T_INTORCARD:
			C_ngi(tp->tp_size);
			break;
		case T_REAL:
			C_ngf(tp->tp_size);
			break;
		default:
			crash("Bad operand to unary -");
		}
		break;
	default:
		crash("Bad unary operator");
	}
}

CodeSet(nd)
	register struct node *nd;
{
	register struct type *tp = nd->nd_type;

	C_zer(tp->tp_size);	/* empty set */
	nd = nd->nd_right;
	while (nd) {
		assert(nd->nd_class == Link && nd->nd_symb == ',');

		CodeEl(nd->nd_left, tp);
		nd = nd->nd_right;
	}
}

CodeEl(nd, tp)
	register struct node *nd;
	register struct type *tp;
{
	register struct type *eltype = ElementType(tp);

	if (nd->nd_class == Link && nd->nd_symb == UPTO) {
		C_loc(tp->tp_size);	/* push size */
		if (eltype->tp_fund == T_SUBRANGE) {
			C_loc(eltype->sub_ub);
		}
		else	C_loc((arith) (eltype->enm_ncst - 1));
		Operands(nd->nd_left, nd->nd_right, word_type);
		C_cal("_LtoUset");	/* library routine to fill set */
		C_asp(4 * word_size);
	}
	else {
		CodePExpr(nd);
		C_set(tp->tp_size);
		C_ior(tp->tp_size);
	}
}

CodePExpr(nd)
	struct node *nd;
{
	/*	Generate code to push the value of the expression "nd"
		on the stack.
	*/
	struct desig designator;

	designator = InitDesig;
	CodeExpr(nd, &designator, NO_LABEL, NO_LABEL);
	CodeValue(&designator, nd->nd_type->tp_size);
}

CodeDAddress(nd)
	struct node *nd;
{
	/*	Generate code to push the address of the designator "nd"
		on the stack.
	*/

	struct desig designator;

	designator = InitDesig;
	CodeDesig(nd, &designator);
	CodeAddress(&designator);
}

CodeDStore(nd)
	register struct node *nd;
{
	/*	Generate code to store the expression on the stack into the
		designator "nd".
	*/

	struct desig designator;

	designator = InitDesig;
	CodeDesig(nd, &designator);
	CodeStore(&designator, nd->nd_type->tp_size);
}

DoHIGH(nd)
	struct node *nd;
{
	/*	Get the high index of a conformant array, indicated by "nd".
		The high index is the second field in the descriptor of
		the array, so it is easily found.
	*/
	register struct def *df = nd->nd_def;
	register arith highoff;

	assert(nd->nd_class == Def);
	assert(df->df_kind == D_VARIABLE);
	assert(IsConformantArray(df->df_type));

	highoff = df->var_off		/* base address and descriptor */
		  + pointer_size	/* skip base address */
		  + word_size;		/* skip first field of descriptor */
	if (df->df_scope->sc_level < proclevel) {
		C_lxa((arith) (proclevel - df->df_scope->sc_level));
		C_lof(highoff);
	}
	else	C_lol(highoff);
}
