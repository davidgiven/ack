/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* O V E R A L L   S T R U C T U R E */

/* $Header$ */

{
#include	"debug.h"

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include	<stb.h>

#include	"strict3rd.h"
#include	"dbsymtab.h"
#include	"main.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"scope.h"
#include	"def.h"
#include	"type.h"
#include	"node.h"
#include	"f_info.h"
#include	"warning.h"
#include	"misc.h"

extern t_def	*GetDefinitionModule();

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
	register t_def	*df;
	t_node		*exportlist;
	int		qualified;
} :
	MODULE IDENT	{ df = DefineLocalModule(dot.TOK_IDF); }
	priority(&(df->mod_priority))
	';'
	import(1)*
	export(&qualified, &exportlist)
			{
#ifdef DBSYMTAB
			  if (options['g']) stb_string(df, D_MODULE);
#endif /* DBSYMTAB */
			}
	block(&(df->mod_body))
	IDENT		{ EnterExportList(exportlist, qualified);
#ifdef DBSYMTAB
			  if (options['g']) stb_string(df, D_END);
#endif /* DBSYMTAB */
			  close_scope(SC_CHKFORW|SC_CHKPROC|SC_REVERSE);
			  match_id(df->df_idf, dot.TOK_IDF);
			}
;

priority(register t_node **prio;):
	[
		'[' ConstExpression(prio) ']'
			{ if (! ((*prio)->nd_type->tp_fund & T_CARDINAL)) {
				node_error(*prio, "illegal priority");
			  }
			}
	|
	]
;

export(int *QUALflag; t_node **ExportList;):
				{ *ExportList = 0; *QUALflag = D_EXPORTED; }
	[
		EXPORT
		[
			QUALIFIED
				{ *QUALflag = D_QEXPORTED; }
		|
		]
		IdentList(ExportList) ';'
	|
	]
;

import(int local;)
{
	t_node		*ImportList;
	register t_node	*FromId = 0;
	register t_def	*df;
} :
	/*
	   When parsing a global module, this is the place where we must
	   read already compiled definition modules.
	   If the FROM clause is present, the identifier in it is a module
	   name, otherwise the names in the import list are module names.
	*/
	[ FROM
	  IDENT		{ FromId = dot2leaf(Name);
			  if (local) {
				df = lookfor(FromId,enclosing(CurrVis),0,D_USED);
			  }
			  else df = GetDefinitionModule(dot.TOK_IDF, 1);
			}
	]?
	IMPORT IdentList(&ImportList) ';'
			{ if (FromId) {
				EnterFromImportList(ImportList, df, FromId);
			  }
			  else {
				EnterImportList(ImportList,
						local,
						enclosing(CurrVis)->sc_scope);
			  }
			  FreeNode(ImportList);
			}
;

DefinitionModule
{
	register t_def	*df;
	t_node		*exportlist;
	int		dummy;
	extern t_idf	*DefId;
	extern int	ForeignFlag;
	extern char	*sprint();
	register t_scope *currscope = CurrentScope;
	char buf[512];
} :
	DEFINITION
	MODULE IDENT	{ df = define(dot.TOK_IDF, GlobalScope, D_MODULE);
			  df->df_flags |= D_BUSY | ForeignFlag;
		  	  currscope->sc_definedby = df;
			  if (DefId &&
			      df->df_idf != DefId &&
			      !is_anon_idf(df->df_idf)) {
				error("DEFINITION MODULE name is \"%s\", not \"%s\"",
					df->df_idf->id_text, DefId->id_text);
			  }
			  sprint(buf, "_%s_", df->df_idf->id_text);
			  currscope->sc_name = Salloc(buf, (unsigned) strlen(buf) + 1);
			  df->mod_vis = CurrVis;
			  df->df_type = standard_type(T_RECORD, 1, (arith) 1);
			  df->df_type->rec_scope = currscope;
			  DefinitionModule++;
			  if (!Defined) {
				Defined = df;
#ifdef DBSYMTAB
				if (options['g']) stb_string(df, D_MODULE);
#endif /* DBSYMTAB */
			  }
			}
	';'
	import(0)* 
	export(&dummy, &exportlist)
		/*	New Modula-2 does not have export lists in definition
			modules. Issue a warning.
		*/
			{ 
			  if (exportlist) {
#ifndef STRICT_3RD_ED
			    if (! options['3'])
node_warning(exportlist, W_OLDFASHIONED, "export list in definition module ignored");
			    else
#endif
				error("export list not allowed in definition module");
			    FreeNode(exportlist);
			  }
			}
	definition* END IDENT
			{ end_definition_list(&(currscope->sc_def));
			  DefinitionModule--;
			  match_id(dot.TOK_IDF, df->df_idf);
			  df->df_flags &= ~D_BUSY;
			  free(FileName);
			}
	'.'
;

definition
{
	register t_def	*df;
	t_def		*dummy;
} :
	CONST [ %persistent ConstantDeclaration ';' ]*
|
	TYPE
	[ %persistent
	  IDENT 	{ df = define(dot.TOK_IDF, CurrentScope, D_TYPE); }
	  [ '=' type(&(df->df_type))
			{ SolveForwardTypeRefs(df); }
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
	  ';'
			{
#ifdef DBSYMTAB
			  if (options['g']) stb_string(df, D_TYPE);
#endif /* DBSYMTAB */
			}
	]*
|
	VAR [ %persistent VariableDeclaration ';' ]*
|
	ProcedureHeading(&dummy, D_PROCHEAD) ';'
;

ProgramModule
{
	register t_def	*df;
} :
	MODULE
	IDENT	{ 
		  if (state == IMPLEMENTATION) {
		  	int len = strlen(dot.TOK_IDF->id_text);

		  	if (len > 10) len = 10;
		  	if (strncmp(FileName, dot.TOK_IDF->id_text, len)) {
				warning(W_ORDINARY, "modulename %s does not match filename %s", dot.TOK_IDF->id_text, FileName);
		  	}
			df = GetDefinitionModule(dot.TOK_IDF, 0);
			CurrVis = df->mod_vis;
		  }
		  else {
			Defined = df = define(dot.TOK_IDF, GlobalScope, D_MODULE);
			open_scope(CLOSEDSCOPE);
			df->mod_vis = CurrVis;
			CurrentScope->sc_name = "__M2M_";
		  	CurrentScope->sc_definedby = df;
#ifdef DBSYMTAB
			if (options['g']) stb_string(df, D_MODULE);
#endif /* DBSYMTAB */
		  }
		}
	priority(&(df->mod_priority))
	';' import(0)*
	block(&(df->mod_body)) IDENT
		{
#ifdef DBSYMTAB
		  if (options['g']) {
			if (state == PROGRAM) {
				C_ms_stb_cst(df->df_idf->id_text,
					     N_MAIN,
					     0,
					     (arith) 0);
			}
			stb_string(df, D_END);
		  }
#endif /* DBSYMTAB */
		  close_scope(SC_CHKFORW|SC_CHKPROC|SC_REVERSE);
		  match_id(df->df_idf, dot.TOK_IDF);
		}
	'.'
;

CompilationUnit:
			{ error("Compiling a definition module");
			  open_scope(CLOSEDSCOPE);
			  state = DEFINITION;
			}
	DefinitionModule
			{ close_scope(SC_CHKFORW); }
|	%default
	[
		IMPLEMENTATION
			{ state = IMPLEMENTATION; }
	|
		/* empty */
			{ state = PROGRAM; }
	]
	ProgramModule
;
