/* D E C L A R A T I O N S */

{
static char *RcsId = "$Header$";

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
}

ProcedureDeclaration
{
	struct def *df;
} :
	ProcedureHeading(&df, D_PROCEDURE)
	';' block IDENT
			{ match_id(dot.TOK_IDF, df->df_idf);
			  df->prc_scope = CurrentScope->sc_scope;
			  close_scope();
			}
;

ProcedureHeading(struct def **pdf; int type;)
{
	struct type *tp;
	struct type *tp1 = 0;
	struct paramlist *params = 0;
	register struct def *df;
} :
	PROCEDURE IDENT
			{ assert(type & (D_PROCEDURE | D_PROCHEAD));
			  if (type == D_PROCHEAD) {
				df = define(dot.TOK_IDF, CurrentScope, type);
			  }
			  else {
				df = lookup(dot.TOK_IDF,
						CurrentScope->sc_scope);
				if (df && df->df_kind == D_PROCHEAD) {
					df->df_kind = type;
					tp1 = df->df_type;
				}
				else {
					df = define(dot.TOK_IDF,
						CurrentScope, type);
				}
				open_scope(OPENSCOPE, 0);
			  }
			}
	FormalParameters(type == D_PROCEDURE, &params, &tp)?
			{
			  df->df_type = tp = construct_type(PROCEDURE, tp);
			  tp->prc_params = params;
			  if (tp1 && !TstTypeEquiv(tp, tp1)) {
error("inconsistent procedure declaration for \"%s\"", df->df_idf->id_text); 
			  }
			  *pdf = df;
			}
;

block:
	declaration* [ BEGIN StatementSequence ]? END
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

FormalParameters(int doparams; struct paramlist **pr; struct type **tp;)
{
	struct def *df;
	register struct paramlist *pr1;
} :
	'('
	[
		FPSection(doparams, pr)	
			{ pr1 = *pr; }
		[
			{ for (; pr1->next; pr1 = pr1->next) ; }
			';' FPSection(doparams, &(pr1->next))
		]*
	]?
	')'
			{ *tp = 0; }
	[	':' qualident(D_TYPE | D_HTYPE, &df, "type", (struct node **) 0)
			{ *tp = df->df_type; }
	]?
;

/*	In the next nonterminal, "doparams" is a flag indicating whether
	the identifiers representing the parameters must be added to the
	symbol table. We must not do so when reading a Definition Module,
	because in this case we only read the header. The Implementation
	might contain different identifiers representing the same paramters.
*/
FPSection(int doparams; struct paramlist **ppr;)
{
	struct node *FPList;
	struct paramlist *ParamList();
	struct type *tp;
	int VARp = 0;
} :
	[
		VAR	{ VARp = 1; }
	]?
	IdentList(&FPList) ':' FormalType(&tp)
		{
		  if (doparams) {
			EnterIdList(FPList, D_VARIABLE, VARp, tp, CurrentScope);
		  }
		  *ppr = ParamList(FPList, tp, VARp);
		  FreeNode(FPList);
		}
;

FormalType(struct type **tp;)
{
	struct def *df;
	int ARRAYflag = 0;
} :
	[ ARRAY OF	{ ARRAYflag = 1; }
	]?
	qualident(D_TYPE | D_HTYPE, &df, "type", (struct node **) 0)
			{ if (ARRAYflag) {
				*tp = construct_type(ARRAY, NULLTYPE);
				(*tp)->arr_elem = df->df_type;
			  }
			  else	*tp = df->df_type;
			}
;

TypeDeclaration
{
	struct def *df;
	struct type *tp;
}:
	IDENT		{ df = define(dot.TOK_IDF, CurrentScope, D_TYPE); }
	'=' type(&tp)
			{ df->df_type = tp;
			  if ((df->df_flags&D_EXPORTED) &&
			      tp->tp_fund == ENUMERATION) {
				exprt_literals(tp->enm_enums,
						enclosing(CurrentScope));
			  }
			  if (df->df_kind == D_HTYPE &&
			      tp->tp_fund != POINTER) {
error("Opaque type \"%s\" is not a pointer type", df->df_idf->id_text);
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
	qualident(D_TYPE | D_HTYPE, &df, "type", (struct node **) 0)
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
} :
	'(' IdentList(&EnumList) ')'
			{
			  *ptp = standard_type(ENUMERATION,int_align,int_size);
			  EnterIdList(EnumList, D_ENUM, 0, *ptp, CurrentScope);
			  FreeNode(EnumList);
			}

;

IdentList(struct node **p;)
{
	register struct node *q;
} :
	IDENT		{ q = MkNode(Value, NULLNODE, NULLNODE, &dot);
			  *p = q;
			}
	[
		',' IDENT
			{ q->next = MkNode(Value,NULLNODE,NULLNODE,&dot);
			  q = q->next;
			}
	]*
			{ q->next = 0; }
;

SubrangeType(struct type **ptp;)
{
	struct node *nd1, *nd2;
	extern struct type *subr_type();
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
			{
			  *ptp = tp2 = construct_type(ARRAY, tp);
			}
	[
		',' SimpleType(&tp)
			{ tp2 = tp2->arr_elem = 
				construct_type(ARRAY, tp);
			}
	]* OF type(&tp)
			{ tp2->arr_elem = tp; }
;

RecordType(struct type **ptp;)
{
	struct scope scope;
}
:
	RECORD
			{ scope.sc_scope = uniq_scope();
			  scope.next = CurrentScope;
			}
	FieldListSequence(&scope)
			{
			  *ptp = standard_type(RECORD, record_align, (arith) 0 /* ???? */);
			  (*ptp)->rec_scope = scope.sc_scope;
			}
	END
;

FieldListSequence(struct scope *scope;):
	FieldList(scope)
	[
		';' FieldList(scope)
	]*
;

FieldList(struct scope *scope;)
{
	struct node *FldList;
	struct idf *id;
	struct def *df, *df1;
	struct type *tp;
} :
[
	IdentList(&FldList) ':' type(&tp)
			{ EnterIdList(FldList, D_FIELD, 0, tp, scope);
			  FreeNode(FldList);
			}
|
	CASE
	[
		IDENT		{ id = dot.TOK_IDF; }
	|
				{ id = gen_anon_idf(); }
	]			/* Changed rule in new modula-2 */
	':' qualident(D_TYPE|D_HTYPE, &df, "type", (struct node **) 0)
				{ df1 = define(id, scope, D_FIELD);
				  df1->df_type = df->df_type;
				}
	OF variant(scope)
	[
		'|' variant(scope)
	]*
	[ ELSE FieldListSequence(scope)
	]?
	END
]?
;

variant(struct scope *scope;):
	[ CaseLabelList ':' FieldListSequence(scope) ]?
					/* Changed rule in new modula-2 */
;

CaseLabelList:
	CaseLabels [ ',' CaseLabels ]*
;

CaseLabels
{
	struct node *nd1, *nd2 = 0;
}:
	ConstExpression(&nd1) [ UPTO ConstExpression(&nd2) ]?
;

SetType(struct type **ptp;)
{
	struct type *tp;
	struct type *set_type();
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
	[ %if ( (df = lookup(dot.TOK_IDF, CurrentScope->sc_scope)))
		/* Either a Module or a Type, but in both cases defined
		   in this scope, so this is the correct identification
		*/
		qualident(D_TYPE|D_HTYPE, &df, "type", (struct node **) 0)
				{
				  if (!df->df_type) {
					error("type \"%s\" not declared",
						df->df_idf->id_text);
					tp = error_type;
				  }
				  else	tp = df->df_type;
				}
	| %if ( nd = new_node(), nd->nd_token = dot,
		df = lookfor(nd, CurrentScope, 0), free_node(nd),
	        df->df_kind == D_MODULE)
		type(&tp)
	|
		IDENT
				{ tp = NULLTYPE; }
	]
				{
				  *ptp = construct_type(POINTER, tp);
				  if (!tp) Forward(&dot, &((*ptp)->next));
				}
;

ProcedureType(struct type **ptp;)
{
	struct paramlist *pr = 0;
	struct type *tp = 0;
} :
	PROCEDURE FormalTypeList(&pr, &tp)?
			{ *ptp = construct_type(PROCVAR, tp);
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
		[ VAR	{ VARp = 1; }
		|	{ VARp = 0; }
		]
		FormalType(&tp)
			{ *ppr = p = new_paramlist();
			  p->par_type = tp;
			  p->par_var = VARp;
			}
		[
			','
			[ VAR	{VARp = 1; }
			|	{VARp = 0; }
			] 
			FormalType(&tp)
				{ p->next = new_paramlist();
				  p = p->next;
				  p->par_type = tp;
				  p->par_var = VARp;
				}
		]*
				{ p->next = 0; }
	]?
	')'
	[ ':' qualident(D_TYPE|D_HTYPE, &df, "type", (struct node **) 0)
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
	struct node *nd = 0;
} :
	IdentList(&VarList)
	[
		ConstExpression(&nd)
	]?
	':' type(&tp)
			{ EnterIdList(VarList, D_VARIABLE, 0, tp, CurrentScope);
			  FreeNode(VarList);
			}
;
