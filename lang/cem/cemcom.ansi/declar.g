/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	DECLARATION SYNTAX PARSER	*/

{
#include	"parameters.h"
#include	<alloc.h>
#include	<flt_arith.h>
#include    "idf.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"label.h"
#include	"code.h"
#include	"type.h"
#include	"proto.h"
#include	"struct.h"
#include	"field.h"
#include	"decspecs.h"
#include	"def.h"
#include	"declar.h"
#include	"label.h"
#include	"expr.h"
#include	"sizes.h"
#include	"level.h"
#ifdef	LINT
#include	"l_lint.h"
#endif	/* LINT */

extern char	options[];
}

/* 3.5 */
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
	is perfectly legal C.
	
	On top of that, multiple occurrences of storage_class_specifiers,
	unsigned_specifiers and size_specifiers are errors, but a second
	type_specifier should end the decl_specifiers and be treated as
	the name to be declared.
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
/*      Accept a single declaration specifier.  Then accept zero or more
	declaration specifiers.  There can be a conflict on both
	TYPE_IDENTIFIER and IDENTIFIER.
	The following rule is used:
	When we see a TYPE_IDENTIFIER, we accept it if no type-specifier was
	given, and it is not directly followed by an identifier.  If a
	type-specifier was given, it is taken as the identifier being
	declared.  If it is followed by an identifier, we assume that an
	error has been  made, (e.g. unsigned typedeffed_int x;) and that
	this will be detected later on.
	When we see an IDENTIFIER, directly followed by another IDENTIFIER,
	we assume that a typing mistake has been made, and we accept it as
	an erroneous type-identifier.
*/

decl_specifiers	/* non-empty */ (register struct decspecs *ds;)
	/*	Reads a non-empty decl_specifiers and fills the struct
		decspecs *ds.
	*/
:
	single_decl_specifier(ds)
	[ %while(  (DOT==TYPE_IDENTIFIER
			&& ds->ds_size == 0
			&& ds->ds_unsigned == 0
			&& ds->ds_type == (struct type *)0)
		    || AHEAD == IDENTIFIER)	/* always an error */
		single_decl_specifier(ds)
	]*
	{do_decspecs(ds);}
;

single_decl_specifier /* non_empty */ (register struct decspecs *ds;)
:
	[ AUTO | STATIC | EXTERN | TYPEDEF | REGISTER ]
	{	if (ds->ds_sc_given)
			error("repeated storage class specifier");
		ds->ds_sc_given = 1;
		ds->ds_sc = DOT;
	}
|
	VOLATILE
	{	if (ds->ds_typequal & TQ_VOLATILE)
			error("repeated type qualifier");
		ds->ds_typequal |= TQ_VOLATILE;
	}
|
	CONST
	{	if (ds->ds_typequal & TQ_CONST)
			error("repeated type qualifier");
		ds->ds_typequal |= TQ_CONST;
	}
|
	[ SHORT | LONG ]
	{	if (ds->ds_size)
			error("repeated size specifier");
		ds->ds_size = DOT;
	}
|
	[ SIGNED | UNSIGNED ]
	{	if (ds->ds_unsigned != 0)
			error("repeated sign specifier");
		ds->ds_unsigned = DOT;
	}
|
	[ VOID | CHAR | INT | FLOAT | DOUBLE ]
	{
		idf2type(dot.tk_idf, &ds->ds_type);
		ds->ds_typedef = 0;
	}
|
	%default TYPE_IDENTIFIER
	{
		idf2type(dot.tk_idf, &ds->ds_type);
		ds->ds_typedef = 1;
	}
|
	%erroneous
	IDENTIFIER
	{
		error("%s is not a type identifier", dot.tk_idf->id_text);
		ds->ds_type = error_type;
		if (dot.tk_idf->id_def) {
			dot.tk_idf->id_def->df_type = error_type;
			dot.tk_idf->id_def->df_sc = TYPEDEF;
		}
	}
|
	%illegal
	IDENTIFIER
|
	struct_or_union_specifier(&ds->ds_type)
|
	enum_specifier(&ds->ds_type)
;

/* 3.5.2 */
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

/* 3.5 */
init_declarator_list(struct decspecs *ds;):
	init_declarator(ds)
	[ ',' init_declarator(ds) ]*
;

init_declarator(register struct decspecs *ds;)
	{
		struct declarator Dc;
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
#ifdef	LINT
		lint_declare_idf(Dc.dc_idf, ds->ds_sc);
#endif	/* LINT */
	}
	[
		initializer(Dc.dc_idf, ds->ds_sc)
	|
		{ code_declaration(Dc.dc_idf, (struct expr *) 0, level, ds->ds_sc); }
	]
]
	{
#ifdef	LINT
		add_auto(Dc.dc_idf);
#endif	/* LINT */
		remove_declarator(&Dc);
	}
;

/* 3.5.7: initializer */
initializer(struct idf *idf; int sc;)
	{
		struct expr *expr = (struct expr *) 0;
		int fund = idf->id_def->df_type->tp_fund;
		int autoagg = (level >= L_LOCAL
				&& sc != STATIC
				&& ( fund == STRUCT
				    || fund == UNION
				    || fund == ARRAY));
		int globalflag = level == L_GLOBAL
				|| (level >= L_LOCAL && sc == STATIC);
	}
:
	{	if (idf->id_def->df_type->tp_fund == FUNCTION)	{
			error("illegal initialization of function");
			idf->id_def->df_type->tp_fund = ERRONEOUS;
		}
		if (level == L_FORMAL2)
			error("illegal initialization of formal parameter");
	}
	'='
	{
		if (AHEAD != '{' && AHEAD != STRING ) autoagg = 0;
#ifdef	LINT
		lint_statement();
#endif	/* LINT */
		if (globalflag) {
			struct expr ex;
			code_declaration(idf, &ex, level, sc);
		}
		else if (autoagg)
			loc_init((struct expr *) 0, idf);
	}
	initial_value((globalflag || autoagg) ?
				&(idf->id_def->df_type)
				: (struct type **)0,
			&expr)
	{	if (! globalflag) {
			if (idf->id_def->df_type->tp_fund == FUNCTION)	{
				free_expression(expr);
				expr = 0;
			}
#ifdef	DEBUG
			print_expr("initializer-expression", expr);
#endif	/* DEBUG */
#ifdef	LINT
			change_state(idf, SET);
#endif	/* LINT */
#ifdef	DBSYMTAB
			if (options['g'] && level >= L_LOCAL && expr) {
				db_line(expr->ex_file, (unsigned) expr->ex_line);
			}
#endif	/* DBSYMTAB */
			if (autoagg)
				loc_init((struct expr *) 0, idf);
			else	code_declaration(idf, expr, level, sc);
		}
#ifdef	DBSYMTAB
		if (options['g'] && globalflag) {
			stb_string(idf->id_def, sc, idf->id_text);
		}
#endif	/* DBSYMTAB */
		idf_initialized(idf);
	}
;

/*
	Functions yielding pointers to functions must be declared as, e.g.,
		int (*hehe(par1, par2))() char *par1, *par2;	{}
	Since the function heading is read as a normal declarator,
	we just include the (formal) parameter list in the declarator
	description list dc.
*/
/* 3.5.4 */
declarator(register struct declarator *dc;)
	{	struct formal *fm = NO_PARAMS;
		struct proto *pl = NO_PROTO;
		arith count;
		int qual;
	}
:
	primary_declarator(dc)
	[/*%while(1)*/
		'('
		[ %if (DOT != IDENTIFIER)
			parameter_type_list(&pl)
		|
			formal_list(&fm)
		|
			/* empty */
		]
		')'
		{	add_decl_unary(dc, FUNCTION, 0, (arith)0, fm, pl);
			fm = NO_PARAMS;
		}
	|
		arrayer(&count)
		{add_decl_unary(dc, ARRAY, 0, count, NO_PARAMS, NO_PROTO);}
	]*
|
	pointer(&qual) declarator(dc)
	{add_decl_unary(dc, POINTER, qual, (arith)0, NO_PARAMS, NO_PROTO);}
;

primary_declarator(register struct declarator *dc;) :
	identifier(&dc->dc_idf)
|
	'(' declarator(dc) ')'
;

arrayer(arith *sizep;)
	{ struct expr *expr; }
:
	'['
		{ *sizep = (arith)-1; }
		[
			constant_expression(&expr)
			{
				check_array_subscript(expr);
				*sizep = expr->VL_VALUE;
				free_expression(expr);
			}
		]?
	']'
;

formal_list (struct formal **fmp;)
:
	formal(fmp) [ %persistent ',' formal(fmp) ]*
;

formal(struct formal **fmp;)
	{struct idf *idf;	}
:
	identifier(&idf)
	{
		register struct formal *new = new_formal();
		
		new->fm_idf = idf;
		new->next = *fmp;
		*fmp = new;
		if (idf->id_def && idf->id_def->df_sc == TYPEDEF) {
			error("typedef name %s may not be redeclared as a parameter", idf->id_text);
		}
	}
;

/* Change 2 */
enum_specifier(register struct type **tpp;)
	{
		struct idf *idf;
		arith l = (arith)0;
	}
:
	{if (*tpp) error("multiple types in declaration");}
	ENUM
	[
		{declare_struct(ENUM, (struct idf *) 0, tpp);}
		enumerator_pack(*tpp, &l)
	|
		identifier(&idf)
		[
			{declare_struct(ENUM, idf, tpp);}
			enumerator_pack(*tpp, &l)
			{
#ifdef DBSYMTAB
				if (options['g']) {
					stb_tag(idf->id_tag, idf->id_text);
				}
#endif /*DBSYMTAB */
			}
		|
			{apply_struct(ENUM, idf, tpp);}
			/* empty */
		]
	]
;

enumerator_pack(register struct type *tp; arith *lp;) :
	'{'
	enumerator(tp, lp)
	[%while (AHEAD != '}')
		','
		enumerator(tp, lp)
	]*
	[
		','	{warning("unexpected trailing comma in enumerator pack");}
	]?
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
struct_or_union_specifier(register struct type **tpp;)
	{
		int fund;
		struct idf *idfX;
		register struct idf *idf;
	}
:
	{if (*tpp) error("multiple types in declaration");}
	[ STRUCT | UNION ]
	{fund = DOT;}
	[
		{
			declare_struct(fund, (struct idf *)0, tpp);
		}
		struct_declaration_pack(*tpp)
	|
		identifier(&idfX)	{ idf = idfX; }
		[
			{
				declare_struct(fund, idf, tpp);
				(idf->id_tag->tg_busy)++;
			}
			struct_declaration_pack(*tpp)
			{
				(idf->id_tag->tg_busy)--;
#ifdef DBSYMTAB
				if (options['g']) {
					stb_tag(idf->id_tag, idf->id_text);
				}
#endif /*DBSYMTAB */
			}
		|
			{
			  /* a ';' means an empty declaration (probably)
			   * this means that we have to declare a new
			   * structure. (yegh)
			   */
			  if (DOT == ';' &&
			      ( !idf->id_tag ||
				 idf->id_tag->tg_level != level ||
				 idf->id_tag->tg_type->tp_size < 0
			     )) declare_struct(fund, idf, tpp);
			  else apply_struct(fund, idf, tpp);
			}
			/* empty */
		]
	]
;

struct_declaration_pack(register struct type *stp;)
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
	{stp->tp_size = align(size, stp->tp_align);
	 completed(stp);
	}
;

struct_declaration(struct type *stp; struct sdef ***sdefpp; arith *szp;)
	{struct type *tp;}
:
	type_specifier(&tp) struct_declarator_list(tp, stp, sdefpp, szp) ';'
;

struct_declarator_list(struct type *tp; struct type *stp;
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
#endif /* NOBITFIELD */
	}
;

/* 8.7 */
cast(struct type **tpp;)
	{struct declarator Dc;}
:
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
abstract_declarator(register struct declarator *dc;)
	{	struct proto *pl = NO_PROTO;
		arith count;
		int qual;
	}
:
	primary_abstract_declarator(dc)
	[
		'('
		[
			parameter_type_list(&pl)
		|
			/* empty */
		]
		')'
		{add_decl_unary(dc, FUNCTION, 0, (arith)0, NO_PARAMS, pl);
		 if (pl) remove_proto_idfs(pl);
		}
	|
		arrayer(&count)
		{add_decl_unary(dc, ARRAY, 0, count, NO_PARAMS, NO_PROTO);}
	]*
|
	pointer(&qual) abstract_declarator(dc)
	{add_decl_unary(dc, POINTER, qual, (arith)0, NO_PARAMS, NO_PROTO);}
;

%first first_of_parameter_type_list, parameter_type_list;

primary_abstract_declarator(struct declarator *dc;)
:
[%if (AHEAD == ')' || first_of_parameter_type_list(AHEAD))
	/* empty */
|
	'(' abstract_declarator(dc) ')'
]
;

parameter_type_list(struct proto **plp;)
	{	int save_level; }
:
	{	if (level > L_PROTO) {
			save_level = level;
			level = L_PROTO;
		} else level--;
	}
	parameter_decl_list(plp)
	[
		',' ELLIPSIS
		{	register struct proto *new = new_proto();

			new->next = *plp;
			new->pl_flag = PL_ELLIPSIS;
			*plp = new;
		}

	]?
	{	check_for_void(*plp);
		if (level == L_PROTO)
			level = save_level;
		else level++;
	}
;

parameter_decl_list(struct proto **plp;)
:
	parameter_decl(plp)
	[ %while (AHEAD != ELLIPSIS)
	  %persistent
		',' parameter_decl(plp)
	]*
;

parameter_decl(struct proto **plp;)
	{	register struct proto *new = new_proto();
		struct declarator Dc;
		struct decspecs Ds;
	}
:
	{	Dc = null_declarator;
		Ds = null_decspecs;
	}
	decl_specifiers(&Ds)
	parameter_declarator(&Dc)
	{	add_proto(new, &Ds, &Dc, level);
		new->next = *plp;
		*plp = new;
		remove_declarator(&Dc);
	}
;

/*	This is weird. Due to the LR structure of the ANSI C grammar
	we have to duplicate the actions of 'declarator' and
	'abstract_declarator'. Calling these separately, as in

	parameter_decl:
		decl_specifiers
		[
			declarator
		|
			abstract_declarator
		]


	gives us a conflict on the terminals '(' and '*'. E.i. on
	some input, it is impossible to decide which rule we take.
	Combining the two declarators into one common declarator
	is out of the question, since this results in an empty
	string for the non-terminal 'declarator'.
	So we combine the two only for the use of parameter_decl,
	since this is the only place where they don't give
	conflicts. However, this makes the grammar messy.
*/
parameter_declarator(register struct declarator *dc;)
	{	struct formal *fm = NO_PARAMS;
		struct proto *pl = NO_PROTO;
		arith count;
		int qual;
	}
:
	primary_parameter_declarator(dc)
	[
		'('
		[ %if (DOT != IDENTIFIER)
			parameter_type_list(&pl)
		|
			formal_list(&fm)
		|
			/* empty */
		]
		')'
		{   add_decl_unary(dc, FUNCTION, 0, (arith)0, fm, pl);
		    reject_params(dc);
		}
	|
		arrayer(&count)
		{add_decl_unary(dc, ARRAY, 0, count, NO_PARAMS, NO_PROTO);}
	]*
|
	pointer(&qual) parameter_declarator(dc)
	{add_decl_unary(dc, POINTER, qual, (arith)0, NO_PARAMS, NO_PROTO);}
;

primary_parameter_declarator(register struct declarator *dc;)
:
[%if (AHEAD == ')' || first_of_parameter_type_list(AHEAD)
				    && (AHEAD != IDENTIFIER))
	/* empty */
|
	identifier(&dc->dc_idf)
|
	'(' parameter_declarator(dc) ')'
]
;

pointer(int *qual;)
:
	'*' type_qualifier_list(qual)
;

/*	Type qualifiers may come in three flavours:
	volatile, const, const volatile.
	These all have different semantic properties:

	volatile:
		means that the object can be modified
		without prior knowledge of the implementation.

	const:
		means that the object can not be modified; thus
		it's illegal to use this as a l-value.

	const volatile:
		means  that the object can be modified without
		prior knowledge of the implementation, but may
		not be used as a l-value.
*/
/* 3.5.4 */
type_qualifier_list(int *qual;)
:
	{ *qual = 0; }
	[
		VOLATILE
		{	if (*qual & TQ_VOLATILE)
				error("repeated type qualifier");
			*qual |= TQ_VOLATILE;
		}
	|
		CONST
		{	if (*qual & TQ_CONST)
				error("repeated type qualifier");
			*qual |= TQ_CONST;
		}
	]*
;

empty:
;
