/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* T Y P E   E Q U I V A L E N C E */

/* $Header$ */

/*	Routines for testing type equivalence, type compatibility, and
	assignment compatibility
*/
#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>

#include	"type.h"
#include	"LLlex.h"
#include	"idf.h"
#include	"def.h"
#include	"node.h"
#include	"warning.h"

extern char *sprint();

int
TstTypeEquiv(tp1, tp2)
	struct type *tp1, *tp2;
{
	/*	test if two types are equivalent.
	*/

	return     tp1 == tp2
		||
		   tp1 == error_type
		||
		   tp2 == error_type;
}

int
TstParEquiv(tp1, tp2)
	register struct type *tp1, *tp2;
{
	/*	test if two parameter types are equivalent. This routine
		is used to check if two different procedure declarations
		(one in the definition module, one in the implementation
		module) are equivalent. A complication comes from dynamic
		arrays.
	*/
	
	return
		   TstTypeEquiv(tp1, tp2)
		||
		   (
		     IsConformantArray(tp1)
		   &&
		     IsConformantArray(tp2)
		   &&
		     TstTypeEquiv(tp1->arr_elem, tp2->arr_elem)
		   );
}

int
TstProcEquiv(tp1, tp2)
	struct type *tp1, *tp2;
{
	/*	Test if two procedure types are equivalent. This routine
		may also be used for the testing of assignment compatibility
		between procedure variables and procedures.
	*/
	register struct paramlist *p1, *p2;

	/* First check if the result types are equivalent
	*/
	if (! TstTypeEquiv(ResultType(tp1), ResultType(tp2))) return 0;

	p1 = ParamList(tp1);
	p2 = ParamList(tp2);

	/* Now check the parameters
	*/
	while (p1 && p2) {
		if (IsVarParam(p1) != IsVarParam(p2) ||
		    !TstParEquiv(TypeOfParam(p1), TypeOfParam(p2))) return 0;
		p1 = p1->par_next;
		p2 = p2->par_next;
	}

	/* Here, at least one of the parameterlists is exhausted.
	   Check that they are both.
	*/
	return p1 == p2;
}

int
TstCompat(tp1, tp2)
	register struct type *tp1, *tp2;
{
	/*	test if two types are compatible. See section 6.3 of the
		Modula-2 Report for a definition of "compatible".
	*/

	if (TstTypeEquiv(tp1, tp2)) return 1;

	tp1 = BaseType(tp1);
	tp2 = BaseType(tp2);
	if (tp2 != intorcard_type &&
	    (tp1 == intorcard_type || tp1 == address_type)) {
		struct type *tmp = tp2;
		
		tp2 = tp1;
		tp1 = tmp;
	}

	return	tp1 == tp2
	    ||
		(  tp2 == intorcard_type
		&&
		   (tp1 == int_type || tp1 == card_type || tp1 == address_type)
		)
	    ||
		(  tp2 == address_type
		&& 
	          ( tp1 == card_type || tp1->tp_fund == T_POINTER)
		)
	;
}

int
TstAssCompat(tp1, tp2)
	register struct type *tp1, *tp2;
{
	/*	Test if two types are assignment compatible.
		See Def 9.1.
	*/
	register struct type *tp;

	if (TstCompat(tp1, tp2)) return 1;

	tp1 = BaseType(tp1);
	tp2 = BaseType(tp2);

	if ((tp1->tp_fund & T_INTORCARD) &&
	    (tp2->tp_fund & T_INTORCARD)) return 1;

	if ((tp1->tp_fund == T_REAL) &&
	    (tp2->tp_fund == T_REAL)) return 1;

	if (tp1->tp_fund == T_PROCEDURE &&
	    tp2->tp_fund == T_PROCEDURE) {
		return TstProcEquiv(tp1, tp2);
	}

	if (tp1->tp_fund == T_ARRAY) {
		/* check for string
		*/
		arith size;

		if (IsConformantArray(tp1)) return 0;

		tp = IndexType(tp1);
		if (tp->tp_fund == T_SUBRANGE) {
			size = tp->sub_ub - tp->sub_lb + 1;
		}
		else	size = tp->enm_ncst;
	    	tp1 = BaseType(tp1->arr_elem);
	    	return
			tp1 == char_type
		    &&	(tp2->tp_fund  == T_STRING && size >= tp2->tp_size)
			;
	}

	return 0;
}

int
TstParCompat(parno, formaltype, VARflag, nd, edf)
	register struct type *formaltype;
	struct node **nd;
	struct def *edf;
{
	/*	Check type compatibility for a parameter in a procedure call.
		Assignment compatibility may do if the parameter is
		a value parameter.
		Otherwise, a conformant array may do, or an ARRAY OF (WORD|BYTE)
		may do too.
		Or: a WORD may do.
	*/
	register struct type *actualtype = (*nd)->nd_type;
	char ebuf[256];
	char ebuf1[256];

	if (edf) {
		sprint(ebuf, "\"%s\", parameter %d: %%s", edf->df_idf->id_text, parno);
	}
	else sprint(ebuf, "parameter %d: %%s", parno);

	if (
		TstTypeEquiv(formaltype, actualtype)
	    ||
		( !VARflag && ChkAssCompat(nd, formaltype, (char *) 0))
	    ||
		(  formaltype == address_type 
		&& actualtype->tp_fund == T_POINTER
		)
	    ||
		(  formaltype == word_type
		&& 
		   (  actualtype->tp_size == word_size
		   ||
		      (  !VARflag
		      &&
			 actualtype->tp_size <= word_size
		      )
		   )
		)
	    ||
		(  formaltype == byte_type
		&& actualtype->tp_size == (arith) 1
		)
	    ||
		(  IsConformantArray(formaltype)
		&&
		   (  formaltype->arr_elem == word_type
		   || formaltype->arr_elem == byte_type
		   ||
		      (  actualtype->tp_fund == T_ARRAY
		      && TstTypeEquiv(formaltype->arr_elem,actualtype->arr_elem)
		      )
		   ||
		      (  actualtype->tp_fund == T_STRING
		      && TstTypeEquiv(formaltype->arr_elem, char_type)
		      )
		   )
		)
	)
		return 1;
	if (VARflag && TstCompat(formaltype, actualtype)) {
		if (formaltype->tp_size == actualtype->tp_size) {
			sprint(ebuf1, ebuf, "identical types required");
			node_warning(*nd,
				     W_OLDFASHIONED,
				     ebuf1);
			return 1;
		}
		sprint(ebuf1, ebuf, "equal sized types required");
		node_error(*nd, ebuf1);
		return 0;
	}
				
	sprint(ebuf1, ebuf, "type incompatibility");
	node_error(*nd, ebuf1);
	return 0;
}

CompatCheck(nd, tp, message, fc)
	struct node **nd;
	struct type *tp;
	char *message;
	int (*fc)();
{
	if (! (*fc)(tp, (*nd)->nd_type)) {
		if (message) {
			node_error(*nd, "type incompatibility in %s", message);
		}
		return 0;
	}
	MkCoercion(nd, tp);
	return 1;
}

ChkAssCompat(nd, tp, message)
	struct node **nd;
	struct type *tp;
	char *message;
{
	/*	Check assignment compatibility of node "nd" with type "tp".
		Give an error message when it fails
	*/

	return CompatCheck(nd, tp, message, TstAssCompat);
}

ChkCompat(nd, tp, message)
	struct node **nd;
	struct type *tp;
	char *message;
{
	/*	Check compatibility of node "nd" with type "tp".
		Give an error message when it fails
	*/

	return CompatCheck(nd, tp, message, TstCompat);
}
