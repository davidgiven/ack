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
	char df_kind;		/* The kind of this definition: */
#define D_MODULE	0x00
#define D_PROCEDURE	0x01
#define D_VARIABLE	0x02
#define D_FIELD		0x03
#define D_TYPE		0x04
#define D_ENUM		0x05
#define D_CONST		0x06
#define D_IMPORT	0x07
#define D_PROCHEAD	0x08	/* A procedure heading in a definition module */
#define D_HIDDEN	0x09	/* A hidden type */
#define D_HTYPE		0x0A	/* Definition of a hidden type seen */
#define D_STDPROC	0x0B	/* A standard procedure */
#define D_STDFUNC	0x0C	/* A standard function */
#define D_ISEXPORTED	0xFF	/* Not yet defined */
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

struct def
	*define(),
	*lookup();
