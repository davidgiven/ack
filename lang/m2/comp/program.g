/* O V E R A L L   S T R U C T U R E */

{
static  char *RcsId = "$Header$";

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	"main.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"scope.h"
#include	"def.h"
#include	"type.h"
#include	"node.h"
#include	"debug.h"

static struct idf *impl_name = 0;
static struct def *impl_df;
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
%start	DefModule, DefinitionModule;

ModuleDeclaration
{
	struct idf *id;
	struct def *df;
} :
	MODULE IDENT		{
				  id = dot.TOK_IDF;
				  df = define(id, CurrentScope, D_MODULE);
				  open_scope(CLOSEDSCOPE, 0);
				  df->mod_scope = CurrentScope->sc_scope;
				  df->df_type = 
					standard_type(RECORD, 0, (arith) 0);
				  df->df_type->rec_scope = df->mod_scope;
				}
	priority? ';'
	import(1)*
	export(0)?
	block
	IDENT			{ close_scope();
				  match_id(id, dot.TOK_IDF);
				}
;

priority
{
	struct node *nd;
}:
	'[' ConstExpression(&nd) ']'
;

export(int def;)
{
	struct node *ExportList;
	int QUALflag = 0;
} :
	EXPORT
	[
		QUALIFIED	{ QUALflag = 1; }
	]?
	IdentList(&ExportList) ';'
			{
			  if (!def) Export(ExportList, QUALflag);
			  else warning("export list in definition module ignored");
			  FreeNode(ExportList);
			}
;

import(int local;)
{
	struct node *ImportList;
	struct node *id = 0;
} :
	[ FROM
	  IDENT		{ id = MkNode(Value, NULLNODE, NULLNODE, &dot); }
	]?
	IMPORT IdentList(&ImportList) ';'
	/*
	   When parsing a global module, this is the place where we must
	   read already compiled definition modules.
	   If the FROM clause is present, the identifier in it is a module
	   name, otherwise the names in the import list are module names.
	*/
			{
			  Import(ImportList, id, local);
			  FreeNode(ImportList);
			  if (id) FreeNode(id);
			}
;

DefinitionModule
{
	register struct def *df;
	struct idf *id;
} :
	DEFINITION
	MODULE IDENT	{ id = dot.TOK_IDF;
			  df = define(id, GlobalScope, D_MODULE);
			  if (!SYSTEMModule) open_scope(CLOSEDSCOPE, 0);
			  df->mod_scope = CurrentScope->sc_scope;
			  df->df_type = standard_type(RECORD, 0, (arith) 0);
			  df->df_type->rec_scope = df->mod_scope;
			  DefinitionModule = 1;
			  DO_DEBUG(1, debug("Definition module \"%s\"", id->id_text));
			}
	';'
	import(0)* 
	export(1)?
	/*	New Modula-2 does not have export lists in definition modules.
		For the time being, we ignore export lists here, and a
		warning is issued.
	*/
	definition* END IDENT
			{
			  if (id == impl_name) {
				/* Just read the definition module of the
				   implementation module being compiled
				*/
				RemImports(&(CurrentScope->sc_def));
				impl_df = CurrentScope->sc_def;
			  }
			  df = CurrentScope->sc_def;
			  while (df) {
				/* Make all definitions "QUALIFIED EXPORT" */
				df->df_flags |= D_QEXPORTED;
				df = df->df_nextinscope;
			  }
			  if (!SYSTEMModule) close_scope();
			  DefinitionModule = 0;
			  match_id(id, dot.TOK_IDF);
			}
	'.'
;

definition
{
	struct def *df;
} :
	CONST [ ConstantDeclaration ';' ]*
|
	TYPE
	[ IDENT 	{ df = define(dot.TOK_IDF, CurrentScope, D_TYPE); }
	  [ '=' type(&(df->df_type))
	  | /* empty */
	    /*
	       Here, the exported type has a hidden implementation.
	       The export is said to be opaque.
	       It is restricted to pointer types.
	    */
	    		{ df->df_kind = D_HIDDEN; }
	  ]
	  ';'
	]*
|
	VAR [ VariableDeclaration ';' ]*
|
	ProcedureHeading(&df, D_PROCHEAD) ';'
;

ProgramModule(int state;)
{
	struct idf *id;
	struct def *df, *GetDefinitionModule();
	int scope = 0;
} :
	MODULE
	IDENT		{ 
			  id = dot.TOK_IDF;
			  if (state == IMPLEMENTATION) {
				   impl_name = id;
				   df = GetDefinitionModule(id);
				   scope = df->mod_scope;
			  }
			  DefinitionModule = 0;
			  open_scope(CLOSEDSCOPE, scope);
			  CurrentScope->sc_def = impl_df;
			}
	priority?
	';' import(0)*
	block IDENT
			{ close_scope();
			  match_id(id, dot.TOK_IDF);
			}
	'.'
;

Module
{
	int state = PROGRAM;
} :
	DefinitionModule
|
	[
		IMPLEMENTATION	{ state = IMPLEMENTATION; }
	]?
	ProgramModule(state)
;

CompilationUnit:
	Module
;
