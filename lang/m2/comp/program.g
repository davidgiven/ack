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

static int DEFofIMPL = 0;	/* Flag indicating that we are currently
				   parsing the definition module of the
				   implementation module currently being
				   compiled
				*/
struct def *currentdef;		/* current definition of module or procedure */
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
	register struct def *df;
	struct def *savecurr = currentdef;
	extern int proclevel;
	static int modulecount = 0;
	char buf[256];
	struct node *nd;
	extern char *sprint(), *Malloc(), *strcpy();
} :
	MODULE IDENT	{
			  id = dot.TOK_IDF;
			  df = define(id, CurrentScope, D_MODULE);
			  currentdef = df;

			  if (!df->mod_scope) {	
			  	open_scope(CLOSEDSCOPE);
			  	df->mod_scope = CurrentScope;
			  }
			  else	CurrentScope = df->mod_scope;

			  df->df_type = standard_type(T_RECORD, 0, (arith) 0);
			  df->df_type->rec_scope = df->mod_scope;
			  df->mod_number = ++modulecount;
			  sprint(buf, "__%d%s", df->mod_number, id->id_text);
			  CurrentScope->sc_name =
				Malloc((unsigned) (strlen(buf) + 1));
			  strcpy(CurrentScope->sc_name, buf);
			  C_ina_dnam(&buf[1]);
			  C_inp(buf);
			}
	priority(&(df->mod_priority))?
	';'
	import(1)*
	export(0)?
	block(&nd)
	IDENT		{ InitProc(nd, df);
			  close_scope(SC_CHKFORW|SC_CHKPROC|SC_REVERSE);
			  match_id(id, dot.TOK_IDF);
			  currentdef = savecurr;
			}
;

priority(arith *pprio;)
{
	struct node *nd;
} :
	'[' ConstExpression(&nd) ']'
			{ if (!(nd->nd_type->tp_fund & T_INTORCARD)) {
				node_error(nd, "Illegal priority");
			  }
			  *pprio = nd->nd_INT;
			  FreeNode(nd);
			}
;

export(int def;)
{
	struct node *ExportList;
	int QUALflag = 0;
} :
	EXPORT
	[
		QUALIFIED
			{ QUALflag = 1; }
	]?
	IdentList(&ExportList) ';'
			{
			  if (!def) {
				Export(ExportList, QUALflag);
			  }
			  else {
node_warning(ExportList, "export list in definition module ignored");
				FreeNode(ExportList);
			  }
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
			}
;

DefinitionModule
{
	register struct def *df;
	struct idf *id;
} :
	DEFINITION
	MODULE IDENT	{ 
			  id = dot.TOK_IDF;
			  df = define(id, GlobalScope, D_MODULE);
			  if (!SYSTEMModule) open_scope(CLOSEDSCOPE);
			  if (!Defined) Defined = df;
			  df->mod_scope = CurrentScope;
			  df->mod_number = 0;
			  CurrentScope->sc_name = id->id_text;
			  df->df_type = standard_type(T_RECORD, 0, (arith) 0);
			  df->df_type->rec_scope = df->mod_scope;
			  DefinitionModule++;
			  DO_DEBUG(1, debug("Definition module \"%s\" %d",
					id->id_text, DefinitionModule));
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
			  if (DEFofIMPL) {
				/* Just read the definition module of the
				   implementation module being compiled
				*/
				RemImports(&(CurrentScope->sc_def));
			  }
			  df = CurrentScope->sc_def;
			  while (df) {
				/* Make all definitions "QUALIFIED EXPORT" */
				df->df_flags |= D_QEXPORTED;
				df = df->df_nextinscope;
			  }
			  if (!SYSTEMModule) close_scope(SC_CHKFORW);
			  DefinitionModule--;
			  match_id(id, dot.TOK_IDF);
			}
	'.'
;

definition
{
	struct def *df;
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
			  df->df_type = construct_type(T_POINTER, NULLTYPE);
			}
	  ]
	  Semicolon
	]*
|
	VAR [ VariableDeclaration Semicolon ]*
|
	ProcedureHeading(&df, D_PROCHEAD) Semicolon
;

Semicolon:
	';'
|
			{ warning("; expected"); }
;

ProgramModule(int state;)
{
	struct idf *id;
	struct def *GetDefinitionModule();
	register struct def *df;
	struct node *nd;
} :
	MODULE
	IDENT	{ 
		  id = dot.TOK_IDF;
		  if (state == IMPLEMENTATION) {
			DEFofIMPL = 1;
			df = GetDefinitionModule(id);
			currentdef = df;
			CurrentScope = df->mod_scope;
			DEFofIMPL = 0;
		  }
		  else {
			df = define(id, CurrentScope, D_MODULE);
		  	Defined = df;
			open_scope(CLOSEDSCOPE);
			df->mod_scope = CurrentScope;
			df->mod_number = 0;
			CurrentScope->sc_name = id->id_text;
		  }
		}
	priority(&(df->mod_priority))?
	';' import(0)*
	block(&nd) IDENT
		{ InitProc(nd, df);
		  close_scope(SC_CHKFORW|SC_CHKPROC|SC_REVERSE);
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
