/* T Y P E   E Q U I V A L E N C E */

/*	Routines for testing type equivalence & type compatibility.
*/

#include    "parameters.h"
#include	"debug.h"

#include	<assert.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"LLlex.h"
#include	"def.h"
#include	"node.h"
#include	"type.h"
#include	"error.h"
#include	"typequiv.h"

int TstTypeEquiv(register struct type *tp1, register struct type *tp2)
{
	/*	test if two types are equivalent.
	*/
	return tp1 == tp2 || tp1 == error_type || tp2 == error_type;
}

arith IsString(register struct type *tp)
{
	/* string = packed array[1..ub] of char and ub > 1 */
	if( tp->tp_fund & T_STRINGCONST ) return tp->tp_psize;

	if( IsConformantArray(tp) ) return 0;

	if( tp->tp_fund & T_ARRAY && IsPacked(tp) &&
					tp->arr_elem == char_type )	{
		arith lb, ub;

		if( BaseType(IndexType(tp)) != int_type ) return 0;
		getbounds(IndexType(tp), &lb, &ub);
		return (lb == 1 && ub > 1) ? ub : (arith) 0;
	}
	return (arith) 0;
}

int TstStrCompat(register struct type *tp1, register struct type *tp2)
{
	/*	test if two types are compatible string-types.
	*/

	arith ub1, ub2;

	ub1 = IsString(tp1);
	ub2 = IsString(tp2);

	if( !ub1 || !ub2 ) return 0;
	else
		return ub1 == ub2;
}

int TstCompat(register struct type *tp1,register struct type *tp2)
{
	/*	test if two types are compatible. ISO 6.4.5
	*/

	/* clause a */
	if( TstTypeEquiv(tp1, tp2) ) return 1;

	/* clause d */
	if( TstStrCompat(tp1, tp2) ) return 1;

	/* type of NIL is compatible with every pointertype */
	if( tp1->tp_fund & T_POINTER && tp2->tp_fund & T_POINTER )
		return tp1 == tp2 || tp1 == nil_type || tp2 == nil_type;

	/* clause c */
	/* if both types are sets then both must be packed or not */
	if( tp1->tp_fund & T_SET && tp2->tp_fund & T_SET )	{
		if( tp1 == emptyset_type || tp2 == emptyset_type )
			return 1;
		if( IsPacked(tp1) != IsPacked(tp2) )
			return 0;
		if( TstCompat(ElementType(tp1), ElementType(tp2)) )	{
			/*
			if( ElementType(tp1) != ElementType(tp2) )
				warning("base-types of sets not equal");
			*/
			return 1;
		}
		else return 0;
	}

	/* no clause, just check for longs and ints */
	/* BaseType is used in case of array indexing */
	if ((BaseType(tp1) == int_type && tp2 == long_type) ||
			(tp1 == long_type && BaseType(tp2) == int_type))
		return 1;


	/* clause b */
	tp1 = BaseType(tp1);
	tp2 = BaseType(tp2);

	return tp1 == tp2;
}

int TstAssCompat(register struct type *tp1,register struct type *tp2)
{
	/*	test if two types are assignment compatible. ISO 6.4.6
	*/

	/* clauses a, c, d and e */
	if( TstCompat(tp1, tp2) )
		return !(tp1->tp_flags & T_HASFILE);

	/* clause b */
	if( tp1 == real_type )
		return BaseType(tp2) == int_type || BaseType(tp2) == long_type;

	return 0;
}

int TstParEquiv(register struct type *tp1, register struct type *tp2)
{
	/*	Test if two parameter types are equivalent.  ISO 6.6.3.6
	*/
	
	return
		   TstTypeEquiv(tp1, tp2)
		||
		   (
		     IsConformantArray(tp1)
		   &&
		     IsConformantArray(tp2)
		   &&
		     IsPacked(tp1) == IsPacked(tp2)
		   &&
		     TstParEquiv(tp1->arr_elem, tp2->arr_elem)
		   )
		||
		   (
		     (
		      (tp1->tp_fund == T_PROCEDURE && tp2->tp_fund == T_PROCEDURE)
		     ||
		      (tp1->tp_fund == T_FUNCTION && tp2->tp_fund == T_FUNCTION)
		     )
		   &&
		     TstProcEquiv(tp1, tp2)
		   );
}

int TstProcEquiv(register struct type *tp1, register struct type *tp2)
{
	/*	Test if two procedure types are equivalent. ISO 6.6.3.6
	*/
	register struct paramlist *p1, *p2;

	/* First check if the result types are equivalent
	*/
	if( !TstTypeEquiv(ResultType(tp1), ResultType(tp2)) )
		return 0;

	p1 = ParamList(tp1);
	p2 = ParamList(tp2);

	/* Now check the parameters
	*/
	while( p1 && p2 )	{
		if( IsVarParam(p1) != IsVarParam(p2) ||
		    !TstParEquiv(TypeOfParam(p1), TypeOfParam(p2)) ) return 0;
		p1 = p1->next;
		p2 = p2->next;
	}

	/* Here, at least one of the parameterlists is exhausted.
	   Check that they are both.
	*/
	return p1 == p2;
}

int TstParCompat(register struct type *formaltype, register struct type *actualtype,
	int VARflag, struct node *nd, int new_par_section)
{
	/*	Check type compatibility for a parameter in a procedure call.
	*/

	if(
		TstTypeEquiv(formaltype, actualtype)
	    ||
		( !VARflag && TstAssCompat(formaltype, actualtype) )
	    ||
		(  formaltype->tp_fund == T_FUNCTION
		 &&
		   actualtype->tp_fund == T_FUNCTION
		 &&
		   TstProcEquiv(formaltype, actualtype)
		)
	    ||
		(  formaltype->tp_fund == T_PROCEDURE
		 &&
		   actualtype->tp_fund == T_PROCEDURE
		 &&
		   TstProcEquiv(formaltype, actualtype)
		)
	    ||
		(  IsConformantArray(formaltype)
		&&
		   TstConform(formaltype, actualtype, new_par_section)
		)
	) {
		if( !VARflag && IsConformantArray(actualtype) )	{
			node_warning(nd,
				"conformant array used as value parameter");
		}
		return 1;
	}
	else return 0;
}

int TstConform(register struct type *formaltype, register struct type * actualtype, int new_par_section)
{
	/*	Check conformability.
		
		DEVIATION FROM STANDARD (ISO 6.6.3.7.2):
		Allow with value parameters also conformant arrays as actual
		type.(ISO only with var. parameters)

		Do as much checking on indextypes as possible.
	*/

	struct type *formalindextp, *actualindextp;
	arith flb, fub, alb, aub;
	static struct type *lastactual;

	if( !new_par_section )
		/* actualparameters of one conformant-array-specification
		   must be equal
		*/
		return TstTypeEquiv(actualtype, lastactual);

	lastactual = actualtype;

	if( actualtype->tp_fund == T_STRINGCONST )	{
		actualindextp = int_type;
		alb = 1;
		aub = actualtype->tp_psize;
	}
	else if( actualtype->tp_fund == T_ARRAY )	{
		actualindextp = IndexType(actualtype);
		if( bounded(actualindextp) )
			getbounds(actualindextp, &alb, &aub);
	}
	else
		return 0;

	/* clause (d) */
	if( IsPacked(actualtype) != IsPacked(formaltype) )
		return 0;

	formalindextp = IndexType(formaltype);

	/* clause (a) */
	if( !TstCompat(actualindextp, formalindextp) )
		return 0;

	/* clause (b) */
	if( bounded(actualindextp) ||
			actualindextp->tp_fund == T_STRINGCONST ) {
		/* test was necessary because the actual type could be confor-
		   mant !!
		*/
		if( bounded(formalindextp) )	{
			getbounds(formalindextp, &flb, &fub);
			if( alb < flb || aub > fub )
				return 0;
		}
	}

	/* clause (c) */
	if( !IsConformantArray(formaltype->arr_elem) )
		return TstTypeEquiv(actualtype->arr_elem, formaltype->arr_elem);
	else
		return TstConform(formaltype->arr_elem, actualtype->arr_elem,
							       new_par_section);
}
