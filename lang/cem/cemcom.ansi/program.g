/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* PROGRAM PARSER */

/*	The presence of typedef declarations renders it impossible to
	make a context-free grammar of C. Consequently we need
	context-sensitive parsing techniques, the simplest one being
	a subtle cooperation between the parser and the lexical scanner.
	The lexical scanner has to know whether to return IDENTIFIER
	or TYPE_IDENTIFIER for a given tag, and it obtains this information
	from the definition list, as constructed by the parser.
	The present grammar is essentially LL(2), and is processed by
	a parser generator which accepts LL(1) with tie breaking rules
	in C, of the form %if(cond) and %while(cond). To solve the LL(1)
	ambiguities, the lexical scanner does a one symbol look-ahead.
	This symbol, however, cannot always be correctly assessed, since
	the present symbol may cause a change in the definition list
	which causes the identification of the look-ahead symbol to be
	invalidated.
	The lexical scanner relies on the parser (or its routines) to
	detect this situation and then update the look-ahead symbol.
	An alternative approach would be to reassess the look-ahead symbol
	in the lexical scanner when it is promoted to dot symbol. This
	would be more beautiful but less correct, since then for a short
	while there would be a discrepancy between the look-ahead symbol
	and the definition list; I think it would nevertheless work in
	correct programs.
	A third solution would be to enter the identifier as soon as it
	is found; its storage class is then known, although its full type
	isn't. We would have to fill that in afterwards.

	At block exit the situation is even worse. Upon reading the
	closing brace, the names declared inside the function are cleared
	from the name list. This action may expose a type identifier that
	is the same as the identifier in the look-ahead symbol. This
	situation certainly invalidates the third solution, and casts
	doubts upon the second.
*/

%lexical	LLlex;
%start		C_program, program;
%start		If_expr, control_if_expression;

{
#include	"parameters.h"
#include	<flt_arith.h>
#include	"arith.h"
#include	"LLlex.h"
#include	"label.h"
#include	"type.h"
#include	"declar.h"
#include	"decspecs.h"
#include	"code.h"
#include	"expr.h"
#include	"def.h"
#include    "idf.h"
#include    "declarator.h"
#include    "stack.h"
#include    "proto.h"
#include    "error.h"
#ifdef	LINT
#include	"l_lint.h"
#endif	/* LINT */

}

control_if_expression
	{
		struct expr *exprX;
	}
:
	constant_expression(&exprX)
		{
		}
;

/* 3.7 */
program:
	[%persistent external_definition]*
	{ unstack_world(); }
;

/*	A C identifier definition is remarkable in that it formulates
	the declaration in a way different from most other languages:
	e.g., rather than defining x as a pointer-to-integer, it defines
	*x as an integer and lets the compiler deduce that x is actually
	pointer-to-integer.  This has profound consequences, both for the
	structure of an identifier definition and for the compiler.
	
	A definition starts with a decl_specifiers, which contains things
	like
		typedef int
	which is implicitly repeated for every definition in the list, and
	then for each identifier a declarator is given, of the form
		*a()
	or so.  The decl_specifiers is kept in a struct decspecs, to be
	used again and again, while the declarator is stored in a struct
	declarator, only to be passed to declare_idf together with the
	struct decspecs.

	With the introduction of prototypes, extra problems for the scope
	administration were introduced as well.  We can have, for example,
	int x(double x);
	and
	int x(double x) { ... use(x) ... }
	In the first case, the parameter name can be forgotten, whereas in
	the second case, the parameter should have a block scope.  The
	problem lies in the fact that the parameter's type is known before
	the type of the function, which causes the def structure to be on
	the end of the list.  Our solution is as follows:
	1-  In case of a declaration, throw the parameter identifier away
	    before the declaration of the outer x.
	2-  In case of a definition, the function begin_proc() changes the
	    def list for the identifier.  This means that declare_idf()
	    contains an extra test in case we already saw a declaration of
	    such a function, because this function is called before
	    begin_proc().
*/

external_definition
	{	struct decspecs Ds;
		struct	declarator Dc;
	}
:
	{	Ds = null_decspecs;
		Dc = null_declarator;
	}
	[ %if (DOT != IDENTIFIER || AHEAD == IDENTIFIER)
		decl_specifiers(&Ds)
	|
		{do_decspecs(&Ds);}
	]
	[
		declarator(&Dc)
		{
			declare_idf(&Ds, &Dc, level);
#ifdef	LINT
			lint_ext_def(Dc.dc_idf, Ds.ds_sc);
#endif	/* LINT */
		}
		[
			function(&Ds, &Dc)
		|
			{	if (! Ds.ds_sc_given && ! Ds.ds_typequal &&
				    Ds.ds_notypegiven) {
					strict("declaration specifiers missing");
				}
			}
			non_function(&Ds, &Dc)
		]
	|
		{	if (! Ds.ds_sc_given && ! Ds.ds_typequal &&
			    Ds.ds_notypegiven) {
				strict("declaration missing");
			}
		}
		';'
	]
	{remove_declarator(&Dc); flush_strings(); }
;

non_function(register struct decspecs *ds; register struct declarator *dc;)
:
	{	reject_params(dc);
	}
	[
		initializer(dc->dc_idf, ds->ds_sc)
	|
		{ code_declaration(dc->dc_idf, (struct expr *) 0, level, ds->ds_sc); }
	]
	{
#ifdef	LINT
		lint_non_function_decl(ds, dc);
#endif	/* LINT */
	}
	[
		','
		init_declarator(ds)
	]*
	';'
;

/* 3.7.1 */
function(struct decspecs *ds; struct declarator *dc;)
	{
		arith fbytes;
		register struct idf *idf = dc->dc_idf;
	}
:
	{
#ifdef	LINT
		lint_start_function();
#endif	/* LINT */
		idf_initialized(idf);
		stack_level();		/* L_FORMAL1 declarations */
		declare_params(dc);
		begin_proc(ds, idf);	/* sets global function info */
		stack_level();		/* L_FORMAL2 declarations */
		declare_protos(dc);
	}
	declaration*
	{
		check_formals(idf, dc);		/* check style-mixtures */
		declare_formals(idf, &fbytes);
#ifdef	LINT
		lint_formals();
#endif	/* LINT */
	}
	compound_statement
	{
		end_proc(fbytes);
#ifdef	LINT
		lint_implicit_return();
#endif	/* LINT */
		unstack_level();	/* L_FORMAL2 declarations */
#ifdef	LINT
		lint_end_formals();
#endif	/* LINT */
		unstack_level();	/* L_FORMAL1 declarations */
#ifdef	LINT
		lint_end_function();
#endif	/* LINT */
	}
;
