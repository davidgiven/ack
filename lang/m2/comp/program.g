/* O V E R A L L   S T R U C T U R E */

{
static  char *RcsId = "$Header$";

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	"idf.h"
#include	"misc.h"
#include	"main.h"
#include	"LLlex.h"
#include	"scope.h"
#include	"def.h"
#include	"type.h"
}
/*
	The grammar as given by Wirth is already almost LL(1); the
	main problem is that the full form of a qualified designator
	may be:
		[ module_ident '.' ]* IDENT [ '.' field_ident ]*
	which is quite confusing to an LL(1) parser. Rather than
	resorting to context-sensitive techniques, I have decided
	to render this as:
		IDENT [ '.' IDENT ]*
	on the grounds that it is quite natural to consider the first
	IDENT to be the name of the object and regard the others as
	field identifiers.
*/

%lexical LLlex;

%start	CompUnit, CompilationUnit;

ModuleDeclaration:
	MODULE IDENT priority? ';' import(1)* export? block IDENT
;

priority:
	'[' ConstExpression ']'
;

export
{
	struct id_list *ExportList;
	int QUALflag = 0;
} :
	EXPORT
	[
		QUALIFIED	{ QUALflag = 1; }
	]?
	IdentList(&ExportList) ';'
			{
			  FreeIdList(ExportList);
			}
;

import(int local;)
{
	struct id_list *ImportList;
	struct idf *id = 0;
} :
	[ FROM
	  IDENT		{ id = dot.TOK_IDF; }
	]?
	IMPORT IdentList(&ImportList) ';'
	/*
	   When parsing a global module, this is the place where we must
	   read already compiled definition modules.
	   If the FROM clause is present, the identifier in it is a module
	   name, otherwise the names in the import list are module names.
	*/
			{
			  FreeIdList(ImportList);
			}
;

DefinitionModule
{
	struct def *df;
} :
	DEFINITION	{ state = DEFINITION; }
	MODULE IDENT	{ 
			  df = define(dot.TOK_IDF, CurrentScope, D_MODULE);
			  open_scope(CLOSEDSCOPE, 0);
			  df->df_value.df_module.mo_scope = CurrentScope->sc_scope;
			}
	';'
	import(0)* 
	/*	export?

	   	New Modula-2 does not have export lists in definition modules.
	*/
	definition* END IDENT '.'
			{ close_scope(); }
;

definition
{
	struct def *df;
} :
	CONST [ ConstantDeclaration ';' ]*
|
	TYPE
	[ IDENT 
	  [ '=' type 
	  | /* empty */
	    /*
	       Here, the exported type has a hidden implementation.
	       The export is said to be opaque.
	       It is restricted to pointer types.
	    */
	  ]
	  ';'
	]*
|
	VAR [ VariableDeclaration ';' ]*
|
	ProcedureHeading(&df, D_PROCHEAD) ';'
;

ProgramModule:
	MODULE		{ if (state != IMPLEMENTATION) state = PROGRAM; }
	IDENT		{ if (state == IMPLEMENTATION) {
				/* ????
				   Read definition module,
				   Look for current identifier,
				   and find out its scope number
				*/
				open_scope(CLOSEDSCOPE, 0);
			  }
			  else	open_scope(CLOSEDSCOPE, 0);
			}
	priority? ';' import(0)* block IDENT
			{ close_scope(); }
	'.'
;

Module:
	DefinitionModule
|
	[
		IMPLEMENTATION	{ state = IMPLEMENTATION; }
	]?
	ProgramModule
;

CompilationUnit:
	Module
;
