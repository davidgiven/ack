/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* T Y P E   D E S C R I P T O R   S T R U C T U R E */

/* $Header$ */

struct paramlist {		/* structure for parameterlist of a PROCEDURE */
	struct paramlist *next;
	struct def *par_def;	/* "df" of parameter */
#define	IsVarParam(xpar)	((int) ((xpar)->par_def->df_flags & D_VARPAR))
#define TypeOfParam(xpar)	((xpar)->par_def->df_type)
};

/* ALLOCDEF "paramlist" 20 */

struct enume {
	struct def *en_enums;	/* Definitions of enumeration literals */
	unsigned int en_ncst;	/* Number of constants */
	label en_rck;		/* Label of range check descriptor */
#define enm_enums	tp_value.tp_enum.en_enums
#define enm_ncst	tp_value.tp_enum.en_ncst
#define enm_rck		tp_value.tp_enum.en_rck
};

struct subrange {
	arith su_lb, su_ub;	/* lower bound and upper bound */
	label su_rck;		/* label of range check descriptor */
#define sub_lb	tp_value.tp_subrange.su_lb
#define sub_ub	tp_value.tp_subrange.su_ub
#define sub_rck	tp_value.tp_subrange.su_rck
};

struct array {
	struct type *ar_elem;	/* type of elements */
	label ar_descr;		/* label of array descriptor */
	arith ar_elsize;	/* size of elements */
#define arr_elem	tp_value.tp_arr.ar_elem
#define arr_descr	tp_value.tp_arr.ar_descr
#define arr_elsize	tp_value.tp_arr.ar_elsize
};

struct record {
	struct scope *rc_scope;	/* scope of this record */
				/* members are in the symbol table */
#define rec_scope	tp_value.tp_record.rc_scope
};

struct proc {
	struct paramlist *pr_params;
	arith pr_nbpar;
#define prc_params	tp_value.tp_proc.pr_params
#define prc_nbpar	tp_value.tp_proc.pr_nbpar
};

struct type	{
	struct type *next;	/* used with ARRAY, PROCEDURE, POINTER, SET,
				   SUBRANGE, EQUAL
				*/
	int tp_fund;		/* fundamental type  or constructor */
#define T_RECORD	0x0001
#define	T_ENUMERATION	0x0002
#define	T_INTEGER	0x0004
#define T_CARDINAL	0x0008
#define T_EQUAL		0x0010
#define T_REAL		0x0020
#define T_HIDDEN	0x0040
#define T_POINTER	0x0080
#define T_CHAR		0x0100
#define T_WORD		0x0200
#define T_SET		0x0400
#define T_SUBRANGE	0x0800
#define T_PROCEDURE	0x1000
#define T_ARRAY		0x2000
#define T_STRING	0x4000
#define T_INTORCARD	(T_INTEGER|T_CARDINAL)
#define T_NUMERIC	(T_INTORCARD|T_REAL)
#define T_INDEX		(T_ENUMERATION|T_CHAR|T_SUBRANGE)
#define T_DISCRETE	(T_INDEX|T_INTORCARD)
#define	T_CONSTRUCTED	(T_ARRAY|T_SET|T_RECORD)
	int tp_align;		/* alignment requirement of this type */
	arith tp_size;		/* size of this type */
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
	*card_type,
	*longint_type,
	*real_type,
	*longreal_type,
	*word_type,
	*byte_type,
	*address_type,
	*intorcard_type,
	*bitset_type,
	*std_type,
	*error_type;		/* All from type.c */

extern int
	word_align,
	short_align,
	int_align,
	long_align,
	float_align,
	double_align,
	pointer_align,
	struct_align;		/* All from type.c */

extern arith
	word_size,
	dword_size,
	short_size,
	int_size,
	long_size,
	float_size,
	double_size,
	pointer_size;		/* All from type.c */

extern arith
	align();		/* type.c */

struct type
	*construct_type(),
	*standard_type(),
	*set_type(),
	*subr_type(),
	*proc_type(),
	*enum_type(),
	*qualified_type(),
	*RemoveEqual();	/* All from type.c */

#define NULLTYPE ((struct type *) 0)

#define IsConformantArray(tpx)	((tpx)->tp_fund==T_ARRAY && (tpx)->next==0)
#define bounded(tpx)		((tpx)->tp_fund & T_INDEX)
#define complex(tpx)		((tpx)->tp_fund & (T_RECORD|T_ARRAY))
#define WA(sz)			(align(sz, (int) word_size))
#ifdef DEBUG
#define ResultType(tpx)		(assert((tpx)->tp_fund == T_PROCEDURE),\
					(tpx)->next)
#define ParamList(tpx)		(assert((tpx)->tp_fund == T_PROCEDURE),\
					(tpx)->prc_params)
#define IndexType(tpx)		(assert((tpx)->tp_fund == T_ARRAY),\
					(tpx)->next)
#define ElementType(tpx)	(assert((tpx)->tp_fund == T_SET),\
					(tpx)->next)
#define PointedtoType(tpx)	(assert((tpx)->tp_fund == T_POINTER),\
					(tpx)->next)
#else DEBUG
#define ResultType(tpx)		((tpx)->next)
#define ParamList(tpx)		((tpx)->prc_params)
#define IndexType(tpx)		((tpx)->next)
#define ElementType(tpx)	((tpx)->next)
#define PointedtoType(tpx)	((tpx)->next)
#endif DEBUG
#define BaseType(tpx)		((tpx)->tp_fund == T_SUBRANGE ? (tpx)->next : \
					(tpx))
#define	IsConstructed(tpx)	((tpx)->tp_fund & T_CONSTRUCTED)

extern long full_mask[];

#define fit(n, i)	(((n) + (0x80<<(((i)-1)*8)) & ~full_mask[(i)]) == 0)
#define ufit(n, i)	(((n) & ~full_mask[(i)]) == 0)
