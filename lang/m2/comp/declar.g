{
#include "idf.h"
#include "idlist.h"
#include "LLlex.h"

static char *RcsId = "$Header$";
}

ProcedureDeclaration:
	ProcedureHeading ';' block IDENT
;

ProcedureHeading:
	PROCEDURE IDENT FormalParameters?
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
	[ ':' qualident ]?
;

FPSection
{
	struct id_list *FPList;
} :
	VAR? IdentList(&FPList) ':' FormalType
;

FormalType:
	[ ARRAY OF ]? qualident
;

TypeDeclaration:
	IDENT '=' type
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

ConstantDeclaration:
	IDENT '=' ConstExpression
;

VariableDeclaration
{
	struct id_list *VarList;
} :
	IdentList(&VarList) ':' type
;
