/* T Y P E   D E S C R I P T O R   S T R U C T U R E */

#include "dbsymtab.h"

struct paramlist {		/* structure for parameterlist of a PROCEDURE */
	struct paramlist *next;
	struct def *par_def;	/* "df" of parameter */
#define	IsVarParam(xpar)	((xpar)->par_def->df_flags & D_VARPAR)
#define TypeOfParam(xpar)	((xpar)->par_def->df_type)
};

/* ALLOCDEF "paramlist" 50 */

struct enume	{
	struct def *en_enums;
	unsigned int en_ncst;	/* number of constants */
	label en_rck;		/* label of range check descriptor */
#define enm_enums	tp_value.tp_enum.en_enums
#define enm_ncst	tp_value.tp_enum.en_ncst
#define enm_rck		tp_value.tp_enum.en_rck
};

struct subrange	{
	arith su_lb, su_ub;	/* lower bound and upper bound */
	label su_rck;		/* label of range check descriptor */
#define sub_lb		tp_value.tp_subrange.su_lb
#define sub_ub		tp_value.tp_subrange.su_ub
#define sub_rck		tp_value.tp_subrange.su_rck
};

struct array	{
	struct type *ar_elem;	/* type of elements */
	union	{
		struct	{	/* normal array */
			arith ar_elsize;	/* size of elements */
			label ar_descr;		/* label of array descriptor */
		} norm_arr;
		struct	{	/* conformant array */
			int cf_sclevel;		/* scope level of declaration */
			arith cf_descr;		/* offset array descriptor */
		} conf_arr;
	} ar_type;
#define arr_elem	tp_value.tp_arr.ar_elem
#define arr_elsize	tp_value.tp_arr.ar_type.norm_arr.ar_elsize
#define arr_ardescr	tp_value.tp_arr.ar_type.norm_arr.ar_descr
#define arr_cfdescr	tp_value.tp_arr.ar_type.conf_arr.cf_descr
#define arr_sclevel	tp_value.tp_arr.ar_type.conf_arr.cf_sclevel
};

struct selector	{
	struct type *sel_type;		/* type of the selector of a variant */
	arith sel_ncst;			/* number of values of selector type */
	arith sel_lb;			/* lower bound of selector type */
	struct selector **sel_ptrs;	/* tagvalue table with pointers to
					   nested variant-selectors */
};

struct record	{
	struct scope *rc_scope;		/* scope of this record */
					/* members are in the symbol table */
	struct selector *rc_selector;	/* selector of variant (if present) */
#define rec_scope	tp_value.tp_record.rc_scope
#define rec_sel		tp_value.tp_record.rc_selector
};

struct proc	{
	struct paramlist *pr_params;
	arith pr_nbpar;
#define prc_params	tp_value.tp_proc.pr_params
#define prc_nbpar	tp_value.tp_proc.pr_nbpar
};

struct type	{
	struct type *next;	/* used with ARRAY, PROCEDURE, FILE, SET,
				   POINTER, SUBRANGE */
	int tp_fund;		/* fundamental type  or constructor */
#define	T_ENUMERATION	0x0001
#define	T_INTEGER	0x0002
#define T_REAL		0x0004
#define T_CHAR		0x0008
#define T_PROCEDURE	0x0010
#define T_FUNCTION	0x0020
#define T_FILE		0x0040
#define T_STRINGCONST	0x0080
#define T_SUBRANGE	0x0100
#define T_SET		0x0200
#define T_ARRAY		0x0400
#define T_RECORD	0x0800
#define T_POINTER	0x1000
#define T_LONG		0x2000
#define T_STRING	0x4000
#define T_ERROR		0x8000	/* bad type */
#define T_NUMERIC	(T_INTEGER | T_REAL | T_LONG)
#define T_INDEX		(T_SUBRANGE | T_ENUMERATION | T_CHAR | T_INTEGER )
#define T_ORDINAL	(T_INDEX | T_LONG)
#define T_CONSTRUCTED	(T_ARRAY | T_SET | T_RECORD | T_FILE | T_STRINGCONST)
#define T_ROUTINE	(T_FUNCTION | T_PROCEDURE)
	unsigned short tp_flags;
#define T_HASFILE	0x1	/* set if type has a filecomponent */
#define T_PACKED	0x2	/* set if type is packed */
#define T_CHECKED	0x4	/* set if array has been checked */
#ifdef DBSYMTAB
	short tp_dbindex;	/* index in debugger symbol table */
#endif
	int tp_align;		/* alignment requirement of this type */
	int tp_palign;		/* in packed structures */
	arith tp_size;		/* size of this type */
	arith tp_psize;		/* in packed structures */
	union {
	    struct enume tp_enum;
	    struct subrange tp_subrange;
	    struct array tp_arr;
	    struct record tp_record;
	    struct proc tp_proc;
	} tp_value;
};

/* ALLOCDEF "type" 50 */

extern struct type
	*bool_type,
	*char_type,
	*int_type,
	*long_type,
	*real_type,
	*string_type,
	*std_type,
	*text_type,
	*nil_type,
	*emptyset_type,
	*void_type,
	*error_type;		/* All from type.c */

#include "nocross.h"
#ifdef NOCROSS
#include "target_sizes.h"
#define	word_align	(AL_WORD)
#define	int_align	(AL_INT)
#define	long_align	(AL_LONG)
#define	pointer_align	(AL_POINTER)
#define	real_align	(AL_REAL)
#define	struct_align	(AL_STRUCT)

#define	word_size	(SZ_WORD)
#define	int_size	(SZ_INT)
#define	long_size	(SZ_LONG)
#define	pointer_size	(SZ_POINTER)
#define	real_size	(SZ_REAL)
#else NOCROSS
extern int
	word_align,
	int_align,
	long_align,
	pointer_align,
	real_align,
	struct_align;		/* All from type.c */

extern arith
	word_size,
	int_size,
	long_size,
	pointer_size,
	real_size;		/* All from type.c */
#endif NOCROSS

extern arith
	align();

struct type
	*construct_type(),
	*standard_type(),
	*proc_type(),
	*func_type(),
	*set_type(),
	*subr_type();		/* All from type.c */

#define NULLTYPE ((struct type *) 0)

#define bounded(tpx)		((tpx)->tp_fund & T_INDEX)
#define WA(sz)			(align(sz, (int) word_size))
#define ResultType(tpx)		(assert((tpx)->tp_fund & T_ROUTINE),(tpx)->next)
#define ElementType(tpx)	(assert((tpx)->tp_fund & T_SET), (tpx)->next)
#define BaseType(tpx)		((tpx)->tp_fund & T_SUBRANGE ? (tpx)->next :\
									(tpx))
#define IndexType(tpx)		(assert((tpx)->tp_fund == T_ARRAY), (tpx)->next)
#define	IsConstructed(tpx)	((tpx)->tp_fund & T_CONSTRUCTED)
#define IsConformantArray(tpx)	((tpx)->tp_fund & T_ARRAY &&\
							(tpx)->tp_size == 0)
#define IsPacked(tpx)		((tpx)->tp_flags & T_PACKED)
#define PointedtoType(tpx)	(assert((tpx)->tp_fund == T_POINTER ||\
					(tpx)->tp_fund == T_FILE), (tpx)->next)
#define ParamList(tpx)		(assert((tpx)->tp_fund & T_ROUTINE),\
					(tpx)->prc_params)

extern long full_mask[];

#define ufit(n, i)	(((n) & ~full_mask[(i)]) == 0)
