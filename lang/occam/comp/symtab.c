/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "symtab.h"
#include "expr.h"
#include "sizes.h"

int curr_level=0;	/* Current local level */
int curr_offset=0;	/* Current offset within this level */
int min_offset=0;	/* Minimum of all offsets within current level */

static struct symtab *sym_table=nil;

char *Malloc();

static struct symbol **search_sym(tree, name)
	struct symbol **tree;
	char *name;
/* Returns a hook in the tree to the where the given name is or should be. */
{
	register struct symbol **aps=tree, *ps;
	register cmp;

	while ((ps= *aps)!=nil && (cmp=strcmp(name, ps->s_name))!=0)
		aps= cmp<0 ? &ps->s_left : &ps->s_right;

	return aps;
}

struct symbol *insert(name, type, arr_siz, info)
	char *name;
	int type, arr_siz;
	union type_info *info;
/* Inserts an object with given name and other info into the current symbol
 * tree.  A pointer is returned to the inserted symbol so that more info may
 * or changed.  Nil is returned on redeclaration.
 */
{
	register struct symbol **aps, *ps;
	extern included;

	if (*(aps=search_sym(&sym_table->local, name))!=nil) {
		report("%s redeclared", name);
		return nil;
	}

	ps= (struct symbol *) Malloc(sizeof *ps);

	ps->s_name=name;

	if (included && curr_level==0)	/* Top_level symbol in include file */
		type|=T_USED;		/* are always used */
	ps->s_type=type;
	ps->s_arr_siz=arr_siz;
	ps->s_info= *info;
	ps->s_left=ps->s_right=nil;
	*aps=ps;

	return ps;
}

struct symbol *searchall(name) char *name;
/* Searches for name in all symbol trees from the inner to the outermost.
 * If it can't be found then it is inserted as undefined.
 */
{
	register struct symtab *tab=sym_table;
	register struct symbol *ps;

	while (tab!=nil) {
		if ((ps= *search_sym(&tab->local, name))!=nil) return ps;

		tab=tab->global;
	}
	report("%s not declared", name);
	return insert(name, T_NOTDECL, 0, &none);
}

void check_recursion(proc)
	register struct expr *proc;
{
	if (proc->kind==E_VAR && proc->u.var->s_type&T_RECURS)
		warning("recursion not allowed");
}

void sym_down()
{
	register struct symtab *ps;

	ps= (struct symtab *) Malloc(sizeof *ps);

	ps->local=nil;
	ps->global=sym_table;
	ps->old_offset=curr_offset;

	sym_table=ps;
}

static void sym_destroy(ps) register struct symbol *ps;
{
	if (ps!=nil) {
		sym_destroy(ps->s_left);
		sym_destroy(ps->s_right);
		if ( !(ps->s_type&T_NOTDECL) ) {
			if ( !(ps->s_type&T_USED) )
				warning("%s: never used", ps->s_name);
			else
			if ( !(ps->s_type&T_ASSIGNED) && (ps->s_type&T_TYPE)==T_VAR)
				warning("%s: never assigned", ps->s_name);
		}
		if ((ps->s_type&T_TYPE)==T_PROC) {
			register struct par_list *par, *junk;

			par=ps->s_info.proc.pars;
			while (par!=nil) {
				junk=par;
				par=par->pr_next;
				free((char *)junk);
			}
		} else
		if ((ps->s_type&T_TYPE)==T_CONST)
			destroy(ps->s_info.t_const);
		free((char *)(ps->s_name));
		free((char *)ps);
	}
}

void sym_up()
{
	register struct symtab *ps;

	ps=sym_table->global;
	curr_offset=sym_table->old_offset;

	sym_destroy(sym_table->local);
	free(sym_table);

	sym_table=ps;
}

void var_memory(info, type, n) register union type_info *info; int type, n;
/* Reserves local memory for an object, and stores it in its info field. */
{
	info->vc.st.level=curr_level;
	curr_offset-= (type&T_BYTE) ? (n+wz-1) & (~(wz-1)) : n*vz;
	info->vc.offset=curr_offset;
	if (curr_offset<min_offset) min_offset=curr_offset;
}

void chan_memory(info, n) register union type_info *info; int n;
{
	info->vc.st.level=curr_level;
	info->vc.offset= curr_offset-=n*(vz+wz);
	if (curr_offset<min_offset) min_offset=curr_offset;
}

int memory(z) int z;
/* Reserves z memory bytes */
{
	curr_offset-=z;
	if (curr_offset<min_offset) min_offset=curr_offset;
	return curr_offset;
}

void pars_add(aapars, type, var)
	register struct par_list ***aapars;
	int type;
	struct symbol *var;
/* Add a formal variable to a parameter list using a hook to a hook. */
{
	register struct par_list *pl;

	pl= (struct par_list *) Malloc(sizeof *pl);

	pl->pr_type=type;
	pl->pr_var=var;
	pl->pr_next= **aapars;

	**aapars=pl;
	*aapars= &pl->pr_next;
}

int form_offsets(pars) register struct par_list *pars;
/* Recursively assign offsets to formal variables. */
{
	register struct symbol *var;

	if (pars==nil) return pz;

	if ((var=pars->pr_var)!=nil) {
		register offset=form_offsets(pars->pr_next);

		switch (var->s_type&T_TYPE) {
		case T_VAR:
		case T_CHAN:
			var->s_info.vc.st.level=curr_level;
			var->s_info.vc.offset=offset;
			return offset+pz;
		case T_VALUE:
			var->s_info.vc.st.level=curr_level;
			var->s_info.vc.offset=offset;
			return offset+ ((var->s_type&T_ARR) ? pz : vz);
		}
	}
	return pz;
}
