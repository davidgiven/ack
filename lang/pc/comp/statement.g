/* S T A T E M E N T S */
{
#include	<alloc.h>
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

int slevel = 0;		/* nesting level of statements */
}


/* ISO section 6.8.3.2, p. 128 */
CompoundStatement:
	BEGIN StatementSequence END
;

/* ISO section 6.8.3.1, p. 128 */
StatementSequence:
	Statement
	[ %persistent
		';' Statement
	]*
					{ chk_labels(slevel + 1); }
;

/* ISO section 6.8.1, p. 126 */
Statement
{
	struct node *nd;
} :
					{
					  slevel++;
					}
	[ Label(&nd) ':'
					{ if( nd ) DefLabel(nd, slevel); }
	]?
					{ if( !options['L'] )
						C_lin((arith) dot.tk_lineno);
					}
	[
		SimpleStatement
	|
		StructuredStatement
	]
					{ slevel--; }
;

/* ISO section 6.8.2.1, p. 126 */
SimpleStatement
{
	struct node *pnd, *expp;
} :
	/* This is a changed rule, because the grammar as specified in the
	 * reference is not LL(1), and this gives conflicts.
	 * Note : the grammar states : AssignmentStatement |
	 *				ProcedureStatement | ...
	 */
	EmptyStatement
|
	GotoStatement
|
	/* Evidently this is the beginning of the changed part
	 */
	IDENT			{ pnd = MkLeaf(Name, &dot); }
	[
		/* At this point the IDENT can be a FunctionIdentifier in
		 * which case the VariableAccessTail must be empty.
		 */
		VariableAccessTail(&pnd)
		[
			BECOMES
		|
			'='	{ error("':=' expected instead of '='"); }
		]
		Expression(&expp)
				{ AssignStat(pnd, expp); }
	|
				{ pnd = MkNode(Call, pnd, NULLNODE, &dot); }
		ActualParameterList(&(pnd->nd_right))?
				{ ProcStat(pnd);

				  if( !err_occurred )
					CodeCall(pnd);

				  FreeNode(pnd);
				}
	]
|
	InputOutputStatement
	/* end of changed part
	 */
;

InputOutputStatement
{
	struct node *nd = NULLNODE;
} :
	/* This is a new rule because the grammar specified by the standard
	 * is not exactly LL(1) (see SimpleStatement).
	 */
	[
		READ ReadParameterList(&nd)		{ ChkRead(nd); }
	|
		READLN ReadParameterList(&nd)?		{ ChkReadln(nd); }
	|
		WRITE WriteParameterList(&nd)		{ ChkWrite(nd); }
	|
		WRITELN WriteParameterList(&nd)?	{ ChkWriteln(nd); }
	]
							{ FreeNode(nd); }
;

EmptyStatement:
	/* empty */
;

/* ISO section 6.8.3.1, p. 128 */
StructuredStatement:
	CompoundStatement
|
	ConditionalStatement
|
	RepetitiveStatement
|
	WithStatement
;

/* ISO section 6.8.2.4, p. 127 */
GotoStatement
{
	struct node *nd;
} :
	GOTO Label(&nd)
					{ if( nd ) TstLabel(nd, slevel); }
;

/* ISO section 6.8.3.3, p. 128 */
ConditionalStatement:
	%default
	CaseStatement
|
	IfStatement
;

/* ISO section 6.8.3.6, p. 129 */
RepetitiveStatement:
	RepeatStatement
|
	WhileStatement
|
	ForStatement
;

/* ISO section 6.8.3.10, p. 132 */
WithStatement
{
	struct scopelist *Save = CurrVis;
	struct node *nd;
} :
	WITH
	RecordVariableList(&nd)
	DO
	Statement	{ EndWith(Save, nd);
			  chk_labels(slevel + 1);
			}
;

RecordVariableList(register struct node **pnd;)
{
	struct node *nd;
} :
	RecordVariable(&nd)
				{ *pnd = nd = MkNode(Link, nd, NULLNODE, &dot);
				  nd->nd_symb = ',';
				}
	[ %persistent
		','		{ nd->nd_right = MkLeaf(Link, &dot);
				  nd = nd->nd_right;
				}
		RecordVariable(&(nd->nd_left))
	]*
;

RecordVariable(register struct node **pnd;):
	VariableAccess(pnd)
				{ WithStat(*pnd); }
;

/* ISO section 6.8.3.4, p. 128 */
IfStatement
{
	struct node *nd;
	label l1 = ++text_label;
	label l2 = ++text_label;
} :
	IF
	BooleanExpression(&nd)
					{ struct desig ds;
					
					  ds = InitDesig;
					  if( !err_occurred )
						CodeExpr(nd, &ds, l1);
					}
	THEN
	Statement			{ chk_labels(slevel + 1); }
	[ %prefer	/* closest matching */
		ELSE
					{ C_bra(l2);
					  C_df_ilb(l1);
					}
		Statement
					{ C_df_ilb(l2);
					  chk_labels(slevel + 1);
					}
	|
		/* empty */
					{ C_df_ilb(l1); }
	]
;

/* ISO section 6.8.3.5, p. 128 */
CaseStatement
{
	struct node *casend, *nd;
	label exit_label;
} :
	/* This is a changed rule, because the grammar as specified in the
	 * reference states that a semicolon is optional before END,
	 * and this is not LL(1).
	 */
	CASE				{ casend = nd = MkLeaf(Link, &dot);
					  casend->nd_lab = ++text_label;
					  exit_label = ++text_label;
					}
	Expression(&(nd->nd_left))
					{ CaseExpr(casend); }
	OF
	CaseListElement(&(nd->nd_right), exit_label)
					{ nd = nd->nd_right; }
	CaseListElementTail(&(nd->nd_right), exit_label)
	END
					{ CaseEnd(casend, exit_label); }
;

CaseListElementTail(register struct node **pnd; label exit_label;):
	/* This is a new rule, all because of a silly semicolon
	 */
	/* empty */
|
%default
	';'
	[
		/* empty */
	|
		CaseListElement(pnd, exit_label)
		CaseListElementTail(&((*pnd)->nd_right), exit_label)
	]
;

CaseListElement(register struct node **pnd; label exit_label;):
	CaseConstantList(pnd)
	':'
				{ *pnd = MkNode(Link, *pnd, NULLNODE, &dot);
				  (*pnd)->nd_lab = ++text_label;
				  C_df_ilb(text_label);
				}
	Statement		{ C_bra(exit_label);
				  chk_labels(slevel + 1);
				}
;

/* ISO section 6.8.3.7, p. 129 */
RepeatStatement
{
	struct node *nd;
	label repeatlb = ++text_label;
} :
	REPEAT
					{ C_df_ilb(repeatlb); }
	StatementSequence
	UNTIL
	BooleanExpression(&nd)
					{ struct desig ds;

					  ds = InitDesig;
					  if( !err_occurred )
						CodeExpr(nd, &ds, repeatlb);
					}
;

/* ISO section 6.8.3.8, p. 129 */
WhileStatement
{
	struct node *nd;
	label whilelb = ++text_label;
	label exitlb = ++text_label;

} :
	WHILE
					{ C_df_ilb(whilelb); }
	BooleanExpression(&nd)
					{ struct desig ds;

					  ds = InitDesig;
					  if( !err_occurred )
						CodeExpr(nd, &ds, exitlb);
					}
	DO
	Statement
					{ C_bra(whilelb);
					  C_df_ilb(exitlb);
					  chk_labels(slevel + 1);
					}
;

/* ISO section 6.8.3.9, p. 130 */
ForStatement
{
	register struct node *nd;
	int stepsize;
	label l1 = ++text_label;
	label l2 = ++text_label;
	arith tmp1 = (arith) 0;
	arith tmp2 = (arith) 0;
} :
	FOR
	/* ControlVariable must be an EntireVariable */
	IDENT			{ nd = MkLeaf(Name, &dot); }
	BECOMES
	Expression(&(nd->nd_left))
	[
		TO		{ stepsize = 1; }
	|
		DOWNTO		{ stepsize = -1; }
	]
	Expression(&(nd->nd_right))
				{ ChkForStat(nd);
				  if( !err_occurred )	{
					tmp1 = CodeInitFor(nd->nd_left, 0);
					tmp2 = CodeInitFor(nd->nd_right, 2);
				  	CodeFor(nd, stepsize, l1, l2, tmp1);
				  }
				}
	DO
	Statement
				{ if( !err_occurred )
				       CodeEndFor(nd, stepsize, l1, l2, tmp2);
				  chk_labels(slevel + 1);
				  FreeNode(nd);
				  if( tmp1 ) FreeInt(tmp1);
				  if( tmp2 ) FreeInt(tmp2);
				}
;

/* SPECIALSPECIALSPECIALSPECIALSPECIALSPECIALSPECIALSPECIALSPECIALSPECIAL */
/* ISO section 6.9, p. 132-136 */
ReadParameterList(register struct node **pnd;)
{
	register struct node *nd;
} :
	/* This is a changed rule, because the grammar as specified in the
	 * reference is not LL(1), and this gives conflicts.
	 */
	'('
		VariableAccess(pnd)	/* possibly a FileVariable */
					{ *pnd = nd =
					     MkNode(Link, *pnd, NULLNODE, &dot);
					  nd->nd_symb = ',';
					}
		[ %persistent
			',' 		{ nd->nd_right = MkLeaf(Link, &dot);
					  nd = nd->nd_right;
					}
			VariableAccess(&(nd->nd_left))
		]*
	')'
;

WriteParameterList(register struct node **pnd;)
{
	register struct node *nd;
} :
	/* This is a changed rule, because the grammar as specified in the
	 * reference is not LL(1), and this gives conflicts.
	 */
	'('
		/* Only the first WriteParameter can be a FileVariable !!
		 */
		WriteParameter(pnd)
					{ *pnd = nd =
					     MkNode(Link, *pnd, NULLNODE, &dot);
					  nd->nd_symb = ',';
					}
		[ %persistent
			',' 		{ nd->nd_right = MkLeaf(Link, &dot);
					  nd = nd->nd_right;
					}
			WriteParameter(&(nd->nd_left))
		]*
	')'
;

WriteParameter(register struct node **pnd;)
{
	register struct node *nd;
} :
	Expression(pnd)
					{ if( !ChkExpression(*pnd) )
						(*pnd)->nd_type = error_type;
					  *pnd = nd =
					     MkNode(Link, *pnd, NULLNODE, &dot);
					  nd->nd_symb = ':';
					}
	[
	/* Here the first Expression can't be a FileVariable
	 */
		':'			{ nd->nd_right = MkLeaf(Link, &dot);
					  nd = nd->nd_right;
					}
		Expression(&(nd->nd_left))
					{ if( !ChkExpression(nd->nd_left) )
					      nd->nd_left->nd_type = error_type;
					}
		[
			':'		{ nd->nd_right = MkLeaf(Link, &dot);
					  nd = nd->nd_right;
					}
			Expression(&(nd->nd_left))
					{ if( !ChkExpression(nd->nd_left) )
					      nd->nd_left->nd_type = error_type;
					}
		]?
	]?
;
