/* EXPRESSIONS */

{
#include	"debug.h"

#include	<assert.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"LLlex.h"
#include	"chk_expr.h"
#include	"def.h"
#include	"main.h"
#include	"misc.h"
#include	"idf.h"
#include	"node.h"
#include	"scope.h"
#include	"type.h"
}

Constant(register struct node **pnd;)
{
	register struct node **nd = pnd;
} :
%default
	[
		Sign(nd)	{ nd = &((*nd)->nd_right); }
	]?
	[ %default
		UnsignedNumber(nd)
	|
		ConstantIdentifier(nd)
	]
				{ (void) ChkConstant(*pnd); }
|
	STRING			{ *pnd = MkLeaf(Value, &dot);
				  if( ((*pnd)->nd_type = toktype) != char_type )
					RomString(*pnd);
				}
;

Sign(register struct node **pnd;):
	['+' | '-']		{ *pnd = MkLeaf(Uoper, &dot); }
;

UnsignedNumber(register struct node **pnd;):
	[INTEGER | REAL]	{ *pnd = MkLeaf(Value, &dot);
				  (*pnd)->nd_type = toktype;
				}
;

ConstantIdentifier(register struct node **pnd;):
	IDENT			{ *pnd = MkLeaf(Name, &dot);
				}
;

/* ISO section 6.7.1, p. 121 */
Expression(register struct node **pnd;):
	SimpleExpression(pnd)
	[
		/* RelationalOperator substituted inline */
		[ '=' | NOTEQUAL | '<' | '>' | LESSEQUAL | GREATEREQUAL | IN ]
				{ *pnd = MkNode(Boper, *pnd, NULLNODE, &dot); }
		SimpleExpression(&((*pnd)->nd_right))
	]?
;

SimpleExpression(register struct node **pnd;):
	/* ISO 6.7.1: The signs and the adding-operators have equal precedence,
		      and are left-associative.
	*/
	[
		Sign(pnd)
		Term(&((*pnd)->nd_right))
	|
		Term(pnd)
	]
	[
		/* AddingOperator substituted inline */
		[ '+' | '-' | OR ]
				{ *pnd = MkNode(Boper, *pnd, NULLNODE, &dot); }
		Term(&((*pnd)->nd_right))
	]*
;

Term(register struct node **pnd;):
	Factor(pnd)
	[
		/* MultiplyingOperator substituted inline */
		[ '*' | '/' | DIV | MOD | AND ]
				{ *pnd = MkNode(Boper, *pnd, NULLNODE, &dot); }
		Factor(&((*pnd)->nd_right))
	]*
;

Factor(register struct node **pnd;)
{
	register struct def *df;
} :
	/* This is a changed rule, because the grammar as specified in the
	 * reference is not LL(1), and this gives conflicts.
	 */
	%default
	%prefer		/* solve conflicts on IDENT and UnsignedConstant */
	IDENT			{ *pnd = MkLeaf(Name, &dot); }
	[
		/* ISO section 6.7.3, p. 126
		 * IDENT is a FunctionIdentifier
		 */
				{
				  *pnd = MkNode(Call, *pnd, NULLNODE, &dot);
				}
		ActualParameterList(&((*pnd)->nd_right))
	|
		/* IDENT can be a BoundIdentifier or a ConstantIdentifier or
		 * a FunctionIdentifier (no parameterlist), in which case
		 * VariableAccessTail is empty.
		 * It could also be the beginning of a normal VariableAccess
		 * (most likely).
		 */
			{ int class;

			  df = lookfor(*pnd, CurrVis, 1);
			  /* df->df_flags |= D_USED; */
			  if( df->df_type->tp_fund & T_ROUTINE )	{
				/* This part is context-sensitive:
				   is the occurence of the proc/func name
				   a call or not ?
				*/
				if( df->df_type == std_type )
					class = Call;
				else
					class = NameOrCall;
				*pnd = MkNode(class, *pnd, NULLNODE, &dot);
				(*pnd)->nd_symb = '(';
			  }
			}

		VariableAccessTail(pnd)
	]
|
	UnsignedConstant(pnd)
|
	SetConstructor(pnd)
|
	'('			{ /* dummy node to force ChkVariable */
				  *pnd = MkLeaf(Uoper, &dot);
				}
	Expression(&((*pnd)->nd_right))
	')'
|
	NOT			{ *pnd = MkLeaf(Uoper, &dot); }
	Factor(&((*pnd)->nd_right))
;

UnsignedConstant(register struct node **pnd;):
	UnsignedNumber(pnd)
|
	STRING			{ *pnd = MkLeaf(Value, &dot);
				  if( ((*pnd)->nd_type = toktype) != char_type )
					RomString(*pnd);
				}
|
	ConstantIdentifier(pnd)
|
	NIL			{ *pnd = MkLeaf(Value, &dot);
				  (*pnd)->nd_type = nil_type;
				  /* to evaluate NIL = NIL */
				  (*pnd)->nd_INT = 0;
				}
;

SetConstructor(register struct node **pnd;)
{
	register struct node *nd;
} :
	'['		{ dot.tk_symb = SET;
			  *pnd = nd = MkLeaf(Xset, &dot);
			}
		[
			MemberDesignator(nd)
			[ %persistent
				{ nd = nd->nd_right; }
				',' MemberDesignator(nd)
			]*
		]?
	']'
;

MemberDesignator(register struct node *nd;)
{
	struct node *nd1;
} :
	Expression(&nd1)
	[ UPTO			{ nd1 = MkNode(Link, nd1, NULLNODE, &dot); }
	  Expression(&(nd1->nd_right))
	]?
			{ nd->nd_right = MkNode(Link, nd1, NULLNODE, &dot);
			  nd->nd_right->nd_symb = ',';
			}
;

/* ISO section 6.7.2.1, p. 123 */
BooleanExpression(register struct node **pnd;):
	Expression(pnd)
			{ if( ChkExpression(*pnd) &&
						(*pnd)->nd_type != bool_type )
				node_error(*pnd, "boolean expression expected");
			  MarkUsed(*pnd);
			}
;

ActualParameterList(register struct node **pnd;)
{
	register struct node *nd;
} :
	'('
		/* ActualParameter substituted inline */
		Expression(pnd)		{ *pnd = nd =
					     MkNode(Link, *pnd, NULLNODE, &dot);
					  nd->nd_symb = ',';
					}
		[ %persistent
			','		{ nd->nd_right = MkLeaf(Link, &dot);
					  nd = nd->nd_right;
					}
			Expression(&(nd->nd_left))
		]*
	')'
;

/* ISO section 6.5.1, p. 105 */
VariableAccess(register struct node **pnd;):
	/* This is a changed rule, because the grammar as specified in the
	 * reference is not LL(1), and this gives conflicts.
	 *
	 * IDENT is an EntireVariable or
	 * a FieldDesignatorIdentifier (see also 6.8.3.10, p. 132).
	 */
	IDENT				{ *pnd = MkLeaf(Name, &dot); }
	VariableAccessTail(pnd)		{ (void) ChkVariable(*pnd); }
;

VariableAccessTail(register struct node **pnd;):
	/* This is a new rule because the grammar specified by the standard
	 * is not exactly LL(1).
	 */

	 /* empty */
|
	/* PointerVariable or FileVariable
	 */

	'^'			{ *pnd = MkNode(Arrow, NULLNODE, *pnd, &dot); }

	/* At this point the VariableAccess is an IdentifiedVariable
	 * ISO section 6.5.4, p. 107 (IdentifiedVariable: PointerVariable '^'),
	 * or
	 * it is a BufferVariable
	 * ISO section 6.5.5, p. 107 (BufferVariable: FileVariable '^').
	 */

	VariableAccessTail(pnd)
|
	/* ArrayVariable
	 */

	'['			{ *pnd = MkNode(Arrsel, *pnd, NULLNODE, &dot); }
		/* IndexExpression substituted inline */
		Expression(&((*pnd)->nd_right))
		[ %persistent
			','	{ *pnd = MkNode(Arrsel, *pnd, NULLNODE, &dot);
				  (*pnd)->nd_symb = '[';
				}
			Expression(&((*pnd)->nd_right))
		]*
	']'

	/* At this point the VariableAccess is an IndexedVariable
	 * ISO section 6.5.3.2, p. 106
	 */

	VariableAccessTail(pnd)
|
	/* RecordVariable
	 */

	'.'			{ *pnd = MkNode(Link, *pnd, NULLNODE, &dot); }
	/* FieldSpecifier & FieldIdentifier substituted inline */
	IDENT			{ (*pnd)->nd_IDF = dot.TOK_IDF; }

	/* At this point the VariableAccess is a FieldDesignator
	 * ISO section 6.5.3.3, p. 107
	 */

	VariableAccessTail(pnd)
;
