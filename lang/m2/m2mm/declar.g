/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* D E C L A R A T I O N S */

/* stripped down version of the one in the Modula-2 compiler */

/* $Id$ */

ProcedureHeading :
	PROCEDURE IDENT
	[
		'('
		[
			FPSection
			[
				';' FPSection
			]*
		|
		]
		')'
		[
			':' qualtype
		|
			/* empty */
		]
	|
		/* empty */
	]
;

block :
	[ %persistent
		declaration
	]*
	[ %default
		BEGIN StatementSequence
	|
		/* empty */
	]
	END
;

declaration :
	CONST [ ConstantDeclaration ';' ]*
|
	TYPE [ TypeDeclaration ';' ]*
|
	VAR [ VariableDeclaration ';' ]*
|
	ProcedureHeading ';' block IDENT ';'
|
	ModuleDeclaration ';'
;

FPSection :
	var IdentList ':' FormalType
;

FormalType :
	ARRAY OF qualtype
|
	 qualtype
;

TypeDeclaration :
	IDENT '=' type
;

type :
 %default
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

SimpleType :
	qualtype
	[
		/* empty */
	|
		SubrangeType
		/* The subrange type is given a base type by the
		   qualident (this is new modula-2).
		*/
	]
|
	enumeration
|
	SubrangeType
;

enumeration :
	'(' IdentList ')'
;

IdentList :
	IDENT
	[ %persistent
		',' IDENT
	]*
;

SubrangeType :
	/*
	   This is not exactly the rule in the new report, but see
	   the rule for "SimpleType".
	*/
	'[' ConstExpression UPTO ConstExpression ']'
;

ArrayType :
	ARRAY SimpleType
	[
		',' SimpleType
	]*
	OF type
;

RecordType :
	RECORD FieldListSequence END
;

FieldListSequence :
	FieldList
	[
		';' FieldList
	]*
;

FieldList :
	IdentList ':' type
|
	CASE
	/* Also accept old fashioned Modula-2 syntax, but give a warning.
	   Sorry for the complicated code.
	*/
	[
		qualident
	  	[
			':' qualtype
			/* This is correct, in both kinds of Modula-2, if
		   	   the first qualident is a single identifier.
			*/
	  	|
			/* empty */
			/* Old fashioned! the first qualident now represents
			   the type
			*/
	  	]
	|
		':' qualtype
	  		/* Aha, third edition. Well done! */
	]
	OF variant
	[
	  	'|' variant
	]*
	[
		ELSE FieldListSequence
	|
		/* empty */
	]
	END
|
	/* empty */
;

variant :
	[
		CaseLabelList ':' FieldListSequence
	|
		/* empty */
	]
			/* Changed rule in new modula-2 */
;

CaseLabelList :
	CaseLabels
	[	
		',' CaseLabels
	]*
;

CaseLabels :
	ConstExpression
	[
		UPTO ConstExpression
	|
		/* empty */
	]
;

SetType :
	SET OF SimpleType
;

/*	In a pointer type definition, the type pointed at does not
	have to be declared yet, so be careful about identifying
	type-identifiers
*/
PointerType :
	POINTER TO type
;

qualtype :
	qualident
;

ProcedureType :
	PROCEDURE 
	[
	  	FormalTypeList
	|
		/* empty */
	]
;

FormalTypeList :
	'('
	[
		VarFormalType
		[
			',' VarFormalType
		]*
	|
		/* empty */
	]
	')'
	[	
		':' qualtype
	|
		/* empty */
	]
;

VarFormalType :
	var FormalType
;

var :
	VAR
|
	/* empty */
;

ConstantDeclaration :
	IDENT '=' ConstExpression
;

VariableDeclaration :
	IdentAddr
	[ %persistent
		',' IdentAddr
	]*
	':' type
;

IdentAddr :
	IDENT
	[	
		'[' ConstExpression ']'
	|
		/* empty */
	]
;
