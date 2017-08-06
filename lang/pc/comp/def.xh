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

struct used	{
	struct def *us_def;	/* used definition */
#define usd_def		df_value.df_used.us_def
};

struct forwtype	{
	struct forwtype *f_next;
	struct node *f_node;
	struct type *f_type;
};

/* ALLOCDEF "forwtype" 50 */

struct dfproc	{			/* used for procedures and functions */
	struct scopelist *pc_vis;	/* scope of this procedure/function */
	char *pc_name;			/* internal name */
	label pc_label;			/* label of name (for tracing) */
	arith pc_res;			/* offset of function result */
	arith pc_bool;			/* offset of run-time boolean */
#define prc_vis		df_value.df_proc.pc_vis
#define prc_name	df_value.df_proc.pc_name
#define prc_label	df_value.df_proc.pc_label
#define prc_res		df_value.df_proc.pc_res
#define prc_bool	df_value.df_proc.pc_bool
};

struct def	{		/* list of definitions for a name */
	struct def *df_next;	/* next definition in definitions chain */
	struct def *df_nextinscope;
				/* link all definitions in a scope */
	struct idf *df_idf;	/* link back to the name */
	struct scope *df_scope;	/* scope in which this definition resides */
	long df_kind;		/* the kind of this definition: */
#define D_PROCEDURE	0x000001L	/* procedure */
#define D_FUNCTION	0x000002L	/* function */
#define D_TYPE		0x000004L	/* a type */
#define D_CONST		0x000008L	/* a constant */
#define D_ENUM		0x000010L	/* an enumeration literal */
#define D_FIELD		0x000020L	/* a field in a record */
#define D_PROGRAM	0x000040L	/* the program */
#define D_VARIABLE	0x000080L	/* a variable */
#define D_PARAMETER	0x000100L	/* program parameter */
#define D_FORWTYPE	0x000200L	/* forward type */
#define D_FTYPE		0x000400L	/* resolved forward type */
#define D_FWPROCEDURE	0x000800L	/* forward procedure */
#define D_FWFUNCTION	0x001000L	/* forward function */
#define D_LABEL		0x002000L	/* a label */
#define D_LBOUND	0x004000L	/* lower bound id. in conform. array */
#define D_UBOUND	0x008000L	/* upper bound id. in conform. array */
#define D_FORWARD	0x010000L	/* directive "forward" */
#define D_EXTERN	0x020000L	/* directive "extern" */
#define D_ERROR		0x040000L	/* a compiler generated definition
					 * for an undefined variable */
#define D_MODULE	0x080000L	/* the module */
#define D_INUSE		0x100000L	/* variable is in use */

/* special values for stab.c */
#define D_END		(D_PROGRAM|D_PROCEDURE)
#define D_PEND		(D_PROGRAM|D_PROCEDURE|D_VARIABLE)

#define D_VALUE		(D_FUNCTION | D_CONST | D_ENUM | D_FIELD | D_VARIABLE\
			 | D_FWFUNCTION | D_LBOUND | D_UBOUND)
#define D_ROUTINE      (D_FUNCTION | D_FWFUNCTION | D_PROCEDURE | D_FWPROCEDURE)
	unsigned short df_flags;
#define D_NOREG		0x001	/* set if it may not reside in a register */
#define D_VALPAR	0x002	/* set if it is a value parameter */
#define D_VARPAR	0x004	/* set if it is a var parameter */
#define D_LOOPVAR	0x008	/* set if it is a control-variable */
#define D_EXTERNAL	0x010	/* set if proc/func is external declared */
#define D_PROGPAR	0x020	/* set if input/output was mentioned in
				 * the program-heading */
#define D_USED		0x040	/* set when the variable is used */
#define D_SET		0x080	/* set when the variable is set */
#define D_INLOOP	0x100	/* set when we are inside a loop */
#define D_WITH		0x200	/* set inside a with statement */
#define D_SETINHIGH	0x400	/* set in a higher scope level (for loops) */

	struct type *df_type;
	union {
		struct constant df_constant;
		struct variable df_variable;
		struct bound df_bound;
		struct enumval df_enum;
		struct field df_field;
		struct lab df_label;
		struct used df_used;
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
