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
#include	"chk_expr.h"

int		proclevel = 0;		/* nesting level of procedures */
int		return_occurred;	/* set if a return occurred in a
					   procedure or function
					*/
}

ProcedureDeclaration
{
	register struct def *df;
	struct def *df1;
} :
			{ ++proclevel;
			  return_occurred = 0;
			}
	ProcedureHeading(&df1, D_PROCEDURE)
			{ CurrentScope->sc_definedby = df = df1;
			  df->prc_vis = CurrVis;
			}
	';' block(&(df->prc_body)) IDENT
			{ match_id(dot.TOK_IDF, df->df_idf);
			  close_scope(SC_CHKFORW|SC_REVERSE);
			  if (! return_occurred && ResultType(df->df_type)) {
error("function procedure %s does not return a value", df->df_idf->id_text);
			  }
			  --proclevel;
			}
;

ProcedureHeading(struct def **pdf; int type;)
{
	struct paramlist *params = 0;
	struct type *tp = 0;
	register struct def *df;
	arith NBytesParams;
} :
	PROCEDURE IDENT
		{ df = DeclProc(type);
		  if (proclevel > 1) {	/* need room for static link */
			NBytesParams = pointer_size;
		  }
		  else	NBytesParams = 0;
		}
	FormalParameters(&params, &tp, &NBytesParams)?
		{ tp = construct_type(T_PROCEDURE, tp);
		  tp->prc_params = params;
		  tp->prc_nbpar = NBytesParams;
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
		}
;

block(struct node **pnd;) :
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
		 struct type **ptp;
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
	[	':' qualtype(ptp)
	]?
;

FPSection(struct paramlist **ppr; arith *parmaddr;)
{
	struct node *FPList;
	struct type *tp;
	int VARp;
	struct paramlist *p = 0;
} :
	var(&VARp) IdentList(&FPList) ':' FormalType(&p, 0)
			{ EnterParamList(ppr, FPList, p->par_def->df_type,
					 VARp, parmaddr);
			  free_def(p->par_def);
			  free_paramlist(p);
			}
;

FormalType(struct paramlist **ppr; int VARp;)
{
	register struct def *df;
	int ARRAYflag;
	register struct type *tp;
	struct type *tp1;
	register struct paramlist *p = new_paramlist();
	extern arith ArrayElSize();
} :
	[ ARRAY OF	{ ARRAYflag = 1; }
	|		{ ARRAYflag = 0; }
	]
	qualtype(&tp1)
		{ if (ARRAYflag) {
			tp = construct_type(T_ARRAY, NULLTYPE);
			tp->arr_elem = tp1;
			tp->arr_elsize = ArrayElSize(tp1);
			tp->tp_align = lcm(word_align, pointer_align);
		  }
		  else	tp = tp1;
		  p->next = *ppr;
		  *ppr = p;
		  p->par_def = df = new_def();
		  df->df_type = tp;
		  df->df_flags = VARp;
		}
;

TypeDeclaration
{
	register struct def *df;
	struct type *tp;
}:
	IDENT		{ df = define(dot.TOK_IDF,CurrentScope,D_TYPE); }
	'=' type(&tp)
			{ if (df->df_type && df->df_type->tp_fund == T_HIDDEN) {
			  	if (tp->tp_fund != T_POINTER) {
error("opaque type \"%s\" is not a pointer type", df->df_idf->id_text);
				}
				/* Careful now ... we might have declarations
				   referring to the hidden type.
				*/
				*(df->df_type) = *tp;
				free_type(tp);
			  }
			  else	df->df_type = tp;
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
	register struct type *tp;
} :
	'(' IdentList(&EnumList) ')'
		{ *ptp = tp = standard_type(T_ENUMERATION, 1, (arith) 1);
		  EnterEnumList(EnumList, tp);
		  if (tp->enm_ncst > 256) { /* ??? is this reasonable ??? */
			error("Too many enumeration literals");
		  }
		}
;

IdentList(struct node **p;)
{
	register struct node *q;
} :
	IDENT		{ *p = q = MkLeaf(Value, &dot); }
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
			{ *ptp = subr_type(nd1, nd2); }
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
		{ *ptp = standard_type(T_RECORD, xalign, WA(count));
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
	register struct idf *id = gen_anon_idf();
	register struct def *df;
	struct type *tp;
	struct node *nd;
	arith tcnt, max;
} :
[
	IdentList(&FldList) ':' type(&tp)
			{ *palign = lcm(*palign, tp->tp_align);
			  EnterFieldList(FldList, tp, scope, cnt);
			}
|
	CASE
	/* Also accept old fashioned Modula-2 syntax, but give a warning
	*/
	[	qualident(0, (struct def **) 0, (char *) 0, &nd)
		[	':' qualtype(&tp)
			/* This is correct, in both kinds of Modula-2, if
			   the first qualident is a single identifier.
			*/
				{ if (nd->nd_class != Name) {
					error("illegal variant tag");
			  	  }
			  	  else	id = nd->nd_IDF;
				}
		|
			/* Old fashioned! the first qualident now represents
			   the type
			*/
				{ warning("Old fashioned Modula-2 syntax!");
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
	|
		/* Aha, third edition. Well done! */
		':' qualtype(&tp)
	]
				{ if (!(tp->tp_fund & T_DISCRETE)) {
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
} :
	SET OF SimpleType(ptp)
			{ *ptp = set_type(*ptp); }
;

/*	In a pointer type definition, the type pointed at does not
	have to be declared yet, so be careful about identifying
	type-identifiers
*/
PointerType(struct type **ptp;)
{
	register struct def *df;
	register struct node *nd;
} :
	POINTER TO
			{ *ptp = construct_type(T_POINTER, NULLTYPE); }
	[ %if ( lookup(dot.TOK_IDF, CurrentScope))
		/* Either a Module or a Type, but in both cases defined
		   in this scope, so this is the correct identification
		*/
		qualtype(&((*ptp)->next))
	| %if ( nd = new_node(), nd->nd_token = dot,
		df = lookfor(nd, CurrVis, 0), free_node(nd),
	        df->df_kind == D_MODULE)
		type(&((*ptp)->next))
	|
		IDENT	{ Forward(&dot, &((*ptp)->next)); }
	]
;

qualtype(struct type **ptp;)
{
	struct def *df;
} :
	qualident(D_ISTYPE, &df, "type", (struct node **) 0)
		{ if (!df->df_type) {
			error("type \"%s\" not declared", df->df_idf->id_text);
			*ptp = error_type;
		  }
		  else	*ptp = df->df_type;
		}
;


ProcedureType(struct type **ptp;)
{
	struct paramlist *pr = 0;
	register struct type *tp;
} :
			{ *ptp = 0; }
	PROCEDURE FormalTypeList(&pr, ptp)?
			{ *ptp = tp = construct_type(T_PROCEDURE, *ptp);
			  tp->prc_params = pr;
			}
;

FormalTypeList(struct paramlist **ppr; struct type **ptp;)
{
	struct def *df;
	int VARp;
} :
	'('		{ *ppr = 0; }
	[
		var(&VARp) FormalType(ppr, VARp)
		[
			',' var(&VARp) FormalType(ppr, VARp)
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
	[
		',' IdentAddr(&(nd->nd_right))
			{ nd = nd->nd_right; }
	]*
	':' type(&tp)
			{ EnterVarList(VarList, tp, proclevel > 0); }
;

IdentAddr(struct node **pnd;) :
	IDENT		{ *pnd = MkLeaf(Name, &dot); }
	ConstExpression(&((*pnd)->nd_left))?
;
