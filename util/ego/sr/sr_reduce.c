/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* S T R E N G T H   R E D U C T I O N
 *
 * S R _ R E D U C E . C
 *
 */


#include <stdlib.h>
#include <em_pseu.h>
#include <em_reg.h>
#include <em_mes.h>
#include <em_mnem.h>
#include <em_spec.h>
#include "../share/types.h"
#include "sr.h"
#include "../share/debug.h"
#include "../share/alloc.h"
#include "../share/def.h"
#include "../share/global.h"
#include "../share/utils.h"
#include "sr_aux.h"
#include "../share/lset.h"
#include "sr_xform.h"
#include "sr_reduce.h"
#include "sr_expr.h"



STATIC lset avail;
/* If an expression such as "iv * const" or "A[iv]" is
 * used more than once in a loop, we only use one temporary
 * local for it and reuse this local each time.
 * After the first occurrence, the expression is said to
 * be available.
 */

STATIC int regtyp(code)
	code_p code;
{
	switch(code->co_instr) {
		case op_mli:
		case op_mlu:
		case op_sli:
		case op_slu:
			return reg_any;
		default:
			return reg_pointer;
	}
	/* NOTREACHED */
}


STATIC gen_regmes(tmp,score,code,p)
	offset tmp;
	int score;
	code_p code;
	proc_p p;
{
	/* generate a register message for the temporary variable and
	 * insert it at the start of the procedure.
	 */

	line_p l,pro;

	l = reg_mes(tmp,code->co_tmpsize,regtyp(code),score);
	pro = p->p_start->b_start; /* every proc. begins with a PRO pseudo */
	l->l_next = pro->l_next;
	PREV(l->l_next) = l;
	pro->l_next = l;
	PREV(l) = pro;
}


STATIC line_p newcode(code,tmp)
	code_p code;
	offset tmp;
{
	/* Construct the EM code that will replace the reducible code,
	 * e.g.   iv * c   ->   tmp
	 *	  a[iv]    ->   *tmp
	 */

	line_p l;

	switch(code->co_instr) {
		case op_mli:
		case op_mlu:
		case op_sli:
		case op_slu:
			/* new code is just a LOL tmp */
			l = int_line(tmp);
			l->l_instr = op_lol;
			break;
		case op_aar:
			/* New code is a LOAD tmp, where tmp is a
			 * pointer variable, so the actual EM code
			 * depends on the pointer size.
			 */
			l = move_pointer(tmp,LOAD);
			break;
		case op_lar:
			/* New code is a load-indirect */
			l = int_line(tmp);
			l->l_instr = op_lil;
			break;
		case op_sar:
			/* New code is a store-indirect */
			l = int_line(tmp);
			l->l_instr = op_sil;
			break;
		default:
			assert(FALSE);
	}
	return l;
}



STATIC replcode(code,text)
	code_p code;
	line_p text;
{
	/* Replace old code (extending from code->co_lfirst to
	 * code->co_llast) by new code (headed by 'text').
	 */

	line_p l, l1, l2;

	for (l = text; l->l_next != (line_p) 0; l = l->l_next);
	/* 'l' now points to last instruction of text */
	l1 = PREV(code->co_lfirst); /* instruction just before old code */
	l2 = code->co_llast->l_next; /* instruction just behind old code */
	if (l1 == (line_p) 0) {
		code->co_block->b_start = text;
		PREV(text) = (line_p) 0;
	} else {
		l1->l_next = text;
		PREV(text) = l1;
	}
	if (l2 != (line_p) 0) {
		PREV(l2) = l;
	}
	l->l_next = l2;
	code->co_llast->l_next = (line_p) 0;
	/* Note that the old code is still accessible via code->co_lfirst */
}

STATIC line_p add_code(pl, l)
	line_p pl, l;
{
	if (! pl) {
		PREV(l) = 0;
	}
	else {
		line_p n = pl->l_next;

		DLINK(pl, l);
		if (n) {
			while (l->l_next) l = l->l_next;
			DLINK(l, n);
		}
		l = pl;
	}
	return l;
}



STATIC init_code(code,tmp)
	code_p code;
	offset tmp;
{
	/* Generate code to set up the temporary local.
	 * For multiplication, its initial value is const*iv_expr,
	 * for array operations it is &a[iv_expr] (where iv_expr is
	 * an expression that is a linear function of the induc. var.
	 * This code is inserted immediately before the loop entry.
	 * As the initializing code looks very much like the
	 * reduced code, we reuse that (old) code.
	 */

	line_p l, *p;

	l = code->co_llast; /* the mli, lar etc. instruction */
	switch(INSTR(l)) {
		case op_mli:
		case op_mlu:
			/* reduced code is: iv_expr * lc  (or lc * iv_expr)
			 * init_code is:    tmp = iv_expr * lc  (or lc*iv_expr)
			 * So we just insert a 'STL tmp'.
			 */
			l->l_next = int_line(tmp);
			l->l_next->l_instr = op_stl;
			break;
		case op_sli:
		case op_slu:
			/* reduced code is: iv_expr << lc
			 * init_code is:    tmp = iv_expr << lc
			 * So we just insert a 'STL tmp'.
			 */
			l->l_next = int_line(tmp);
			l->l_next->l_instr = op_stl;
			break;
		case op_lar:
		case op_sar:
			/* reduced code is: ...= A[iv_expr] resp.
			 *		    A[iv]_expr = ..
			 * init_code is: tmp = &A[iv_expr].
			 * So just change the lar or sar into a aar ...
			 */
			l->l_instr = (byte) op_aar;
			/* ... and fall through !! */
		case op_aar:
			/* append code to store a pointer in temp. local */
			l->l_next = move_pointer(tmp,STORE);
			break;
		default:
			assert(FALSE); /* non-reducible instruction */
	}
	PREV(l->l_next) = l;
	/* Now insert the code at the end of the header block */
	p = &code->co_loop->LP_INSTR;
	if (*p == (line_p) 0 || (PREV((*p)) == 0 && INSTR((*p)) == op_bra)) {
		/* LP_INSTR points to last instruction of header block,
		 * so if it is 0, the header block is empty yet.
		 */
		code->co_loop->LP_HEADER->b_start =
		  add_code(code->co_loop->LP_HEADER->b_start, code->co_lfirst);
	} else if (INSTR((*p)) == op_bra) {
		add_code(PREV((*p)), code->co_lfirst);
	}
	else	add_code(*p, code->co_lfirst);
	while (l->l_next) l = l->l_next;
	*p = l; /* new last instruction */
}

STATIC incr_code(code,tmp)
	code_p  code;
	offset  tmp;
{
	/* Generate code to increment the temporary local variable.
	 * The variable is incremented by
	 *  1) multiply -->   step value of iv * loop constant
	 *  2) array    -->   step value of iv * element size
	 * This value can be determined statically.
	 * If the induction variable is used in a linear
	 * expression in which its sign is negative
	 * (such as in: "5-(6-(-iv))" ), this value is negated.
	 * The generated code looks like:
	 *  LOL tmp ; LOC incr ; ADI ws ; STL tmp
	 * For pointer-increments we generate a "ADP c", rather than 
	 * a "LOC c; ADS ws".
	 * This code is put just after the code that increments
	 * the induction variable.
	 */

	line_p load_tmp, loc, add, store_tmp, l;

	add = newline(OPSHORT);
	SHORT(add) = ws;  /* the add instruction, can be ADI,ADU or ADS */
	switch(code->co_instr) {
		case op_mli:
		case op_mlu:
			loc = int_line(
				  code->co_sign *
				  off_set(code->c_o.co_loadlc) *
				  code->co_iv->iv_step);
			loc->l_instr = op_loc;
			add->l_instr = op_adi;
			load_tmp = int_line(tmp);
			load_tmp->l_instr = op_lol;
			store_tmp = int_line(tmp);
			store_tmp->l_instr = op_stl;
			break;
		case op_sli:
		case op_slu:
			loc = int_line(
				  code->co_sign *
				  code->co_iv->iv_step *
				  (1 << off_set(code->c_o.co_loadlc)));
			loc->l_instr = op_loc;
			add->l_instr = op_adi;
			load_tmp = int_line(tmp);
			load_tmp->l_instr = op_lol;
			store_tmp = int_line(tmp);
			store_tmp->l_instr = op_stl;
			break;
		case op_lar:
		case op_sar:
		case op_aar:
			loc = (line_p) 0;
			add = int_line(
			        code->co_sign *
				code->co_iv->iv_step *
				elemsize(code->c_o.co_desc));
			add->l_instr = op_adp;
			load_tmp = move_pointer(tmp,LOAD);
			store_tmp = move_pointer(tmp,STORE);
			break;
		default:
			assert(FALSE);
	}
	/* Now we've got pieces of code to load the temp. local,
	 * load the constant, add the two and store the result in
	 * the local. This code will be put just after the code that
	 * increments the induction variable.
	 */
	if (loc != (line_p) 0) concatenate(load_tmp,loc);
	concatenate(load_tmp,add);
	concatenate(load_tmp,store_tmp);
	/* Now load_tmp points to a list of EM instructions */
	l = code->co_iv->iv_incr;
	if (l->l_next != (line_p) 0) {
		DLINK(store_tmp,l->l_next);
	}
	DLINK(l,load_tmp); /* doubly link them */
}


STATIC remcode(c)
	code_p c;
{
	line_p l, next;

	for (l = c->co_lfirst; l != (line_p) 0; l = next) {
		next = l->l_next;
		oldline(l);
	}
	oldcinfo(c);
}


STATIC bool same_address(l1,l2,vars)
	line_p l1,l2;
	lset   vars;
{
	/* See if l1 and l2 load the same address */

	if (INSTR(l1) != INSTR(l2)) return FALSE;
	switch(INSTR(l1)) {
		case op_lae:
			return OBJ(l1) == OBJ(l2);
		case op_lal:
			return off_set(l1) == off_set(l2);
		case op_lol:
			return ps == ws &&
			  off_set(l1) == off_set(l2) &&
			  is_loopconst(l1,vars);
		case op_ldl:
			return ps == 2*ws &&
			  off_set(l1) == off_set(l2) &&
			  is_loopconst(l1,vars);
		default:
			return FALSE;
	}
}


STATIC bool same_expr(lb1,le1,lb2,le2)
	line_p lb1,le1,lb2,le2;
{
	/* See if the code from lb1 to le1 is the same
	 * expression as the code from lb2 to le2.
	 */


	register line_p l1,l2;

	l1 = lb1;
	l2 = lb2;
	for (;;) {
		if (INSTR(l1) != INSTR(l2)) return FALSE;
		switch(TYPE(l1)) {
			case OPSHORT:
				if (TYPE(l2) != OPSHORT ||
				    SHORT(l1) != SHORT(l2)) return FALSE;
				break;
			case OPOFFSET:
				if (TYPE(l2) != OPOFFSET ||
				    OFFSET(l1) != OFFSET(l2)) return FALSE;
				break;
			case OPNO:
				break;
			default:
				return FALSE;
		}
		if (l1 == le1 ) return l2 == le2;
		if (l2 == le2) return FALSE;
		l1 = l1->l_next;
		l2 = l2->l_next;
	}
}

STATIC bool same_code(c1,c2,vars)
	code_p c1,c2;
	lset   vars;
{
	/* See if c1 and c2 compute the same expression. Two array
	 * references can be the same even if one is e.g a fetch
	 * and the other a store.
	 */

	switch(c1->co_instr) {
		case op_mli:
		case op_mlu:
		case op_sli:
		case op_slu:
			return c1->co_instr == c2->co_instr &&
			off_set(c1->c_o.co_loadlc) ==
			off_set(c2->c_o.co_loadlc) &&
			same_expr(c1->co_ivexpr,c1->co_endexpr,
				  c2->co_ivexpr,c2->co_endexpr);
		case op_aar:
		case op_lar:
		case op_sar:
			return ( c2->co_instr == op_aar ||
				 c2->co_instr == op_lar ||
				 c2->co_instr == op_sar) &&
			same_expr(c1->co_ivexpr,c1->co_endexpr,
				  c2->co_ivexpr,c2->co_endexpr) &&
			same_address(c1->c_o.co_desc,c2->c_o.co_desc,vars) &&
			same_address(c1->co_lfirst,c2->co_lfirst,vars);
		default:
			assert(FALSE);
	}
	/* NOTREACHED */
}


STATIC code_p available(c,vars)
	code_p c;
	lset   vars;
{
	/* See if the code is already available.
	 * If so, return a pointer to the first occurrence
	 * of the code.
	 */

	Lindex i;
	code_p cp;

	for (i = Lfirst(avail); i != (Lindex) 0; i = Lnext(i,avail)) {
		cp = (code_p) Lelem(i);
		if (same_code(c,cp,vars)) {
			return cp;
		}
	}
	return (code_p) 0;
}

STATIC fix_header(lp)
	loop_p lp;
{
	/* Check if a header block was added, and if so, add a branch to
	 * the entry block.
	 * If it was added, it was added to the end of the procedure, so
	 * move the END pseudo.
	 */
	bblock_p b = curproc->p_start;

	if (lp->LP_HEADER->b_next == 0) {
		line_p l = last_instr(lp->LP_HEADER);
		line_p e;

		assert(l != 0);
		if (INSTR(l) != op_bra) {
			line_p j = newline(OPINSTRLAB);

			assert(INSTR(lp->lp_entry->b_start) == op_lab);
			INSTRLAB(j) = INSTRLAB(lp->lp_entry->b_start);
			j->l_instr = op_bra;
			DLINK(l, j);
			l = j;
		}

		while (b->b_next != lp->LP_HEADER) b = b->b_next;
		e = last_instr(b);
		assert(INSTR(e) == ps_end);
		assert(PREV(e) != 0);
		PREV(e)->l_next = 0;
		DLINK(l, e);
	}
}

STATIC reduce(code,vars)
	code_p code;
	lset   vars;
{
	/* Perform the actual transformations. The code on the left
	 * gets transformed into the code on the right. Note that
	 * each piece of code is assigned a name, that will be
	 * used to describe the whole process.
	 *
	 *					t = iv * 118;	(init_code)
	 *	do		--->		do
	 *	   .. iv * 118 ..		   .. t ..	(new_code)
	 *	   iv++;			   iv++;
	 *					   t += 118;	(incr_code)
	 *	od				od
	 */

	offset tmp;
	code_p ac;

	OUTTRACE("succeeded!!",0);
	if ((ac = available(code,vars)) != (code_p) 0) {
		/* The expression is already available, so we
		 * don't have to generate a new temporary local for it.
		 */
		OUTTRACE("expression was already available",0);
		replcode(code,newcode(code,ac->co_temp));
		remcode(code);
	} else {
		make_header(code->co_loop);
		/* make sure there's a header block */
		tmp = tmplocal(curproc,(offset) code->co_tmpsize);
		code->co_temp = tmp;
		/* create a new local variable in the stack frame
		 * of current proc.
		 */
		gen_regmes(tmp,3,code,curproc); /* generate register message */
		/* score is set to 3, as TMP is used at least 3 times */
		replcode(code,newcode(code,tmp));
		OUTTRACE("replaced old code by new code",0);
		/* Construct the EM-code that will replace the reducible code
	 	 * and replace the old code by the new code.
	 	 */
		init_code(code,tmp);
		OUTTRACE("emitted initializing code",0);
		/* Emit code to initialize the temporary local. This code is
	 	 * put in the loop header block.
	 	 */
		incr_code(code,tmp); /* emit code to increment temp. local */
		OUTTRACE("emitted increment code",0);
		Ladd(code,&avail);
		fix_header(code->co_loop);
	}
}



STATIC void try_multiply(lp,ivs,vars,b,mul)
	loop_p   lp;
	lset	 ivs,vars;
	bblock_p b;
	line_p   mul;
{
	/* See if we can reduce the strength of the multiply
	 * instruction. If so, then set up the global common
	 * data structure 'c' (containing information about the
	 * code to be reduced) and call 'reduce'.
	 */

	line_p l2,lbegin;
	iv_p   iv;
	code_p c;
	int    sign;

	VL(mul);
	OUTTRACE("trying multiply instruction on line %d",linecount);
	if (ovfl_harmful && !IS_STRONG(b)) return;
	/* If b is not a strong block, optimization may
	 * introduce an overflow error in the initializing code.
	 */

	l2 = PREV(mul); /* Instruction before the multiply */
	if ( (is_ivexpr(l2,ivs,vars,&lbegin,&iv,&sign)) &&
		is_const(PREV(lbegin)) ) {
			/* recognized expression "const * iv_expr" */
			c = newcinfo();
			c->c_o.co_loadlc = PREV(l2);
			c->co_endexpr = l2;
			c->co_lfirst = PREV(lbegin);
	} else {
		if (is_const(l2) &&
			(is_ivexpr(PREV(l2),ivs,vars,&lbegin,&iv,&sign))) {
				/* recognized "iv * const " */
				c = newcinfo();
				c->c_o.co_loadlc = l2;
				c->co_endexpr = PREV(l2);
				c->co_lfirst = lbegin;
		} else {
			OUTTRACE("failed",0);
			return;
		}
	}
	/* common part for both patterns */
	c->co_iv = iv;
	c->co_loop = lp;
	c->co_block = b;
	c->co_llast = mul;
	c->co_ivexpr = lbegin;
	c->co_sign = sign;
	c->co_tmpsize = ws; /* temp. local is a word */
	c->co_instr = INSTR(mul);
	OUTVERBOSE("sr: multiply in proc %d loop %d",
		curproc->p_id, lp->lp_id);
	Ssr++;
	reduce(c,vars);
}



STATIC void try_leftshift(lp,ivs,vars,b,shft)
	loop_p   lp;
	lset	 ivs,vars;
	bblock_p b;
	line_p   shft;
{
	/* See if we can reduce the strength of the leftshift
	 * instruction. If so, then set up the global common
	 * data structure 'c' (containing information about the
	 * code to be reduced) and call 'reduce'.
	 */

	line_p l2,lbegin;
	iv_p   iv;
	code_p c;
	int    sign;

	VL(shft);
	OUTTRACE("trying leftshift instruction on line %d",linecount);
	if (ovfl_harmful && !IS_STRONG(b)) return;
	/* If b is not a strong block, optimization may
	 * introduce an overflow error in the initializing code.
	 */

	l2 = PREV(shft); /* Instruction before the shift */
	if (is_const(l2) && off_set(l2) > sli_threshold &&
		(is_ivexpr(PREV(l2),ivs,vars,&lbegin,&iv,&sign))) {
			/* recognized "iv << const " */
			c = newcinfo();
			c->c_o.co_loadlc = l2;
			c->co_endexpr = PREV(l2);
			c->co_lfirst = lbegin;
	} else {
		OUTTRACE("failed",0);
		return;
	}
	c->co_iv = iv;
	c->co_loop = lp;
	c->co_block = b;
	c->co_llast = shft;
	c->co_ivexpr = lbegin;
	c->co_sign = sign;
	c->co_tmpsize = ws; /* temp. local is a word */
	c->co_instr = INSTR(shft);
	OUTVERBOSE("sr: leftshift in proc %d loop %d",
		curproc->p_id, lp->lp_id);
	Ssr++;
	reduce(c,vars);
}


STATIC void try_array(lp,ivs,vars,b,arr)
	loop_p   lp;
	lset	 ivs,vars;
	bblock_p b;
	line_p   arr;
{
	/* See if we can reduce the strength of the array reference
	 * instruction 'arr'.
	 */

	line_p l2,l3,lbegin;
	iv_p   iv;
	code_p c;
	int    sign;

	/* Try to recognize the pattern:
	 *	LOAD ADDRES OF A
	 *      LOAD IV
	 *      LOAD ADDRESS OF DESCRIPTOR
	 */
	VL(arr);
	OUTTRACE("trying array instruction on line %d",linecount);
	if (arrbound_harmful && !IS_STRONG(b)) return;
	/* If b is not a strong block, optimization may
	 * introduce an array bound error in the initializing code.
	 */
	l2 = PREV(arr);
	if (is_caddress(l2,vars) &&
		(INSTR(arr) == op_aar || elemsize(l2) == ws) &&
		(is_ivexpr(PREV(l2),ivs,vars,&lbegin,&iv,&sign)) ) {
		l3 = PREV(lbegin);
		if (is_caddress(l3,vars)) {
			c = newcinfo();
			c->co_iv = iv;
			c->co_loop = lp;
			c->co_block = b;
			c->co_lfirst = l3;
			c->co_llast = arr;
			c->co_ivexpr = lbegin;
			c->co_endexpr = PREV(l2);
			c->co_sign = sign;
			c->co_tmpsize = ps; /* temp. local is pointer */
			c->co_instr = INSTR(arr);
			c->c_o.co_desc = l2;
			OUTVERBOSE("sr: array in proc %d loop %d",
				curproc->p_id,lp->lp_id);
			Ssr++;
			reduce(c,vars);
		}
	}
}



STATIC clean_avail()
{
	Lindex i;

	for (i = Lfirst(avail); i != (Lindex) 0; i = Lnext(i,avail)) {
		oldcinfo(Lelem(i));
	}
	Ldeleteset(avail);
}



strength_reduction(lp,ivs,vars)
	loop_p lp;	/* description of the loop */
	lset    ivs;	/* set of induction variables of the loop */
	lset	vars;	/* set of local variables changed in loop */
{
	/* Find all expensive instructions (leftshift, multiply, array) and see
	 * if they can be reduced. We branch to several instruction-specific
	 * routines (try_...) that check if reduction is possible,
	 * and that set up a common data structure (code_info).
	 * The actual transformations are done by 'reduce', that is
	 * essentially instruction-independend.
	 */

	bblock_p b;
	line_p   l, next;
	Lindex   i;

	avail = Lempty_set();
	for (i = Lfirst(lp->LP_BLOCKS); i != (Lindex) 0;
			i = Lnext(i,lp->LP_BLOCKS)) {
		b = (bblock_p) Lelem(i);
		for (l = b->b_start; l != (line_p) 0; l = next) {
			next = l->l_next;
			if (TYPE(l) == OPSHORT && SHORT(l) == ws) {
				switch(INSTR(l)) {
					case op_sli:
					case op_slu:
						try_leftshift(lp,ivs,vars,b,l);
						break;
					case op_mlu:
					case op_mli:
						try_multiply(lp,ivs,vars,b,l);
						break;
					case op_lar:
					case op_sar:
					case op_aar:
						try_array(lp,ivs,vars,b,l);
						break;
				}
			}
		}
	}
	clean_avail();
}
