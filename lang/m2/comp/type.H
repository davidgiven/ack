/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* T Y P E   D E S C R I P T O R   S T R U C T U R E */

/* $Header$ */

#include "dbsymtab.h"

struct paramlist {		/* structure for parameterlist of a PROCEDURE */
	struct paramlist *par_next;
	struct def *par_def;	/* "df" of parameter */
#define	IsVarParam(xpar)	((int) ((xpar)->par_def->df_flags & D_VARPAR))
#define TypeOfParam(xpar)	((xpar)->par_def->df_type)
};

typedef struct paramlist t_param;

/* ALLOCDEF "paramlist" 20 */

struct enume {
	struct def *en_enums;	/* Definitions of enumeration literals */
	arith en_ncst;		/* Number of constants */
	label en_rck;		/* Label of range check descriptor */
#define enm_enums	tp_value.tp_enum->en_enums
#define enm_ncst	tp_value.tp_enum->en_ncst
#define enm_rck		tp_value.tp_enum->en_rck
};

/* ALLOCDEF "enume" 5 */

struct subrange {
	arith su_lb, su_ub;	/* lower bound and upper bound */
	label su_rck;		/* label of range check descriptor */
#define sub_lb	tp_value.tp_subrange->su_lb
#define sub_ub	tp_value.tp_subrange->su_ub
#define sub_rck	tp_value.tp_subrange->su_rck
};

/* ALLOCDEF "subrange" 5 */

struct array {
	struct type *ar_elem;	/* type of elements */
	label ar_descr;		/* label of array descriptor */
	arith ar_elsize;	/* size of elements */
	arith ar_low;		/* lower bound of index */
	arith ar_high;		/* upper bound of index */
#define arr_elem	tp_value.tp_arr->ar_elem
#define arr_descr	tp_value.tp_arr->ar_descr
#define arr_elsize	tp_value.tp_arr->ar_elsize
#define arr_low		tp_value.tp_arr->ar_low
#define arr_high	tp_value.tp_arr->ar_high
};

/* ALLOCDEF "array" 5 */

struct record {
	struct scope *rc_scope;	/* scope of this record */
				/* members are in the symbol table */
#define rec_scope	tp_value.tp_record.rc_scope
};

struct proc {
	struct paramlist *pr_params;
	arith pr_nbpar;		/* number of bytes parameters accessed */
#define prc_params	tp_value.tp_proc.pr_params
#define prc_nbpar	tp_value.tp_proc.pr_nbpar
};

struct set {
	arith st_low;		/* lowerbound of subrange type of set */
	unsigned st_sz;		/* size of constant set in compiler */
#define set_low		tp_value.tp_set.st_low
#define set_sz		tp_value.tp_set.st_sz
};

struct type	{
	struct type *tp_next;	/* used with ARRAY, PROCEDURE, POINTER, SET,
				   SUBRANGE, EQUAL
				*/
	short tp_fund;		/* fundamental type  or constructor */
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
#define T_NOSUB		(T_INTORCARD|T_ENUMERATION|T_CHAR)
#define T_NUMERIC	(T_INTORCARD|T_REAL)
#define T_INDEX		(T_ENUMERATION|T_CHAR|T_SUBRANGE)
#define T_DISCRETE	(T_INDEX|T_INTORCARD)
#define	T_CONSTRUCTED	(T_ARRAY|T_SET|T_RECORD)
#ifdef DBSYMTAB
	short tp_dbindex;	/* index in debugger symbol table */
#endif
	int tp_align;		/* alignment requirement of this type */
	arith tp_size;		/* size of this type */
	union {
	    struct enume *tp_enum;
	    struct subrange *tp_subrange;
	    struct array *tp_arr;
	    struct record tp_record;
	    struct proc tp_proc;
	    struct set tp_set;
	} tp_value;
};

typedef struct type t_type;

/* ALLOCDEF "type" 50 */

extern t_type
	*bool_type,
	*char_type,
	*int_type,
	*card_type,
	*longint_type,
	*longcard_type,
	*real_type,
	*longreal_type,
	*word_type,
	*byte_type,
	*address_type,
	*intorcard_type,
	*bitset_type,
	*void_type,
	*std_type,
	*error_type;		/* All from type.c */

#include "nocross.h"
#ifdef NOCROSS
#include "target_sizes.h"
#define	word_align	(AL_WORD)
#define	short_align	(AL_SHORT)
#define	int_align	(AL_INT)
#define	long_align	(AL_LONG)
#define	float_align	(AL_FLOAT)
#define	double_align	(AL_DOUBLE)
#define	pointer_align	(AL_POINTER)
#define	struct_align	(AL_STRUCT)

#define	word_size	(SZ_WORD)
#define	dword_size	(2 * SZ_WORD)
#define	int_size	(SZ_INT)
#define	short_size	(SZ_SHORT)
#define	long_size	(SZ_LONG)
#define	float_size	(SZ_FLOAT)
#define	double_size	(SZ_DOUBLE)
#define	pointer_size	(SZ_POINTER)

#define wrd_bits	(8*(int)word_size)
#else NOCROSS

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

extern unsigned int
	wrd_bits;		/* from cstoper.c */
#endif NOCROSS

extern arith
	ret_area_size;

extern arith
	align();		/* type.c */

extern t_type
	*construct_type(),
	*standard_type(),
	*set_type(),
	*subr_type(),
	*proc_type(),
	*enum_type(),
	*qualified_type(),
	*intorcard(),
	*RemoveEqual();	/* All from type.c */

#define NULLTYPE ((t_type *) 0)

#define IsConformantArray(tpx)	((tpx)->tp_fund==T_ARRAY && (tpx)->tp_size==0)
#define bounded(tpx)		((tpx)->tp_fund & T_INDEX)
#define complex(tpx)		((tpx)->tp_fund & (T_RECORD|T_ARRAY))
#define WA(sz)			(align(sz, (int) word_size))
#ifdef DEBUG
#define ResultType(tpx)		(assert((tpx)->tp_fund == T_PROCEDURE),\
					(tpx)->tp_next)
#define ParamList(tpx)		(assert((tpx)->tp_fund == T_PROCEDURE),\
					(tpx)->prc_params)
#define IndexType(tpx)		(assert((tpx)->tp_fund == T_ARRAY),\
					(tpx)->tp_next)
#define ElementType(tpx)	(assert((tpx)->tp_fund == T_SET),\
					(tpx)->tp_next)
#define PointedtoType(tpx)	(assert((tpx)->tp_fund == T_POINTER),\
					(tpx)->tp_next)
#define SubBaseType(tpx)	(assert((tpx)->tp_fund == T_SUBRANGE), \
					(tpx)->tp_next)
#else DEBUG
#define ResultType(tpx)		((tpx)->tp_next)
#define ParamList(tpx)		((tpx)->prc_params)
#define IndexType(tpx)		((tpx)->tp_next)
#define ElementType(tpx)	((tpx)->tp_next)
#define PointedtoType(tpx)	((tpx)->tp_next)
#define SubBaseType(tpx)	((tpx)->tp_next)
#endif DEBUG
#define BaseType(tpx)		((tpx)->tp_fund == T_SUBRANGE ? (tpx)->tp_next : \
					(tpx))
#define	IsConstructed(tpx)	((tpx)->tp_fund & T_CONSTRUCTED)
#define	TooBigForReturnArea(tpx) ((tpx)->tp_size > ret_area_size)

extern arith full_mask[];
extern arith max_int[];
extern arith min_int[];

#define ufit(n, i)	(((n) & ~full_mask[(i)]) == 0)
