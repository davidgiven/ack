/* $Id$ */

/* internal type representation */

/* structure for struct/union elements */
struct fields {
  long fld_pos;			/* position of field */
  long fld_bitsize;		/* size in bits */
  struct type *fld_type;	/* type of field */
  char *fld_name;		/* name of field */
};

/* structure for enumeration literals */
struct literal {
  long lit_val;			/* value of literal */
  char *lit_name;		/* name of literal */
};

/* structure for parameters */
struct param {
  struct type *par_type;	/* type of parameter */
  long par_off;			/* offset of parameter */
  char par_kind;		/* kind of parameter ('p', 'i', or 'v') */
};

typedef struct type {
  short		ty_class;
#define T_SUBRANGE	 1
#define T_ARRAY		 2
#define T_STRUCT	 3
#define T_UNION		 4
#define T_ENUM		 5
#define T_POINTER	 6
#define T_FILE		 7
#define T_PROCEDURE	 8
#define T_SET		 9
#define T_REAL		10
#define T_INTEGER	11
#define T_VOID		12
#define T_UNSIGNED	13
#define T_STRING	14	/* only for string constants ... */
#define T_CROSS		15	/* cross reference to type */
#define T_INCOMPLETE   100
  long		ty_size;
  struct symbol	*ty_sym;
  union {
     /* cross references */
     struct type    *typ_cross;
#define ty_cross	ty_v.typ_cross
     /* procedures/functions: */
     struct {
	int	    typ_nparams;
	struct type *typ_retval;
	struct param *typ_params;
	long	    typ_nbparams;
     } ty_proc;
#define ty_nparams	ty_v.ty_proc.typ_nparams
#define ty_retval	ty_v.ty_proc.typ_retval
#define ty_params	ty_v.ty_proc.typ_params
#define ty_nbparams	ty_v.ty_proc.typ_nbparams
     /* pointers, files: */
     struct type *typ_ptrto;
#define ty_ptrto	ty_v.typ_ptrto
#define ty_fileof	ty_v.typ_ptrto
     /* arrays: */
     struct {
	long typ_lb, typ_hb;
	struct type *typ_index;
	struct type *typ_elements;
     } ty_array;
#define ty_lb		ty_v.ty_array.typ_lb
#define ty_hb		ty_v.ty_array.typ_hb
#define ty_index	ty_v.ty_array.typ_index
#define ty_elements	ty_v.ty_array.typ_elements
     /* subranges: */
     struct {
	long typ_low, typ_up;
	int typ_A;
	struct type *typ_base;
     } ty_subrange;
#define ty_A		ty_v.ty_subrange.typ_A
#define ty_low		ty_v.ty_subrange.typ_low
#define ty_up		ty_v.ty_subrange.typ_up
#define ty_base		ty_v.ty_subrange.typ_base
     /* structures/unions: */
     struct {
	unsigned typ_nfields;		/* number of field structures */
	struct fields *typ_fields;
     } ty_struct;
#define ty_nfields	ty_v.ty_struct.typ_nfields
#define ty_fields	ty_v.ty_struct.typ_fields
     /* enumerations: */
     struct {
	unsigned typ_nenums;		/* number of enumeration literals */
	struct literal *typ_literals;
     } ty_enum;
#define ty_nenums	ty_v.ty_enum.typ_nenums
#define ty_literals	ty_v.ty_enum.typ_literals
     /* bit sets: */
     struct {
	struct type *typ_setbase;	/* base type of set elements */
	long typ_setlow;		/* low bound */
     } ty_set;
#define ty_setbase	ty_v.ty_set.typ_setbase
#define ty_setlow	ty_v.ty_set.typ_setlow
  } ty_v;
} t_type, *p_type;

/* ALLOCDEF "type" 50 */

extern p_type
	subrange_type(),
	array_type(),
	*tp_lookup();
extern long
	param_size(),
	compute_size();

extern p_type	char_type, uchar_type, bool_type, int_type,
		long_type, double_type, string_type, address_type;
extern p_type	void_type;
extern long	int_size, short_size, pointer_size, long_size,
		float_size, double_size;

