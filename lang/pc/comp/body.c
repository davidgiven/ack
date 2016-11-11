#include    "parameters.h"
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
#include	"misc.h"
#include	"node.h"
#include	"scope.h"
#include	"type.h"

MarkDef(nd, flags, on)
	register struct node *nd;
	unsigned short flags;
{
	while( nd && nd->nd_class != Def ) {
		if( (nd->nd_class == Arrsel) ||
		    (nd->nd_class == LinkDef) )
			nd = nd->nd_left;
		else if( nd->nd_class == Arrow )
			nd = nd->nd_right;
		else break;
	}
	if( nd && (nd->nd_class == Def) ) {
		if( (flags & D_SET) && on &&
		    BlockScope != nd->nd_def->df_scope )
			nd->nd_def->df_flags |= D_SETINHIGH;
		if( on ) {
			/*
			if( (flags & D_SET) &&
			    (nd->nd_def->df_flags & D_WITH) )
				node_warning(nd,
				"variable \"%s\" already referenced in with",
				nd->nd_def->df_idf->id_text);
			*/
			nd->nd_def->df_flags |= flags;
		}
		else
			nd->nd_def->df_flags &= ~flags;
	}
}

void
AssertStat(expp, line)
	register struct node *expp;
	unsigned short line;
{
	struct desig dsr;

	if( !ChkExpression(expp) )
		return;

	if( expp->nd_type != bool_type )	{
		node_error(expp, "type of assertion should be boolean");
		return;
	}

	if( !options['a'] && !err_occurred ) {
		dsr = InitDesig;
		CodeExpr(expp, &dsr, NO_LABEL);
		C_loc((arith)line);
		C_cal("_ass");
	}
}

void
AssignStat(left, right)
	register struct node *left, *right;
{
	register struct type *ltp, *rtp;
	int retval = 0;
	struct desig dsr;

	retval = ChkExpression(right);
	MarkUsed(right);
	retval &= ChkLhs(left);

	ltp = left->nd_type;
	rtp = right->nd_type;

	MarkDef(left, (unsigned short)D_SET, 1);

	if( !retval ) return;

	if( ltp == int_type && rtp == long_type )	{
		right = MkNode(IntReduc, NULLNODE, right, &dot);
		right->nd_type = int_type;
	}
	else if( ltp == long_type && rtp == int_type )	{
		right = MkNode(IntCoerc, NULLNODE, right, &dot);
		right->nd_type = long_type;
	}

	if( !TstAssCompat(ltp, rtp) )	{
		node_error(left, "type incompatibility in assignment");
		return;
	}

	if( left->nd_class == Def &&
	    (left->nd_def->df_flags & D_INLOOP) )	{
		node_error(left, "assignment to a control variable");
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
				Int2Real(rtp->tp_size);

			RangeCheck(ltp, rtp);
		}
		CodeMove(&dsr, left, rtp);
	}

	FreeNode(left);
	FreeNode(right);
}

void
ProcStat(nd)
	register struct node *nd;
{
	if( !ChkCall(nd) ) return;

	if( nd->nd_type )	{
		node_error(nd, "procedure call expected");
		return;
	}
}

void
ChkForStat(nd)
	register struct node *nd;
{
	register struct def *df;
	int retvar = 0;

	retvar = ChkVariable(nd);
	retvar &= ChkExpression(nd->nd_left);
	MarkUsed(nd->nd_left);
	retvar &= ChkExpression(nd->nd_right);
	MarkUsed(nd->nd_right);
	if( !retvar ) return;

	assert(nd->nd_class == Def);

	df = nd->nd_def;

	if( df->df_scope != BlockScope )	{
		node_error(nd, "for loop: control variable must be local");
		return;
	}

	assert(df->df_kind == D_VARIABLE);

	if( df->df_scope != GlobalScope && df->var_off >= 0 )	{
		node_error(nd,
			    "for loop: control variable can't be a parameter");
		MarkDef(nd,(unsigned short)(D_LOOPVAR | D_SET | D_USED), 1);
		return;
	}

	if( !(df->df_type->tp_fund & T_ORDINAL) )	{
		node_error(nd, "for loop: control variable must be ordinal");
		MarkDef(nd,(unsigned short)(D_LOOPVAR | D_SET | D_USED), 1);
		return;
	}

	if( !TstCompat(df->df_type, nd->nd_left->nd_type) )
		node_error(nd,
		  "for loop: initial value incompatible with control variable");

	if( !TstCompat(df->df_type, nd->nd_right->nd_type) )
		node_error(nd,
		    "for loop: final value incompatible with control variable");
	
	if( df->df_type == long_type )
		node_error(nd, "for loop: control variable can not be a long");

	if( df->df_flags & D_INLOOP )
		node_error(nd, "for loop: control variable already used");

	if( df->df_flags & D_SETINHIGH )
		node_error(nd,
			    "for loop: control variable already set in block");

	MarkDef(nd,(unsigned short) (D_LOOPVAR | D_INLOOP | D_SET | D_USED), 1);

	return;
}

void
EndForStat(nd)
	register struct node *nd;
{
	register struct def *df;

	df = nd->nd_def;

	if( (df->df_scope != BlockScope) ||
	    (df->df_scope != GlobalScope && df->var_off >= 0) ||
	    !(df->df_type->tp_fund & T_ORDINAL)
	  )
		return;

	MarkDef(nd,(unsigned short) (D_INLOOP | D_SET), 0);
}

arith
CodeInitFor(nd, priority)
	register struct node *nd;
{
	/* Push final-value, the value may only be evaluated
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

CodeFor(nd, stepsize, l1, l2)
	struct node *nd;
	label l1, l2;
{
	/* Test if loop has to be done */
	if( stepsize == 1 )	/* TO */
		C_bgt(l2);
	else			/* DOWNTO */
		C_blt(l2);

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
	C_asp(int_size);
}

void
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

	MarkDef(nd, (unsigned short)(D_USED | D_SET | D_WITH), 1);
	/*
	if( (nd->nd_class == Arrow) &&
	    (nd->nd_right->nd_type->tp_fund & T_FILE) ) {
		nd->nd_right->nd_def->df_flags |= D_WITH;
	}
	*/

	scl = new_scopelist();
	scl->sc_scope = nd->nd_type->rec_scope;
	scl->next = CurrVis;
	CurrVis = scl;

	if( err_occurred ) return;

	/* Generate code */

	CodeDAddress(nd);

	wds = new_withdesig();
	wds->w_next = WithDesigs;
	WithDesigs = wds;
	wds->w_scope = scl->sc_scope;

	/* create a desig structure for the temporary */
	ds.dsg_kind = DSG_FIXED;
	ds.dsg_offset = NewPtr(1);
	ds.dsg_name = 0;

	/* need some pointertype to store pointer */
	CodeStore(&ds, nil_type);

	/* record is indirectly available */
	ds.dsg_kind = DSG_PFIXED;
	wds->w_desig = ds;
}

EndWith(saved_scl, nd)
	struct scopelist *saved_scl;
	struct node *nd;
{
	/* restore scope, and release structures */
	struct scopelist *scl;
	struct withdesig *wds;
	struct node *nd1;

	while( CurrVis != saved_scl )	{

		/* release scopelist */
		scl = CurrVis;
		CurrVis = CurrVis->next;
		free_scopelist(scl);

		if( WithDesigs == 0 )
			continue;	/* we didn't generate any code */

		/* release temporary */
		FreePtr(WithDesigs->w_desig.dsg_offset);

		/* release withdesig */
		wds = WithDesigs;
		WithDesigs = WithDesigs->w_next;
		free_withdesig(wds);
	}

	for( nd1 = nd; nd1 != NULLNODE; nd1 = nd1->nd_right ) {
		MarkDef(nd1->nd_left, (unsigned short)(D_WITH), 0);
	}

	FreeNode(nd);
}
