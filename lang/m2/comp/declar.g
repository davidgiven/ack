/* D E C L A R A T I O N S */

{
static char *RcsId = "$Header$";

#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>
#include	"idf.h"
#include	"misc.h"
#include	"LLlex.h"
#include	"def.h"
#include	"type.h"
#include	"scope.h"
}

ProcedureDeclaration
{
	struct def *df;
} :
	ProcedureHeading(&df, D_PROCEDURE)
	';' block IDENT
			{ match_id(dot.TOK_IDF, df->df_idf);
			  close_scope();
			}
;

ProcedureHeading(struct def **pdf; int type;)
{
	struct type *tp;
	struct paramlist *params = 0;
} :
	PROCEDURE IDENT
			{ assert(type == D_PROCEDURE || type == D_PROCHEAD);
			  *pdf = define(dot.TOK_IDF, CurrentScope, type);
			  if (type == D_PROCEDURE) {
				open_scope(OPENSCOPE, 0);
			  }
			}
	FormalParameters(type, &params, &tp)?
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
		[
			{ for (pr1 = *pr; pr1->next; pr1 = pr1->next) ; }
			';' FPSection(doparams, &(pr1->next))
		]*
	]?
	')'
			{ *tp = 0; }
	[ ':' qualident(D_TYPE | D_HTYPE, &df, "type")
			{ /* ???? *tp = df->df_type; */ }
	]?
;

FPSection(int doparams; struct paramlist **ppr;)
{
	struct id_list *FPList;
	register struct id_list *pid;
	register struct paramlist *pr = 0;
	int VARflag = 0;
} :
	[
		VAR	{ VARflag = 1; }
	]?
	IdentList(&FPList) ':' FormalType
			{
			  if (doparams) {
				EnterIdList(FPList,
					    D_VARIABLE,
					    VARflag,
					    (struct type *) 0	/* ???? */,
					    CurrentScope
				);
			  }
			  *ppr = pr = new_paramlist();
			  pr->par_type = 0;	/* ??? */
			  pr->par_var = VARflag;
			  for (pid = FPList->next; pid; pid = pid->next) {
				pr->next = new_paramlist();
				pr = pr->next;
				pr->par_type = 0;	/* ??? */
				pr->par_var = VARflag;
			  }
			  pr->next = 0;
			  FreeIdList(FPList);
			}
;

FormalType
{
	struct def *df;
	int ARRAYflag = 0;
} :
	[ ARRAY OF	{ ARRAYflag = 1; }
	]?
	qualident(D_TYPE | D_HTYPE, &df, "type")
;

TypeDeclaration
{
	struct def *df;
	struct type *tp;
}:
	IDENT		{ df = define(dot.TOK_IDF, CurrentScope, D_TYPE); }
	'=' type(&tp)
			{ df->df_type = tp;
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
	qualident(D_TYPE | D_HTYPE, &df, "type")
	[

	|
		SubrangeType(ptp)
		/*
		 * The subrange type is given a base type by the
		 * qualident (this is new modula-2).
		 */
			{ /* ???? (*ptp)->next = df->df_type; */ }
	]
|
	enumeration(ptp)
|
	SubrangeType(ptp)
;

enumeration(struct type **ptp;)
{
	struct id_list *EnumList;
} :
	'(' IdentList(&EnumList) ')'
			{
			  *ptp = standard_type(ENUMERATION,int_align,int_size);
			  EnterIdList(EnumList, D_ENUM, 0, *ptp, CurrentScope);
			  FreeIdList(EnumList);
			}

;

IdentList(struct id_list **p;)
{
	register struct id_list *q = new_id_list();
} :
	IDENT			{ q->id_ptr = dot.TOK_IDF;  *p = q;}
	[
		',' IDENT	{ q->next = new_id_list();
				  q = q->next;
				  q->id_ptr = dot.TOK_IDF;
				}
	]*
				{ q->next = 0; }
;

SubrangeType(struct type **ptp;)
{
	struct type *tp;
}:
	/*
	   This is not exactly the rule in the new report, but see
	   the rule for "SimpleType".
	*/
	'[' ConstExpression
	UPTO ConstExpression
	']'
	/*
	   Evaluate the expressions. Check that they are indeed constant.
	   ???
	   Leave the basetype of the subrange in tp;
	*/
			{
			  /* For the time being: */
			  tp = int_type;
			  tp = construct_type(SUBRANGE, tp, (arith) 0);
			  *ptp = tp;
			}
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
			{ tp2 = tp2->tp_value.tp_arr.ar_elem = 
				construct_type(ARRAY, tp);
			}
	]* OF type(&tp)
			{ tp2->tp_value.tp_arr.ar_elem = tp; }
;

RecordType(struct type **ptp;)
{
	int scopenr;
}
:
	RECORD
			{ scopenr = uniq_scope(); }
	FieldListSequence(scopenr)
			{
			  *ptp = standard_type(RECORD, record_align, (arith) 0 /* ???? */);
			  (*ptp)->tp_value.tp_record.rc_scopenr = scopenr;
			}
	END
;

FieldListSequence(int scopenr;):
	FieldList(scopenr)
	[
		';' FieldList(scopenr)
	]*
;

FieldList(int scopenr;)
{
	struct id_list *FldList;
	struct idf *id;
	struct def *df, *df1;
	struct type *tp;
} :
[
	IdentList(&FldList) ':' type(&tp)
|
	CASE
	[
		IDENT		{ id = dot.TOK_IDF; }
	|
				{ id = gen_anon_idf(); }
	]			/* Changed rule in new modula-2 */
	':' qualident(D_TYPE|D_HTYPE, &df, "type")
				{ df1 = define(id, scopenr, D_FIELD);
				  df1->df_type = df->df_type;
				}
	OF variant(scopenr)
	[
		'|' variant(scopenr)
	]*
	[ ELSE FieldListSequence(scopenr)
	]?
	END
]?
;

variant(int scopenr;):
	[ CaseLabelList ':' FieldListSequence(scopenr) ]?
					/* Changed rule in new modula-2 */
;

CaseLabelList:
	CaseLabels [ ',' CaseLabels ]*
;

CaseLabels:
	ConstExpression [ UPTO ConstExpression ]?
;

SetType(struct type **ptp;)
{
	struct type *tp;
} :
	SET OF SimpleType(&tp)
			{
			  *ptp = construct_type(SET, tp, (arith) 0 /* ???? */);
			}
;

PointerType(struct type **ptp;)
{
	struct type *tp;
	register struct def *df;
	struct def *lookfor();
} :
	POINTER TO
	[ %if ( (df = lookup(dot.TOK_IDF, CurrentScope)))
		IDENT
				{
				  if (!(df->df_kind & (D_TYPE | D_HTYPE))) {
					error("\"%s\" is not a type identifier",
						df->df_idf->id_text);
				  }
				  if (!df->df_type) {
					error("type \"%s\" not declared",
						df->df_idf->id_text);
				  }
				  *ptp = df->df_type;
				}
	| %if (df = lookfor(dot.TOK_IDF, 0), df->df_kind == D_MODULE)
		type(&tp)
				{ *ptp = construct_type(POINTER, tp); }
	|
		IDENT
				{ *ptp = construct_type(POINTER, NULLTYPE);
				  Forward(&dot, &((*ptp)->next));
				}
	]
;

ProcedureType(struct type **ptp;):
	PROCEDURE FormalTypeList?
			{ *ptp = 0; }
;

FormalTypeList
{
	struct def *df;
} :
	'(' [ VAR? FormalType [ ',' VAR? FormalType ]* ]? ')'
	[ ':' qualident(1, &df, "type")
	]?
;

ConstantDeclaration
{
	struct def *df;
	struct idf *id;
}:
	IDENT			{ id = dot.TOK_IDF; }
	'=' ConstExpression	{ df = define(id, CurrentScope, D_CONST);
				  /* ???? */
				}
;

VariableDeclaration
{
	struct id_list *VarList;
	struct type *tp;
} :
	IdentList(&VarList)
	[
		ConstExpression
	]?
	':' type(&tp)
			{ EnterIdList(VarList, D_VARIABLE, 0, tp, CurrentScope);
			  FreeIdList(VarList);
			}
;
