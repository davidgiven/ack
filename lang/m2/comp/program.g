/* O V E R A L L   S T R U C T U R E */

{
#ifndef NORCSID
static  char *RcsId = "$Header$";
#endif

#include	"debug.h"

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
#include	"f_info.h"

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
	struct idf *id;			/* save module identifier */
	register struct def *df;
	struct node *exportlist = 0;
	int qualified;
} :
	MODULE IDENT	{ id = dot.TOK_IDF;
			  df = DefineLocalModule(id);
			}
	priority(&(df->mod_priority))?
	';'
	import(1)*
	export(&qualified, &exportlist)?
	block(&(df->mod_body))
	IDENT		{ if (exportlist) {
				EnterExportList(exportlist, qualified);
			  }
			  close_scope(SC_CHKFORW|SC_CHKPROC|SC_REVERSE);
			  match_id(id, dot.TOK_IDF);
			}
;

priority(arith *pprio;)
{
	struct node *nd;
} :
	'[' ConstExpression(&nd) ']'
			{ if (!(nd->nd_type->tp_fund & T_CARDINAL)) {
				node_error(nd, "Illegal priority");
			  }
			  *pprio = nd->nd_INT;
			  FreeNode(nd);
			}
;

export(int *QUALflag; struct node **ExportList;)
{
} :
	EXPORT
	[
		QUALIFIED
			{ *QUALflag = D_QEXPORTED; }
	|
			{ *QUALflag = D_EXPORTED; }
	]
	IdentList(ExportList) ';'
;

import(int local;)
{
	struct node *ImportList;
	register struct def *df;
	int fromid;
	extern struct def *GetDefinitionModule();
} :
	[ FROM
	  IDENT		{ fromid = 1;
			  if (local) {
				struct node *nd = MkLeaf(Name, &dot);

				df = lookfor(nd,enclosing(CurrVis),0);
				FreeNode(nd);
			  }
			  else	df = GetDefinitionModule(dot.TOK_IDF);
			}
	|
			{ fromid = 0; }
	]
	IMPORT IdentList(&ImportList) ';'
	/*
	   When parsing a global module, this is the place where we must
	   read already compiled definition modules.
	   If the FROM clause is present, the identifier in it is a module
	   name, otherwise the names in the import list are module names.
	*/
			{ if (fromid) EnterFromImportList(ImportList, df);
			  else EnterImportList(ImportList, local);
			}
;

DefinitionModule
{
	register struct def *df;
	struct idf *id;			/* save module identifier */
	struct node *exportlist;
	int dummy;
} :
	DEFINITION
	MODULE IDENT	{ id = dot.TOK_IDF;
			  df = define(id, GlobalScope, D_MODULE);
			  if (!Defined) Defined = df;
			  if (!SYSTEMModule) open_scope(CLOSEDSCOPE);
			  CurrentScope->sc_name = id->id_text;
			  df->mod_vis = CurrVis;
			  df->df_type = standard_type(T_RECORD, 0, (arith) 0);
			  df->df_type->rec_scope = df->mod_vis->sc_scope;
			  DefinitionModule++;
			}
	';'
	import(0)* 
	[
		export(&dummy, &exportlist)
		/*	New Modula-2 does not have export lists in definition
			modules. Issue a warning.
		*/
			{ 
node_warning(exportlist, "export list in definition module ignored");
				FreeNode(exportlist);
			}
	|
		/* empty */
	]
	definition* END IDENT
			{ df = CurrentScope->sc_def;
			  while (df) {
				/* Make all definitions "QUALIFIED EXPORT" */
				df->df_flags |= D_QEXPORTED;
				df = df->df_nextinscope;
			  }
			  close_scope(SC_CHKFORW);
			  DefinitionModule--;
			  match_id(id, dot.TOK_IDF);
			}
	'.'
;

definition
{
	register struct def *df;
	struct def *dummy;
} :
	CONST [ ConstantDeclaration Semicolon ]*
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
	    		{ df->df_kind = D_HIDDEN;
			  df->df_type = construct_type(T_HIDDEN, NULLTYPE);
			}
	  ]
	  Semicolon
	]*
|
	VAR [ VariableDeclaration Semicolon ]*
|
	ProcedureHeading(&dummy, D_PROCHEAD)
	Semicolon
;

/*	The next nonterminal is used to relax the grammar a little.
*/
Semicolon:
	';'
|
	/* empty */	{ warning("; expected"); }
;

ProgramModule
{
	struct idf *id;
	struct def *GetDefinitionModule();
	register struct def *df;
} :
	MODULE
	IDENT	{ id = dot.TOK_IDF;
		  if (state == IMPLEMENTATION) {
			df = GetDefinitionModule(id);
			CurrVis = df->mod_vis;
			RemoveImports(&(CurrentScope->sc_def));
		  }
		  else {
			Defined = df = define(id, CurrentScope, D_MODULE);
			open_scope(CLOSEDSCOPE);
			df->mod_vis = CurrVis;
			CurrentScope->sc_name = "_M2M";
		  }
		  CurrentScope->sc_definedby = df;
		}
	priority(&(df->mod_priority))?
	';' import(0)*
	block(&(df->mod_body)) IDENT
		{ close_scope(SC_CHKFORW|SC_CHKPROC|SC_REVERSE);
		  match_id(id, dot.TOK_IDF);
		}
	'.'
;

Module:
	DefinitionModule
|
	[
		IMPLEMENTATION	{ state = IMPLEMENTATION; }
	|
		/* empty */	{ state = PROGRAM; }
	]
	ProgramModule
;

CompilationUnit:
	Module
;
