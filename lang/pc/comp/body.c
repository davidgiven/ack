#include	"debug.h"

#include	<alloc.h>
#include	<assert.h>
#include	<em.h>

#include	"LLlex.h"
#include	"chk_expr.h"
#include	"def.h"
#include	"desig.h"
#include	"idf.h"
#include	"main.h"
#include	"node.h"
#include	"scope.h"
#include	"type.h"


AssignStat(left, right)
	register struct node *left, *right;
{
	register struct type *ltp, *rtp;
	struct desig dsr;

	if( !(ChkExpression(right) && ChkLhs(left)) )
		return;

	ltp = left->nd_type;
	rtp = right->nd_type;

	if( !TstAssCompat(ltp, rtp) )	{
		node_error(left, "type incompatibility in assignment");
		return;
	}

	if( rtp == emptyset_type )
		right->nd_type = ltp;

	if( !err_occurred )	{
		dsr = InitDesig;
		CodeExpr(right, &dsr, NO_LABEL);

		if( rtp->tp_fund & (T_ARRAY | T_RECORD) )
			CodeAddress(&dsr);
		else	{
			CodeValue(&dsr, rtp);

			if( ltp == real_type && BaseType(rtp) == int_type )
				Int2Real();

			RangeCheck(ltp, rtp);
		}
		CodeMove(&dsr, left, rtp);
	}

	FreeNode(left);
	FreeNode(right);
}

ProcStat(nd)
	register struct node *nd;
{
	if( !ChkCall(nd) ) return;

	if( nd->nd_type )	{
		node_error(nd, "procedure call expected");
		return;
	}
}

ChkForStat(nd)
	register struct node *nd;
{
	register struct def *df;

	if( !(ChkVariable(nd) && ChkExpression(nd->nd_left) &&
						ChkExpression(nd->nd_right)) )
		return;
	
	assert(nd->nd_class == Def);

	df = nd->nd_def;

	if( df->df_scope != BlockScope )	{
		node_error(nd, "for loop: control variable must be local");
		return;
	}

	assert(df->df_kind == D_VARIABLE);

	if( df->df_scope != GlobalScope && df->var_off >= 0 )	{
	       node_error(nd,"for loop: control variable can't be a parameter");
	       return;
	}

	if( !(df->df_type->tp_fund & T_ORDINAL) )	{
		node_error(nd, "for loop: control variable must be ordinal");
		return;
	}

	if( !TstCompat(df->df_type, nd->nd_left->nd_type) )
		node_error(nd,
		  "for loop: initial value incompatible with control variable");

	if( !TstCompat(df->df_type, nd->nd_right->nd_type) )
		node_error(nd,
		    "for loop: final value incompatible with control variable");
	
	df->df_flags |= D_LOOPVAR;

	return;
}

arith
CodeInitFor(nd, priority)
	register struct node *nd;
{
	/* Push init-value or final-value, the value may only be evaluated
	   once, so generate a temporary for it, when not a constant.
	*/

	arith tmp;

	CodePExpr(nd);
	if( nd->nd_class != Value )	{
		tmp = NewInt(priority);
		C_dup(int_size);
		C_stl(tmp);
		return tmp;
	}
	return (arith) 0;
}

CodeFor(nd, stepsize, l1, l2, tmp1)
	struct node *nd;
	label l1, l2;
	arith tmp1;
{
	/* Test if loop has to be done */
	if( stepsize == 1 )	/* TO */
		C_bgt(l2);
	else			/* DOWNTO */
		C_blt(l2);

	/* Store init-value in control-variable */
	if( tmp1 )
		C_lol(tmp1);
	else
		CodePExpr(nd->nd_left);

	/* Label at begin of the body */
	C_df_ilb(l1);

	RangeCheck(nd->nd_type, nd->nd_left->nd_type);
	CodeDStore(nd);
}

CodeEndFor(nd, stepsize, l1, l2, tmp2)
	struct node *nd;
	label l1, l2;
	arith tmp2;
{
	/* Test if loop has to be done once more */
	CodePExpr(nd);
	C_dup(int_size);
	if( tmp2 )
		C_lol(tmp2);
	else
		CodePExpr(nd->nd_right);
	C_beq(l2);

	/* Increment/decrement the control-variable */
	if( stepsize == 1 )	/* TO */
		C_inc();
	else			/* DOWNTO */
		C_dec();
	C_bra(l1);

	/* Exit label */
	C_df_ilb(l2);
}

WithStat(nd)
	struct node *nd;
{
	struct withdesig *wds;
	struct desig ds;
	struct scopelist *scl;

	if( nd->nd_type->tp_fund != T_RECORD )	{
		node_error(nd, "record variable expected");
		return;
	}

	if( err_occurred ) return;

	/* Generate code */

	CodeDAddress(nd);

	wds = new_withdesig();
	wds->w_next = WithDesigs;
	WithDesigs = wds;
	wds->w_scope = nd->nd_type->rec_scope;

	/* create a desig structure for the temporary */
	ds.dsg_kind = DSG_FIXED;
	ds.dsg_offset = NewPtr(1);
	ds.dsg_name = 0;

	/* need some pointertype to store pointer */
	CodeStore(&ds, nil_type);

	/* record is indirectly available */
	ds.dsg_kind = DSG_PFIXED;
	wds->w_desig = ds;

	scl = new_scopelist();
	scl->sc_scope = wds->w_scope;
	scl->next = CurrVis;
	CurrVis = scl;
}

EndWith(saved_scl, nd)
	struct scopelist *saved_scl;
	struct node *nd;
{
	/* restore scope, and release structures */
	struct scopelist *scl;
	struct withdesig *wds;

	while( CurrVis != saved_scl )	{

		/* release scopelist */
		scl = CurrVis;
		CurrVis = CurrVis->next;
		free_scopelist(scl);

		/* release temporary */
		FreePtr(WithDesigs->w_desig.dsg_offset);

		/* release withdesig */
		wds = WithDesigs;
		WithDesigs = WithDesigs->w_next;
		free_withdesig(wds);
	}
	FreeNode(nd);
}
