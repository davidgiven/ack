/* $Header$ */

#define MAXARGS	3

typedef struct tree {
  int	t_oper;		/* operator */
  t_addr t_address;	/* some operators use an address */
  int	t_itemno;	/* item number in status list */
  union {
	long tt_ival;
	struct {
		struct idf *tt_idf;
		char *tt_str;
		struct scope *tt_scope;	
	} tt_x;
	struct tree *tt_args[MAXARGS];
	t_position tt_pos;
  } t_xxxx;
#define t_ival	t_xxxx.tt_ival
#define t_idf	t_xxxx.tt_x.tt_idf
#define t_str	t_xxxx.tt_x.tt_str
#define t_sc	t_xxxx.tt_x.tt_scope
#define t_args	t_xxxx.tt_args
#define t_lino t_xxxx.tt_pos.lineno
#define t_filename t_xxxx.tt_pos.filename
#define t_pos	t_xxxx.tt_pos
} t_tree, *p_tree;

/* ALLOCDEF "tree" 100 */

extern p_tree	mknode();
extern p_tree	run_command;
