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
} :
	PROCEDURE IDENT
			{ assert(type == D_PROCEDURE || type == D_PROCHEAD);
			  *pdf = define(dot.TOK_IDF, CurrentScope, D_PROCHEAD);
			  if (type == D_PROCEDURE) {
				open_scope(OPENSCOPE, 0);
			  }
			}
	FormalParameters(type, &((*pdf)->df_type))?
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

FormalParameters(int doparams; struct type **tp;) :
	'(' [ FPSection(doparams) [ ';' FPSection(doparams)]* ]? ')'
	[ ':' qualident
	]?
;

FPSection(int doparams;)
{
	struct id_list *FPList;
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
					    (struct type *) 0	/* ???? */
				);
			  }
			  FreeIdList(FPList);
			}
;

FormalType:
	[ ARRAY OF ]? qualident
;

TypeDeclaration
{
	struct def *df;
	struct idf *id;
}:
	IDENT		{ id = dot.TOK_IDF; }
	'=' type	{ df = define(id, CurrentScope, D_TYPE);
			  /* ???? */
			}
;

type:
	SimpleType
|
	ArrayType
|
	RecordType
|
	SetType
|
	PointerType
|
	ProcedureType
;

SimpleType:
	qualident
	[

	|
		SubrangeType
		/*
		 * The subrange type is given a base type by the
		 * qualident (this is new modula-2).
		 */
	]
|
	enumeration
|
	SubrangeType
;

enumeration
{
	struct id_list *EnumList;
} :
	'(' IdentList(&EnumList) ')'
			{
			  EnterIdList(EnumList,
				      D_ENUM,
				      0,
				      (struct type *) 0 /* ???? */
			  );
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

SubrangeType:
	/*
	   This is not exactly the rule in the new report, but see
	   the rule for "SimpleType".
	*/
	'[' ConstExpression UPTO ConstExpression ']'
;

ArrayType:
	ARRAY SimpleType [ ',' SimpleType ]* OF type
;

RecordType:
	RECORD FieldListSequence END
;

FieldListSequence:
	FieldList [ ';' FieldList ]*
;

FieldList
{
	struct id_list *FldList;
} :
[
	IdentList(&FldList) ':' type
|
	CASE IDENT?			/* Changed rule in new modula-2 */
	':' qualident
	OF variant [ '|' variant ]*
	[ ELSE FieldListSequence ]?
	END
]?
;

variant:
	[ CaseLabelList ':' FieldListSequence ]?
					/* Changed rule in new modula-2 */
;

CaseLabelList:
	CaseLabels [ ',' CaseLabels ]*
;

CaseLabels:
	ConstExpression [ UPTO ConstExpression ]?
;

SetType:
	SET OF SimpleType
;

PointerType:
	POINTER TO type
;

ProcedureType:
	PROCEDURE FormalTypeList?
;

FormalTypeList:
	'(' [ VAR? FormalType [ ',' VAR? FormalType ]* ]? ')'
	[ ':' qualident ]?
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
} :
	IdentList(&VarList)
	[
		ConstExpression
	]?
	':' type
			{ EnterIdList(VarList,
				      D_VARIABLE,
				      0,
				      (struct type *) 0	/* ???? */
				     );
			  FreeIdList(VarList);
			}
;
