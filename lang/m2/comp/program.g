/* O V E R A L L   S T R U C T U R E */

{
static  char *RcsId = "$Header$";

#include	<alloc.h>
#include	<em_arith.h>
#include	"idf.h"
#include	"misc.h"
#include	"main.h"
#include	"LLlex.h"
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
} :
	EXPORT QUALIFIED? IdentList(&ExportList) ';'
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

DefinitionModule:
	DEFINITION	{ state = DEFINITION; }
	MODULE IDENT
	';'
	import(0)* 
	/*	export?

	   	New Modula-2 does not have export lists in definition modules.
	*/
	definition* END IDENT '.'
;

definition:
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
	ProcedureHeading ';'
;

ProgramModule:
	MODULE		{ if (state != IMPLEMENTATION) state = PROGRAM; }
	IDENT priority? ';' import(0)* block IDENT '.'
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
