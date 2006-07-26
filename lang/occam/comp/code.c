/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "occam-em.h"
#include "expr.h"
#include "symtab.h"
#include "sizes.h"
#include "Lpars.h"
#include "code.h"

extern err;

static void subscript();
enum addr_val { address, value };

void code_val(e) register struct expr *e;
/* Compile e for its value, which is put on the stack. */
{
	register struct expr *left, *right;

	if (err) return;

	switch(e->kind) {
	case E_NODE:
		left=e->u.node.left;
		right=e->u.node.right;

		switch (e->u.node.op) {
		case '+':
		case '-':
		case '*':
		case '/':
		case BS:
			code_val(left);
			code_val(right);
			xxi(e->u.node.op);
			break;
		case '<':
		case '>':
		case LE:
		case GE:
		case NE:
		case '=':
			code_val(left);
			code_val(right);
			cmi();
			Txx(e->u.node.op);
			break;
		case AFTER:
			code_val(left);
			code_val(right);
			xxi('-');
			cvw();
			tst();
			Txx('>');
			break;
		case BA:
			code_val(left);
			code_val(right);
			and();
			break;
		case BO:
			code_val(left);
			code_val(right);
			ior();
			break;
		case BX:
			code_val(left);
			code_val(right);
			xor();
			break;
		case AND:
		case OR: {
			int T=0, F=0, L=0;

			code_bool(e, positive, &T, &F);
			Label(T);
			Loc(-1L);
			branch(&L);
			Label(F);
			Loc(0L);
			Label(L);
			}break;
		case LS:
			code_val(left);
			code_val(right);
			cvw();
			sli();
			break;
		case RS:
			code_val(left);
			code_val(right);
			cvw();
			sri();
			break;
		case '~':
			code_val(left);
			ngi();
			break;
		case NOT:
			code_val(left);
			com();
			break;
		case '[':
			subscript(e, value);
			break;
		}
		break;
	case E_VAR: {
		register struct symbol *var=e->u.var;

		if (var->s_type&T_BUILTIN)
			Loe(var->s_info.vc.st.builtin, var->s_info.vc.offset);
		else
		if (var->s_info.vc.st.level==curr_level)
			if (var->s_type&T_PARAM && (var->s_type&T_TYPE)!=T_VALUE)
				Lil(var->s_info.vc.offset);
			else
				Lol(var->s_info.vc.offset);
		else {
			if (var->s_info.vc.offset<0)
				lxl(curr_level-var->s_info.vc.st.level);
			else
				lxa(curr_level-var->s_info.vc.st.level);
			if (var->s_type&T_PARAM && (var->s_type&T_TYPE)!=T_VALUE)
				Lif(var->s_info.vc.offset);
			else
				Lof(var->s_info.vc.offset);
		}
		}break;
	case E_CONST:
		Loc(e->u.cst);
		break;
	case E_NOW:
		cal("now");
		lfr(vz);
		break;
	}
}

static void subscript(e, av) register struct expr *e; enum addr_val av;
/* Produce code to compute the address or value of e->left[e->right] or
 * the address of e->left[e->right->left FOR e->right->right].
 */
{
	register char *des;
	register struct expr *left;
	register struct expr *index;

	code_addr(left=e->u.node.left);

	if ((index=e->u.node.right)->kind==E_NODE && index->u.node.op==FOR)
		index=index->u.node.left;

	if (left->arr_siz==0) {
		if ((left->type&T_TYPE)==T_CHAN)
			des="maxcdes";
		else
			des= e->type&T_BYTE ? "maxbdes" : "maxwdes";
	} else {
		register lsiz=left->arr_siz;

		if (left->type&T_BYTE && !(e->type&T_BYTE))
			lsiz/=vz;
		else
		if (!(left->type&T_BYTE) && e->type&T_BYTE)
			lsiz*=vz;

		if (e->type&T_ARR)
			lsiz-=(e->arr_siz -1);

		if (constant(index)) {
			if (index->u.cst<0 || index->u.cst>=lsiz) {
				warning("constant index outside vector");
				lin();
				loc(0);
				trp();
			}
		} else {
			loc(lsiz);

			if ((left->type&T_TYPE)==T_CHAN)
				des="chandes";
			else
				des= e->type&T_BYTE ? "bytedes" : "worddes";
			ste(des, wz);
		}
	}
	if (constant(index)) {
		register offset=index->u.cst;

		if ((left->type&T_TYPE)==T_CHAN)
			offset*=(wz+vz);
		else
		if ( !(e->type&T_BYTE) )
			offset*=vz;

		if (av==address)
			adp(offset);
		else {
			if (e->type&T_BYTE) {
				adp(offset);
				loi(1);
				cwv();
			} else
				Lof(offset);
		}
	} else {
		code_val(index);
		cvw();
		lin();
		lae(des, 0);
		if (av==address) {
			aar();
		} else {
			lar();
			if (e->type&T_BYTE) cwv();
		}
	}
}

void code_addr(e) register struct expr *e;
/* The address of e is wat we want. */
{
	if (err) return;

	switch(e->kind) {
	case E_NODE:
		subscript(e, address);
		break;
	case E_VAR: {	/* variable or channel */
		register struct symbol *var=e->u.var;

		if (var->s_type&T_BUILTIN)
			lae(var->s_info.vc.st.builtin, var->s_info.vc.offset);
		else
		if (var->s_info.vc.st.level==curr_level)
			if (var->s_type&T_PARAM
			    && (var->s_type&(T_TYPE|T_ARR))!=T_VALUE)
				Lolp(var->s_info.vc.offset);
			else
				lal(var->s_info.vc.offset);
		else {
			if (var->s_info.vc.offset<0)
				lxl(curr_level-var->s_info.vc.st.level);
			else
				lxa(curr_level-var->s_info.vc.st.level);
			if (var->s_type&T_PARAM
			    && (var->s_type&(T_TYPE|T_ARR))!=T_VALUE)
				Lofp(var->s_info.vc.offset);
			else
				adp(var->s_info.vc.offset);
		}
		} break;
	case E_TABLE:
	case E_BTAB:
		laedot(e->u.tab);
		break;
	}
}

void code_bool(e, pos, T, F)
	register struct expr *e;
	register pos;
	register int *T, *F;
/* if e = pos then
	fall through or jump to T;
   else
	jump to F;
   fi
 */
{
	register Default=0;

	if (err) return;

	if (e->kind==E_NODE) {
		register struct expr *left=e->u.node.left;
		register struct expr *right=e->u.node.right;

		switch(e->u.node.op) {
		case '<':
		case '>':
		case LE:
		case GE:
		case NE:
		case '=':
		case AFTER:
			code_val(left);
			code_val(right);
			bxx(pos, e->u.node.op, new_label(F));
			break;
		case AND:
		case OR:
			if ((e->u.node.op==AND && pos)
			 || (e->u.node.op==OR && !pos)
			) {
				int L=0;
				code_bool(left, pos, &L, F);
				Label(L);
				code_bool(right, pos, T, F);
			} else {
				int L=0;
				code_bool(left, !pos, &L, T);
				Label(L);
				code_bool(right, pos, T, F);
			}
			break;
		case NOT:
			code_bool(left, !pos, T, F);
			break;
		default:
			Default=1;
		}
	} else
		Default=1;

	if (Default) {
		code_val(e);
		if (vz>wz) {
			ldc0();
			cmi();
		} else
			tst();
		if (pos) zeq(new_label(F)); else zne(new_label(F));
	}
}

void code_assignment(e) register struct expr *e;
/* e->left := e->right */
{
	register struct expr *left=e->u.node.left;
	register struct expr *right=e->u.node.right;

	if (left->type&T_ARR) {
		register siz=left->arr_siz;

		code_addr(right);
		code_addr(left);
		blm(left->type&T_BYTE ? siz : siz*vz);
	} else {
		code_val(right);
		code_addr(left);
		sti(left->type&T_BYTE ? 1 : vz);
	}
}

void code_input(e) register struct expr *e;
/* Input one v from c ? v0; v1; ... */
{
	if (e==nil) {
		lae("any", 0);
		cal("chan_in");
		asp(pz);
	} else
	if (e->type&T_ARR) {
		loc(e->arr_siz);
		code_addr(e);
		cal(e->type&T_BYTE ? "c_ba_in" : "c_wa_in");
		asp(pz+wz);
	} else {
		code_addr(e);
		cal(e->type&T_BYTE ? "cbyte_in" : "chan_in");
		asp(pz);
	}
}

void code_output(e) register struct expr *e;
/* Output one e from c ? e0; e1; ... */
{
	if (e==nil) {
		Loc(0L);
		cal("chan_out");
		asp(vz);
	} else
	if (e->type&T_ARR) {
		loc(e->arr_siz);
		code_addr(e);
		cal(e->type&T_BYTE ? "c_ba_out" : "c_wa_out");
		asp(pz+wz);
	} else {
		code_val(e);
		cal("chan_out");
		asp(vz);
	} 
}

void code_any(e, NO) register struct expr *e; int *NO;
/* Test if the channel (push address on stack) has input. If not so remove the
 * channel pointer and jump to NO.  Otherwise input values.
 */
{
	int YES=0;
	register struct expr_list *elp;

	if (err) return;

	code_addr(e->u.io.chan);
	cal("chan_any");
	lfr(wz);
	tst();
	zne(new_label(&YES));
	asp(pz);
	branch(NO);
	Label(YES);
	elp=e->u.io.args;
	while (elp!=nil) {
		code_input(elp->arg);
		elp=elp->next;
	}
	asp(pz);
}

void code_void(e) register struct expr *e;
/* Assignment, I/O, or procedure call. */
{
	if (err) return;

	switch (e->kind) {
	case E_NODE:	/* Must be assignment */
		code_assignment(e);
		break;
	case E_IO: {
		register struct expr_list *elp;

		code_addr(e->u.io.chan);

		elp=e->u.io.args;
		while (elp!=nil) {
			if (e->u.io.out)
				code_output(elp->arg);
			else
				code_input(elp->arg);
			elp=elp->next;
		}
		asp(pz);
		}
		break;
	case E_CALL: {
		register size=0;
		register struct expr_list *elp=e->u.call.c_args;
		register struct symbol *proc=e->u.call.c_proc->u.var;
		register struct par_list *pars=proc->s_info.proc.pars;

		while (elp!=nil) {
			if (pars->pr_type==T_VALUE) {
				code_val(elp->arg);
				size+=vz;
			} else {
				code_addr(elp->arg);
				size+=pz;
			}
			elp=elp->next;
			pars=pars->pr_next;
		}
		if (proc->s_type&T_BUILTIN) {
			cal(proc->s_info.proc.st.builtin);
			asp(size);
		} else {
			if (proc->s_info.proc.st.level>curr_level) {
				/* Call down */
				lor0();
			} else
			if (proc->s_info.proc.st.level==curr_level) {
				/* Call at same level */
				Lolp(0);
			} else {
				/* Call up */
				lxa(curr_level-proc->s_info.proc.st.level);
				loi(pz);
			}
			cal(proc_label(proc->s_info.proc.label, proc->s_name));
			asp(size+pz);
			if (proc->s_info.proc.file!=curr_file) fil();
		}
		} break;
	}
}

void prologue(proc) register struct symbol *proc;
/* Open up the scope for a new proc definition. */
{
	static P=0;

	if (err) return;

	proc->s_info.proc.st.level= ++curr_level;
	proc->s_info.proc.file= curr_file;
	proc->s_info.proc.label= ++P;
	curr_offset=min_offset=0;
	pro(proc_label(proc->s_info.proc.label, proc->s_name));
	if (curr_level==1) fil();
}

void epilogue(proc) register struct symbol *proc;
/* Close the scope of a proc def. */
{
	if (err) return;

	curr_level--;
	ret(0);
	x_end(-min_offset);
}

void rep_init(v, e1, e2, r_info)
	struct symbol *v;
	register struct expr *e1, *e2;
	register struct replicator *r_info;
/* Compile v=[e1 FOR e2].  Info tells rep_test what decisions rep_init makes. */
{
	if (err) return;

	r_info->BEGIN=r_info->END=0;

	code_val(e1);
	Stl(v->s_info.vc.offset);

	if (!constant(e1) || !constant(e2)) {
		if (constant(e2) && word_constant(e2->u.cst)) {
			r_info->counter=memory(wz);
			loc((int) e2->u.cst);
			stl(r_info->counter);
		} else {
			r_info->counter=memory(vz);
			code_val(e2);
			Stl(r_info->counter);
		}
	}
	if (!constant(e2) || e2->u.cst<=0L)
		branch(&r_info->END);
	Label(new_label(&r_info->BEGIN));
}

void rep_test(v, e1, e2, r_info)
	register struct symbol *v;
	register struct expr *e1, *e2;
	register struct replicator *r_info;
{
	if (err) return;

	Inl(v->s_info.vc.offset);

	if (constant(e1) && constant(e2)) {
		Lol(v->s_info.vc.offset);
		Loc(e1->u.cst+e2->u.cst);
		if (vz>wz) {
			cmi();
			zlt(r_info->BEGIN);
		} else
			blt(r_info->BEGIN);
		Label(r_info->END);
	} else {
		if (constant(e2) && word_constant(e2->u.cst)) {
			del(r_info->counter);
			Label(r_info->END);
			lol(r_info->counter);
			tst();
		} else {
			Del(r_info->counter);
			Label(r_info->END);
			Lol(r_info->counter);
			if (vz>wz) {
				ldc0();
				cmi();
			} else
				tst();
		}
		zgt(r_info->BEGIN);
	}
}

void chan_init(info, arr_siz) union type_info *info; int arr_siz;
/* Garbage disposal unit for fresh channels. */
{
	if (err) return;

	loc(arr_siz);
	lal(info->vc.offset);
	cal("c_init");
	asp(wz+pz);
}

void leader()
{
	init();
	openfile((char *) nil);
	magic();
	meswp();
	maxdes();
}

void header()
{
	exp("main");
	pro("main");
	init_rt();
	main_fil();
}

void trailer()
{
	if (err)
		meserr();
	else {
		loc(0);
		ret(wz);
		x_end(-min_offset);
	}
	closefile();
}
