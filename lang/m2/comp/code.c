/* C O D E   G E N E R A T I O N   R O U T I N E S */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

/*	Code generation for expressions and coercions
*/

#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>

#include	"type.h"
#include	"def.h"
#include	"scope.h"
#include	"desig.h"
#include	"LLlex.h"
#include	"node.h"
#include	"Lpars.h"

extern label	data_label();
extern char	*long2str();
extern char	*symbol2str();
extern int	proclevel;

CodeConst(cst, size)
	arith cst, size;
{
	/*	Generate code to push constant "cst" with size "size"
	*/
	label dlab;

	if (size <= word_size) {
		C_loc(cst);
	}
	else if (size == dword_size) {
		C_ldc(cst);
	}
	else {
		C_df_dlb(dlab = data_label());
		C_rom_icon(long2str((long) cst), 10);
		C_lae_dlb(dlab);
		C_loi(size);
	}
}

CodeString(nd)
	struct node *nd;
{
	
	label lab;
	
	C_df_dlb(lab = data_label());
	C_rom_scon(nd->nd_STR, nd->nd_SLE);
	C_lae_dlb(lab);
}

CodeReal(nd)
	struct node *nd;
{
	label lab;
	
	C_df_dlb(lab = data_label());
	C_rom_fcon(nd->nd_REL, nd->nd_type->tp_size);
	C_lae_dlb(lab);
	C_loi(nd->nd_type->tp_size);
}

CodeExpr(nd, ds, true_label, false_label)
	struct node *nd;
	struct desig *ds;
	label true_label, false_label;
{
	struct desig ds1, ds2;

	switch(nd->nd_class) {
	case Def:
		CodeDesig(nd, ds);
		break;

	case Oper:
		if (nd->nd_symb == '[') {
			CodeDesig(nd, ds);
			break;
		}
		CodeOper(nd, true_label, false_label);
		if (true_label == 0) ds->dsg_kind = DSG_LOADED;
		else {
			*ds = InitDesig;
			true_label = 0;
		}
		break;

	case Uoper:
		if (nd->nd_symb == '^') {
			CodeDesig(nd, ds);
			break;
		}
		CodeExpr(nd->nd_right, ds, NO_LABEL, NO_LABEL);
		CodeValue(ds, nd->nd_right->nd_type->tp_size);
		CodeUoper(nd);
		ds->dsg_kind = DSG_LOADED;
		break;

	case Value:
		switch(nd->nd_symb) {
		case REAL:
			CodeReal(nd);
			break;
		case STRING:
			CodeString(nd);
			break;
		case INTEGER:
			CodeConst(nd->nd_INT, nd->nd_type->tp_size);
			break;
		default:
			crash("Value error");
		}
		ds->dsg_kind = DSG_LOADED;
		break;

	case Link:
		CodeDesig(nd, ds);
		break;
		
	case Call:
		CodeCall(nd);
		ds->dsg_kind = DSG_LOADED;
		break;

	case Xset:
	case Set:
		/* ??? */
		ds->dsg_kind = DSG_LOADED;
		break;
		
	default:
		crash("(CodeExpr) bad node type");
	}

	if (true_label != 0) {
		CodeValue(ds, nd->nd_type->tp_size);
		*ds = InitDesig;
		C_zne(true_label);
		C_bra(false_label);
	}
}

CodeCoercion(t1, t2)
	struct type *t1, *t2;
{
	/* ??? */
}

CodeCall(nd)
	struct node *nd;
{
	/*	Generate code for a procedure call. Checking of parameters
		and result is already done.
	*/
	register struct node *left = nd->nd_left;
	register struct node *arg = nd;
	register struct paramlist *param;
	struct type *tp;
	arith pushed = 0;
	struct desig Des;

	if (left->nd_type == std_type) {
		CodeStd(nd);
		return;
	}	
	tp = left->nd_type;

	assert(tp->tp_fund == T_PROCEDURE);

	for (param = left->nd_type->prc_params; param; param = param->next) {
		Des = InitDesig;
		arg = arg->nd_right;
		assert(arg != 0);
		if (param->par_var) {
			CodeDesig(arg->nd_left, &Des);
			CodeAddress(&Des);
			pushed += pointer_size;
		}
		else {
			CodeExpr(arg->nd_left, &Des, NO_LABEL, NO_LABEL);
			CodeValue(&Des, arg->nd_left->nd_type->tp_size);
			pushed += align(arg->nd_left->nd_type->tp_size, word_align);
		}
		/* ??? Conformant arrays */
	}

	if (left->nd_class == Def && left->nd_def->df_kind == D_PROCEDURE) {
		if (left->nd_def->df_scope->sc_level > 0) {
			C_lxl((arith) proclevel - left->nd_def->df_scope->sc_level);
			pushed += pointer_size;
		}
		C_cal(left->nd_def->prc_vis->sc_scope->sc_name);
	}
	else if (left->nd_class == Def && left->nd_def->df_kind == D_PROCHEAD) {
		C_cal(left->nd_def->for_name);
	}
	else {
		Des = InitDesig;
		CodeDesig(left, &Des);
		CodeAddress(&Des);
		C_cai();
	}
	C_asp(pushed);
	if (tp->next) {
		C_lfr(align(tp->next->tp_size, word_align));
	}
}

CodeStd(nd)
	struct node *nd;
{
	/* ??? */
}

CodeAssign(nd, dst, dss)
	struct node *nd;
	struct desig *dst, dss;
{
	/*	Generate code for an assignment. Testing of type
		compatibility and the like is already done.
	*/
	
	CodeCoercion(nd->nd_right->nd_type, nd->nd_left->nd_type);
	/* ??? */
}

Operands(leftop, rightop)
	struct node *leftop, *rightop;
{
	struct desig Des;

	Des = InitDesig;
	CodeExpr(leftop, &Des, NO_LABEL, NO_LABEL);
	CodeValue(&Des, leftop->nd_type->tp_size);
	Des = InitDesig;

	if (rightop->nd_type->tp_fund == T_POINTER && 
	    leftop->nd_type->tp_size != pointer_size) {
		CodeCoercion(leftop->nd_type, rightop->nd_type);
		leftop->nd_type = rightop->nd_type;
	}

	CodeExpr(rightop, &Des, NO_LABEL, NO_LABEL);
	CodeValue(&Des, rightop->nd_type->tp_size);
}

CodeOper(expr, true_label, false_label)
	struct node *expr;	/* the expression tree itself		*/
	label true_label;
	label false_label;	/* labels to jump to in logical expr's	*/
{
	register int oper = expr->nd_symb;
	register struct node *leftop = expr->nd_left;
	register struct node *rightop = expr->nd_right;
	register struct type *tp = expr->nd_type;
	struct desig Des;
	register struct desig *ds = &Des;

	switch (oper)	{
	case '+':
		Operands(leftop, rightop);
		switch (tp->tp_fund)	{
		case T_INTEGER:
			C_adi(tp->tp_size);
			break;
		case T_POINTER:
			C_ads(rightop->nd_type->tp_size);
			break;
		case T_REAL:
			C_adf(tp->tp_size);
			break;
		case T_CARDINAL:
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
		Operands(leftop, rightop);
		switch (tp->tp_fund)	{
		case T_INTEGER:
			C_sbi(tp->tp_size);
			break;
		case T_POINTER:
			if (rightop->nd_type->tp_fund == T_POINTER) {
				C_sbs(pointer_size);
			}
			else	{
				C_ngi(rightop->nd_type->tp_size);
				C_ads(rightop->nd_type->tp_size);
			}
			break;
		case T_REAL:
			C_sbf(tp->tp_size);
			break;
		case T_CARDINAL:
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
		Operands(leftop, rightop);
		switch (tp->tp_fund)	{
		case T_INTEGER:
			C_mli(tp->tp_size);
			break;
		case T_POINTER:
			CodeCoercion(rightop->nd_type, tp);
			/* Fall through */
		case T_CARDINAL:
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
		Operands(leftop, rightop);
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
		Operands(leftop, rightop);
		switch(tp->tp_fund)	{
		case T_INTEGER:
			C_dvi(tp->tp_size);
			break;
		case T_POINTER:
			CodeCoercion(rightop->nd_type, tp);
			/* Fall through */
		case T_CARDINAL:
			C_dvu(tp->tp_size);
			break;
		default:
			crash("bad type DIV");
		}
		break;
	case MOD:
		Operands(leftop, rightop);
		switch(tp->tp_fund)	{
		case T_INTEGER:
			C_rmi(tp->tp_size);
			break;
		case T_POINTER:
			CodeCoercion(rightop->nd_type, tp);
			/* Fall through */
		case T_CARDINAL:
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
	case UNEQUAL:
	case '#':
		Operands(leftop, rightop);
		CodeCoercion(rightop->nd_type, leftop->nd_type);
		switch (tp->tp_fund)	{
		case T_INTEGER:
			C_cmi(leftop->nd_type->tp_size);
			break;
		case T_POINTER:
			C_cmp();
			break;
		case T_CARDINAL:
			C_cmu(leftop->nd_type->tp_size);
			break;
		case T_ENUMERATION:
		case T_CHAR:
			C_cmu(word_size);
			break;
		case T_REAL:
			C_cmf(leftop->nd_type->tp_size);
			break;
		case T_SET:
			C_cms(leftop->nd_type->tp_size);
			break;
		default:
			crash("bad type COMPARE");
		}
		if (true_label != 0)	{
			compare(oper, true_label);
			C_bra(false_label);
		}
		else	{
			truthvalue(oper);
		}
		break;
	case IN:
		Operands(leftop, rightop);
		CodeCoercion(rightop->nd_type, word_type);
		C_inn(leftop->nd_type->tp_size);
		break;
	case AND:
	case '&':
		if (true_label == 0)	{
			label l_true = text_label();
			label l_false = text_label();
			label l_maybe = text_label();
			label l_end = text_label();
			struct desig Des;

			Des = InitDesig;
			CodeExpr(leftop, &Des, l_maybe, l_false);
			C_df_ilb(l_maybe);
			Des = InitDesig;
			CodeExpr(rightop, &Des, l_true, l_false);
			C_df_ilb(l_true);
			C_loc((arith)1);
			C_bra(l_end);
			C_df_ilb(l_false);
			C_loc((arith)0);
			C_df_ilb(l_end);
		}
		else	{
			label l_maybe = text_label();
			struct desig Des;

			Des = InitDesig;
			CodeExpr(leftop, &Des, l_maybe, false_label);
			Des = InitDesig;
			C_df_ilb(l_maybe);
			CodeExpr(rightop, &Des, true_label, false_label);
		}
		break;
	case OR:
		if (true_label == 0)	{
			label l_true = text_label();
			label l_false = text_label();
			label l_maybe = text_label();
			label l_end = text_label();
			struct desig Des;

			Des = InitDesig;
			CodeExpr(leftop, &Des, l_true, l_maybe);
			C_df_ilb(l_maybe);
			Des = InitDesig;
			CodeExpr(rightop, &Des, l_true, l_false);
			C_df_ilb(l_false);
			C_loc((arith)0);
			C_bra(l_end);
			C_df_ilb(l_true);
			C_loc((arith)1);
			C_df_ilb(l_end);
		}
		else	{
			label l_maybe = text_label();
			struct desig Des;

			Des = InitDesig;
			CodeExpr(leftop, &Des, true_label, l_maybe);
			C_df_ilb(l_maybe);
			Des = InitDesig;
			CodeExpr(rightop, &Des, true_label, false_label);
		}
		break;
	default:
		crash("(CodeOper) Bad operator %s\n", symbol2str(oper));
	}
}

/*	compare() serves as an auxiliary function of CodeOper	*/
compare(relop, lbl)
	int relop;
	label lbl;
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
	case UNEQUAL:
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
	case UNEQUAL:
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

	switch(nd->nd_symb) {
	case '~':
	case NOT:
		C_teq();
		break;
	case '-':
		switch(tp->tp_fund) {
		case T_INTEGER:
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
