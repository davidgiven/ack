/* $Header$ */
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
#include	"nopp.h"
#include	"alloc.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"idf.h"
#include	"label.h"
#include	"type.h"
#include	"declar.h"
#include	"decspecs.h"
#include	"code.h"
#include	"expr.h"
#include	"def.h"

#ifndef NOPP
extern arith ifval;
#endif NOPP

/*VARARGS*/
extern error();
}

control_if_expression
	{
		struct expr *expr;
	}
:
	constant_expression(&expr)
		{
#ifndef NOPP
			if (expr->ex_flags & EX_SIZEOF)
				expr_error(expr,
					"sizeof not allowed in preprocessor");
			ifval = expr->VL_VALUE;
			free_expression(expr);
#endif NOPP
		}
;

/* 10 */
program:
	[%persistent external_definition]*
	{unstack_world();}
;

/*	A C identifier definition is remarkable in that it formulates
	the declaration in a way different from most other languages:
	e.g., rather than defining x as a pointer-to-integer, it defines
	*x as an integer and lets the compiler deduce that x is actually
	pointer-to-integer.  This has profound consequences, but for the
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
*/

external_definition
	{
		struct decspecs Ds;
		struct declarator Dc;
	}
:
	{
		Ds = null_decspecs;
		Dc = null_declarator;
	}
[
	ext_decl_specifiers(&Ds)
	[
		declarator(&Dc)
		{declare_idf(&Ds, &Dc, level);}
		[%if (Dc.dc_idf->id_def->df_type->tp_fund == FUNCTION)
			/*	int i (1) {2, 3}
				is a function, not an old-fashioned
				initialization.
			*/
			function(&Dc)
		|
			non_function(&Ds, &Dc)
		]
	|
		';'
	]
	{remove_declarator(&Dc);}
|
	asm_statement			/* top level, would you believe */
]
;

ext_decl_specifiers(struct decspecs *ds;) :
[%prefer /* the thin ice in  R.M. 11.1 */
	decl_specifiers(ds)
|
	empty
	{do_decspecs(ds);}
]
;

non_function(struct decspecs *ds; struct declarator *dc;)
	{
		struct expr *expr = (struct expr *) 0;
	}
:
	{reject_params(dc);}
	initializer(dc->dc_idf, &expr)?
		{
			code_declaration(dc->dc_idf, expr, level, ds->ds_sc);
			free_expression(expr);
		}
	[
		','
		init_declarator(ds)
	]*
	';'
;

/* 10.1 */
function(struct declarator *dc;)
	{
		arith fbytes, nbytes;
	}
:
	{	struct idf *idf = dc->dc_idf;
		
		init_idf(idf);
		stack_level();		/* L_FORMAL1 declarations */
		declare_params(dc);
		begin_proc(idf->id_text, idf->id_def);
		stack_level();		/* L_FORMAL2 declarations */
	}
	declaration*
	{
		declare_formals(&fbytes);
	}
	compound_statement(&nbytes)
	{
		unstack_level();	/* L_FORMAL2 declarations */
		unstack_level();	/* L_FORMAL1 declarations */
		end_proc(fbytes, nbytes);
	}
;
