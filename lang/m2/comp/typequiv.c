/* T Y P E   E Q U I V A L E N C E */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

/*	Routines for testing type equivalence, type compatibility, and
	assignment compatibility
*/
#include	<em_arith.h>
#include	<em_label.h>

#include	"type.h"
#include	"def.h"

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
	register struct type *tp1, *tp2;
{
	/*	Test if two procedure types are equivalent. This routine
		may also be used for the testing of assignment compatibility
		between procedure variables and procedures.
	*/
	register struct paramlist *p1, *p2;

	/* First check if the result types are equivalent
	*/
	if (! TstTypeEquiv(tp1->next, tp2->next)) return 0;

	p1 = tp1->prc_params;
	p2 = tp2->prc_params;

	/* Now check the parameters
	*/
	while (p1 && p2) {
		if (p1->par_var != p2->par_var ||
		    !TstParEquiv(p1->par_type, p2->par_type)) return 0;
		p1 = p1->next;
		p2 = p2->next;
	}

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

	if (tp1->tp_fund == T_SUBRANGE) tp1 = tp1->next;
	if (tp2->tp_fund == T_SUBRANGE) tp2 = tp2->next;

	return	tp1 == tp2
	    ||
		(  tp1 == intorcard_type
		&&
		   (tp2 == int_type || tp2 == card_type)
		)
	    ||
		(  tp2 == intorcard_type
		&&
		   (tp1 == int_type || tp1 == card_type)
		)
	    ||
		(tp1 == char_type && tp2 == charc_type)
	    ||
		(tp2 == char_type && tp1 == charc_type)
	    ||
		(  tp1 == address_type
		&& 
	          (  tp2 == card_type
		  || tp2 == intorcard_type
		  || tp2->tp_fund == T_POINTER
		  )
		)
	    ||
		(  tp2 == address_type
		&& 
	          (  tp1 == card_type
		  || tp1 == intorcard_type
		  || tp1->tp_fund == T_POINTER
		  )
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

	if (tp1->tp_fund == T_SUBRANGE) tp1 = tp1->next;
	if (tp2->tp_fund == T_SUBRANGE) tp2 = tp2->next;

	if ((tp1->tp_fund & T_INTORCARD) &&
	    (tp2->tp_fund & T_INTORCARD)) return 1;

	if (tp1 == char_type && tp2 == charc_type) return 1;

	if (tp1->tp_fund == T_ARRAY) {
		/* check for string
		*/
		arith size;

		if (!(tp = tp1->next)) return 0;

		if (tp->tp_fund == T_SUBRANGE) {
			size = tp->sub_ub - tp->sub_lb + 1;
		}
		else	size = tp->enm_ncst;
	    	tp1 = tp1->arr_elem;
		if (tp1->tp_fund == T_SUBRANGE) tp1 = tp1->next;
	    	return
			tp1 == char_type
		    &&
			(
			    tp2 == charc_type
			||
			    (tp2->tp_fund == T_STRING && size >= tp2->tp_size)
			);
	}

	return 0;
}

int
TstParCompat(formaltype, actualtype, VARflag)
	struct type *formaltype, *actualtype;
{
	/*	Check type compatibility for a parameter in a procedure
		call. Ordinary type compatibility is sufficient in any case.
		Assignment compatibility may do if the parameter is
		a value parameter.
		Otherwise, a conformant array may do, or an ARRAY OF WORD
		may do too.
		Or: a WORD may do.
	*/

	return
		TstCompat(formaltype, actualtype)
	    ||
		( !VARflag && TstAssCompat(formaltype, actualtype))
	    ||
		(  formaltype == word_type && actualtype->tp_size == word_size)
	    ||
		(  IsConformantArray(formaltype)
		&&
		   (  formaltype->arr_elem == word_type
		   ||
		      (  actualtype->tp_fund == T_ARRAY
		      && TstTypeEquiv(formaltype->arr_elem,actualtype->arr_elem)
		      )
		   ||
		      (  actualtype->tp_fund == T_STRING
		      && TstTypeEquiv(formaltype->arr_elem, char_type)
		      )
		   )
		);
}
