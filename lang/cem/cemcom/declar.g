/* $Header$ */
/*	DECLARATION SYNTAX PARSER	*/

{
#include	"nobitfield.h"
#include	"debug.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"idf.h"
#include	"type.h"
#include	"struct.h"
#include	"field.h"
#include	"decspecs.h"
#include	"def.h"
#include	"declar.h"
#include	"label.h"
#include	"expr.h"
#include	"sizes.h"
#include	"storage.h"

extern char options[];
}

/* 8 */
declaration
	{struct decspecs Ds;}
:
	{Ds = null_decspecs;}
	decl_specifiers(&Ds)
	init_declarator_list(&Ds)?
	';'
;

/*	A `decl_specifiers' describes a sequence of a storage_class_specifier,
	an unsigned_specifier, a size_specifier and a simple type_specifier,
	which may occur in arbitrary order and each of which may be absent;
	at least one of them must be present, however, since the totally
	empty case has already be dealt with in `external_definition'.
	This means that something like:
		unsigned extern int short xx;
	is perfectly good C.
	
	On top of that, multiple occurrences of storage_class_specifiers,
	unsigned_specifiers and size_specifiers are errors, but a second
	type_specifier should end the decl_specifiers and be treated as
	the name to be declared (see the thin ice in RM11.1).
	Such a language is not easily expressed in a grammar; enumeration
	of the permutations is unattractive. We solve the problem by
	having a regular grammar for the "soft" items, handling the single
	occurrence of the type_specifier in the grammar (we have no choice),
	collecting all data in a `struct decspecs' and turning that data
	structure into what we want.
	
	The existence of declarations like
		short typedef yepp;
	makes all hope of writing a specific grammar for typedefs illusory.
*/

decl_specifiers	/* non-empty */ (struct decspecs *ds;)
	/*	Reads a non-empty decl_specifiers and fills the struct
		decspecs *ds.
	*/
:
[
	other_specifier(ds)+
	[%prefer /* the thin ice in R.M. 11.1 */
		single_type_specifier(ds) other_specifier(ds)*
	|
		empty
	]
|
	single_type_specifier(ds) other_specifier(ds)*
]
	{do_decspecs(ds);}
;

/* 8.1 */
other_specifier(struct decspecs *ds;):
[
	[ AUTO | STATIC | EXTERN | TYPEDEF | REGISTER ]
	{	if (ds->ds_sc_given)
			error("repeated storage class specifier");
		else	{
			ds->ds_sc_given = 1;
			ds->ds_sc = DOT;
		}
	}
|
	[ SHORT | LONG ]
	{	if (ds->ds_size)
			error("repeated size specifier");
		else	ds->ds_size = DOT;
	}
|
	UNSIGNED
	{	if (ds->ds_unsigned)
			error("unsigned specified twice");
		else	ds->ds_unsigned = 1;
	}
]
;

/* 8.2 */
type_specifier(struct type **tpp;)
	/*	Used in struct/union declarations and in casts; only the
		type is relevant.
	*/
	{struct decspecs Ds; Ds = null_decspecs;}
:
	decl_specifiers(&Ds)
	{
		if (Ds.ds_sc_given)
			error("storage class ignored");
		if (Ds.ds_sc == REGISTER)
			error("register ignored");
	}
	{*tpp = Ds.ds_type;}
;

single_type_specifier(struct decspecs *ds;):
[
	TYPE_IDENTIFIER		/* this includes INT, CHAR, etc. */
	{idf2type(dot.tk_idf, &ds->ds_type);}
|
	struct_or_union_specifier(&ds->ds_type)
|
	enum_specifier(&ds->ds_type)
]
;

/* 8.3 */
init_declarator_list(struct decspecs *ds;):
	init_declarator(ds)
	[ ',' init_declarator(ds) ]*
;

init_declarator(struct decspecs *ds;)
	{
		struct declarator Dc;
		struct expr *expr = (struct expr *) 0;
	}
:
	{
		Dc = null_declarator;
	}
[
	declarator(&Dc)
	{
		reject_params(&Dc);
		declare_idf(ds, &Dc, level);
	}
	initializer(Dc.dc_idf, &expr)?
	{
		code_declaration(Dc.dc_idf, expr, level, ds->ds_sc);
	}
]
	{remove_declarator(&Dc);}
;

/*
	Functions yielding pointers to functions must be declared as, e.g.,
		int (*hehe(par1, par2))() char *par1, *par2;	{}
	Since the function heading is read as a normal declarator,
	we just include the (formal) parameter list in the declarator
	description list dc.
*/
declarator(struct declarator *dc;)
	{
		arith count;
		struct formal *fm = 0;
	}
:
[
	primary_declarator(dc)
	[%while(1)			/*	int i (M + 2) / 4;
						is a function, not an
						old-fashioned initialization.
					*/
		'('
		formal_list(&fm) ?	/* semantic check later...	*/
		')'
		{
			add_decl_unary(dc, FUNCTION, (arith)0, fm);
			fm = 0;
		}
	|
		arrayer(&count)
		{add_decl_unary(dc, ARRAY, count, NO_PARAMS);}
	]*
|
	'*' declarator(dc)
	{add_decl_unary(dc, POINTER, (arith)0, NO_PARAMS);}
]
;

primary_declarator(struct declarator *dc;) :
[
	identifier(&dc->dc_idf)
|
	'(' declarator(dc) ')'
]
;

arrayer(arith *sizep;)
	{ struct expr *expr; }
:
	'['
		[
			constant_expression(&expr)
			{
				array_subscript(expr);
				*sizep = expr->VL_VALUE;
				free_expression(expr);
			}
		|
			empty
			{ *sizep = (arith)-1; }
		]
	']'
;

formal_list (struct formal **fmp;)
:
	formal(fmp) [ ',' formal(fmp) ]*
;

formal(struct formal **fmp;)
	{struct idf *idf;	}
:
	identifier(&idf)
	{
		struct formal *new = new_formal();
		
		new->fm_idf = idf;
		new->next = *fmp;
		*fmp = new;
	}
;

/* Change 2 */
enum_specifier(struct type **tpp;)
	{
		struct idf *idf;
		arith l = (arith)0;
	}
:
	ENUM
	[
		{declare_struct(ENUM, (struct idf *) 0, tpp);}
		enumerator_pack(*tpp, &l)
	|
		identifier(&idf)
		[
			{declare_struct(ENUM, idf, tpp);}
			enumerator_pack(*tpp, &l)
		|
			{apply_struct(ENUM, idf, tpp);}
			empty
		]
	]
;

enumerator_pack(struct type *tp; arith *lp;) :
	'{'
	enumerator(tp, lp)
	[%while(AHEAD != '}')		/* >>> conflict on ',' */
		','
		enumerator(tp, lp)
	]*
	','?				/* optional trailing comma */
	'}'
	{tp->tp_size = int_size;}
	/*	fancy implementations that put small enums in 1 byte
		or so should start here.
	*/
;

enumerator(struct type *tp; arith *lp;)
	{
		struct idf *idf;
		struct expr *expr;
	}
:
	identifier(&idf)
	[
		'='
		constant_expression(&expr)
		{
			*lp = expr->VL_VALUE;
			free_expression(expr);
		}
	]?
	{declare_enum(tp, idf, (*lp)++);}
;

/* 8.5 */
struct_or_union_specifier(struct type **tpp;)
	{
		int fund;
		struct idf *idf;
	}
:
	[ STRUCT | UNION ]
	{fund = DOT;}
	[
		{
			declare_struct(fund, (struct idf *)0, tpp);
		}
		struct_declaration_pack(*tpp)
	|
		identifier(&idf)
		[
			{
				declare_struct(fund, idf, tpp);
				(idf->id_struct->tg_busy)++;
			}
			struct_declaration_pack(*tpp)
			{
				(idf->id_struct->tg_busy)--;
			}
		|
			{apply_struct(fund, idf, tpp);}
			empty
		]
	]
;

struct_declaration_pack(struct type *stp;)
	{
		struct sdef **sdefp = &stp->tp_sdef;
		arith size = (arith)0;
	}
:
	/*	The size is only filled in after the whole struct has
		been read, to prevent recursive definitions.
	*/
	'{'
	struct_declaration(stp, &sdefp, &size)+
	'}'
	{stp->tp_size = align(size, stp->tp_align);}
;

struct_declaration(struct type *stp; struct sdef ***sdefpp; arith *szp;)
	{struct type *tp;}
:
	type_specifier(&tp)
	struct_declarator_list(tp, stp, sdefpp, szp)
	[	/*	in some standard UNIX compilers the semicolon
			is optional, would you believe!
		*/
		';'
	|
		empty
		{warning("no semicolon after declarator");}
	]
;

struct_declarator_list(struct type *tp, *stp;
			struct sdef ***sdefpp; arith *szp;)
:
	struct_declarator(tp, stp, sdefpp, szp)
	[ ',' struct_declarator(tp, stp, sdefpp, szp) ]*
;

struct_declarator(struct type *tp; struct type *stp;
			struct sdef ***sdefpp; arith *szp;)
	{
		struct declarator Dc;
		struct field *fd = 0;
	}
:
	{
		Dc = null_declarator;
	}
[
	declarator(&Dc)
	{reject_params(&Dc);}
	bit_expression(&fd)?
|
	{Dc.dc_idf = gen_idf();}
	bit_expression(&fd)
]
	{add_sel(stp, declare_type(tp, &Dc), Dc.dc_idf, sdefpp, szp, fd);}
	{remove_declarator(&Dc);}
;

bit_expression(struct field **fd;)
	{ struct expr *expr; }
:
	{
		*fd = new_field();
	}
	':'
	constant_expression(&expr)
	{
		(*fd)->fd_width = expr->VL_VALUE;
		free_expression(expr);
#ifdef NOBITFIELD
		error("bitfields are not implemented");
#endif NOBITFIELD
	}
;

/* 8.6 */
initializer(struct idf *idf; struct expr **expp;) :
	[
		'='
	|
		empty
		{warning("old-fashioned initialization, insert =");}
		/*	This causes trouble at declarator and at
			external_definition, q.v.
		*/
	]
	initial_value(expp)
	{
		if (idf->id_def->df_type->tp_fund == FUNCTION)	{
			error("illegal initialization of function");
			free_expression(*expp);
			*expp = 0;
		}
		init_idf(idf);
#ifdef	DEBUG
		print_expr("initializer-expression", *expp);
#endif	DEBUG
	}
;

/* 8.7 */
cast(struct type **tpp;)	{struct declarator Dc;} :
	{Dc = null_declarator;}
	'('
	type_specifier(tpp)
	abstract_declarator(&Dc)
	')'
	{*tpp = declare_type(*tpp, &Dc);}
	{remove_declarator(&Dc);}
;

/*	This code is an abject copy of that of 'declarator', for lack of
	a two-level grammar.
*/
abstract_declarator(struct declarator *dc;)
	{arith count;}
:
[
	primary_abstract_declarator(dc)
	[
		'(' ')'
		{add_decl_unary(dc, FUNCTION, (arith)0, NO_PARAMS);}
	|
		arrayer(&count)
		{add_decl_unary(dc, ARRAY, count, NO_PARAMS);}
	]*
|
	'*' abstract_declarator(dc)
	{add_decl_unary(dc, POINTER, (arith)0, NO_PARAMS);}
]
;

primary_abstract_declarator(struct declarator *dc;) :
[%if (AHEAD == ')')
	empty
|
	'(' abstract_declarator(dc) ')'
]
;

empty:
;

/* 8.8 */
/* included in the IDENTIFIER/TYPE_IDENTIFIER mechanism */
