/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* T Y P E   E Q U I V A L E N C E */

/* $Id$ */

/*	Routines for testing type equivalence, type compatibility, and
	assignment compatibility
*/
#include "parameters.h"
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
#include	"main.h"
#include	"Lpars.h"

extern char *sprint();

int
TstTypeEquiv(tp1, tp2)
	t_type *tp1, *tp2;
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
	register t_type *tp1, *tp2;
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
	t_type *tp1, *tp2;
{
	/*	Test if two procedure types are equivalent. This routine
		may also be used for the testing of assignment compatibility
		between procedure variables and procedures.
	*/
	register t_param *p1, *p2;

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
	register t_type *tp1, *tp2;
{
	/*	test if two types are compatible. See section 6.3 of the
		Modula-2 Report for a definition of "compatible".
	*/

	if (TstTypeEquiv(tp1, tp2)) return 1;

	tp1 = BaseType(tp1);
	tp2 = BaseType(tp2);
	if (tp2->tp_fund != T_INTORCARD &&
	    (tp1->tp_fund == T_INTORCARD || tp1 == address_type)) {
		t_type *tmp = tp2;
		
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
		(  tp2 == longintorcard_type
		&&
		   (tp1 == longint_type || tp1 == longcard_type || tp1 == address_type)
		)
	    ||
		(  tp2 == address_type
		&& 
	          ( tp1->tp_fund == T_CARDINAL || tp1->tp_fund == T_POINTER)
		)
	;
}

int
TstAssCompat(tp1, tp2)
	register t_type *tp1, *tp2;
{
	/*	Test if two types are assignment compatible.
		See Def 9.1.
	*/

	if (TstCompat(tp1, tp2)) return 1;

	tp1 = BaseType(tp1);
	tp2 = BaseType(tp2);

	if (((tp1->tp_fund & T_INTORCARD) || tp1 == address_type) &&
	    ((tp2->tp_fund & T_INTORCARD) || tp2 == address_type)) return 1;

	if ((tp1->tp_fund == T_REAL) &&
	    (tp2->tp_fund == T_REAL)) return 1;

	if (tp1->tp_fund == T_PROCEDURE &&
	    tp2->tp_fund == T_PROCEDURE) {
		return TstProcEquiv(tp1, tp2);
	}

	if (tp1->tp_fund == T_ARRAY) {
		/* check for string
		*/
		if (IsConformantArray(tp1)) return 0;

	    	return
			BaseType(tp1->arr_elem) == char_type
		    &&	tp2->tp_fund  == T_STRING
		    &&  (tp1->arr_high - tp1->arr_low + 1) >= tp2->tp_size
			;
	}

	return 0;
}

char *
incompat(tp1, tp2)
	register t_type *tp1, *tp2;
{
	
	if (tp1->tp_fund == T_HIDDEN || tp2->tp_fund == T_HIDDEN) {
		return "properties of opaque type are hidden; illegal use";
	}
	return "type incompatibility";
}

int
TstParCompat(parno, formaltype, VARflag, nd, edf)
	register t_type *formaltype;
	t_node **nd;
	t_def *edf;
{
	/*	Check type compatibility for a parameter in a procedure call.
		Assignment compatibility may do if the parameter is
		a value parameter.
		Otherwise, a conformant array may do, or an ARRAY OF (WORD|BYTE)
		may do too.
		Or: a WORD may do.
	*/
	register t_type *actualtype = (*nd)->nd_type;
	char ebuf[256];

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
		      &&
			 ! IsConformantArray(actualtype)
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
#ifndef STRICT_3RD_ED
	if (! options['3'] && VARflag && TstCompat(formaltype, actualtype)) {
		if (formaltype->tp_size == actualtype->tp_size) {
			node_warning(*nd,
				     W_OLDFASHIONED,
				     ebuf,
				     "identical types required");
			return 1;
		}
		node_error(*nd, ebuf, "equal sized types required");
		return 0;
	}
#endif
	node_error(*nd, ebuf, incompat(formaltype, actualtype));
	return 0;
}

CompatCheck(nd, tp, message, fc)
	register t_node **nd;
	t_type *tp;
	char *message;
	int (*fc)();
{
	if (! (*fc)(tp, (*nd)->nd_type)) {
		if (message) {
			node_error(*nd, "%s in %s",
					incompat(tp, (*nd)->nd_type),
					message);
		}
		return 0;
	}
	MkCoercion(nd, tp);
	return 1;
}

ChkAssCompat(nd, tp, message)
	t_node **nd;
	t_type *tp;
	char *message;
{
	/*	Check assignment compatibility of node "nd" with type "tp".
		Give an error message when it fails
	*/

	if ((*nd)->nd_symb == STRING) {
		TryToString((*nd), tp);
	}
	return CompatCheck(nd, tp, message, TstAssCompat);
}

ChkCompat(nd, tp, message)
	t_node **nd;
	t_type *tp;
	char *message;
{
	/*	Check compatibility of node "nd" with type "tp".
		Give an error message when it fails
	*/

	return CompatCheck(nd, tp, message, TstCompat);
}
