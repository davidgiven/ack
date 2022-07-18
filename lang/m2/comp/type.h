/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */
#ifndef TYPE_H_
#define TYPE_H_

/* T Y P E   D E S C R I P T O R   S T R U C T U R E */

#include "em_arith.h"



struct def;
struct node;

struct paramlist {		/* structure for parameterlist of a PROCEDURE */
	struct paramlist *par_next;
	struct def *par_def;	/* "df" of parameter */
#define	IsVarParam(xpar)	((int) ((xpar)->par_def->df_flags & D_VARPAR))
#define TypeOfParam(xpar)	((xpar)->par_def->df_type)
};

struct paramlist;
#define	new_paramlist() ((struct paramlist*) calloc(1, sizeof(struct paramlist)))
#define	free_paramlist(p) free(p)

struct enume {
	struct def *en_enums;	/* Definitions of enumeration literals */
	arith en_ncst;		/* Number of constants */
	label en_rck;		/* Label of range check descriptor */
#define enm_enums	tp_value.tp_enum->en_enums
#define enm_ncst	tp_value.tp_enum->en_ncst
#define enm_rck		tp_value.tp_enum->en_rck
};

#define	new_enume() ((struct enume*) calloc(1, sizeof(struct enume)))
#define	free_enume(p) free(p)

struct subrange {
	arith su_lb, su_ub;	/* lower bound and upper bound */
	label su_rck;		/* label of range check descriptor */
#define sub_lb	tp_value.tp_subrange->su_lb
#define sub_ub	tp_value.tp_subrange->su_ub
#define sub_rck	tp_value.tp_subrange->su_rck
};

#define	new_subrange() ((struct subrange*) calloc(1, sizeof(struct subrange)))
#define	free_subrange(p) free(p)

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

#define	new_array() ((struct array*) calloc(1, sizeof(struct array)))
#define	free_array(p) free(p)

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

struct type;

#define	new_type() ((struct type*) calloc(1, sizeof(struct type)))
#define	free_type(p) free(p)

extern struct type 
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
	*longintorcard_type,
	*bitset_type,
	*void_type,
	*std_type,
	*error_type;		/* All from type.c */

#ifdef NOCROSS
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
#else /* NOCROSS */

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
#endif /* NOCROSS */

extern arith
	ret_area_size;



#define NULLTYPE ((struct type  *) 0)

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
#else /* DEBUG */
#define ResultType(tpx)		((tpx)->tp_next)
#define ParamList(tpx)		((tpx)->prc_params)
#define IndexType(tpx)		((tpx)->tp_next)
#define ElementType(tpx)	((tpx)->tp_next)
#define PointedtoType(tpx)	((tpx)->tp_next)
#define SubBaseType(tpx)	((tpx)->tp_next)
#endif /* DEBUG */
#define BaseType(tpx)		((tpx)->tp_fund == T_SUBRANGE ? (tpx)->tp_next : \
					(tpx))
#define	IsConstructed(tpx)	((tpx)->tp_fund & T_CONSTRUCTED)
#define	TooBigForReturnArea(tpx) ((tpx)->tp_size > ret_area_size)

extern arith full_mask[];
extern arith max_int[];
extern arith min_int[];

#define ufit(n, i)	(((n) & ~full_mask[(i)]) == 0)

/* Forward declarations. */

/* Initialize predefined types. */
void InitTypes(void);
/* Compare arith's, and return 1 if "l2" is
 * greater or equal than "l1", otherwise returns 0.
 * Takes into account that arith might be unsigned. */
int chk_bounds(arith l1, arith l2, int fund);
/* Aligns "pos" to the specified alignment "al" and returns the 
   aligned "pos".
 */
arith align(arith pos, int al);
/* Create a new standard type "fund" with specified
   alignment "algn" and "size" bytes. */ 
struct type  *standard_type(int fund, int algn, arith size);
struct type  *enum_type(struct node *EnumList);
struct type  *construct_type(int fund, register struct type  *tp);
struct type  *qualified_type(struct node **pnd);
/*	Check that the value "i" fits in the subrange or enumeration
	type "tp".  Return 1 if so, 0 otherwise
*/
int in_range(arith i, register struct type  *tp);
/*	Construct a subrange type from the constant expressions
	indicated by "lb" and "ub", but first perform some
	checks. "base" is either a user-specified base-type, or NULL.
*/
struct type  *subr_type(struct node *lb, struct node *ub, struct type  *base);
struct type  *proc_type(struct type  *result_type, struct paramlist *parameters, arith n_bytes_params);
void genrck(register struct type  *tp);
/*	Get the bounds of a bounded type. */
void getbounds(register struct type  *tp, arith *plo, arith *phi);
/*	Construct a set type with base type "tp", but first 
 *  perform some checks */
struct type  *set_type(register struct type  *tp);
void ArrayElSize(register struct type  *tp);
void ArraySizes(register struct type  *tp);
void FreeType(register struct type  *tp);
void DeclareType(struct node *nd, register struct def *df, register struct type  *tp);
void SolveForwardTypeRefs(register struct def *df);
void ForceForwardTypeDef(register struct def *df);
struct type  *RemoveEqual(register struct type  *tpx);
int type_or_forward(struct type  *tp);
struct type  *intorcard(register struct type  *left, register struct type  *right);
#ifdef DEBUG
void DumpType(register struct type  *tp);
#endif
int fit(arith sz, int nbytes);
/* Greatest common divisotr. */
int gcd(int m, int n);
/* Least common multiple. */
int lcm(int m, int n);



#endif /* TYPE_H_ */
