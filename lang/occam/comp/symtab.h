/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef nil
#define nil 0
#endif

	/* Symbol/Expression type: */
#define	T_VAR		0x0000
#define	T_CHAN		0x0001
#define	T_CONST		0x0002
#define	T_VALUE		0x0003
#define	T_PROC		0x0004
#define T_NOW		0x0005
#define T_VOID		0x0006

#define	T_TYPE		0x0007	/* Mask for type bits */

	/* Flags: */
#define	T_ARR		0x0008	/* Object is an array */
#define	T_BYTE		0x0010	/* Object is a byte array if T_ARR */
#define	T_PARAM		0x0020	/* Formal parameter */
#define	T_LVALUE	0x0040	/* This object may be assigned */
#define T_NOTDECL	0x0080	/* If you didn't declare it */
#define T_USED		0x0100	/* If you've used it */
#define T_ASSIGNED	0x0200	/* Or assigned it */
#define T_REP		0x0400	/* Replicator index */
#define T_BUILTIN	0x0800	/* Builtin name */
#define T_RECURS	0x1000	/* This proc is now compiled */
/* Note that some types and flags are only used for symbols, and others only
 * for expressions.
 */

struct symbol;

struct par_list {	/* List of parameter types for a proc object */
	struct par_list *pr_next;
	struct symbol *pr_var;	/* The formal parameter while visible */
	int pr_type;		/* Its type */
};

struct expr;

union storage {		/* An object is found */
	int level;	/* either at a certain local level */
	char *builtin;	/* or using a global builtin name */
};

union type_info {
	struct {
		union storage st;
		int offset;	/* from its local level or builtin name */
	} vc;			/* Variable or channel */

	struct expr *t_const;

	struct {
		union storage st;
		char *file;	/* file it is in */
		int label;	/* A unique id*/
		struct par_list *pars;
	} proc;
};

struct symbol {
	char	*s_name;
	short	s_type;
	int	s_arr_siz;
	union type_info	s_info;
	struct symbol	*s_left, *s_right;
};

struct symtab {
	struct symbol *local;
	struct symtab *global;
	int old_offset;
};

struct symbol *insert();
struct symbol *searchall();

void sym_down();
void sym_up();
void var_memory(), chan_memory();

void pars_add();
int form_offsets();
void check_recursion();

#define var_constant(v)	(((v)->s_type&T_TYPE)==T_CONST)
#define var_proc(v)	(((v)->s_type&T_TYPE)==T_PROC)
#define var_declared(v)	(! ((v)->s_type&T_NOTDECL))

extern union type_info none;
