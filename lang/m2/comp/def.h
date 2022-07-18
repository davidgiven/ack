/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */
#ifndef DEF_H_
#define DEF_H_

/* I D E N T I F I E R   D E S C R I P T O R   S T R U C T U R E */

/* $Id$ */

struct module {
	struct node *mo_priority;/* priority of a module */
	struct scopelist *mo_vis;/* scope of this module */
	struct node *mo_body;	/* body of this module */
#define mod_priority	df_value.df_module.mo_priority
#define mod_vis		df_value.df_module.mo_vis
#define mod_body	df_value.df_module.mo_body
};

struct variable {
	arith va_off;		/* address or offset of variable */
	char *va_name;		/* name of variable if given */
#define var_off		df_value.df_variable.va_off
#define var_name	df_value.df_variable.va_name
};

struct constant {
	struct token co_const;	/* result of a constant expression */
#define con_const	df_value.df_constant.co_const
};

struct enumval {
	arith en_val;	/* value of this enumeration literal */
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
#define prc_vis		df_value.df_forward.fo_vis
#define prc_body	df_value.df_forward.fo_node
#define prc_name	df_value.df_forward.fo_name
};

struct forwtype {
	struct node *f_node;
	struct def *f_def;
#define df_forw_node	df_value.df_fortype.f_node
#define df_forw_def	df_value.df_fortype.f_def
};

struct def	{		/* list of definitions for a name */
	struct def *df_next;	/* next definition in definitions chain */
	struct def *df_nextinscope;
				/* link all definitions in a scope */
	struct idf *df_idf;	/* link back to the name */
	struct scope *df_scope;	/* scope in which this definition resides */
	unsigned short df_kind;	/* the kind of this definition: */
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
#define D_FORWARD	0x0400	/* not yet defined */
#define D_FORWMODULE	0x0800	/* module must be declared later */
#define D_FORWTYPE	0x1000	/* forward type */
#define D_ERROR		0x4000	/* a compiler generated definition for an
				   undefined variable
				*/
#define D_INUSE		0x8000	/* identification in this scope (like D_IMPORT)
				*/
#define D_END		(D_MODULE|D_PROCEDURE)	/* special value for stab.c */
#define D_PEND		(D_MODULE|D_PROCEDURE|D_VARIABLE)	/* special value for stab.c */
#define D_VALUE	(D_PROCEDURE|D_VARIABLE|D_FIELD|D_ENUM|D_CONST|D_PROCHEAD)
#define D_ISTYPE	(D_HIDDEN|D_TYPE)
#define D_IMPORTED	(D_IMPORT|D_INUSE)
#define is_type(dfx)	((dfx)->df_kind & D_ISTYPE)
	unsigned short df_flags;
#define D_NOREG		0x01	/* set if it may not reside in a register */
#define D_USED		0x02	/* set if used */
#define D_DEFINED	0x04	/* set if it is assigned a value */
#define D_VARPAR	0x08	/* set if it is a VAR parameter */
#define D_VALPAR	0x10	/* set if it is a value parameter */
#define D_EXPORTED	0x20	/* set if exported */
#define D_QEXPORTED	0x40	/* set if qualified exported */
#define D_BUSY		0x80	/* set if busy reading this definition module */
#define D_FOREIGN	0x100	/* set for foreign language modules */
#define D_ADDRGIVEN	0x200	/* set if address given for variable */
#define D_FORLOOP	0x400	/* set if busy in for-loop */
#define D_IMP_BY_EXP	0x800	/* imported definition by export */
	struct type *df_type;
	union {
		struct module df_module;
		struct variable df_variable;
		struct constant df_constant;
		struct enumval df_enum;
		struct field df_field;
		struct import df_import;
		struct dforward df_forward;	/* also used for proc */
		struct forwtype df_fortype;
		int df_stdname;	/* define for standard name */
	} df_value;
};


#define	new_def() ((struct def*) calloc(1, sizeof(struct def)))
#define	free_def(p) free(p)


#define NULLDEF ((struct def *) 0)

struct scope;
struct idf;


struct def *MkDef(register struct idf *id, register struct scope *scope, int kind);
struct def *define(register struct idf *id, register struct scope *scope, int kind);
void RemoveFromIdList(register struct def *df);
struct def * DeclProc(int type, register struct idf *id);
void EndProc(register struct def *df, struct idf *id);
struct def * DefineLocalModule(struct idf *id);
void CheckWithDef(register struct def *df, struct type *tp);
void end_definition_list(register struct def **pdf);

#endif /* DEF_H_ */
