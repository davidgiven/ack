/* D E C L A R A T I O N S */

{
#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<alloc.h>
#include	<assert.h>

#include	"idf.h"
#include	"LLlex.h"
#include	"def.h"
#include	"type.h"
#include	"scope.h"
#include	"node.h"
#include	"misc.h"
#include	"main.h"
#include	"chk_expr.h"
#include	"warning.h"

int		proclevel = 0;		/* nesting level of procedures */
int		return_occurred;	/* set if a return occurs in a block */
}

ProcedureDeclaration
{
	struct def *df;
} :
			{ ++proclevel; }
	ProcedureHeading(&df, D_PROCEDURE)
	';' block(&(df->prc_body))
	IDENT
			{ EndProc(df, dot.TOK_IDF);
			  --proclevel;
			}
;

ProcedureHeading(struct def **pdf; int type;)
{
	struct type *tp = 0;
#define needs_static_link()	(proclevel > 1)
	arith parmaddr = needs_static_link() ? pointer_size : 0;
	struct paramlist *pr = 0;
} :
	PROCEDURE IDENT
			{ *pdf = DeclProc(type, dot.TOK_IDF); }
	FormalParameters(&pr, &parmaddr, &tp)?
			{ CheckWithDef(*pdf, proc_type(tp, pr, parmaddr));
			  if (tp && IsConstructed(tp)) {
warning(W_STRICT, "procedure \"%s\" has a constructed result type",
	(*pdf)->df_idf->id_text);
			  }
			}
;

block(struct node **pnd;) :
	[	%persistent
		declaration
	]*
			{ return_occurred = 0; *pnd = 0; }
	[	%persistent
		BEGIN
		StatementSequence(pnd)
	]?
	END
;

declaration:
	CONST [ %persistent ConstantDeclaration ';' ]*
|
	TYPE [ %persistent TypeDeclaration ';' ]*
|
	VAR [ %persistent VariableDeclaration ';' ]*
|
	ProcedureDeclaration ';'
|
	ModuleDeclaration ';'
;

FormalParameters(struct paramlist **ppr; arith *parmaddr; struct type **ptp;):
	'('
	[
		FPSection(ppr, parmaddr)
		[
			';' FPSection(ppr, parmaddr)
		]*
	]?
	')'
	[	':' qualtype(ptp)
	]?
;

FPSection(struct paramlist **ppr; arith *parmaddr;)
{
	struct node *FPList;
	struct type *tp;
	int VARp;
} :
	var(&VARp) IdentList(&FPList) ':' FormalType(&tp)
			{ EnterParamList(ppr, FPList, tp, VARp, parmaddr); }
;

FormalType(struct type **ptp;)
{
	extern arith ArrayElSize();
} :
	ARRAY OF qualtype(ptp)
		{ register struct type *tp = construct_type(T_ARRAY, NULLTYPE);

		  tp->arr_elem = *ptp;
		  *ptp = tp;
		  tp->arr_elsize = ArrayElSize(tp->arr_elem);
		  tp->tp_align = tp->arr_elem->tp_align;
		}
|
	 qualtype(ptp)
;

TypeDeclaration
{
	struct def *df;
	struct type *tp;
	struct node *nd;
}:
	IDENT		{ df = define(dot.TOK_IDF, CurrentScope, D_TYPE);
			  nd = MkLeaf(Name, &dot);
			}
	'=' type(&tp)
			{ DeclareType(nd, df, tp);
			  free_node(nd);
			}
;

type(struct type **ptp;):
	%default SimpleType(ptp)
|
	ArrayType(ptp)
|
	RecordType(ptp)
|
	SetType(ptp)
|
	PointerType(ptp)
|
	ProcedureType(ptp)
;

SimpleType(struct type **ptp;)
{
	struct type *tp;
} :
	qualtype(ptp)
	[
		/* nothing */
	|
		SubrangeType(&tp)
		/* The subrange type is given a base type by the
		   qualident (this is new modula-2).
		*/
			{ chk_basesubrange(tp, *ptp); }
	]
|
	enumeration(ptp)
|
	SubrangeType(ptp)
;

enumeration(struct type **ptp;)
{
	struct node *EnumList;
} :
	'(' IdentList(&EnumList) ')'
		{ register struct type *tp =
		  	standard_type(T_ENUMERATION, int_align, int_size);

		  *ptp = tp;
		  EnterEnumList(EnumList, tp);
		  if (ufit(tp->enm_ncst-1, 1)) {
			tp->tp_size = 1;
			tp->tp_align = 1;
		  }
		  else if (ufit(tp->enm_ncst-1, short_size)) {
			tp->tp_size = short_size;
			tp->tp_align = short_align;
		  }
		}
;

IdentList(struct node **p;)
{
	register struct node *q;
} :
	IDENT		{ *p = q = MkLeaf(Value, &dot); }
	[ %persistent
		',' IDENT
			{ q->next = MkLeaf(Value, &dot);
			  q = q->next;
			}
	]*
			{ q->next = 0; }
;

SubrangeType(struct type **ptp;)
{
	struct node *nd1, *nd2;
}:
	/*
	   This is not exactly the rule in the new report, but see
	   the rule for "SimpleType".
	*/
	'[' ConstExpression(&nd1)
	UPTO ConstExpression(&nd2)
	']'
			{ *ptp = subr_type(nd1, nd2);
			  free_node(nd1);
			  free_node(nd2);
			}
;

ArrayType(struct type **ptp;)
{
	struct type *tp;
	register struct type *tp2;
} :
	ARRAY SimpleType(&tp)
			{ *ptp = tp2 = construct_type(T_ARRAY, tp); }
	[
		',' SimpleType(&tp)
			{ tp2->arr_elem = construct_type(T_ARRAY, tp);
			  tp2 = tp2->arr_elem;
			}
	]* OF type(&tp)
			{ tp2->arr_elem = tp;
			  ArraySizes(*ptp);
			}
;

RecordType(struct type **ptp;)
{
	register struct scope *scope;
	arith size = 0;
	int xalign = struct_align;
}
:
	RECORD
		{ open_scope(OPENSCOPE);	/* scope for fields of record */
		  scope = CurrentScope;
		  close_scope(0);
		}
	FieldListSequence(scope, &size, &xalign)
		{ if (size == 0) {
			warning(W_ORDINARY, "empty record declaration");
			size = 1;
		  }
		  *ptp = standard_type(T_RECORD, xalign, size);
		  (*ptp)->rec_scope = scope;
		}
	END
;

FieldListSequence(struct scope *scope; arith *cnt; int *palign;):
	FieldList(scope, cnt, palign)
	[
		';' FieldList(scope, cnt, palign)
	]*
;

FieldList(struct scope *scope; arith *cnt; int *palign;)
{
	struct node *FldList;
	register struct idf *id = 0;
	struct type *tp;
	struct node *nd1;
	register struct node *nd;
	arith tcnt, max;
} :
[
	IdentList(&FldList) ':' type(&tp)
			{ *palign = lcm(*palign, tp->tp_align);
			  EnterFieldList(FldList, tp, scope, cnt);
			}
|
	CASE
	/* Also accept old fashioned Modula-2 syntax, but give a warning.
	   Sorry for the complicated code.
	*/
	[ qualident(&nd1)
			{ nd = nd1; }
	  [ ':' qualtype(&tp)
			/* This is correct, in both kinds of Modula-2, if
		   	   the first qualident is a single identifier.
			*/
			{ if (nd->nd_class != Name) {
				error("illegal variant tag");
		  	  }
		  	  else	id = nd->nd_IDF;
			  FreeNode(nd);
			}
	  |		/* Old fashioned! the first qualident now represents
			   the type
			*/
			{ warning(W_OLDFASHIONED, "old fashioned Modula-2 syntax; ':' missing");
			  if (ChkDesignator(nd) &&
			      (nd->nd_class != Def ||
			       !(nd->nd_def->df_kind&(D_ERROR|D_ISTYPE)) ||
			       !nd->nd_def->df_type)) {
				node_error(nd, "type expected");
				tp = error_type;
			  }
			  else tp = nd->nd_def->df_type;
			  FreeNode(nd);
			}
	  ]
	| ':' qualtype(&tp)
	  /* Aha, third edition. Well done! */
	]
			{ if (id) {
			  	register struct def *df = define(id,
								 scope,
								 D_FIELD);
			  	if (!(tp->tp_fund & T_DISCRETE)) {
					error("illegal type in variant");
			  	}
			  	df->df_type = tp;
			  	df->fld_off = align(*cnt, tp->tp_align);
			  	*cnt = df->fld_off + tp->tp_size;
			  	df->df_flags |= D_QEXPORTED;
			  }
			  tcnt = *cnt;
			}
	OF variant(scope, &tcnt, tp, palign)
			{ max = tcnt; tcnt = *cnt; }
	[
	  '|' variant(scope, &tcnt, tp, palign)
			{ if (tcnt > max) max = tcnt; tcnt = *cnt; }
	]*
	[ ELSE FieldListSequence(scope, &tcnt, palign)
			{ if (tcnt > max) max = tcnt; }
	]?
	END
			{ *cnt = max; }
]?
;

variant(struct scope *scope; arith *cnt; struct type *tp; int *palign;)
{
	struct node *nd;
} :
	[
		CaseLabelList(&tp, &nd)
			{ /* Ignore the cases for the time being.
			     Maybe a checking version will be supplied
			     later ??? (Improbable)
			  */
			  FreeNode(nd);
			}
		':' FieldListSequence(scope, cnt, palign)
	]?
			/* Changed rule in new modula-2 */
;

CaseLabelList(struct type **ptp; struct node **pnd;):
	CaseLabels(ptp, pnd)
	[	
			{ *pnd = MkNode(Link, *pnd, NULLNODE, &dot); }
		',' CaseLabels(ptp, &((*pnd)->nd_right))
			{ pnd = &((*pnd)->nd_right); }
	]*
;

CaseLabels(struct type **ptp; register struct node **pnd;)
{
	register struct node *nd1;
}:
	ConstExpression(pnd)
			{ nd1 = *pnd; }
	[
		UPTO	{ *pnd = MkNode(Link,nd1,NULLNODE,&dot); }
		ConstExpression(&(*pnd)->nd_right)
			{ if (!TstCompat(nd1->nd_type,
					 (*pnd)->nd_right->nd_type)) {
				node_error((*pnd)->nd_right,
					  "type incompatibility in case label");
			  	nd1->nd_type = error_type;
			  }
			}
	]?
			{ if (*ptp != 0 && !TstCompat(*ptp, nd1->nd_type)) {
				node_error(nd1,
					  "type incompatibility in case label");
			  }
			  *ptp = nd1->nd_type;
			}
;

SetType(struct type **ptp;) :
	SET OF SimpleType(ptp)
			{ *ptp = set_type(*ptp); }
;

/*	In a pointer type definition, the type pointed at does not
	have to be declared yet, so be careful about identifying
	type-identifiers
*/
PointerType(struct type **ptp;)
{
	register struct node *nd = 0;
} :
	POINTER TO
			{ *ptp = construct_type(T_POINTER, NULLTYPE); }
	[ %if	( lookup(dot.TOK_IDF, CurrentScope, 1)
			/* Either a Module or a Type, but in both cases defined
		   	   in this scope, so this is the correct identification
			*/
		||
		  ( nd = new_node(),
		    nd->nd_token = dot,
		    lookfor(nd, CurrVis, 0)->df_kind == D_MODULE
		  )
			/* A Modulename in one of the enclosing scopes.
			   It is not clear from the language definition that
			   it is correct to handle these like this, but
			   existing compilers do it like this, and the
			   alternative is difficult with a lookahead of only
			   one token.
			   ???
			*/
		)
	  type(&((*ptp)->next)) 
			{ if (nd) free_node(nd); }
	|
	  IDENT		{ if (nd) {
				/* nd could be a null pointer, if we had a
				   syntax error exactly at this alternation.
				   MORAL: Be careful with %if resolvers with
				   side effects!
				*/
				Forward(nd, (*ptp));
			  }
			}
	]
;

qualtype(struct type **ptp;)
{
	register struct node *nd;
	struct node *nd1;		/* because &nd is illegal */
} :
	qualident(&nd1)
		{ nd = nd1;
		  *ptp = error_type;
		  if (ChkDesignator(nd)) {
			if (nd->nd_class != Def) {
				node_error(nd, "type expected");
			}
			else {
				register struct def *df = nd->nd_def;

				if (df->df_kind&(D_ISTYPE|D_FORWARD|D_ERROR)) {
				    if (! df->df_type) {
node_error(nd,"type \"%s\" not (yet) declared", df->df_idf->id_text);
				    }
				    else *ptp = df->df_type;
				}
				else {
node_error(nd,"identifier \"%s\" is not a type", df->df_idf->id_text);
				}
			}
		  }
		  FreeNode(nd);
		}
;

ProcedureType(struct type **ptp;)
{
	struct paramlist *pr = 0;
	arith parmaddr = 0;
}
:
			{ *ptp = 0; }
	PROCEDURE 
	[
	  	FormalTypeList(&pr, &parmaddr, ptp)
	]?
			{ *ptp = proc_type(*ptp, pr, parmaddr); }
;

FormalTypeList(struct paramlist **ppr; arith *parmaddr; struct type **ptp;)
{
	struct type *tp;
	int VARp;
} :
	'('
	[
		var(&VARp) FormalType(&tp)
			{ EnterParamList(ppr,NULLNODE,tp,VARp,parmaddr); }
		[
			',' var(&VARp) FormalType(&tp)
			{ EnterParamList(ppr,NULLNODE,tp,VARp,parmaddr); }
		]*
	]?
	')'
	[ ':' qualtype(ptp)
	]?
;

var(int *VARp;):
	VAR		{ *VARp = D_VARPAR; }
|
	/* empty */	{ *VARp = D_VALPAR; }
;

ConstantDeclaration
{
	struct idf *id;
	struct node *nd;
}:
	IDENT		{ id = dot.TOK_IDF; }
	'=' ConstExpression(&nd)
			{ define(id,CurrentScope,D_CONST)->con_const = nd; }
;

VariableDeclaration
{
	struct node *VarList;
	register struct node *nd;
	struct type *tp;
} :
	IdentAddr(&VarList)
			{ nd = VarList; }
	[ %persistent
		',' IdentAddr(&(nd->nd_right))
			{ nd = nd->nd_right; }
	]*
	':' type(&tp)
			{ EnterVarList(VarList, tp, proclevel > 0); }
;

IdentAddr(struct node **pnd;) :
	IDENT		{ *pnd = MkLeaf(Name, &dot); }
	[	'['
		ConstExpression(&((*pnd)->nd_left))
		']'
	]?
;
