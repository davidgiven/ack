/* D E C L A R A T I O N S */

{
#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

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

int		proclevel = 0;	/* nesting level of procedures */
extern char	*sprint();
}

ProcedureDeclaration
{
	struct def *df;
} :
			{ proclevel++; }
	ProcedureHeading(&df, D_PROCEDURE)
			{
			  CurrentScope->sc_definedby = df;
			  df->prc_vis = CurrVis;
			}
	';' block(&(df->prc_body)) IDENT
			{
			  match_id(dot.TOK_IDF, df->df_idf);
			  close_scope(SC_CHKFORW|SC_REVERSE);
			  proclevel--;
			}
;

ProcedureHeading(struct def **pdf; int type;)
{
	struct type *tp = 0;
	struct paramlist *params = 0;
	register struct def *df;
	struct def *DeclProc();
} :
	PROCEDURE IDENT
		{
		  df = DeclProc(type);
		  tp = construct_type(T_PROCEDURE, tp);
		  if (proclevel > 1) {
			/* Room for static link
			*/
			tp->prc_nbpar = pointer_size;
		  }
		  else	tp->prc_nbpar = 0;
		}
	FormalParameters(&params, &(tp->next), &(tp->prc_nbpar))?
		{
		  tp->prc_params = params;
		  if (df->df_type) {
			/* We already saw a definition of this type
			   in the definition module.
			*/
		  	if (!TstProcEquiv(tp, df->df_type)) {
error("inconsistent procedure declaration for \"%s\"", df->df_idf->id_text); 
			}
			FreeType(df->df_type);
		  }
		  df->df_type = tp;
		  *pdf = df;

		  if (type == D_PROCHEAD) close_scope(0);

		  DO_DEBUG(1, type == D_PROCEDURE && 
				(print("proc %s:", df->df_idf->id_text),
				 DumpType(tp), print("\n")));
		}
;

block(struct node **pnd;)
{
}:
	declaration*
	[
		BEGIN
		StatementSequence(pnd)
	|
			{ *pnd = 0; }
	]
	END
;

declaration:
	CONST [ ConstantDeclaration ';' ]*
|
	TYPE [ TypeDeclaration ';' ]*
|
	VAR [ VariableDeclaration ';' ]*
|
	ProcedureDeclaration ';'
|
	ModuleDeclaration ';'
;

FormalParameters(struct paramlist **pr;
		 struct type **tp;
		 arith *parmaddr;)
{
	struct def *df;
} :
	'('
	[
		FPSection(pr, parmaddr)
		[
			';' FPSection(pr, parmaddr)
		]*
	]?
	')'
			{ *tp = 0; }
	[	':' qualident(D_ISTYPE, &df, "type", (struct node **) 0)
			{ *tp = df->df_type;
			}
	]?
;

FPSection(struct paramlist **ppr; arith *parmaddr;)
{
	struct node *FPList;
	struct type *tp;
	int VARp = D_VALPAR;
} :
	[
		VAR	{ VARp = D_VARPAR; }
	]?
	IdentList(&FPList) ':' FormalType(&tp)
		{
		  ParamList(ppr, FPList, tp, VARp, parmaddr);
		  FreeNode(FPList);
		}
;

FormalType(struct type **ptp;)
{
	struct def *df;
	int ARRAYflag = 0;
	register struct type *tp;
	extern arith ArrayElSize();
} :
	[ ARRAY OF	{ ARRAYflag = 1; }
	]?
	qualident(D_ISTYPE, &df, "type", (struct node **) 0)
		{ if (ARRAYflag) {
			*ptp = tp = construct_type(T_ARRAY, NULLTYPE);
			tp->arr_elem = df->df_type;
			tp->arr_elsize = ArrayElSize(df->df_type);
			tp->tp_align = lcm(word_align, pointer_align);
		  }
		  else	*ptp = df->df_type;
		}
;

TypeDeclaration
{
	register struct def *df;
	struct type *tp;
}:
	IDENT		{ df = lookup(dot.TOK_IDF, CurrentScope);
			  if (!df) df = define(dot.TOK_IDF,CurrentScope,D_TYPE);
			}
	'=' type(&tp)
			{ 
			  if (df->df_kind == D_HIDDEN) {
			  	if (tp->tp_fund != T_POINTER) {
error("opaque type \"%s\" is not a pointer type", df->df_idf->id_text);
				}
				df->df_kind = D_TYPE;
				*(df->df_type) = *tp;
				free_type(tp);
			  }
			  else {	
			  	df->df_type = tp;
			  	df->df_kind = D_TYPE;
			  }
			}
;

type(struct type **ptp;):
	SimpleType(ptp)
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
	struct def *df;
} :
	qualident(D_ISTYPE, &df, "type", (struct node **) 0)
	[
		/* nothing */
			{ *ptp = df->df_type; }
	|
		SubrangeType(ptp)
		/* The subrange type is given a base type by the
		   qualident (this is new modula-2).
		*/
			{
			  chk_basesubrange(*ptp, df->df_type);
			}
	]
|
	enumeration(ptp)
|
	SubrangeType(ptp)
;

enumeration(struct type **ptp;)
{
	struct node *EnumList;
	register struct type *tp;
} :
	'(' IdentList(&EnumList) ')'
		{
		  *ptp = tp = standard_type(T_ENUMERATION, 1, (arith) 1);
		  EnterIdList(EnumList, D_ENUM, 0, tp,
				 CurrentScope, (arith *) 0);
		  FreeNode(EnumList);
		  if (tp->enm_ncst > 256) {
			/* ??? is this reasonable ??? */
			error("Too many enumeration literals");
		  }
		}
;

IdentList(struct node **p;)
{
	register struct node *q;
} :
	IDENT		{ q = MkLeaf(Value, &dot);
			  *p = q;
			}
	[
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
			}
;

ArrayType(struct type **ptp;)
{
	struct type *tp;
	register struct type *tp2;
} :
	ARRAY SimpleType(&tp)
			{
			  *ptp = tp2 = construct_type(T_ARRAY, tp);
			}
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
	struct scope *scope;
	arith count;
	int xalign = struct_align;
}
:
	RECORD
			{ open_scope(OPENSCOPE);
			  scope = CurrentScope;
			  close_scope(0);
			  count = 0;
			}
	FieldListSequence(scope, &count, &xalign)
		{
		  *ptp = standard_type(T_RECORD, xalign, count);
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
	struct idf *id;
	struct def *df;
	struct type *tp;
	struct node *nd;
	arith tcnt, max;
} :
[
	IdentList(&FldList) ':' type(&tp)
			{ *palign = lcm(*palign, tp->tp_align);
			  EnterIdList(FldList, D_FIELD, D_QEXPORTED,
					tp, scope, cnt);
			  FreeNode(FldList);
			}
|
	CASE
	/* Also accept old fashioned Modula-2 syntax, but give a warning
	*/
	[	qualident(0, &df, (char *) 0, &nd)
		[	/* This is good, in both kinds of Modula-2, if
			   the first qualident is a single identifier.
			*/
			{ if (nd->nd_class != Name) {
				error("illegal variant tag");
				id = gen_anon_idf();
			  }
			  else	id = nd->nd_IDF;
			}
		':' qualident(D_ISTYPE, &df, "type", (struct node **) 0)
		|
			/* Old fashioned! the first qualident now represents
			   the type
			*/
				{ warning("Old fashioned Modula-2 syntax!");
				  id = gen_anon_idf();
				  df = ill_df;
				  if (chk_designator(nd, 0, D_REFERRED) &&
				      (nd->nd_class != Def ||
				       !(nd->nd_def->df_kind &
					 (D_ERROR|D_ISTYPE)))) {
					node_error(nd, "type expected");
				  }
				  else df = nd->nd_def;
				  FreeNode(nd);
				}
		]
	|
		/* Aha, third edition? */
		':' qualident(D_ISTYPE, &df, "type", (struct node **) 0)
				{ id = gen_anon_idf(); }
	]
				{ tp = df->df_type;
				  if (!(tp->tp_fund & T_DISCRETE)) {
					error("Illegal type in variant");
				  }
				  df = define(id, scope, D_FIELD);
				  df->df_type = tp;
				  df->fld_off = align(*cnt, tp->tp_align);
				  *cnt = tcnt = df->fld_off + tp->tp_size;
				  df->df_flags |= D_QEXPORTED;
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
	struct type *tp1 = tp;
	struct node *nd;
} :
	[
		CaseLabelList(&tp1, &nd)
				{ /* Ignore the cases for the time being.
				     Maybe a checking version will be supplied
				     later ???
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

CaseLabels(struct type **ptp; struct node **pnd;)
{
	struct node *nd1, *nd2 = 0;
}:
	ConstExpression(&nd1)	{ *pnd = nd1; }
	[
		UPTO		{ *pnd = MkNode(Link,nd1,NULLNODE,&dot); }
		ConstExpression(&nd2)
				{ if (!TstCompat(nd1->nd_type, nd2->nd_type)) {
node_error(nd2,"type incompatibility in case label");
				  	nd1->nd_type = error_type;
				  }
				  (*pnd)->nd_right = nd2;
				}
	]?
				{ if (*ptp != 0 &&
				       !TstCompat(*ptp, nd1->nd_type)) {
node_error(nd1,"type incompatibility in case label");
				  }
				  *ptp = nd1->nd_type;
				}
;

SetType(struct type **ptp;)
{
	struct type *tp;
} :
	SET OF SimpleType(&tp)
			{ 
			  *ptp = set_type(tp);
			}
;

/*	In a pointer type definition, the type pointed at does not
	have to be declared yet, so be careful about identifying
	type-identifiers
*/
PointerType(struct type **ptp;)
{
	struct type *tp;
	struct def *df;
	struct def *lookfor();
	struct node *nd;
} :
	POINTER TO
	[ %if ( (df = lookup(dot.TOK_IDF, CurrentScope)))
		/* Either a Module or a Type, but in both cases defined
		   in this scope, so this is the correct identification
		*/
		qualident(D_ISTYPE, &df, "type", (struct node **) 0)
				{
				  if (!df->df_type) {
					error("type \"%s\" not declared",
						df->df_idf->id_text);
					tp = error_type;
				  }
				  else	tp = df->df_type;
				}
	| %if ( nd = new_node(), nd->nd_token = dot,
		df = lookfor(nd, CurrVis, 0), free_node(nd),
	        df->df_kind == D_MODULE)
		type(&tp)
	|
		IDENT
				{ tp = NULLTYPE; }
	]
				{
				  *ptp = construct_type(T_POINTER, tp);
				  if (!tp) Forward(&dot, &((*ptp)->next));
				}
;

ProcedureType(struct type **ptp;)
{
	struct paramlist *pr = 0;
	struct type *tp = 0;
} :
	PROCEDURE FormalTypeList(&pr, &tp)?
			{ *ptp = construct_type(T_PROCEDURE, tp);
			  (*ptp)->prc_params = pr;
			}
;

FormalTypeList(struct paramlist **ppr; struct type **ptp;)
{
	struct def *df;
	struct type *tp;
	struct paramlist *p;
	int VARp;
} :
	'('		{ *ppr = 0; }
	[
		[ VAR	{ VARp = D_VARPAR; }
		|	{ VARp = D_VALPAR; }
		]
		FormalType(&tp)
			{ *ppr = p = new_paramlist();
			  p->next = 0;
			  p->par_def = df = new_def();
			  df->df_type = tp;
			  df->df_flags = VARp;
			}
		[
			','
			[ VAR	{VARp = D_VARPAR; }
			|	{VARp = D_VALPAR; }
			] 
			FormalType(&tp)
				{ p = new_paramlist();
				  p->next = *ppr; *ppr = p;
			  	  p->par_def = df = new_def();
			  	  df->df_type = tp;
			  	  df->df_flags = VARp;
				}
		]*
	]?
	')'
	[ ':' qualident(D_TYPE, &df, "type", (struct node **) 0)
				{ *ptp = df->df_type; }
	]?
;

ConstantDeclaration
{
	struct def *df;
	struct idf *id;
	struct node *nd;
}:
	IDENT			{ id = dot.TOK_IDF; }
	'=' ConstExpression(&nd){ df = define(id, CurrentScope, D_CONST);
				  df->con_const = nd;
				}
;

VariableDeclaration
{
	struct node *VarList;
	struct type *tp;
} :
	IdentAddrList(&VarList)
	':' type(&tp)
			{ EnterVarList(VarList, tp, proclevel > 0);
			  FreeNode(VarList);
			}
;

IdentAddrList(struct node **pnd;)
{
} :
	IDENT		{ *pnd = MkLeaf(Name, &dot); }
	ConstExpression(&(*pnd)->nd_left)?
	[		{ pnd = &((*pnd)->nd_right); }
		',' IDENT
			{ *pnd = MkLeaf(Name, &dot); }
		ConstExpression(&(*pnd)->nd_left)?
	]*
;
