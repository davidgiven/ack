/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* C O D E   G E N E R A T I O N   R O U T I N E S */

/* $Id$ */

/*	Code generation for expressions and coercions
*/

#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include	<em_abs.h>
#include	<assert.h>
#include	<alloc.h>

#include	"type.h"
#include	"LLlex.h"
#include	"def.h"
#include	"scope.h"
#include	"desig.h"
#include	"node.h"
#include	"Lpars.h"
#include	"standards.h"
#include	"walk.h"
#include	"bigresult.h"

extern int	proclevel;
extern char	options[];
extern t_desig	null_desig;
int		fp_used;

CodeConst(cst, size)
	arith cst;
	int size;
{
	/*	Generate code to push constant "cst" with size "size"
	*/

	if (size <= (int) word_size) {
		C_loc(cst);
	}
	else if (size == (int) dword_size) {
		C_ldc(cst);
	}
	else {
		crash("(CodeConst)");
	}
}

CodeString(nd)
	register t_node *nd;
{
	if (nd->nd_type->tp_fund != T_STRING) {
		/* Character constant */
		C_loc(nd->nd_INT);
		return;
	}
	C_df_dlb(++data_label);
	C_rom_scon(nd->nd_STR, WA((arith)(nd->nd_SLE + 1)));
	c_lae_dlb(data_label);
}

CodeExpr(nd, ds, true_label, false_label)
	register t_node *nd;
	register t_desig *ds;
	label true_label, false_label;
{
	register t_type *tp = nd->nd_type;

	DoLineno(nd);
	if (tp->tp_fund == T_REAL) fp_used = 1;
	switch(nd->nd_class) {
	case Def:
		if (nd->nd_def->df_kind & (D_PROCEDURE|D_PROCHEAD)) {
			C_lpi(nd->nd_def->prc_name);
			ds->dsg_kind = DSG_LOADED;
			break;
		}
		/* Fall through */

	case Link:
	case Arrsel:
	case Arrow:
		CodeDesig(nd, ds);
		break;

	case Oper:
		CodeOper(nd, true_label, false_label);
		ds->dsg_kind = DSG_LOADED;
		true_label = NO_LABEL;
		break;

	case Uoper:
		CodeUoper(nd);
		ds->dsg_kind = DSG_LOADED;
		break;

	case Value:
		switch(nd->nd_symb) {
		case REAL:
			C_df_dlb(++data_label);
			if (! nd->nd_RSTR) {
				static char buf[FLT_STRLEN];

				flt_flt2str(&nd->nd_RVAL, buf, FLT_STRLEN);
				C_rom_fcon(buf, tp->tp_size);
			}
			else C_rom_fcon(nd->nd_RSTR, tp->tp_size);
			c_lae_dlb(data_label);
			C_loi(tp->tp_size);
			break;
		case STRING:
			CodeString(nd);
			break;
		case INTEGER:
			CodeConst(nd->nd_INT, (int) (tp->tp_size));
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
		register unsigned i = (unsigned) (tp->tp_size) / (int) word_size;
		register arith *st = nd->nd_set + i;
		int null_set = 1;

		ds->dsg_kind = DSG_LOADED;
		for (; i; i--) { 
			if (*--st != 0) null_set = 0;
		}
		if (! null_set) {
			i = (unsigned) (tp->tp_size) / (int) word_size;
			st = nd->nd_set + i;
			for (; i; i--) { 
				C_loc(*--st);
			}
		}
		FreeSet(nd->nd_set);
		CodeSet(nd, null_set);
		}
		break;

	default:
		crash("(CodeExpr) bad node type");
	}

	if (true_label != NO_LABEL) {
		/* Only for boolean expressions
		*/
		CodeValue(ds, tp);
		C_zne(true_label);
		c_bra(false_label);
	}
}

CodeCoercion(t1, t2)
	t_type *t1, *t2;
{
	int fund1, fund2;
	int sz1 = t1->tp_size;
	int sz2;

	t1 = BaseType(t1);
	t2 = BaseType(t2);
	sz2 = t2->tp_size;
	switch(fund1 = t1->tp_fund) {
	case T_WORD:
		fund1 = T_INTEGER;
		break;
	case T_CHAR:
	case T_ENUMERATION:
	case T_CARDINAL:
	case T_INTORCARD:
		if (sz1 < (int) word_size) sz1 = word_size;
		/* fall through */
	case T_EQUAL:
	case T_POINTER:
		fund1 = T_CARDINAL;
		break;
	}
	switch(fund2 = t2->tp_fund) {
	case T_WORD:
		fund2 = T_INTEGER;
		break;
	case T_CHAR:
	case T_ENUMERATION:
		sz2 = word_size;
		/* fall through */
	case T_EQUAL:
	case T_POINTER:
		fund2 = T_CARDINAL;
		break;
	}

	switch(fund1) {
	case T_INTEGER:
		if (sz1 < (int) word_size) {
			c_loc(sz1);
			c_loc((int) word_size);
			C_cii();
			sz1 = word_size;
		}
		c_loc(sz1);
		c_loc(sz2);
		switch(fund2) {
		case T_REAL:
			C_cif();
			break;
		case T_INTEGER:
			C_cii();
			break;
		case T_CARDINAL:
			C_ciu();
			break;
		default:
			crash("Funny integer conversion");
		}
		break;

	case T_CARDINAL:
	case T_INTORCARD:
		c_loc(sz1);
		c_loc(sz2);
		switch(fund2) {
		case T_REAL:
			C_cuf();
			break;
		case T_CARDINAL:
		case T_INTORCARD:
			C_cuu();
			break;
		case T_INTEGER:
			C_cui();
			break;
		default:
			crash("Funny cardinal conversion");
		}
		break;

	case T_REAL:
		switch(fund2) {
		case T_REAL:
			c_loc(sz1);
			c_loc(sz2);
			C_cff();
			break;
		case T_INTEGER:
			c_loc(sz1);
			c_loc(sz2);
			C_cfi();
			break;
		case T_CARDINAL:
			if (! options['R']) {
				label lb = ++text_label;
				arith asz1 = sz1;

				C_dup(asz1);
				C_zrf(asz1);
				C_cmf(asz1);
				C_zge(lb);
				c_loc(ECONV);
				C_trp();
				def_ilb(lb);
			}
			c_loc(sz1);
			c_loc(sz2);
			C_cfu();
			break;
		default:
			crash("Funny REAL conversion");
		}
		break;
	}
}

CodeCall(nd)
	register t_node *nd;
{
	/*	Generate code for a procedure call. Checking of parameters
		and result is already done.
	*/
	register t_node *left = nd->nd_LEFT;
	t_type *result_tp;
	int needs_fn;

	if (left->nd_type == std_type) {
		CodeStd(nd);
		return;
	}	

	assert(IsProc(left));

	result_tp = ResultType(left->nd_type);
#ifdef BIG_RESULT_ON_STACK
	if (result_tp && TooBigForReturnArea(result_tp)) {
		C_asp(-WA(result_tp->tp_size));
	}
#endif

	if (nd->nd_RIGHT) {
		CodeParameters(ParamList(left->nd_type), nd->nd_RIGHT);
	}

	switch(left->nd_class) {
	case Def: {
		register t_def *df = left->nd_def;

		if (df->df_kind == D_CONST) {
			/* a procedure address */
			df = df->con_const.tk_data.tk_def;
		}
		if (df->df_kind & (D_PROCEDURE|D_PROCHEAD)) {
			int level = df->df_scope->sc_level;

			if (level > 0) {
				C_lxl((arith) (proclevel - level));
			}
			needs_fn = df->df_scope->sc_defmodule;
			C_cal(df->prc_name);
			break;
		}}
		/* Fall through */
	default:
		needs_fn = 1;
		CodePExpr(left);
		C_cai();
	}
	C_asp(left->nd_type->prc_nbpar);
	if (result_tp) {
		arith sz = WA(result_tp->tp_size);
		if (TooBigForReturnArea(result_tp)) {
#ifndef BIG_RESULT_ON_STACK
			C_lfr(pointer_size);
			C_loi(sz);
#endif
		}
		else	C_lfr(sz);
	}
	DoFilename(needs_fn);
	DoLineno(nd);
}

CodeParameters(param, arg)
	t_param *param;
	register t_node *arg;
{
	register t_type *tp;
	register t_type *arg_type;

	assert(param != 0 && arg != 0);

	if (param->par_next) {
		CodeParameters(param->par_next, arg->nd_RIGHT);
	}

	tp = TypeOfParam(param);
	arg = arg->nd_LEFT;
	arg_type = arg->nd_type;
	if (IsConformantArray(tp)) {
		register t_type *elem = tp->arr_elem;

		C_loc(tp->arr_elsize);
		if (IsConformantArray(arg_type)) {
			DoHIGH(arg->nd_def);
			if (elem->tp_size != arg_type->arr_elem->tp_size) {
				/* This can only happen if the formal type is
				   ARRAY OF (WORD|BYTE)
				*/
				C_loc(arg_type->arr_elem->tp_size);
				C_mlu(word_size);
				if (elem == word_type) {
					c_loc((int) word_size - 1);
					C_adu(word_size);
					c_loc((int) word_size - 1);
					C_and(word_size);
				}
				else {
					assert(elem == byte_type);
				}
			}
		}
		else if (arg->nd_symb == STRING) {
			c_loc((int) arg->nd_SLE - 1);
		}
		else if (elem == word_type) {
			C_loc((arg_type->tp_size+word_size-1) / word_size - 1);
		}
		else if (elem == byte_type) {
			C_loc(arg_type->tp_size - 1);
		}
		else {
			C_loc(arg_type->arr_high - arg_type->arr_low);
		}
		c_loc(0);
	}
	if (IsConformantArray(tp) || IsVarParam(param)) {
		if (arg->nd_symb == STRING) {
			CodeString(arg);
		}
		else switch(arg->nd_class) {
		case Arrsel:
		case Arrow:
		case Def:
			CodeDAddress(arg, IsVarParam(param));
			break;
		default:{
			arith tmp, TmpSpace();
			arith sz = WA(arg->nd_type->tp_size);

			CodePExpr(arg);
			tmp = TmpSpace(sz, arg->nd_type->tp_align);
			STL(tmp, sz);
			C_lal(tmp);
			}
			break;
		}
		return;
	}
	if (arg_type->tp_fund == T_STRING) {
		CodePString(arg, tp);
		return;
	}
	CodePExpr(arg);
}

CodePString(nd, tp)
	t_node *nd;
	t_type *tp;
{
	arith szarg = WA(nd->nd_type->tp_size);
	register arith zersz = WA(tp->tp_size) - szarg;

	if (zersz) {
		/* null padding required */
		assert(zersz > 0);
		C_zer(zersz);
	}
	CodeString(nd);	/* push address of string */
	C_loi(szarg);
}

static
subu(sz)
	int sz;
{
	if (! options['R']) {
		C_cal(sz == (int) word_size ? "subuchk" : "subulchk");
	}
	C_sbu((arith) sz);
}

static
addu(sz)
	int sz;
{
	if (! options['R']) {
		C_cal(sz == (int) word_size ? "adduchk" : "addulchk");
	}
	C_adu((arith)sz);
}

static int
complex_lhs(nd)
	register t_node *nd;
{
	switch(nd->nd_class) {
	case Value:
	case Name:
	case Set:
	case Def:
		return 0;
	case Select:
		return complex_lhs(nd->nd_NEXT);
	default:
		return 1;
	}
}

CodeStd(nd)
	t_node *nd;
{
	register t_node *arg = nd->nd_RIGHT;
	register t_node *left = 0;
	register t_type *tp = 0;
	int std = nd->nd_LEFT->nd_def->df_value.df_stdname;

	if (arg) {
		left = arg->nd_LEFT;
		tp = BaseType(left->nd_type);
		arg = arg->nd_RIGHT;
	}

	switch(std) {
	case S_ORD:
	case S_VAL:
		CodePExpr(left);
		break;

	case S_ABS:
		CodePExpr(left);
		if (tp->tp_fund == T_INTEGER) {
			CAL((int)(tp->tp_size) == (int)int_size ? "absi" : "absl", (int)(tp->tp_size));
		}
		else if (tp->tp_fund == T_REAL) {
			CAL((int)(tp->tp_size) == (int)float_size ? "absf" : "absd", (int)(tp->tp_size));
		}
		C_lfr(tp->tp_size);
		break;

	case S_CAP:
		CodePExpr(left);
		C_cal("cap");
		break;

	case S_HIGH:
		assert(IsConformantArray(tp));
		DoHIGH(left->nd_def);
		break;

	case S_SIZE:
	case S_TSIZE:
		assert(IsConformantArray(tp));
		DoHIGH(left->nd_def);
		C_inc();
		C_loc(tp->arr_elem->tp_size);
		C_mlu(word_size);
		break;

	case S_ODD:
		CodePExpr(left);
		if ((int) tp->tp_size == (int) word_size) {
			c_loc(1);
			C_and(word_size);
		}
		else {
			assert(tp->tp_size == dword_size);
			C_ldc((arith) 1);
			C_and(dword_size);
			C_ior(word_size);
		}
		break;

	case S_ADR:
		CodeDAddress(left, 1);
		break;

	case S_DEC:
	case S_INC: {
		register arith size;
		int compl = complex_lhs(left);
		arith tmp = 0;

		size = left->nd_type->tp_size;
		if ((int) size < (int) word_size) size = word_size;
		if (compl) {
			tmp = NewPtr();
			CodeDAddress(left, 1);
			STL(tmp, pointer_size);
			LOL(tmp, pointer_size);
			C_loi(left->nd_type->tp_size);
		}
		else CodePExpr(left);
		CodeCoercion(left->nd_type, tp);
		if (arg) {
			CodePExpr(arg->nd_LEFT);
			CodeCoercion(arg->nd_LEFT->nd_type, tp);
		}
		else	{
			c_loc(1);
			CodeCoercion(intorcard_type, tp);
		}
		if (std == S_DEC) {
			if (tp->tp_fund == T_INTEGER) C_sbi(size);
			else	subu((int) size);
		}
		else {
			if (tp->tp_fund == T_INTEGER) C_adi(size);
			else	addu((int) size);
		}
		if ((int) size == (int) word_size) {
			RangeCheck(left->nd_type, tp->tp_fund == T_INTEGER ?
						int_type : card_type);
		}
		if (compl) {
			LOL(tmp, pointer_size);
			C_sti(left->nd_type->tp_size);
			FreePtr(tmp);
		}
		else CodeDStore(left);
		break;
		}

	case S_HALT:
		C_cal("halt");
		break;

	case S_INCL:
	case S_EXCL: {
		int compl = complex_lhs(left);
		arith tmp = 0;

		if (compl) {
			tmp = NewPtr();
			CodeDAddress(left, 1);
			STL(tmp, pointer_size);
			LOL(tmp, pointer_size);
			C_loi(left->nd_type->tp_size);
		}
		else CodePExpr(left);
		CodePExpr(arg->nd_LEFT);
		C_loc(tp->set_low);
		C_sbi(word_size);
		C_set(tp->tp_size);
		if (std == S_INCL) {
			C_ior(tp->tp_size);
		}
		else {
			C_com(tp->tp_size);
			C_and(tp->tp_size);
		}
		if (compl) {
			LOL(tmp, pointer_size);
			C_sti(left->nd_type->tp_size);
			FreePtr(tmp);
		}
		else CodeDStore(left);
		break;
		}

	default:
		crash("(CodeStd)");
	}
}

int
needs_rangecheck(tpl, tpr)
	register t_type	*tpl, *tpr;
{
	arith rlo, rhi;

	if (bounded(tpl)) {
		/* In this case we might need a range check.
		   If both types are restricted. check the bounds
		   to see wether we need a range check.
		   We don't need one if the range of values of the
		   right hand side is a subset of the range of values
		   of the left hand side.
		*/
		if (bounded(tpr)) {
			getbounds(tpr, &rlo, &rhi);
			if (in_range(rlo, tpl) && in_range(rhi, tpl)) {
				return 0;
			}
		}
		return 1;
	}
	return 0;
}

RangeCheck(tpl, tpr)
	register t_type *tpl, *tpr;
{
	/*	Generate a range check if neccessary
	*/

	arith rlo, rhi;

	if (options['R']) return;

	if (needs_rangecheck(tpl, tpr)) {
		genrck(tpl);
		return;
	}
	tpr = BaseType(tpr);
	if ((tpl->tp_fund == T_INTEGER && tpr->tp_fund == T_CARDINAL) ||
	     (tpr->tp_fund == T_INTEGER && tpl->tp_fund == T_CARDINAL)) {
		label lb = ++text_label;

		C_dup(tpr->tp_size);
		C_zer(tpr->tp_size);
		C_cmi(tpr->tp_size);
		C_zge(lb);
		c_loc(ECONV);
		C_trp();
		def_ilb(lb);
	}
}

Operands(nd)
	register t_node *nd;
{

	CodePExpr(nd->nd_LEFT);
	CodePExpr(nd->nd_RIGHT);
	DoLineno(nd);
}

CodeOper(expr, true_label, false_label)
	register t_node *expr;	/* the expression tree itself	*/
	label true_label;
	label false_label;	/* labels to jump to in logical expr's	*/
{
	register t_node *leftop = expr->nd_LEFT;
	register t_node *rightop = expr->nd_RIGHT;
	int fund = expr->nd_type->tp_fund;
	arith size = expr->nd_type->tp_size;

	switch (expr->nd_symb)	{
	case '+':
		Operands(expr);
		switch (fund)	{
		case T_INTEGER:
			C_adi(size);
			break;
		case T_REAL:
			C_adf(size);
			break;
		case T_POINTER:
		case T_EQUAL:
			C_ads(rightop->nd_type->tp_size);
			break;
		case T_CARDINAL:
		case T_INTORCARD:
			addu((int) size);
			break;
		case T_SET:
			C_ior(size);
			break;
		default:
			crash("bad type +");
		}
		break;
	case '-':
		Operands(expr);
		switch (fund)	{
		case T_INTEGER:
			C_sbi(size);
			break;
		case T_REAL:
			C_sbf(size);
			break;
		case T_POINTER:
		case T_EQUAL:
			if (rightop->nd_type == address_type) {
				C_sbs(size);
				break;
			}
			C_ngi(rightop->nd_type->tp_size);
			C_ads(rightop->nd_type->tp_size);
			break;
		case T_INTORCARD:
		case T_CARDINAL:
			subu((int) size);
			break;
		case T_SET:
			C_com(size);
			C_and(size);
			break;
		default:
			crash("bad type -");
		}
		break;
	case '*':
		Operands(expr);
		switch (fund)	{
		case T_INTEGER:
			C_mli(size);
			break;
		case T_POINTER:
		case T_EQUAL:
		case T_CARDINAL:
		case T_INTORCARD:
			if (! options['R']) {
				C_cal((int)(size) <= (int)word_size ?
					"muluchk" :
					"mululchk");
			}
			C_mlu(size);
			break;
		case T_REAL:
			C_mlf(size);
			break;
		case T_SET:
			C_and(size);
			break;
		default:
			crash("bad type *");
		}
		break;
	case '/':
		Operands(expr);
		switch (fund)	{
		case T_REAL:
			C_dvf(size);
			break;
		case T_SET:
			C_xor(size);
			break;
		default:
			crash("bad type /");
		}
		break;
	case DIV:
		Operands(expr);
		switch(fund)	{
		case T_INTEGER:
			C_cal((int)(size) == (int)word_size 
				? "dvi"
				: "dvil");
			C_asp(2*size);
			C_lfr(size);
			break;
		case T_POINTER:
		case T_EQUAL:
		case T_CARDINAL:
		case T_INTORCARD:
			C_dvu(size);
			break;
		default:
			crash("bad type DIV");
		}
		break;
	case MOD:
		Operands(expr);
		switch(fund)	{
		case T_INTEGER:
			C_cal((int)(size) == (int)word_size 
				? "rmi"
				: "rmil");
			C_asp(2*size);
			C_lfr(size);
			break;
		case T_POINTER:
		case T_EQUAL:
		case T_CARDINAL:
		case T_INTORCARD:
			C_rmu(size);
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
	case '#': {
		t_type *tp;

		Operands(expr);
		tp = BaseType(leftop->nd_type);
		if (tp->tp_fund == T_INTORCARD) tp = BaseType(rightop->nd_type);
		size = tp->tp_size;
		switch (tp->tp_fund)	{
		case T_INTEGER:
			C_cmi(size);
			break;
		case T_POINTER:
		case T_HIDDEN:
		case T_EQUAL:
			C_cmp();
			break;
		case T_CARDINAL:
		case T_INTORCARD:
			C_cmu(size);
			break;
		case T_ENUMERATION:
		case T_CHAR:
			C_cmu(word_size);
			break;
		case T_REAL:
			C_cmf(size);
			break;
		case T_SET:
			if (expr->nd_symb == GREATEREQUAL) {
				/* A >= B is the same as A equals A + B
				*/
				C_dup(size << 1);
				C_asp(size);
				C_ior(size);
				expr->nd_symb = '=';
			}
			else if (expr->nd_symb == LESSEQUAL) {
				/* A <= B is the same as A - B = {}
				*/
				C_com(size);
				C_and(size);
				C_zer(size);
				expr->nd_symb = '=';
			}
			C_cms(size);
			break;
		default:
			crash("bad type COMPARE");
		}
		if (true_label != NO_LABEL)	{
			compare(expr->nd_symb, true_label);
			c_bra(false_label);
			break;
		}
		truthvalue(expr->nd_symb);
		break;
		}

	case IN: {
		/* In this case, evaluate right hand side first! The
		   INN instruction expects the bit number on top of the
		   stack
		*/
		label l_toolarge = NO_LABEL, l_cont = NO_LABEL;
		t_type *ltp = leftop->nd_type;

		if (leftop->nd_symb == COERCION) {
			/* Could be coercion to word_type. */
			ltp = leftop->nd_RIGHT->nd_type;
		}
		if (leftop->nd_class == Value) {
			if (! in_range(leftop->nd_INT, ElementType(rightop->nd_type))) {
				if (true_label != NO_LABEL) {
					c_bra(false_label);
				}
				else	c_loc(0);
				break;
			}
			CodePExpr(rightop);
			C_loc(leftop->nd_INT - rightop->nd_type->set_low);
		}
		else {
			CodePExpr(rightop);
			CodePExpr(leftop);
			C_loc(rightop->nd_type->set_low);
			C_sbu(word_size);
			if (needs_rangecheck(ElementType(rightop->nd_type), ltp)) {
				l_toolarge = ++text_label;
				C_dup(word_size);
				C_loc(rightop->nd_type->tp_size*8);
				C_cmu(word_size);
				C_zge(l_toolarge);
			}
		}
		C_inn(rightop->nd_type->tp_size);
		if (true_label != NO_LABEL) {
			C_zne(true_label);
			c_bra(false_label);
		}
		else {
			l_cont =  ++text_label;
			c_bra(l_cont);
		}
		if (l_toolarge != NO_LABEL) {
			def_ilb(l_toolarge);
			C_asp(word_size+rightop->nd_type->tp_size);
			if (true_label != NO_LABEL) {
				c_bra(false_label);
			}
			else	c_loc(0);
		}
		if (l_cont != NO_LABEL) {
			def_ilb(l_cont);
		}
		break;
		}
	case OR:
	case AND: {
		label  l_maybe = ++text_label, l_end = NO_LABEL;
		t_desig Des;

		Des = null_desig;

		if (true_label == NO_LABEL)	{
			true_label = ++text_label;
			false_label = ++text_label;
			l_end = ++text_label;
		}

		if (expr->nd_symb == OR) {
			CodeExpr(leftop, &Des, true_label, l_maybe);
		}
		else	CodeExpr(leftop, &Des, l_maybe, false_label);
		def_ilb(l_maybe);
		Des = null_desig;
		CodeExpr(rightop, &Des, true_label, false_label);
		if (l_end != NO_LABEL) {
			def_ilb(true_label);
			c_loc(1);
			c_bra(l_end);
			def_ilb(false_label);
			c_loc(0);
			def_ilb(l_end);
		}
		break;
		}
	default:
		crash("(CodeOper) Bad operator");
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
	register t_node *nd;
{
	register t_type *tp = nd->nd_type;

	CodePExpr(nd->nd_RIGHT);
	switch(nd->nd_symb) {
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
	case COERCION:
		CodeCoercion(nd->nd_RIGHT->nd_type, tp);
		RangeCheck(tp, nd->nd_RIGHT->nd_type);
		break;
	case CAST:
		break;
	default:
		crash("Bad unary operator");
	}
}

CodeSet(nd, null_set)
	register t_node *nd;
{
	register t_type *tp = nd->nd_type;

	nd = nd->nd_NEXT;
	while (nd) {
		assert(nd->nd_class == Link && nd->nd_symb == ',');

		if (nd->nd_LEFT) {
			CodeEl(nd->nd_LEFT, tp, null_set);
			null_set = 0;
		}
		nd = nd->nd_RIGHT;
	}
	if (null_set) C_zer(tp->tp_size);
}

CodeEl(nd, tp, null_set)
	register t_node *nd;
	register t_type *tp;
{
	register t_type *eltype = ElementType(tp);

	if (nd->nd_class == Link && nd->nd_symb == UPTO) {
		if (null_set) C_zer(tp->tp_size);
		C_loc(tp->set_low);
		C_loc(tp->tp_size);	/* push size */
		if (eltype->tp_fund == T_SUBRANGE) {
			C_loc(eltype->sub_ub);
		}
		else	C_loc(eltype->enm_ncst - 1);
		Operands(nd);
		CAL("LtoUset", 5 * (int) word_size);
		/* library routine to fill set */
	}
	else {
		CodePExpr(nd);
		C_loc(tp->set_low);
		C_sbi(word_size);
		C_set(tp->tp_size);
		if (! null_set) C_ior(tp->tp_size);
	}
}

CodePExpr(nd)
	register t_node *nd;
{
	/*	Generate code to push the value of the expression "nd"
		on the stack.
	*/
	t_desig designator;

	designator = null_desig;
	CodeExpr(nd, &designator, NO_LABEL, NO_LABEL);
	CodeValue(&designator, nd->nd_type);
}

CodeDAddress(nd, chk_controlvar)
	t_node *nd;
{
	/*	Generate code to push the address of the designator "nd"
		on the stack.
	*/

	t_desig designator;
	int chkptr;

	designator = null_desig;
	if (chk_controlvar) ChkForFOR(nd);
	CodeDesig(nd, &designator);
	chkptr = designator.dsg_kind==DSG_PLOADED ||
		 designator.dsg_kind==DSG_PFIXED;
	CodeAddress(&designator);

	/*	Generate dummy use of pointer, to get possible error message
		as soon as possible
	*/
	if (chkptr && ! options['R']) {
		C_dup(pointer_size);
		C_loi((arith) 1);
		C_asp(word_size);
	}
}

CodeDStore(nd)
	register t_node *nd;
{
	/*	Generate code to store the expression on the stack into the
		designator "nd".
	*/

	t_desig designator;

	designator = null_desig;
	ChkForFOR(nd);
	CodeDesig(nd, &designator);
	CodeStore(&designator, nd->nd_type);
}

DoHIGH(df)
	register t_def *df;
{
	/*	Get the high index of a conformant array, indicated by "nd".
		The high index is the second field in the descriptor of
		the array, so it is easily found.
	*/
	register arith highoff;

	assert(df->df_kind == D_VARIABLE);
	assert(IsConformantArray(df->df_type));

	highoff = df->var_off		/* base address and descriptor */
		  + word_size + pointer_size;
					/* skip base and first field of
					   descriptor
					*/
	if (df->df_scope->sc_level < proclevel) {
		C_lxa((arith) (proclevel - df->df_scope->sc_level));
		C_lof(highoff);
	}
	else	C_lol(highoff);
}

#ifdef SQUEEZE
c_bra(l)
	label l;
{
	C_bra((label) l);
}

c_loc(n)
{
	C_loc((arith) n);
}

c_lae_dlb(l)
	label l;
{
	C_lae_dlb(l, (arith) 0);
}

CAL(name, ssp)
	char *name;
	int ssp;
{
	C_cal(name);
	C_asp((arith) ssp);
}
#endif
