/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define E_NODE	0
#define E_VAR	1	/* Variable *or* channel */
#define E_CONST	2
#define E_TABLE	3
#define E_BTAB	4
#define E_NOW	5
#define E_IO	6
#define E_CALL	7

struct table {
	struct table	*next;
	long	val;
};

struct expr;

struct expr_list {
	struct expr_list	*next;
	struct expr *arg;
};

struct expr {
	short kind;
	short type;
	int arr_siz;
	union {
		struct {
			int op;
			struct expr *left, *right;
		} node;

		struct symbol *var;

		long cst;

		int tab;

		struct {
			int out;
			struct expr *chan;
			struct expr_list *args;
		} io;

		struct {
			struct expr *c_proc;
			struct expr_list *c_args;
		} call;
	} u;
};

struct expr *new_node(), *new_var(), *new_const(), *new_table(), *new_now();
struct expr *new_io(), *new_call(), *copy_const();
void table_add(), expr_list_add();
void check_param(), check_io(), check_wait();
void destroy(), used();

#define valueless(e)		(((e)->type&T_TYPE)==T_VOID)
#define valued(e)		(((e)->type&T_TYPE)==T_VALUE)
#define input_process(e)	((e)->kind==E_IO && !(e)->u.io.out)
#define constant(e)		((e)->kind==E_CONST)
#define arr_constant(e)		((e)->kind==E_TABLE || (e)->kind==E_BTAB)
