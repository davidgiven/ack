/* I D E N T I F I E R   D E S C R I P T O R   S T R U C T U R E */

/* $Header$ */

struct module {
	int mo_priority;	/* priority of a module */
	int mo_scope;		/* scope of this module */
#define mod_priority	df_value.df_module.mo_priority
#define mod_scope	df_value.df_module.mo_scope
};

struct variable {
	arith va_off;		/* address or offset of variable */
#define var_off		df_value.df_variable.va_off
};

struct constant {
	arith co_const;		/* result of a constant expression */
#define con_const	df_value.df_variable.con_const
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
	int pr_scope;		/* scope number of procedure */
#define prc_scope	df_value.df_proc.pr_scope
};

struct import {
	struct def *im_def;	/* imported definition */
#define imp_def		df_value.df_import.im_def
};

struct def	{		/* list of definitions for a name */
	struct def *next;	/* next definition in definitions chain */
	struct def *df_nextinscope;
				/* link all definitions in a scope */
	struct idf *df_idf;	/* link back to the name */
	int df_scope;		/* scope in which this definition resides */
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
#define D_STDPROC	0x0800	/* a standard procedure */
#define D_STDFUNC	0x1000	/* a standard function */
#define D_ERROR		0x2000	/* a compiler generated definition for an
				   undefined variable
				*/
#define D_ISEXPORTED	0x4000	/* not yet defined */
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
		int df_stdname;	/* define for standard name */
	} df_value;
};

/* ALLOCDEF "def" */

extern struct def
	*define(),
	*lookup(),
	*ill_df;

#define NULLDEF ((struct def *) 0)
