/* T Y P E   D E S C R I P T O R   S T R U C T U R E */

/* $Header$ */

struct paramlist {		/* structure for parameterlist of a PROCEDURE */
	struct paramlist *next;
	struct type *par_type;	/* Parameter type */
	int par_var;		/* flag, set if VAR parameter */
};

/* ALLOCDEF "paramlist" */

struct enume {
	struct def *en_enums;	/* Definitions of enumeration literals */
	unsigned int en_ncst;	/* Number of constants */
	label en_rck;		/* Label of range check descriptor */
#define enm_enums	tp_value.tp_enum.en_enums
#define enm_ncst	tp_value.tp_enum.en_ncst
#define enm_rck		tp_value.tp_enum.enm_rck
};

struct subrange {
	arith su_lb, su_ub;	/* Lower bound and upper bound */
	label su_rck;		/* Label of range check descriptor */
#define sub_lb	tp_value.tp_subrange.su_lb
#define sub_ub	tp_value.tp_subrange.su_ub
#define sub_rck	tp_value.tp_subrange.su_rck
};

struct array {
	struct type *ar_elem;	/* Type of elements */
	arith ar_lb, ar_ub;	/* Lower bound and upper bound */
	label ar_descr;		/* Label of array descriptor */
#define arr_elem	tp_value.tp_arr.ar_elem
#define arr_lb		tp_value.tp_arr.ar_lb
#define arr_ub		tp_value.tp_arr.ar_ub
#define arr_descr	tp_value.tp_arr.ar_descr
};

struct record {
	int rc_scopenr;		/* Scope number of this record */
				/* Members are in the symbol table */
#define rec_scopenr	tp_value.tp_record.rc_scopenr
};

struct proc {
	struct paramlist *pr_params;
#define prc_params	tp_value.tp_proc.pr_params
};

struct type	{
	struct type *next;	/* used with ARRAY, PROCEDURE, POINTER, SET,
				   SUBRANGE
				*/
	int tp_fund;		/* fundamental type  or constructor */
	int tp_align;		/* alignment requirement of this type */
	arith tp_size;		/* size of this type */
/*	struct idf *tp_idf;	/* name of this type */
	union {
	    struct enume tp_enum;
	    struct subrange tp_subrange;
	    struct array tp_arr;
	    struct record tp_record;
	    struct proc tp_proc;
	} tp_value;
};

/* ALLOCDEF "type" */

extern struct type
	*bool_type,
	*char_type,
	*int_type,
	*card_type,
	*longint_type,
	*real_type,
	*longreal_type,
	*nil_type,
	*error_type;

extern int
	wrd_align,
	int_align,
	lint_align,
	real_align,
	lreal_align,
	ptr_align,
	record_align;

extern arith
	wrd_size,
	int_size,
	lint_size,
	real_size,
	lreal_size,
	ptr_size;

extern arith
	align();

struct type
	*create_type(),
	*construct_type(),
	*standard_type();

#define NULLTYPE ((struct type *) 0)
