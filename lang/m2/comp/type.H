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
};

struct subrange {
	arith su_lb, su_ub;	/* Lower bound and upper bound */
	label su_rck;		/* Label of range check descriptor */
};

struct array {
	struct type *ar_index;	/* Type of index */
	arith ar_lb, ar_ub;	/* Lower bound and upper bound */
	label ar_descr;		/* Label of array descriptor */
};

struct record {
	int rc_scopenr;		/* Scope number of this record */
				/* Members are in the symbol table */
};

struct proc {
	struct paramlist *pr_params;
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
	*char_type,
	*int_type,
	*card_type,
	*longint_type,
	*real_type,
	*longreal_type,
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
