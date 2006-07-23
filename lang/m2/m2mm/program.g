/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* O V E R A L L   S T R U C T U R E */

/* stripped down version of the one in the Modula-2 compiler */

/* $Id$ */

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

{
#include	"main.h"
#include	"idf.h"
#include	"f_info.h"
#include	"LLlex.h"
#include	<alloc.h>

struct lnk *
new_lnk()
{
	static struct lnk *p;
	static int cnt;

	if (cnt-- <= 0) {
		p = (struct lnk *)Malloc(50*sizeof(struct lnk));
		cnt = 49;
	}
	p->lnk_next = 0;
	p->lnk_imp = 0;
	return p++;
}
}

%lexical LLlex;

%start	CompUnit, CompilationUnit;
%start	DefModule, DefinitionModule;

ModuleDeclaration :
	MODULE IDENT priority ';' import((struct lnk **) 0)* export?
	block IDENT
;

priority:
	[
		'[' ConstExpression ']'
	|
		/* empty */
	]
;

export :
	EXPORT
	[
		QUALIFIED
	|
		/* empty */
	]
	IdentList ';'
;

import(register struct lnk **p;)
{
	register struct idf *fromid = 0;
	struct idf *id;
}
:
				{ if (p) while (*p) p = &((*p)->lnk_next); }
	[ FROM
	  identifier(&id)	{ fromid = id;
				  if (p) {
					if (AddToList(fromid->id_text, ".def")) {
						*p = new_lnk();
						(*p)->lnk_imp = fromid;
					}
				  }
				}
	]?
	IMPORT 
	identifier(&id)		{ if (! fromid && p) {
					if (AddToList(id->id_text, ".def")) {
						*p = new_lnk();
						(*p)->lnk_imp = id;
						p = &((*p)->lnk_next);
					}
				  }
				}
	[
		',' identifier(&id)
				{ if (! fromid && p) {
					if (AddToList(id->id_text, ".def")) {
						*p = new_lnk();
						(*p)->lnk_imp = id;
						p = &((*p)->lnk_next);
					}
				  }
				}
	]*
	';'
	/*
	   When parsing a global module, this is the place where we must
	   read already compiled definition modules.
	   If the FROM clause is present, the identifier in it is a module
	   name, otherwise the names in the import list are module names.
	*/
;

DefinitionModule
{
	struct idf *id;
	extern char *strrchr();
}
:
	DEFINITION
	MODULE identifier(&id)
			{ if (! ForeignFlag) {
				AddToList(id->id_text, ".mod");
			  }
			  if (! id->id_type) {
				id->id_type = DEFINITION;
			  }
			  else if (id->id_type != IMPLEMENTATION) {
				error("multiple declaration for module %s",
					id->id_text);
			  }
			  if (! id->id_dir) {
				id->id_dir = WorkingDir;
			  }
			  else if (strcmp(id->id_dir, WorkingDir)) {
				Gerror("definition and implementation of module %s reside in different directories", id->id_text);
			  }
			  id->id_def = strrchr(FileName, '/');
			  if (! id->id_def) id->id_def = FileName;
			  else (id->id_def)++;
			  CurrentArg->a_idf = id;
			}
	';'
	import(&(id->id_defimports))* 
	[
		export
	|
		/* empty */
	]
	definition* END IDENT
	'.'
;

definition :
	CONST [ %persistent ConstantDeclaration ';' ]*
|
	TYPE
	[ %persistent
	  IDENT
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
	VAR [ %persistent VariableDeclaration ';' ]*
|
	ProcedureHeading
	';'
;

ProgramModule
{
	struct idf *id;
}
:
	MODULE
	identifier(&id)	{ if (! id->id_type) {
				id->id_type = state;
			  }
			  else if (id->id_type != DEFINITION ||
				   state != IMPLEMENTATION) {
				error("multiple declaration for module %s",
					id->id_text);
			  }
			  if (! id->id_dir) {
				id->id_dir = WorkingDir;
			  }
			  else if (strcmp(id->id_dir, WorkingDir)) {
				Gerror("definition and implementation of module %s reside in different directories", id->id_text);
			  }
			  CurrentArg->a_idf = id;
			}
	priority
	';' import(&(id->id_modimports))*
	block IDENT
	'.'
;

Module:
	DEFINITION
				{ fatal("Definition module in .mod file"); }
|	%default
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

identifier(struct idf **id;):
	IDENT
				{ extern char idfbuf[];
				  *id = str2idf(idfbuf, 1);
				}
;
