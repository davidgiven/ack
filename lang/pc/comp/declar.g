/* D E C L A R A T I O N S */

{
#include    "parameters.h"
/* next line DEBUG */
#include	"debug.h"

#include	<alloc.h>
#include	<assert.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<pc_file.h>

#include	"LLlex.h"
#include	"chk_expr.h"
#include	"def.h"
#include	"idf.h"
#include	"main.h"
#include	"misc.h"
#include	"node.h"
#include	"scope.h"
#include	"type.h"

#define	PC_BUFSIZ	(sizeof(struct file) - (int)((struct file *)0)->bufadr)

int proclevel = 0;		/* nesting level of procedures */
int parlevel = 0;		/* nesting level of parametersections */
int expect_label = 0;		/* so the parser knows that we expect a label */
static int in_type_defs;	/* in type definition part or not */
}

/* ISO section 6.2.1, p. 93 */
Block(struct def *df;)
{
	arith i;
} :
					{ text_label = (label) 0; }
	LabelDeclarationPart
	Module(df, &i)
	CompoundStatement
					{ if( !err_occurred )
						CodeEndBlock(df, i);
					  if( df ) EndBlock(df);
					  FreeNode(BlockScope->sc_lablist);
					}
;

LabelDeclarationPart
{
	struct node *nd;
} :
	[
		LABEL Label(&nd)
				{ if( nd )	{
					DeclLabel(nd);
					nd->nd_next = CurrentScope->sc_lablist;
					CurrentScope->sc_lablist = nd;
				  }
				}
		[ %persistent
			',' Label(&nd)
				{ if( nd )	{
					DeclLabel(nd);
					nd->nd_next = CurrentScope->sc_lablist;
					CurrentScope->sc_lablist = nd;
				  }
				}
		]*
		';'
	]?
;

Module(struct def *df; arith *i;)
{
	label save_label;
} :
	ConstantDefinitionPart
					{ in_type_defs = 1; }
	TypeDefinitionPart
					{ in_type_defs = 0;
					  /* resolve forward references */
					  chk_forw_types();
					}
	VariableDeclarationPart
					{ if( !proclevel )	{
						chk_prog_params();
						BssVar();
					  }
					  proclevel++;
					  save_label = text_label;
					}
	ProcedureAndFunctionDeclarationPart
					{ text_label = save_label;

					  proclevel--;
					  chk_directives();

					  /* needed with labeldefinitions
					     and for-statement
					  */
					  BlockScope = CurrentScope;

					  if( !err_occurred )
						*i = CodeBeginBlock( df );
					}
;




ConstantDefinitionPart:
	[
		CONST
		[ %persistent
			ConstantDefinition ';'
		]+
	]?
;

TypeDefinitionPart:
	[
		TYPE
		[ %persistent
			TypeDefinition ';'
		]+
	]?
;

VariableDeclarationPart:
	[
		VAR 
		[ %persistent
			VariableDeclaration ';'
		]+
	]?
;

ProcedureAndFunctionDeclarationPart:
	[
		[
			ProcedureDeclaration
		|
			FunctionDeclaration
		] ';'
	]*
;

/* ISO section 6.1.6, p. 92 */
Label(struct node **pnd;)
{
	char lab[5];
	extern char *sprint();
} :	{ expect_label = 1; }
	INTEGER		/* not really an integer, in [0..9999] */
	{ if( dot.TOK_INT < 0 || dot.TOK_INT > 9999 )	{
		if( dot.TOK_INT != -1 )		/* This means insertion */
			error("label must lie in closed interval [0..9999]");
		*pnd = NULLNODE;
	  }
	  else	{
		sprint(lab, "%d", (int) dot.TOK_INT);
		*pnd = MkLeaf(Name, &dot);
		(*pnd)->nd_IDF = str2idf(lab, 1);
	  }
	  expect_label = 0;
	}
;


/* ISO section 6.3, p. 95 */
ConstantDefinition
{
	register struct idf *id;
	register struct def *df;
	struct node *nd;
} :
	IDENT			{ id = dot.TOK_IDF; }
	'=' Constant(&nd)
			{ if( df = define(id,CurrentScope,D_CONST) )	{
			  	df->con_const = nd;
				df->df_type = nd->nd_type;
				df->df_flags |= D_SET;
#ifdef DBSYMTAB
				if (options['g']) stb_string(df, D_CONST);
#endif /* DBSYMTAB */
			  }
			}
;

/* ISO section 6.4.1, p. 96 */
TypeDefinition
{
	register struct idf *id;
	register struct def *df;
	struct type *tp;
} :
	IDENT			{ id = dot.TOK_IDF; }
	'=' TypeDenoter(&tp)
			{ if( df = define(id, CurrentScope, D_TYPE) ) {
			  	df->df_type = tp;
				df->df_flags |= D_SET;
#ifdef DBSYMTAB
				if (options['g']) stb_string(df, D_TYPE);
#endif /* DBSYMTAB */
			  }
			}
;

TypeDenoter(register struct type **ptp;):
	/* This is a changed rule, because the grammar as specified in the
	 * reference is not LL(1), and this gives conflicts.
	 */
	TypeIdentifierOrSubrangeType(ptp)
|
	PointerType(ptp)
|
	StructuredType(ptp)
|
	EnumeratedType(ptp)
;

TypeIdentifierOrSubrangeType(register struct type **ptp;)
{
	struct node *nd1, *nd2;
} :
	/* This is a new rule because the grammar specified by the standard
	 * is not exactly LL(1) (see TypeDenoter).
	 */
[
	%prefer
	IDENT			{ nd1 = MkLeaf(Name, &dot); }
	[
		/* empty */
		/* at this point IDENT must be a TypeIdentifier !! */
				{ chk_type_id(ptp, nd1);
			  	  FreeNode(nd1);
				}
	|
		/* at this point IDENT must be a Constant !! */
				{ (void) ChkConstant(nd1); }
		UPTO Constant(&nd2)
				{ *ptp = subr_type(nd1, nd2);
				  FreeNode(nd1);
				  FreeNode(nd2);
				}
	]
|
	Constant(&nd1) UPTO Constant(&nd2)
				{ *ptp = subr_type(nd1, nd2);
				  FreeNode(nd1);
				  FreeNode(nd2);
				}
]
;

TypeIdentifier(register struct type **ptp;):
	IDENT			{ register struct node *nd = MkLeaf(Name, &dot);
				  chk_type_id(ptp, nd);
				  FreeNode(nd);
				}
;

/* ISO section 6.5.1, p. 105 */
VariableDeclaration
{
	struct node *VarList;
	struct type *tp;
} :
	IdentifierList(&VarList) ':' TypeDenoter(&tp)
				{ EnterVarList(VarList, tp, proclevel > 0); }
;

/* ISO section 6.6.1, p. 108 */
ProcedureDeclaration
{
	struct node *nd;
	struct type *tp;
	register struct scopelist *scl;
	register struct def *df;
} :
	/* This is a changed rule, because the grammar as specified in the
	 * reference is not LL(1), and this gives conflicts.
	 *
	 * ProcedureHeading without a FormalParameterList can be a
	 * ProcedureIdentification, i.e. the IDENT used in the Heading is
	 * also used in a "forward" declaration.
	 */
				{ open_scope(); }
	ProcedureHeading(&nd, &tp) ';'
				{ scl = CurrVis; close_scope(0); }
	[
		Directive
				{ DoDirective(dot.TOK_IDF, nd, tp, scl, 0); }
	|
				{ df = DeclProc(nd, tp, scl);
#ifdef DBSYMTAB
				  if (options['g']) stb_string(df, D_PROCEDURE);
#endif /* DBSYMTAB */
				}
		Block(df)
				{ /* open_scope() is simulated in DeclProc() */
#ifdef DBSYMTAB
				  if (options['g']) stb_string(df, D_PEND);
#endif /* DBSYMTAB */
				  close_scope(1);
				}
	]
;

ProcedureHeading(register struct node **pnd; register struct type **ptp;)
{
	struct node *fpl;
} :
	PROCEDURE
	IDENT			{
				  *pnd = MkLeaf(Name, &dot);
				}
	[
		FormalParameterList(&fpl)
				{ arith nb_pars = 0;
				  struct paramlist *pr = 0;

				  if( !parlevel )
					/* procedure declaration */
					nb_pars = EnterParamList(fpl, &pr);
				  else
					/* procedure parameter */
					nb_pars = EnterParTypes(fpl, &pr);
				
				  *ptp = proc_type(pr, nb_pars);
				  FreeNode(fpl);
				}
	|
		/* empty */
				{ *ptp =
				    proc_type((struct paramlist *)0,
						(proclevel > 1) ? pointer_size : (arith) 0);
				}
	]
;

Directive:
	/* see also Functiondeclaration (6.6.2, p. 110)
	 * Not actually an identifier but 'letter {letter | digit}'
	 */
	IDENT
;

/* ISO section 6.6.1, p. 108 */
FunctionDeclaration
{
	struct node *nd;
	struct type *tp;
	register struct scopelist *scl;
	register struct def *df;
} :
	/* This is a changed rule, because the grammar as specified in the
	 * reference is not LL(1), and this gives conflicts.
	 */
				{ open_scope(); }
	FunctionHeading(&nd, &tp) ';'
				{ scl = CurrVis; close_scope(0); }
	[
		Directive
				{ if( !tp )	{
					node_error(nd,
					 "function \"%s\": illegal declaration",
							nd->nd_IDF->id_text);
				  }
				  else DoDirective(dot.TOK_IDF, nd, tp, scl, 1);
				}
	|
				{ if( df = DeclFunc(nd, tp, scl) ) {
					df->prc_res =
					     - ResultType(df->df_type)->tp_size;
					df->prc_bool =
						CurrentScope->sc_off =
							df->prc_res - int_size;
#ifdef DBSYMTAB
					if (options['g']) stb_string(df, D_FUNCTION);
#endif /* DBSYMTAB */
				    }
				}
			Block(df)
				{ if( df ) {
#ifdef DBSYMTAB
					if (options['g']) stb_string(df, D_PEND);
#endif /* DBSYMTAB */
					EndFunc(df);
				  }

				  /* open_scope() is simulated in DeclFunc() */
				  close_scope(1);
				}
	]
;

FunctionHeading(register struct node **pnd; register struct type **ptp;)
{
	/*	This is the Function AND FunctionIdentification part.
		If it is a identification, *ptp is set to NULLTYPE.
	*/
	struct node *fpl = NULLNODE;
	struct type *tp;
	struct paramlist *pr = 0;
	arith nb_pars = (proclevel > 1) ? pointer_size : 0;
} :
	FUNCTION
	IDENT			{ *pnd = MkLeaf(Name, &dot);
				  *ptp = NULLTYPE;
				}
[
	[
		FormalParameterList(&fpl)
				{ if( !parlevel )
					/* function declaration */
					nb_pars = EnterParamList(fpl, &pr);
				  else
					/* function parameter */
					nb_pars = EnterParTypes(fpl, &pr);
				}
	|
		/* empty */
	]
	':' TypeIdentifier(&tp)
				{ if( IsConstructed(tp) )	{
				        node_error(*pnd,
				         "function has an illegal result type");
					tp = error_type;
				  }
				  *ptp = func_type(pr, nb_pars, tp);
				  FreeNode(fpl);
				}
]?
;

/* ISO section 6.4.2.1, p. 96 */
OrdinalType(register struct type **ptp;):
	/* This is a changed rule, because the grammar as specified in the
	 * reference states that a SubrangeType can start with an IDENT and
	 * so can an OrdinalTypeIdentifier, and this is not LL(1).
	 */
	TypeIdentifierOrSubrangeType(ptp)
|
	EnumeratedType(ptp)
;

/* ISO section 6.4.2.3, p. 97 */
EnumeratedType(register struct type **ptp;)
{
	struct node *EnumList;
	arith i = (arith) 1;
} :
	'(' IdentifierList(&EnumList) ')'
		{ register struct type *tp =
		  	standard_type(T_ENUMERATION, word_align, word_size);

		  *ptp = tp;
		  EnterEnumList(EnumList, tp);
		  if( tp->enm_ncst == 0 )
			*ptp = error_type;
		  else do	{
			if( ufit(tp->enm_ncst-1, i) )	{
				tp->tp_psize = i;
				tp->tp_palign = i;
				break;
		  	}
			i <<= 1;
		  } while( i < word_size );
		}
;

IdentifierList(register struct node **nd;)
{
	register struct node *tnd;
} :
	IDENT		{ *nd = tnd = MkLeaf(Name, &dot); }
	[ %persistent
		',' IDENT
			{ tnd->nd_next = MkLeaf(Name, &dot);
			  tnd = tnd->nd_next;
			}
	]*
;

/* ISO section 6.4.3.2, p. 98 */
StructuredType(register struct type **ptp;)
{
	unsigned short packed = 0;
} :
	[
		PACKED { packed = T_PACKED; }
	]?
	UnpackedStructuredType(ptp, packed)
;

UnpackedStructuredType(register struct type **ptp; unsigned short packed;):
	ArrayType(ptp, packed)
|
	RecordType(ptp, packed)
|
	SetType(ptp, packed)
|
	FileType(ptp)
;

/* ISO section 6.4.3.2, p. 98 */
ArrayType(register struct type **ptp; unsigned short packed;)
{
	struct type *tp;
	register struct type *tp2;
} :
	ARRAY
	'['
		Indextype(&tp)
			{ *ptp = tp2 = construct_type(T_ARRAY, tp);
			  tp2->tp_flags |= packed;
			}
		[ %persistent
			',' Indextype(&tp)
			{ tp2->arr_elem = construct_type(T_ARRAY, tp);
			  tp2 = tp2->arr_elem;
			  tp2->tp_flags |= packed;
			}
		]*
	']'
	OF ComponentType(&tp)
			{ tp2->arr_elem = tp;
			  ArraySizes(*ptp);
			  if( tp->tp_flags & T_HASFILE )
			  	(*ptp)->tp_flags |= T_HASFILE;
			}
;

Indextype(register struct type **ptp;):
	OrdinalType(ptp)
;

ComponentType(register struct type **ptp;):
	TypeDenoter(ptp)
;

/* ISO section 6.4.3.3, p. 99 */
RecordType(register struct type **ptp; unsigned short packed;)
{
	register struct scope *scope;
	register struct def *df;
	struct selector *sel = 0;
	arith size = 0;
	int xalign = struct_align;
} :
	RECORD
		{ open_scope();		/* scope for fields of record */
		  scope = CurrentScope;
		  close_scope(0);
		}
	FieldList(scope, &size, &xalign, packed, &sel)
		{ if( size == 0 )	{
			warning("empty record declaration");
			size = 1;
		  }
		  *ptp = standard_type(T_RECORD, xalign, size);
		  (*ptp)->rec_scope = scope;
		  (*ptp)->rec_sel = sel;
		  (*ptp)->tp_flags |= packed;

		  /* copy the file component flag */
		  df = scope->sc_def;
		  while( df && !(df->df_type->tp_flags & T_HASFILE) )
			df = df->df_nextinscope;

		  if( df )
			(*ptp)->tp_flags |= T_HASFILE;
		}
	END
;

FieldList(struct scope *scope; arith *cnt; int *palign; unsigned short packed;
							struct selector **sel;):
	/* This is a changed rule, because the grammar as specified in the
	 * reference is not LL(1), and this gives conflicts.
	 * Those irritating, annoying (Siklossy !!) semicolons.
	 */

	/* empty */
|
	FixedPart(scope, cnt, palign, packed, sel)
|
	VariantPart(scope, cnt, palign, packed, sel)
;

FixedPart(struct scope *scope; arith *cnt; int *palign; unsigned short packed;
							struct selector **sel;):
	/* This is a changed rule, because the grammar as specified in the
	 * reference is not LL(1), and this gives conflicts.
	 * Again those frustrating semicolons !!
	 */
	RecordSection(scope, cnt, palign, packed)
	FixedPartTail(scope, cnt, palign, packed, sel)
;

FixedPartTail(struct scope *scope; arith *cnt; int *palign;
				unsigned short packed; struct selector **sel;):
	/* This is a new rule because the grammar specified by the standard
	 * is not exactly LL(1).
	 * We see the light at the end of the tunnel !
	 */

	/* empty */
|
	%default
	';'
	[
		/* empty */
	|
		VariantPart(scope, cnt, palign, packed, sel)
	|
		RecordSection(scope, cnt, palign, packed)
		FixedPartTail(scope, cnt, palign, packed, sel)
	]
;

RecordSection(struct scope *scope; arith *cnt; int *palign;
							unsigned short packed;)
{
	struct node *FldList;
	struct type *tp;
} :

	IdentifierList(&FldList) ':' TypeDenoter(&tp)
			{ *palign =
			      lcm(*palign, packed ? tp->tp_palign : word_align);
			  EnterFieldList(FldList, tp, scope, cnt, packed);
			}
;

VariantPart(struct scope *scope; arith *cnt; int *palign;
				unsigned short packed; struct selector **sel;)
{
	struct type *tp;
	struct def *df = 0;
	struct idf *id = 0;
	arith tcnt, max;
	register arith ncst = 0;/* the number of values of the tagtype */
	register struct selector **sp;
	extern char *Malloc();
} :
	/* This is a changed rule, because the grammar as specified in the
	 * reference is not LL(1), and this gives conflicts.
	 * We're almost there !!
	 */

		{ *sel = (struct selector *) Malloc(sizeof(struct selector));
		  (*sel)->sel_ptrs = 0;
		}
	CASE
	VariantSelector(&tp, &id)
		{ if (id)
		  	df = define(id, scope, D_FIELD);
/* ISO 6.4.3.3 (p. 100)
 * The standard permits the integertype as tagtype, but demands that the set
 * of values denoted by the case-constants is equal to the set of values
 * specified by the tagtype.
 */
	  	  if( !(tp->tp_fund & T_INDEX)) {
			error("illegal type in variant");
			tp = error_type;
		  }
		  else	{
			arith lb, ub;

			getbounds(tp, &lb, &ub);
			ncst = ub - lb + 1;
			if (ncst < 0 || ncst > (~(1L<<(8*sizeof(arith)-1)))/sizeof(struct selector *)) {
				error("range of variant tag too wide");
				tp = error_type;
			}
			else {
				/* initialize selector */
				(*sel)->sel_ptrs = (struct selector **)
			   	  Malloc((unsigned)ncst * sizeof(struct selector *));
				(*sel)->sel_ncst = ncst;
				(*sel)->sel_lb = lb;
	
				/* initialize tagvalue-table */
				sp = (*sel)->sel_ptrs;
				while( ncst-- ) *sp++ = *sel;
			}
		  }
		  (*sel)->sel_type = tp;
		  if( df )	{
	  		df->df_type = tp;
	  		df->fld_flags |=
				  packed ? (F_PACKED | F_SELECTOR) : F_SELECTOR;
	  		df->fld_off = align(*cnt,
					 packed ? tp->tp_palign : tp->tp_align);
	  		*cnt = df->fld_off +
					 (packed ? tp->tp_psize : tp->tp_size);
		  }
		  tcnt = *cnt;
		}
	OF
	Variant(scope, &tcnt, palign, packed, *sel)
			{ max = tcnt; }
	VariantTail(scope, &tcnt, &max, cnt, palign, packed, *sel)
			{ *cnt = max;
			  if( sp = (*sel)->sel_ptrs )	{
				int errflag = 0;

				ncst = (*sel)->sel_ncst;
				while( ncst-- )
					if( *sp == *sel )	{
						*sp++ = 0;
						errflag = 1;
					}
					else *sp++;
				if( errflag )
		error("record variant part: each tagvalue must have a variant");
			  }
			}
;

VariantTail(register struct scope *scope; arith *tcnt; arith *max; arith *cnt;
		int *palign; unsigned short packed; struct selector *sel;):
	/* This is a new rule because the grammar specified by the standard
	 * is not exactly LL(1).
	 * At last, the garden of Eden !!
	 */

	/* empty */
|
%default
	';'
	[
		/* empty */
	|
					{ *tcnt = *cnt; }
		Variant(scope, tcnt, palign, packed, sel)
					{ if( *tcnt > *max ) *max = *tcnt; }
		VariantTail(scope, tcnt, max, cnt, palign, packed, sel)
	]
;

VariantSelector(register struct type **ptp; register struct idf **pid;)
{
	register struct node *nd;
} :
	/* This is a changed rule, because the grammar as specified in the
	 * reference is not LL(1), and this gives conflicts.
	 */

	IDENT				{ nd = MkLeaf(Name, &dot); }
	[
		/* Old fashioned ! at this point the IDENT represents
		 * the TagType
		 */
				{ warning("old-fashioned syntax ':' missing");
				  chk_type_id(ptp, nd);
				  FreeNode(nd);
				}
	|
		/* IDENT is now the TagField */
		':'
		TypeIdentifier(ptp)
					{ *pid = nd->nd_IDF;
					  FreeNode(nd);
					}
	]
;

Variant(struct scope *scope; arith *cnt; int *palign; unsigned short packed;
							struct selector *sel;)
{
	struct node *nd;
	struct selector *sel1 = 0;
} :
	CaseConstantList(&nd)
	':'
	'(' FieldList(scope, cnt, palign, packed, &sel1) ')'
					{ TstCaseConstants(nd, sel, sel1);
					  FreeNode(nd);
					}
;

CaseConstantList(struct node **nd;)
{
	struct node *nd1;
} :
	Constant(&nd1)			{ *nd = nd1; }
	[ %persistent
		',' Constant(&(nd1->nd_next))
					{ nd1 = nd1->nd_next; }
	]*
;

/* ISO section 6.4.3.4, p. 101 */
SetType(register struct type **ptp; unsigned short packed;):
	SET OF OrdinalType(ptp)
		{ *ptp = set_type(*ptp, packed); }
;

/* ISO section 6.4.3.5, p. 101 */
FileType(register struct type **ptp;):
	FILE OF
			{ *ptp = construct_type(T_FILE, NULLTYPE);
			  (*ptp)->tp_flags |= T_HASFILE;
			}
	ComponentType(&(*ptp)->next)
			{ if( (*ptp)->next->tp_flags & T_HASFILE ) {
			      error("file type has an illegal component type");
			      (*ptp)->next = error_type;
			  }
			  else {
				if( (*ptp)->next->tp_size > PC_BUFSIZ )
					(*ptp)->tp_size = (*ptp)->tp_psize =
					    (*ptp)->next->tp_size +
					    sizeof(struct file) - PC_BUFSIZ;
			  }
			}
;

/* ISO section 6.4.4, p. 103 */
PointerType(register struct type **ptp;)
{
	register struct node *nd;
	register struct def *df;
} :
	'^'
			{ *ptp = construct_type(T_POINTER, NULLTYPE); }
	IDENT
			{ nd = MkLeaf(Name, &dot);
			  df = lookup(nd->nd_IDF, CurrentScope, D_INUSE);
			  /* if( !df && CurrentScope == GlobalScope)
			      df = lookup(nd->nd_IDF, PervasiveScope, D_INUSE);
			  */
			  if( in_type_defs &&
			      (!df || (df->df_kind & (D_ERROR | D_FORWTYPE)))
			    )
				/* forward declarations only in typedefintion
				   part
				*/
				Forward(nd, *ptp);
			  else	{
				chk_type_id(&(*ptp)->next, nd);
			  	FreeNode(nd);
			  }
			}
;

/* ISO section 6.6.3.1, p. 112 */
FormalParameterList(struct node **pnd;)
{
	struct node *nd;
} :
	'('
					{ *pnd = nd = MkLeaf(Link, &dot); }
		FormalParameterSection(nd)
		[ %persistent
					{ nd->nd_right = MkLeaf(Link, &dot);
					  nd = nd->nd_right;
					}
		';' FormalParameterSection(nd)
		]*
	')'
;

FormalParameterSection(struct node *nd;):
/* This is a changed rule, because the grammar as specified
 * in the reference is not LL(1), and this gives conflicts.
 */
					{ /* kind of parameter */
					  nd->nd_INT = 0;
					}
[
	[
		/* ValueParameterSpecification */
		/* empty */
					{ nd->nd_INT = (D_VALPAR | D_SET); }
	|
		/* VariableParameterSpecification */
		VAR
					{ nd->nd_INT = (D_VARPAR | D_USED); }
	]
	IdentifierList(&(nd->nd_left)) ':'
	[
		/* ISO section 6.6.3.7.1, p. 115 */
		/* ConformantArrayParameterSpecification */
		ConformantArraySchema(&(nd->nd_type))
	|
		TypeIdentifier(&(nd->nd_type))
	]
			{ if( nd->nd_type->tp_flags & T_HASFILE  &&
			      (nd->nd_INT  & D_VALPAR) ) {
			    error("value parameter can't have a filecomponent");
			    nd->nd_type = error_type;
			  }
			}
|
	ProceduralParameterSpecification(&(nd->nd_left), &(nd->nd_type))
					{ nd->nd_INT = (D_VALPAR | D_SET); }
|
	FunctionalParameterSpecification(&(nd->nd_left), &(nd->nd_type))
					{ nd->nd_INT = (D_VALPAR | D_SET); }
]
;

ProceduralParameterSpecification(register struct node **pnd;
						register struct type **ptp;):
				{ parlevel++; }
	ProcedureHeading(pnd, ptp)
				{ parlevel--; }
;

FunctionalParameterSpecification(register struct node **pnd;
						register struct type **ptp;):
				{ parlevel++; }
	FunctionHeading(pnd, ptp)
				{ parlevel--;
				  if( !*ptp )	{
				      node_error(*pnd,
				      "illegal function parameter declaration");
				      *ptp = error_type;
				  }
				}
;

ConformantArraySchema(register struct type **ptp;):
	PackedConformantArraySchema(ptp)
|
	%default
	UnpackedConformantArraySchema(ptp)
;

PackedConformantArraySchema(register struct type **ptp;)
{
	struct type *tp;
} :
	PACKED ARRAY
				{ tp = construct_type(T_ARRAY, NULLTYPE);
				  tp->tp_flags |= T_PACKED;
				}
	'['
		Index_TypeSpecification(ptp, tp)
				{ tp->next = *ptp; }
	']'
	OF TypeIdentifier(ptp)
				{ if( (*ptp)->tp_flags & T_HASFILE )
					tp->tp_flags |= T_HASFILE;
				  tp->arr_elem = *ptp;
				  *ptp = tp;
				}
;

UnpackedConformantArraySchema(register struct type **ptp;)
{
	struct type *tp, *tp2;
} :
	ARRAY
				{ *ptp = tp = construct_type(T_ARRAY,NULLTYPE);}
	'['
		Index_TypeSpecification(&tp2, tp)
				{ tp->next = tp2; }
		[
				{ tp->arr_elem =
					construct_type(T_ARRAY, NULLTYPE);
				  tp = tp->arr_elem;
				}
		';' Index_TypeSpecification(&tp2, tp)
				{ tp->next = tp2; }
		]*
	']'
	OF
	[
		TypeIdentifier(&tp2)
	|
		ConformantArraySchema(&tp2)
	]
				{ if( tp2->tp_flags & T_HASFILE )
					(*ptp)->tp_flags |= T_HASFILE;
				  tp->arr_elem = tp2;
				}
;

Index_TypeSpecification(register struct type **ptp; register struct type *tp;)
{
	register struct def *df1, *df2;
} :
	IDENT
			{ if( df1 =
			    define(dot.TOK_IDF, CurrentScope, D_LBOUND)) {
				df1->bnd_type = tp;	/* type conf. array */
				df1->df_flags |= D_SET;
			  }
			}
	UPTO
	IDENT
			{ if( df2 =
			    define(dot.TOK_IDF, CurrentScope, D_UBOUND)) {
				df2->bnd_type = tp;	/* type conf. array */
				df2->df_flags |= D_SET;
			  }
			}
	':' TypeIdentifier(ptp)
			{ if( !bounded(*ptp) &&
			      (*ptp)->tp_fund != T_INTEGER )	{
				error("Indextypespecification: illegal type");
				*ptp = error_type;
			  }
			  df1->df_type = df2->df_type = *ptp;
			}
;
