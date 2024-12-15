#ifndef TREE_H
#define TREE_H

#define MAXARGS 3

typedef struct tree
{
	short t_oper; /* tree operator */
	short t_whichoper; /* expression operator */
	t_addr t_address; /* some operators use an address */
	union
	{
		long tt_ival;
		char* tt_sval;
		double tt_fval;
		struct
		{
			struct idf* tt_idf;
			char* tt_str;
		} tt_x;
		struct tree* tt_args[MAXARGS];
		t_position tt_pos;
	} t_xxxx;
#define t_ival t_xxxx.tt_ival
#define t_sval t_xxxx.tt_sval
#define t_fval t_xxxx.tt_fval
#define t_idf t_xxxx.tt_x.tt_idf
#define t_str t_xxxx.tt_x.tt_str
#define t_args t_xxxx.tt_args
#define t_lino t_xxxx.tt_pos.lineno
#define t_filename t_xxxx.tt_pos.filename
#define t_pos t_xxxx.tt_pos
} t_tree, *p_tree;

/* ALLOCDEF "tree" 100 */

extern void freenode(p_tree p);
extern p_tree mknode(int, ...);
extern void eval(p_tree p);
extern int repeatable(p_tree com);
extern int in_status(p_tree com);
extern void newfile(struct idf* id);
extern void print_node(FILE* f, p_tree p, int top_level);
extern t_addr get_addr_from_node(p_tree p);
extern void perform(p_tree p, t_addr a);

#endif
