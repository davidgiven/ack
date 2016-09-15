/*	L A B E L   H A N D L I N G	*/

#include	<alloc.h>
#include	<em.h>

#include    "parameters.h"
#include	"LLlex.h"
#include	"def.h"
#include	"idf.h"
#include	"main.h"
#include	"node.h"
#include	"scope.h"
#include	"type.h"


DeclLabel(nd)
	struct node *nd;
{
	struct def *df;

	if( !(df = define(nd->nd_IDF, CurrentScope, D_LABEL)) ) {
		node_error(nd, "label %s redeclared", nd->nd_IDF->id_text);
	}
	else	{
		df->lab_no = ++text_label;
		nd->nd_def = df;
	}
}

chk_labels(Slevel)
{
	register struct node *labnd = BlockScope->sc_lablist;
	register struct def *df;

	while( labnd )	{
		df = labnd->nd_def;
		if( Slevel == 1 )	{
			if( !df->lab_level )
				if( df->lab_next )
				/* jump to undefined label */
					error("jump to undefined label %s",
							df->df_idf->id_text);
				else
					warning(
					  "label %s declared but never defined",
							df->df_idf->id_text);
		}
		else if( df->lab_level == Slevel )
			df->lab_level = -1;
		else if( !df->lab_level )	{
			struct lab *plab = df->lab_next;

			while( plab )	{
				if( plab->lb_level == Slevel )
					plab->lb_level--;
				plab = plab->lb_next;
			}
		}
		labnd = labnd->nd_next;
	}
}

TstLabel(nd, Slevel)
	register struct node *nd;
{
	register struct def *df;

	df = lookfor(nd, CurrVis, 0);
	if( df->df_kind == D_ERROR )	{
		node_error(nd, "label %s not declared", df->df_idf->id_text);
		df->df_kind = D_LABEL;
		nd->nd_def = df;
		nd->nd_next = BlockScope->sc_lablist;
		BlockScope->sc_lablist = nd;
	}
	else
		FreeNode(nd);

	df->df_flags = D_USED;
	if( !df->lab_level )	{
		/* forward jump */
		register struct lab *labelptr;

		labelptr = new_lab();
		labelptr->lb_next = df->lab_next;
		df->lab_next = labelptr;
		if( df->df_scope == BlockScope )	{
			/* local jump */
			labelptr->lb_level = Slevel;
			CodeLabel(df, 1);
		}
		else	{
			/* non-local jump, only permitted to
			   outermost level (ISO 6.8.1 Note 2)
			*/
			labelptr->lb_level = 1;
			CodeLabel(df, 0);
		}
	}
	else if( df->lab_level == -1 || df->lab_level > Slevel )
		node_error(nd, "illegal jump to label %s", df->df_idf->id_text);
	else
		CodeLabel(df, 1);
}

DefLabel(nd, Slevel)
	register struct node *nd;
{
	register struct def *df;

	if( !(df = lookup(nd->nd_IDF, BlockScope, D_INUSE)) )	{
		node_error(nd, "label %s must be declared in same block"
							, nd->nd_IDF->id_text);
		df = define(nd->nd_IDF, BlockScope, D_LABEL);
		nd->nd_def = df;
		df->lab_no = ++text_label;
		nd->nd_next = BlockScope->sc_lablist;
		BlockScope->sc_lablist = nd;
	}
	else FreeNode(nd);

	df->df_flags |= D_SET;
	if( df->lab_level)
		node_error(nd, "label %s already defined", nd->nd_IDF->id_text);
	else	{
		register struct lab *labelptr;

		df->lab_level = Slevel;
		labelptr = df->lab_next;
		while( labelptr )	{
			if( labelptr->lb_level < Slevel )	{
				node_error(nd, "illegal jump to label %s",
							nd->nd_IDF->id_text);
				return;
			}
			labelptr = labelptr->lb_next;
		}
		C_df_ilb(df->lab_no);
	}
}

CodeLabel(df, local)
	register struct def *df;
{
	if( err_occurred ) return;

	if( local )
		C_bra(df->lab_no);
	else	{
		/* non-local jump */
		int level = df->df_scope->sc_level;

		if( !df->lab_descr )	{
			/* generate label for goto descriptor */
			df->lab_descr = ++data_label;
			C_ina_dlb(data_label);
		}
		/* perform the jump */
		C_lae_dlb(df->lab_descr, (arith) 0);

		/* LB of target procedure */
		if( level > 0 )
			C_lxl((arith) proclevel - level);
		else
			C_zer(pointer_size);
		C_cal("_gto");
		C_asp( 2 * pointer_size);
	}
}
