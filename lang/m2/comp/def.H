/* I D E N T I F I E R   D E S C R I P T O R   S T R U C T U R E */

/* $Header$ */

struct module {
	int mo_priority;	/* Priority of a module */
	int mo_scope;		/* Scope of this module */
};

struct variable {
	char va_fixedaddress;	/* Flag, set if an address was given */
	arith va_off;		/* Address or offset of variable */
};

struct constant {
	struct expr *co_const;	/* A constant expression */
};

struct enumval {
	unsigned int en_val;	/* Value of this enumeration literal */
	struct def *en_next;	/* Next enumeration literal */
};

struct field {
	arith fld_off;
	struct variant {
		struct caselabellist *fld_cases;
		label fld_casedescr;
		struct def *fld_varianttag;
	} *fld_variant;
};

struct import {
	int im_scopenr;		/* Scope number from which imported */
};

struct def	{		/* list of definitions for a name */
	struct def *next;
	struct idf *df_idf;	/* link back to the name */
	int df_scope;		/* Scope in which this definition resides */
	short df_kind;		/* The kind of this definition: */
#define D_MODULE	0x0001
#define D_PROCEDURE	0x0002
#define D_VARIABLE	0x0004
#define D_FIELD		0x0008
#define D_TYPE		0x0010
#define D_ENUM		0x0020
#define D_CONST		0x0040
#define D_IMPORT	0x0080
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
