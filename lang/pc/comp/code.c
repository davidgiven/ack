/* C O D E   G E N E R A T I O N   R O U T I N E S */

#include	"debug.h"
#include	<assert.h>
#include	<em.h>
#include	<em_reg.h>
#include	<em_abs.h>

#include	"LLlex.h"
#include	"Lpars.h"
#include	"def.h"
#include	"desig.h"
#include	"f_info.h"
#include	"idf.h"
#include	"main.h"
#include	"misc.h"
#include	"node.h"
#include	"required.h"
#include	"scope.h"
#include	"type.h"

int	fp_used;

CodeFil()
{
	if ( !options['L'] )
		C_fil_dlb((label) 1, (arith) 0);
}

routine_label(df)
	register struct def * df;
{
	df->prc_label = ++data_label;
	C_df_dlb(df->prc_label);
	C_rom_scon(df->df_idf->id_text, strlen(df->df_idf->id_text) + 1);
}

RomString(nd)
	register struct node *nd;
{
	C_df_dlb(++data_label);

	/* A string of the string_type is null-terminated. */
	if( nd->nd_type == string_type )
		C_rom_scon(nd->nd_STR, nd->nd_SLE + 1);	/* with trailing '\0' */
	else
		C_rom_scon(nd->nd_STR, nd->nd_SLE);	/* no trailing '\0' */

	nd->nd_SLA = data_label;
}

RomReal(nd)
	register struct node *nd;
{
	C_df_dlb(++data_label);
	C_rom_fcon(nd->nd_REL, nd->nd_type->tp_size);
	nd->nd_RLA = nd->nd_RIV->r_lab = data_label;
}

BssVar()
{
	/* generate bss segments for global variables */
	register struct def *df = GlobalScope->sc_def;

	while( df )	{
		if( df->df_kind == D_VARIABLE )	{
			C_df_dnam(df->var_name);

			/* ??? undefined value ??? */
			C_bss_cst(df->df_type->tp_size, (arith) 0, 0);
		}
		df = df->df_nextinscope;
	}
}

arith
CodeGtoDescr(sc)
	register struct scope *sc;
{
	/*	Create code for goto descriptors
	*/

	register struct node *lb = sc->sc_lablist;
	int first = 1;

	while( lb )	{
		if( lb->nd_def->lab_descr )	{
			if( first )	{
				/* create local for target SP */
				sc->sc_off = -WA(pointer_size - sc->sc_off);
				C_ms_gto();
				first = 0;
			}
			C_df_dlb(lb->nd_def->lab_descr);
			C_rom_ilb(lb->nd_def->lab_no);
			C_rom_cst(sc->sc_off);
		}
		lb = lb->nd_next;
	}
	if( !first )
		return sc->sc_off;
	else
		return (arith) 0;
}

arith
CodeBeginBlock(df)
	register struct def *df;
{
	/*	Generate code at the beginning of the main program,
		procedure or function.
	*/

	arith StackAdjustment = 0;
	arith offset = 0;		/* offset to save StackPointer */

	TmpOpen(df->prc_vis->sc_scope);

	if ( df->df_kind == D_MODULE) /* nothing */ ;
	else if (df->df_kind == D_PROGRAM ) {
		C_exp("m_a_i_n");
		C_pro_narg("m_a_i_n");
		C_ms_par((arith) 0);
		offset = CodeGtoDescr(df->prc_vis->sc_scope);
		CodeFil();

		/* initialize external files */
		call_ini();
		/* ignore floating point underflow */
		C_lim();
		C_loc((arith) (1 << EFUNFL));
		C_ior(int_size);
		C_sim();
	}
	else if (df->df_kind & (D_PROCEDURE | D_FUNCTION)) {
		struct type *tp;
		register struct paramlist *param;

		C_pro_narg(df->prc_name);
		C_ms_par(df->df_type->prc_nbpar);

		offset = CodeGtoDescr(df->prc_vis->sc_scope);
		CodeFil();

		if( options['t'] ) {
			C_lae_dlb(df->prc_label,(arith)0);
			C_cal("procentry");
			C_asp(pointer_size);
		}

		/* prc_bool is the local variable that indicates if the
		 * function result is assigned. This and can be disabled
		 * with the -R option. The variable, however, is always
		 * allocated and initialized.
		 */
		if( df->prc_res ) {
			C_zer((arith) int_size);
			C_stl(df->prc_bool);
		}
		for( param = ParamList(df->df_type); param; param = param->next) {
			if( !IsVarParam(param) )	{
				tp = TypeOfParam(param);

				if( IsConformantArray(tp) )	{
					/* Here, we have to make a copy of the
					   array. We must also remember how much
					   room is reserved for copies, because
					   we have to adjust the stack pointer
					   before we return.
					*/

					if( !StackAdjustment )	{
						/* First time we get here
						*/
						StackAdjustment = NewInt(0);
						C_loc((arith) 0);
						C_stl(StackAdjustment);
					}
					/* Address of array */
					C_lol(param->par_def->var_off);

					/* First compute size of the array */
					C_lol(tp->arr_cfdescr + word_size);
					C_inc();
						/* gives number of elements */
					C_lol(tp->arr_cfdescr + 2 * word_size);
							/* size of elements */
					C_mli(word_size);
					C_loc(word_size - 1);
					C_adi(word_size);
					C_loc(word_size);
					C_dvi(word_size);
							/* size in words */
					C_loc(word_size);
					C_mli(word_size);
							/* size in bytes */
					C_dup(word_size);
					C_lol(StackAdjustment);
					C_adi(word_size);
					C_stl(StackAdjustment);
						/* remember stack adjustments */

					C_los(word_size);	/* copy */
					C_lor((arith) 1);	
						/* push new address of array
						   ... downwards ... ???
						*/
					C_stl(param->par_def->var_off);
				}
			}
		}
	}
	else {
		crash("(CodeBeginBlock)");
		/*NOTREACHED*/
	}

	if( offset )	{
		/* save SP for non-local jump */
		C_lor((arith) 1);
		C_stl(offset);
	}
	return StackAdjustment;
}

CodeEndBlock(df, StackAdjustment)
	register struct def *df;
	arith StackAdjustment;
{
	if( df->df_kind == D_PROGRAM) {
		C_loc((arith) 0);
		C_cal("_hlt");
	}
	else if (df->df_kind & (D_PROCEDURE | D_FUNCTION)) {
		struct type *tp;

		if( StackAdjustment )	{
			/* remove copies of conformant arrays */
			C_lol(StackAdjustment);
			C_ass(word_size);
			FreeInt(StackAdjustment);
		}
		if( !options['n'] )
			RegisterMessages(df->prc_vis->sc_scope->sc_def);

		if( options['t'] ) {
			C_lae_dlb(df->prc_label,(arith)0);
			C_cal("procexit");
			C_asp(pointer_size);
		}
		if( tp = ResultType(df->df_type) )	{
			if( !options['R'] ) {
				C_lin(LineNumber);
				C_lol(df->prc_bool);
				C_cal("_nfa");
				C_asp(word_size);
			}
			if( tp->tp_size == word_size )
				C_lol(-tp->tp_size);
			if( tp->tp_size == 2 * word_size )
				C_ldl(-tp->tp_size);
			else {
				C_lal(-tp->tp_size);
				C_loi(tp->tp_size);
			}

			C_ret(tp->tp_size);
		}
		else
			C_ret((arith) 0);
	}
	else {
		crash("(CodeEndBlock)");
		/*NOTREACHED*/
	}

	C_end(- df->prc_vis->sc_scope->sc_off);
	TmpClose();
}

CodeExpr(nd, ds, true_label)
	register struct node *nd;
	register struct desig *ds;
	label true_label;
{
	register struct type *tp = nd->nd_type;

	if( tp->tp_fund == T_REAL ) fp_used = 1;

	switch( nd->nd_class )	{
	case Value:
		switch( nd->nd_symb )	{
		case INTEGER:
			C_loc(nd->nd_INT);
			break;
		case REAL:
			C_lae_dlb(nd->nd_RLA, (arith) 0);
			C_loi(tp->tp_size);
			if( nd->nd_RSI )
				C_ngf(tp->tp_size);
			break;
		case STRING:
			if( tp->tp_fund == T_CHAR )
				C_loc(nd->nd_INT);
			else
				C_lae_dlb(nd->nd_SLA, (arith) 0);
			break;
		case NIL:
			C_zer(pointer_size);
			break;
		default:
			crash("(CodeExpr Value)");
			/*NOTREACHED*/
		}
		ds->dsg_kind = DSG_LOADED;
		break;

	case Uoper:
		CodeUoper(nd);
		ds->dsg_kind = DSG_LOADED;
		break;

	case Boper:
		CodeBoper(nd, true_label);
		ds->dsg_kind = DSG_LOADED;
		true_label = NO_LABEL;
		break;

	case Set:	{
		register arith *st = nd->nd_set;
		register int i;

		ds->dsg_kind = DSG_LOADED;
		if( !st )	{
			C_zer(tp->tp_size);
			break;
		}
		for( i = tp->tp_size / word_size, st += i; i > 0; i--)
			C_loc(*--st);

		}
		break;

	case Xset:
		CodeSet(nd);
		ds->dsg_kind = DSG_LOADED;
		break;

	case Call:
		CodeCall(nd);
		ds->dsg_kind = DSG_LOADED;
		break;

	case NameOrCall:	{
		/* actual procedure/function parameter */
		struct node *left = nd->nd_left;
		struct def *df = left->nd_def;

		if( df->df_kind & D_ROUTINE )	{
			int level = df->df_scope->sc_level;

			if( level <= 0 || (df->df_flags & D_EXTERNAL) )
				C_zer(pointer_size);
			else
				C_lxl((arith) (proclevel - level));

			C_lpi(df->prc_name);
			ds->dsg_kind = DSG_LOADED;
			break;
		}
		assert(df->df_kind == D_VARIABLE);
		assert(df->df_type->tp_fund & T_ROUTINE);

		CodeDesig(left, ds);
		break;
	}

	case Arrow:
	case Arrsel:
	case Def:
	case LinkDef:
		CodeDesig(nd, ds);
		break;

	case Cast:	{
		/* convert integer to real */
		struct node *right = nd->nd_right;

		CodePExpr(right);
		Int2Real(right->nd_type->tp_size);
		ds->dsg_kind = DSG_LOADED;
		break;
	}
	case IntCoerc:	{
		/* convert integer to long integer */
		struct node *right = nd->nd_right;

		CodePExpr(right);
		Int2Long();
		ds->dsg_kind = DSG_LOADED;
		break;
	}
	case IntReduc:	{
		/* convert a long to an integer */
		struct node *right = nd->nd_right;

		CodePExpr(right);
		Long2Int();
		ds->dsg_kind = DSG_LOADED;
		break;
	}
	default:
		crash("(CodeExpr : bad node type)");
		/*NOTREACHED*/
	} /* switch class */

	if( true_label )	{
		/* Only for boolean expressions
		*/
		CodeValue(ds, tp);
		C_zeq(true_label);
	}
}

CodeUoper(nd)
	register struct node *nd;
{
	register struct type *tp = nd->nd_type;

	CodePExpr(nd->nd_right);

	switch( nd->nd_symb )	{
		case '-':
			assert(tp->tp_fund & T_NUMERIC);
			if( tp->tp_fund == T_INTEGER || tp->tp_fund == T_LONG )
				C_ngi(tp->tp_size);
			else
				C_ngf(tp->tp_size);
			break;

		case NOT:
			C_teq();
			break;

		case '(':
			break;

		default:
			crash("(CodeUoper)");
			/*NOTREACHED*/
	}
}

Operands(leftop, rightop)
	register struct node *leftop, *rightop;
{
	CodePExpr(leftop);
	CodePExpr(rightop);
}

CodeBoper(expr, true_label)
	register struct node *expr;	/* the expression tree itself	*/
	label true_label;		/* label to jump to in logical exprs */
{
	register struct node *leftop = expr->nd_left;
	register struct node *rightop = expr->nd_right;
	register struct type *tp = expr->nd_type;

	switch( expr->nd_symb )	{
		case '+':
			Operands(leftop, rightop);
			switch( tp->tp_fund )	{
				case T_INTEGER:
				case T_LONG:
					C_adi(tp->tp_size);
					break;
				case T_REAL:
					C_adf(tp->tp_size);
					break;
				case T_SET:
					C_ior(tp->tp_size);
					break;
				default:
					crash("(CodeBoper: bad type +)");
			}
			break;

		case '-':
			Operands(leftop, rightop);
			switch( tp->tp_fund )	{
				case T_INTEGER:
				case T_LONG:
					C_sbi(tp->tp_size);
					break;
				case T_REAL:
					C_sbf(tp->tp_size);
					break;
				case T_SET:
					C_com(tp->tp_size);
					C_and(tp->tp_size);
					break;
				default:
					crash("(CodeBoper: bad type -)");
			}
			break;

		case '*':
			Operands(leftop, rightop);
			switch( tp->tp_fund )	{
				case T_INTEGER:
				case T_LONG:
					C_mli(tp->tp_size);
					break;
				case T_REAL:
					C_mlf(tp->tp_size);
					break;
				case T_SET:
					C_and(tp->tp_size);
					break;
				default:
					crash("(CodeBoper: bad type *)");
			}
			break;

		case '/':
			Operands(leftop, rightop);
			if( tp->tp_fund == T_REAL )
				C_dvf(tp->tp_size);
			else
				crash("(CodeBoper: bad type /)");
			break;

		case DIV:
			Operands(leftop, rightop);
			if( tp->tp_fund == T_INTEGER || tp->tp_fund == T_LONG)
				C_dvi(tp->tp_size);
			else
				crash("(CodeBoper: bad type DIV)");
			break;

		case MOD:
			Operands(leftop, rightop);
			if( tp->tp_fund == T_INTEGER ) {
				C_cal("_mdi");
				C_asp(2 * tp->tp_size);
				C_lfr(tp->tp_size);
			}
			else if( tp->tp_fund == T_LONG) {
				C_cal("_mdil");
				C_asp(2 * tp->tp_size);
				C_lfr(tp->tp_size);
			}
			else
				crash("(CodeBoper: bad type MOD)");
			break;

		case '<':
		case LESSEQUAL:
		case '>':
		case GREATEREQUAL:
		case '=':
		case NOTEQUAL:
			CodePExpr(leftop);
			CodePExpr(rightop);
			tp = BaseType(rightop->nd_type);

			switch( tp->tp_fund )	{
				case T_INTEGER:
				case T_LONG:
					C_cmi(tp->tp_size);
					break;
				case T_REAL:
					C_cmf(tp->tp_size);
					break;
				case T_ENUMERATION:
				case T_CHAR:
					C_cmu(word_size);
					break;
				case T_POINTER:
					C_cmp();
					break;

				case T_SET:
					if( expr->nd_symb == GREATEREQUAL ) {
					/* A >= B is the same as A equals A + B
					*/
						C_dup(2 * tp->tp_size);
						C_asp(tp->tp_size);
						C_ior(tp->tp_size);
						expr->nd_symb = '=';
					}
					else if( expr->nd_symb == LESSEQUAL ) {
					/* A <= B is the same as A - B = []
					*/
						C_com(tp->tp_size);
						C_and(tp->tp_size);
						C_zer(tp->tp_size);
						expr->nd_symb = '=';
					}
					C_cms(tp->tp_size);
					break;

				case T_STRINGCONST:
				case T_ARRAY:
					C_loc((arith) IsString(tp));
					C_cal("_bcp");
					C_asp(2 * pointer_size + word_size);
					C_lfr(word_size);
					break;

				case T_STRING:
					C_cmp();
					break;

				default:
					crash("(CodeBoper : bad type COMPARE)");
			}
			truthvalue(expr->nd_symb);
			if( true_label != NO_LABEL )
				C_zeq(true_label);
			break;

		case IN:
		/* In this case, evaluate right hand side first! The INN
		   instruction expects the bit number on top of the stack
		*/
			CodePExpr(rightop);
			CodePExpr(leftop);
			if( rightop->nd_type == emptyset_type )
				C_and(rightop->nd_type->tp_size);
			else
				C_inn(rightop->nd_type->tp_size);

			if( true_label != NO_LABEL )
				C_zeq(true_label);
			break;

		case AND:
		case OR:
			Operands(leftop, rightop);
			if( expr->nd_symb == AND )
				C_and(tp->tp_size);
			else
				C_ior(tp->tp_size);
			if( true_label != NO_LABEL )
				C_zeq(true_label);
			break;
		default:
			crash("(CodeBoper Bad operator %s\n)",
						symbol2str(expr->nd_symb));
	}
}

/*	truthvalue() serves as an auxiliary function of CodeBoper	*/
truthvalue(relop)
{
	switch( relop )	{
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
		case NOTEQUAL:
			C_tne();
			break;
		default:
			crash("(truthvalue)");
			/*NOTREACHED*/
	}
}

CodeSet(nd)
	register struct node *nd;
{
	register struct type *tp = nd->nd_type;

	C_zer(tp->tp_size);
	nd = nd->nd_right;
	while( nd )	{
		assert(nd->nd_class == Link && nd->nd_symb == ',');

		CodeEl(nd->nd_left, tp);
		nd = nd->nd_right;
	}
}

CodeEl(nd, tp)
	register struct node *nd;
	register struct type *tp;
{
	if( nd->nd_class == Link && nd->nd_symb == UPTO )	{
		Operands(nd->nd_left, nd->nd_right);
		C_loc(tp->tp_size);	/* push size */
		C_cal("_bts");		/* library routine to fill set */
		C_asp(3 * word_size);
	}
	else	{
		CodePExpr(nd);
		C_set(tp->tp_size);
		C_ior(tp->tp_size);
	}
}

struct type *
CodeParameters(param, arg)
	struct paramlist *param;
	struct node *arg;
{
	register struct type *tp, *left_tp, *last_tp = (struct type *) 0;
	struct node *left;
	struct desig ds;

	assert(param && arg);

	if( param->next )
		last_tp = CodeParameters(param->next, arg->nd_right);

	tp = TypeOfParam(param);
	left = arg->nd_left;
	left_tp = left->nd_type;

	if( IsConformantArray(tp) )	{
		if( last_tp != tp )
			/* push descriptors only once */
			CodeConfDescr(tp, left_tp);

		CodeDAddress(left);
		return tp;
	}
	if( IsVarParam(param) )	{
		CodeDAddress(left);
		return tp;
	}
	if( left_tp->tp_fund == T_STRINGCONST )	{
		CodePString(left, tp);
		return tp;
	}

	ds = InitDesig;
	CodeExpr(left, &ds, NO_LABEL);
	CodeValue(&ds, left_tp);

	RangeCheck(tp, left_tp);
	if( tp == real_type && BaseType(left_tp) == int_type )
		Int2Real(int_size);

	return tp;
}

CodeConfDescr(ftp, atp)
	register struct type *ftp, *atp;
{
	struct type *elemtp = ftp->arr_elem;

	if( IsConformantArray(elemtp) )
		CodeConfDescr(elemtp, atp->arr_elem);

	if( atp->tp_fund == T_STRINGCONST )	{
		C_loc((arith) 1);
		C_loc(atp->tp_psize - 1);
		C_loc((arith) 1);
	}
	else if( IsConformantArray(atp) )	{
		if( atp->arr_sclevel < proclevel )	{
			C_lxa((arith) proclevel - atp->arr_sclevel);
			C_adp(atp->arr_cfdescr);
		}
		else
			C_lal(atp->arr_cfdescr);

		C_loi(3 * word_size);
	}
	else	{		/* normal array */
		assert(atp->tp_fund == T_ARRAY);
		assert(!IsConformantArray(atp));
		C_lae_dlb(atp->arr_ardescr, (arith) 0);
		C_loi( 3 * word_size);
	}
}

CodePString(nd, tp)
	struct node *nd;
	struct type *tp;
{
	/* no null padding */
	C_lae_dlb(nd->nd_SLA, (arith) 0);
	C_loi(tp->tp_size);
}

CodeCall(nd)
	register struct node *nd;
{
	/*	Generate code for a procedure call. Checking of parameters
		and result is already done.
	*/
	register struct node *left = nd->nd_left;
	register struct node *right = nd->nd_right;
	register struct def *df = left->nd_def;
	register struct type *result_tp;

	assert(IsProcCall(left));

	if( left->nd_type == std_type )	{
		CodeStd(nd);
		return;
	}	

	if( right )
		(void) CodeParameters(ParamList(left->nd_type), right);

	assert(left->nd_class == Def);


	if( df->df_kind & D_ROUTINE )	{
		int level = df->df_scope->sc_level;

		if( level > 0 && !(df->df_flags & D_EXTERNAL) )
			C_lxl((arith) (proclevel - level));
		C_cal(df->prc_name);
		C_asp(left->nd_type->prc_nbpar);
	}
	else	{
		label l1 = ++text_label;
		label l2 = ++text_label;

		assert(df->df_kind == D_VARIABLE);

		/* Push value of procedure/function parameter */
		CodePExpr(left);

		/* Test if value is a global or local procedure/function */
		C_exg(pointer_size);
		C_dup(pointer_size);
		C_zer(pointer_size);
		C_cmp();

		C_zeq(l1);
				/* At this point, on top of the stack the LB */
		C_exg(pointer_size);
				/* Now, the name of the procedure/function */
		C_cai();
		C_asp(pointer_size + left->nd_type->prc_nbpar);
		C_bra(l2);

		/* value is a global procedure/function */
		C_df_ilb(l1);
		C_asp(pointer_size);	/* no LB needed */
		C_cai();
		C_asp(left->nd_type->prc_nbpar);
		C_df_ilb(l2);
	}

	if( result_tp = ResultType(left->nd_type) )
		C_lfr(result_tp->tp_size);
}

CodeStd(nd)
	struct node *nd;
{
	register struct node *arg = nd->nd_right;
	register struct node *left = arg->nd_left;
	register struct type *tp = BaseType(left->nd_type);
	int req = nd->nd_left->nd_def->df_value.df_reqname;

	assert(arg->nd_class == Link && arg->nd_symb == ',');

	switch( req )	{
		case R_ABS:
			CodePExpr(left);
			if( tp == int_type )
				C_cal("_abi");
			else if ( tp == long_type )
				C_cal("_abl");
			else
				C_cal("_abr");
			C_asp(tp->tp_size);
			C_lfr(tp->tp_size);
			break;

		case R_SQR:
			CodePExpr(left);
			C_dup(tp->tp_size);
			if( tp == int_type || tp == long_type )
				C_mli(tp->tp_size);
			else
				C_mlf(real_size);
			break;

		case R_SIN:
		case R_COS:
		case R_EXP:
		case R_LN:
		case R_SQRT:
		case R_ARCTAN:
			assert(tp == real_type);
			CodePExpr(left);
			switch( req )	{
				case R_SIN:
					C_cal("_sin");
					break;
				case R_COS:
					C_cal("_cos");
					break;
				case R_EXP:
					C_cal("_exp");
					break;
				case R_LN:
					C_cal("_log");
					break;
				case R_SQRT:
					C_cal("_sqt");
					break;
				case R_ARCTAN:
					C_cal("_atn");
					break;
				default:
					crash("(CodeStd)");
					/*NOTREACHED*/
			}
			C_asp(real_size);
			C_lfr(real_size);
			break;

		case R_TRUNC:
			assert(tp == real_type);
			CodePExpr(left);
			Real2Int();
			break;

		case R_ROUND:
			assert(tp == real_type);
			CodePExpr(left);
			C_cal("_rnd");
			C_asp(real_size);
			C_lfr(real_size);
			Real2Int();
			break;

		case R_ORD:
			CodePExpr(left);
			break;

		case R_CHR:
			CodePExpr(left);
			genrck(char_type);
			break;

		case R_SUCC:
		case R_PRED:
			CodePExpr(left);
			C_loc((arith)1);
			if( tp == long_type) Int2Long();

			if( req == R_SUCC )
				C_adi(tp->tp_size);
			else
				C_sbi(tp->tp_size);

			if( bounded(left->nd_type) )
				genrck(left->nd_type);
			break;

		case R_ODD:
			CodePExpr(left);
			C_loc((arith) 1);
			if(  tp == long_type ) Int2Long();
			C_and(tp->tp_size);
			if( tp == long_type ) Long2Int(); /* bool_size == int_size */
			break;

		case R_EOF:
		case R_EOLN:
			CodeDAddress(left);
			if( req == R_EOF )
				C_cal("_efl");
			else
				C_cal("_eln");
			C_asp(pointer_size);
			C_lfr(word_size);
			break;

		case R_REWRITE:
		case R_RESET:
			CodeDAddress(left);
			if( tp == text_type )
				C_loc((arith) 0);
			else
				C_loc(tp->next->tp_psize);
					/* ??? elements of packed size ??? */
			if( req == R_REWRITE )
				C_cal("_cre");
			else
				C_cal("_opn");
			C_asp(pointer_size + word_size);
			break;

		case R_PUT:
		case R_GET:
			CodeDAddress(left);
			if( req == R_PUT )
				C_cal("_put");
			else
				C_cal("_get");
			C_asp(pointer_size);
			break;

		case R_PAGE:
			CodeDAddress(left);
			C_cal("_pag");
			C_asp(pointer_size);
			break;

		case R_PACK:	{
			label lba = tp->arr_ardescr;

			CodeDAddress(left);
			arg = arg->nd_right;
			left = arg->nd_left;
			CodePExpr(left);
			arg = arg->nd_right;
			left = arg->nd_left;
			CodeDAddress(left);
			C_lae_dlb(left->nd_type->arr_ardescr, (arith) 0);
			C_lae_dlb(lba, (arith) 0);
			C_cal("_pac");
			C_asp(4 * pointer_size + word_size);
			break;
		}

		case R_UNPACK:	{
			/* change sequence of arguments of the library routine
			   _unp to merge code of R_PACK and R_UNPACK.
			*/
			label lba, lbz = tp->arr_ardescr;

			CodeDAddress(left);
			arg = arg->nd_right;
			left = arg->nd_left;
			CodeDAddress(left);
			lba = left->nd_type->arr_ardescr;
			arg = arg->nd_right;
			left = arg->nd_left;
			CodePExpr(left);
			C_lae_dlb(lbz, (arith) 0);
			C_lae_dlb(lba, (arith) 0);
			C_cal("_unp");
			C_asp(4 * pointer_size + word_size);
			break;
		}

		case R_NEW:
		case R_DISPOSE:
			CodeDAddress(left);
			C_loc(PointedtoType(tp)->tp_size);
			if( req == R_NEW )
				C_cal("_new");
			else
				C_cal("_dis");
			C_asp(pointer_size + word_size);
			break;

		case R_MARK:
		case R_RELEASE:
			CodeDAddress(left);
			if( req == R_MARK )
				C_cal("_sav");
			else
				C_cal("_rst");
			C_asp(pointer_size);
			break;

		case R_HALT:
			if( left )
				CodePExpr(left);
			else
				C_zer(int_size);
			C_cal("_hlt");			/* can't return */
			C_asp(int_size);	/* help the optimizer(s) */
			break;

		default:
			crash("(CodeStd)");
			/*NOTREACHED*/
	}
}

Long2Int()
{
	/* convert a long to integer */

	if (int_size == long_size) return;

	C_loc(long_size);
	C_loc(int_size);
	C_cii();
}

Int2Long()
{
	/* convert integer to long */

	if (int_size == long_size) return;
	C_loc(int_size);
	C_loc(long_size);
	C_cii();
}

Int2Real(size)		/* size is different for integers and longs */
arith size;
{
	/* convert integer to real */
	C_loc(size);
	C_loc(real_size);
	C_cif();
}

Real2Int()
{
	/* convert real to integer */
	C_loc(real_size);
	C_loc(int_size);
	C_cfi();
}

RangeCheck(tpl, tpr)
	register struct type *tpl, *tpr;
{
	/*	Generate a range check if neccessary
	*/

	arith llo, lhi, rlo, rhi;

	if( bounded(tpl) )	{
		/* in this case we might need a range check */
		if( !bounded(tpr) )
			/* yes, we need one */
			genrck(tpl);
		else	{
			/* both types are restricted. check the bounds to see
			   whether we need a range check.  We don't need one
			   if the range of values of the right hand side is a
			   subset of the range of values of the left hand side.
			*/
			getbounds(tpl, &llo, &lhi);
			getbounds(tpr, &rlo, &rhi);
			if( llo > rlo || lhi < rhi )
				genrck(tpl);
		}
	}
}

genrck(tp)
	register struct type *tp;
{
	/*	Generate a range check descriptor for type "tp" when
		necessary. Return its label.
	*/

	arith lb, ub;
	register label o1;
	int newlabel = 0;

	if( options['R'] ) return;

	getbounds(tp, &lb, &ub);

	if( tp->tp_fund == T_SUBRANGE )	{
		if( !(o1 = tp->sub_rck) )	{
			tp->sub_rck = o1 = ++data_label;
			newlabel = 1;
		}
	}
	else if( !(o1 = tp->enm_rck) )	{
		tp->enm_rck = o1 = ++data_label;
		newlabel = 1;
	}
	if( newlabel )	{
		C_df_dlb(o1);
		C_rom_cst(lb);
		C_rom_cst(ub);
	}
	C_lae_dlb(o1, (arith) 0);
	C_rck(word_size);
}

CodePExpr(nd)
	register struct node *nd;
{
	/*	Generate code to push the value of the expression "nd"
		on the stack.
	*/

	struct desig designator;
	struct type *tp = BaseType(nd->nd_type);
	
	designator = InitDesig;
	CodeExpr(nd, &designator, NO_LABEL);
	if( tp->tp_fund & (T_ARRAY | T_RECORD) )
		CodeAddress(&designator);
	else
		CodeValue(&designator, nd->nd_type);
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
	/*	Generate code to store the expression on the stack
		into the designator "nd".
	*/

	struct desig designator;
	
	designator = InitDesig;
	CodeDesig(nd, &designator);
	CodeStore(&designator, nd->nd_type);
}

RegisterMessages(df)
	register struct def *df;
{
	register struct type *tp;

	for( ; df; df = df->df_nextinscope )	{
		if( df->df_kind == D_VARIABLE && !(df->df_flags & D_NOREG) ) {
			/* Examine type and size
			*/
			tp = BaseType(df->df_type);
			if( df->df_flags & D_VARPAR || tp->tp_fund & T_POINTER )
				C_ms_reg(df->var_off, pointer_size,
					 reg_pointer, 0);

			else if( df->df_flags & D_LOOPVAR )
				C_ms_reg(df->var_off, tp->tp_size, reg_loop,2);
			else if( tp->tp_fund & T_NUMERIC )
				C_ms_reg(df->var_off, tp->tp_size,
				tp->tp_fund == T_REAL ? reg_float : reg_any, 0);
		}
	}
}
