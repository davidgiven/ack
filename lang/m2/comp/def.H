/* I D E N T I F I E R   D E S C R I P T O R   S T R U C T U R E */

/* $Header$ */

struct module {
	int mo_priority;	/* Priority of a module */
	int mo_scope;		/* Scope of this module */
#define mod_priority	df_value.df_module.mo_priority
#define mod_scope	df_value.df_module.mo_scope
};

struct variable {
	arith va_off;		/* Address or offset of variable */
#define var_off		df_value.df_variable.va_off
};

struct constant {
	arith co_const;		/* result of a constant expression */
#define con_const	df_value.df_variable.con_const
};

struct enumval {
	unsigned int en_val;	/* Value of this enumeration literal */
	struct def *en_next;	/* Next enumeration literal */
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

struct import {
	int im_scopenr;		/* Scope number from which imported */
#define imp_scopenr	df_value.df_import.im_scopenr
};

struct def	{		/* list of definitions for a name */
	struct def *next;	/* next definition in definitions chain */
	struct def *df_nextinscope;
				/* link all definitions in a scope */
	struct idf *df_idf;	/* link back to the name */
	int df_scope;		/* Scope in which this definition resides */
	short df_kind;		/* The kind of this definition: */
#define D_MODULE	0x0001	/* A module */
#define D_PROCEDURE	0x0002	/* Procedure of function */
#define D_VARIABLE	0x0004	/* A variable */
#define D_FIELD		0x0008	/* A field in a record */
#define D_TYPE		0x0010	/* A type */
#define D_ENUM		0x0020	/* An enumeration literal */
#define D_CONST		0x0040	/* A constant */
#define D_IMPORT	0x0080	/* An imported definition */
#define D_PROCHEAD	0x0100	/* A procedure heading in a definition module */
#define D_HIDDEN	0x0200	/* A hidden type */
#define D_HTYPE		0x0400	/* Definition of a hidden type seen */
#define D_STDPROC	0x0800	/* A standard procedure */
#define D_STDFUNC	0x1000	/* A standard function */
#define D_ERROR		0x2000	/* A compiler generated definition for an
				   undefined variable
				*/
#define D_ISEXPORTED	0x4000	/* Not yet defined */
	char df_flags;
#define D_ADDRESS	0x01	/* Set if address was taken */
#define D_USED		0x02	/* Set if used */
#define D_DEFINED	0x04	/* Set if it is assigned a value */
#define D_VARPAR	0x08	/* Set if it is a VAR parameter */
#define D_EXPORTED	0x40	/* Set if exported */
#define D_QEXPORTED	0x80	/* Set if qualified exported */
	struct type *df_type;
	union {
		struct module df_module;
		struct variable df_variable;
		struct constant df_constant;
		struct enumval df_enum;
		struct field df_field;
		struct import df_import;
		int df_stdname;	/* Define for standard name */
	} df_value;
};

/* ALLOCDEF "def" */

extern struct def
	*define(),
	*lookup(),
	*ill_df;

#define NULLDEF ((struct def *) 0)
