/* D E C L A R A T I O N S */

{
static char *RcsId = "$Header$";

#include	<em_arith.h>
#include	<em_label.h>
#include	"idf.h"
#include	"misc.h"
#include	"LLlex.h"
#include	"def.h"
#include	"type.h"
#include	"scope.h"
}

ProcedureDeclaration
{
	register struct def *df;
} :
	/* ProcedureHeading(&df) */
	PROCEDURE IDENT
			{ df = define(dot.TOK_IDF, CurrentScope, D_PROCEDURE);
			  open_scope(OPENSCOPE, 0);
			}
	FormalParameters?
	';' block IDENT
			{ match_id(dot.TOK_IDF, df->df_idf);
			  close_scope();
			}
;

ProcedureHeading
{
	register struct def *df;
} :
	/*	Only used for definition modules
	*/
	PROCEDURE IDENT
			{ df = define(dot.TOK_IDF, CurrentScope, D_PROCHEAD); }
	FormalParameters?
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

FormalParameters:
	'(' [ FPSection [ ';' FPSection ]* ]? ')'
	[ ':' qualident
	]?
;

FPSection
{
	struct id_list *FPList;
	int VARflag = 0;
} :
	[
		VAR	{ VARflag = 1; }
	]?
	IdentList(&FPList) ':' FormalType
			{
			  FreeIdList(FPList);
			}
;

FormalType:
	[ ARRAY OF ]? qualident
;

TypeDeclaration
{
	register struct def *df;
}:
	IDENT		{ df = define(dot.TOK_IDF, CurrentScope, D_TYPE); }
	'=' type
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
;

IdentList(struct id_list **p;)
{
	register struct id_list *q = new_id_list();
} :
	IDENT			{ q->id_ptr = dot.TOK_IDF; }
	[
		',' IDENT	{ q->next = new_id_list();
				  q = q->next;
				  q->id_ptr = dot.TOK_IDF;
				}
	]*
				{ q->next = 0;
				  *p = q;
				}
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
	register struct def *df;
}:
	IDENT		{ df = define(dot.TOK_IDF, CurrentScope, D_CONST); }
	'=' ConstExpression
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
;
