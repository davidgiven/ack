/* I D E N T I F I E R   D E S C R I P T O R   S T R U C T U R E */

struct constant	{
	struct node *co_const;	/* result of a constant expression */
#define con_const	df_value.df_constant.co_const
};

struct variable	{
	arith va_off;		/* address of variable */
	char *va_name;		/* name of variable if given */
#define var_off		df_value.df_variable.va_off
#define var_name	df_value.df_variable.va_name
};

struct bound	{
	struct type *bo_type;	/* type of conformant array */
#define bnd_type	df_value.df_bound.bo_type
};

struct enumval	{
	unsigned int en_val;	/* value of this enumeration literal */
	struct def *en_next;	/* next enumeration literal */
#define enm_val		df_value.df_enum.en_val
#define enm_next	df_value.df_enum.en_next
};

struct field	{
	arith fd_off;
	unsigned short fd_flags;
#define F_SELECTOR	0x1	/* set if field is a variant selector */
#define F_PACKED	0x2	/* set if record is packed */

#define fld_off		df_value.df_field.fd_off
#define fld_flags	df_value.df_field.fd_flags
};

struct lab	{
	struct lab *lb_next;	/* list of goto statements to this label */
	int lb_level;		/* level of nesting */
	label lb_no;		/* instruction label */
	label lb_descr;		/* label of goto descriptor */
#define lab_next	df_value.df_label.lb_next
#define lab_level	df_value.df_label.lb_level
#define lab_no		df_value.df_label.lb_no
#define lab_descr	df_value.df_label.lb_descr
};

/* ALLOCDEF "lab" 10 */

struct forwtype	{
	struct forwtype *f_next;
	struct node *f_node;
	struct type *f_type;
};

/* ALLOCDEF "forwtype" 50 */

struct dfproc	{			/* used for procedures and functions */
	struct scopelist *pc_vis;	/* scope of this procedure/function */
	char *pc_name;			/* internal name */
	arith pc_res;			/* offset of function result */
#define prc_vis		df_value.df_proc.pc_vis
#define prc_name	df_value.df_proc.pc_name
#define prc_res		df_value.df_proc.pc_res
};

struct def	{		/* list of definitions for a name */
	struct def *df_next;	/* next definition in definitions chain */
	struct def *df_nextinscope;
				/* link all definitions in a scope */
	struct idf *df_idf;	/* link back to the name */
	struct scope *df_scope;	/* scope in which this definition resides */
	unsigned int df_kind;	/* the kind of this definition: */
#define D_PROCEDURE	0x00001	/* procedure */
#define D_FUNCTION	0x00002	/* function */
#define D_TYPE		0x00004	/* a type */
#define D_CONST		0x00008	/* a constant */
#define D_ENUM		0x00010	/* an enumeration literal */
#define D_FIELD		0x00020	/* a field in a record */
#define D_PROGRAM	0x00040	/* the program */
#define D_VARIABLE	0x00080	/* a variable */
#define D_PARAMETER	0x00100	/* program parameter */
#define D_FORWTYPE	0x00200	/* forward type */
#define D_FTYPE		0x00400	/* resolved forward type */
#define D_FWPROCEDURE	0x00800	/* forward procedure */
#define D_FWFUNCTION	0x01000	/* forward function */
#define D_LABEL		0x02000	/* a label */
#define D_LBOUND	0x04000	/* lower bound identifier in conformant array */
#define D_UBOUND	0x08000	/* upper bound identifier in conformant array */
#define D_FORWARD	0x10000	/* directive "forward" */
#define D_EXTERN	0x20000	/* directive "extern" */
#define D_ERROR		0x40000	/* a compiler generated definition for an
				 * undefined variable
				 */
#define D_VALUE		(D_FUNCTION | D_CONST | D_ENUM | D_FIELD | D_VARIABLE\
			 | D_FWFUNCTION | D_LBOUND | D_UBOUND)
#define D_ROUTINE      (D_FUNCTION | D_FWFUNCTION | D_PROCEDURE | D_FWPROCEDURE)
	unsigned short df_flags;
#define D_NOREG		0x01	/* set if it may not reside in a register */
#define D_VALPAR	0x02	/* set if it is a value parameter */
#define D_VARPAR	0x04	/* set if it is a var parameter */
#define D_LOOPVAR	0x08	/* set if it is a contol-variable */
#define D_EXTERNAL	0x10	/* set if proc/func is external declared */
#define D_PROGPAR	0x20	/* set if input/output was mentioned in
				 * the program-heading
				 */
	struct type *df_type;
	union {
		struct constant df_constant;
		struct variable df_variable;
		struct bound df_bound;
		struct enumval df_enum;
		struct field df_field;
		struct lab df_label;
		struct forwtype *df_fwtype;
		struct dfproc df_proc;
		int df_reqname;	/* define for required name */
	} df_value;
#define df_fortype	df_value.df_fwtype
};

/* ALLOCDEF "def" 50 */

extern struct def
	*define(),
	*MkDef(),
	*DeclProc(),
	*DeclFunc();

extern struct def
	*lookup(),
	*lookfor();

#define NULLDEF ((struct def *) 0)
