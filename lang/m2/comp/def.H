/* I D E N T I F I E R   D E S C R I P T O R   S T R U C T U R E */

/* $Header$ */

struct module {
	arith mo_priority;	/* priority of a module */
	struct scopelist *mo_vis;/* scope of this module */
	struct node *mo_body;	/* body of this module */
#define mod_priority	df_value.df_module.mo_priority
#define mod_vis		df_value.df_module.mo_vis
#define mod_body	df_value.df_module.mo_body
};

struct variable {
	arith va_off;		/* address or offset of variable */
	char *va_name;		/* name of variable if given */
	char va_addrgiven;	/* an address was given in the program */
#define var_off		df_value.df_variable.va_off
#define var_name	df_value.df_variable.va_name
#define var_addrgiven	df_value.df_variable.va_addrgiven
};

struct constant {
	struct node *co_const;	/* result of a constant expression */
#define con_const	df_value.df_constant.co_const
};

struct enumval {
	unsigned int en_val;	/* value of this enumeration literal */
	struct def *en_next;	/* next enumeration literal */
#define enm_val		df_value.df_enum.en_val
#define enm_next	df_value.df_enum.en_next
};

struct field {
	arith fd_off;
	struct variant {
		struct caselabellist *v_cases;
		label v_casedescr;
		struct def *v_varianttag;
	} *fd_variant;
#define fld_off		df_value.df_field.fd_off
#define fld_variant	df_value.df_field.fd_variant
};

struct dfproc {
	struct scopelist *pr_vis; /* scope of procedure */
	struct node *pr_body;	/* body of this procedure */
#define prc_vis		df_value.df_proc.pr_vis
#define prc_body	df_value.df_proc.pr_body
};

struct import {
	struct def *im_def;	/* imported definition */
#define imp_def		df_value.df_import.im_def
};

struct dforward {
	struct scopelist *fo_vis;
	struct node *fo_node;
	char *fo_name;
#define for_node	df_value.df_forward.fo_node
#define for_vis		df_value.df_forward.fo_vis
#define for_scopes	df_value.df_forward.fo_scopes
#define for_name	df_value.df_forward.fo_name
};

struct def	{		/* list of definitions for a name */
	struct def *next;	/* next definition in definitions chain */
	struct def *df_nextinscope;
				/* link all definitions in a scope */
	struct idf *df_idf;	/* link back to the name */
	struct scope *df_scope;	/* scope in which this definition resides */
	short df_kind;		/* the kind of this definition: */
#define D_MODULE	0x0001	/* a module */
#define D_PROCEDURE	0x0002	/* procedure of function */
#define D_VARIABLE	0x0004	/* a variable */
#define D_FIELD		0x0008	/* a field in a record */
#define D_TYPE		0x0010	/* a type */
#define D_ENUM		0x0020	/* an enumeration literal */
#define D_CONST		0x0040	/* a constant */
#define D_IMPORT	0x0080	/* an imported definition */
#define D_PROCHEAD	0x0100	/* a procedure heading in a definition module */
#define D_HIDDEN	0x0200	/* a hidden type */
#define D_HTYPE		0x0400	/* definition of a hidden type seen */
#define D_FORWARD	0x0800	/* not yet defined */
#define D_UNDEF_IMPORT	0x1000	/* imported from an undefined name */
#define D_FORWMODULE	0x2000	/* module must be declared later */
#define D_ERROR		0x4000	/* a compiler generated definition for an
				   undefined variable
				*/
	char df_flags;
#define D_ADDRESS	0x01	/* set if address was taken */
#define D_USED		0x02	/* set if used */
#define D_DEFINED	0x04	/* set if it is assigned a value */
#define D_VARPAR	0x08	/* set if it is a VAR parameter */
#define D_VALPAR	0x10	/* set if it is a value parameter */
#define D_EXPORTED	0x40	/* set if exported */
#define D_QEXPORTED	0x80	/* set if qualified exported */
	struct type *df_type;
	union {
		struct module df_module;
		struct variable df_variable;
		struct constant df_constant;
		struct enumval df_enum;
		struct field df_field;
		struct import df_import;
		struct dfproc df_proc;
		struct dforward df_forward;
		int df_stdname;	/* define for standard name */
	} df_value;
};

/* ALLOCDEF "def" */

extern struct def
	*define(),
	*lookup(),
	*ill_df;

#define NULLDEF ((struct def *) 0)
