/* S H A R E D   F I L E
 *
 * G E T . C
 */

#include <stdio.h>
#include "types.h"
#include "def.h"
#include "debug.h"
#include "global.h"
#include "lset.h"
#include "cset.h"
#include "get.h"
#include "alloc.h"
#include "map.h"
#include "aux.h"
#include "../../../h/em_spec.h"
#include "../../../h/em_mnem.h"
#include "../../../h/em_pseu.h"
#include "../../../h/em_mes.h"
#include "../../../h/em_flag.h"

extern char em_flag[];


/* global variables */

static	FILE	*f;
STATIC	block_id lastbid;	/* block identifying number */
STATIC	lab_id	 lastlabid;	/* last label identifier */

/* creating new identifying numbers, i.e. numbers that did not
 * appear in the input.
 */

bblock_p freshblock()
{
	bblock_p b;
	b = newbblock();
	b->b_id = ++lastbid;
	return b;
}


lab_id freshlabel()
{
	curproc->p_nrlabels++;
	return ++lastlabid;
}


/* local routines */

#define getbyte()	getc(f)

#define getmark()	getbyte()

STATIC short getshort() {
	register int l_byte, h_byte;

	l_byte = getbyte();
	h_byte = getbyte();
	if ( h_byte>=128 ) h_byte -= 256 ;
	return l_byte | (h_byte*256) ;
}


STATIC offset getoff() {
	register long l;
	register int h_byte;

	l = getbyte();
	l |= ((unsigned) getbyte())*256 ;
	l |= getbyte()*256L*256L ;
	h_byte = getbyte() ;
	if ( h_byte>=128 ) h_byte -= 256 ;
	return l | (h_byte*256L*256*256L) ;
}

STATIC int getint()
{
	/* Read an integer from the input file. This routine is
	 * only used when reading a bitvector-set. We expect  an
	 * integer to be either a short or a long.
	 */

	if (sizeof(int) == sizeof(short)) {
		return getshort();
	} else {
		assert (sizeof(int) == sizeof(offset));
		return getoff();
	}
}

/* getptable */

loop_p getloop(id)
	loop_id id;
{
	/* Map a loop identifier onto a loop struct.
	 * If no struct was alocated yet for this identifier then
	 * allocate one now and update the loop-map table.
	 */


	assert (id > 0 && id <=lplength);
	if (lpmap[id] == (loop_p) 0) {
		lpmap[id] = newloop();
		lpmap[id]->lp_id = id;
	}
	return (lpmap[id]);
}

bblock_p getblock(id)
	block_id id;
{
	/* Map a basic block identifier onto a block struct
	 * If no struct was alocated yet for this identifier then
	 * allocate one now and update the block-map table.
	 */


	assert (id >= 0 && id <=blength);
	if (id == 0) return (bblock_p) 0;
	if (bmap[id] == (bblock_p) 0) {
		bmap[id] = newbblock();
		bmap[id]->b_id = id;
	}
	return (bmap[id]);
}


lset getlset(p)
	char *((*p) ());
{
	/* Read a 'long' set. Such a set is represented externally
	 * as a sequence of identifying numbers terminated by a 0.
	 * The procedural parameter p maps such a number onto a
	 * pointer to a struct (bblock_p, loop_p etc.).
	 */

	lset s;
	int id;

	s = Lempty_set();
	while (id = getshort()) {
		Ladd( (*p) (id), &s);
	}
	return s;
}


cset getcset()
{
	/* Read a 'compact' set. Such a set is represented externally
	 * a row of bytes (its bitvector) preceded by its length.
	 */

	cset s;
	register short i;

	s = Cempty_set(getshort());
	for (i = 0; i <= DIVWL(s->v_size-1);i++) {
		s->v_bits[i] = getint();
	}
	return s;
}


proc_p getptable(pname)
	char *pname;
{
	short i;
	proc_p head, p, *pp;
	short all;

	if ((f = fopen(pname,"r")) == NULL) {
		error("cannot open %s",pname);
	}

	plength = getshort();  /* table is preceded by its length */
	assert(plength >= 0);
	assert(plength < 1000);  /* See if its a reasonable number */
	pmap = (proc_p *) newmap(plength);   /* allocate the pmap table */

	all = getshort();
	head = (proc_p) 0;
	pp = &head;
	for (i = 0; i < plength; i++) {
		if (feof(f)) {
			error("unexpected eof %s", pname);
		}
		p = newproc();
		p->p_id = getshort();
		assert(p->p_id > 0 && p->p_id <= plength);
		pmap[p->p_id] = p;
		p->p_flags1 = getbyte();
		if (p->p_flags1 & PF_BODYSEEN) {
			p->p_nrlabels = getshort();
			p->p_localbytes = getoff();
			p->p_nrformals = getoff();
			if (all) {
				p->p_change = newchange();
				p->p_change->c_ext = getcset();
				p->p_change->c_flags = getshort();
				p->p_use = newuse();
				p->p_use->u_flags = getshort();
				p->p_calling = getcset();
			}
		}
		*pp = p;
		pp = &(p->p_next);
	}
	fclose(f);
	OUTTRACE("have read proc table of length %d",plength);
	return head;	/* pointer to first structure of list */
}



/* getdtable */

dblock_p getdtable(dname)
	char *dname;
{
	/* Read the data block table. Every data block may
	 * have a list of objects and a list of values (arguments),
	 * each of which is also represented by a structure.
	 * So the input file contains a mixture of dblock,
	 * obj and arg records, each one having its own
	 * attributes. A mark indicates which one comes next.
	 * We assume that the syntactic structure of the input
	 * is correct.
	 */

	dblock_p head, d, *dp;
	obj_p    obj, *op;
	arg_p    arg, *ap;
	/* dp, op an ap tell how the next dblock/obj/arg
	 * has to be linked.
	 */
	int n;

	head = (dblock_p) 0;
	dp = &head;
	if ((f = fopen(dname,"r")) == NULL) {
		error("cannot open %s", dname);
	}
	olength = getshort();
	assert(olength >= 0);
	assert(olength < 5000);  /* See if its a reasonable number */
	/* total number of objects */
	omap = (obj_p *) newmap(olength);  /* allocate omap table */

	while (TRUE) {
		n = getmark();
		if (feof(f)) break;
		switch(n) {
			case MARK_DBLOCK:
				d = *dp = newdblock();
				op = &d->d_objlist;
				ap = &d->d_values;
				dp = &d->d_next;
				d->d_id = getshort();
				d->d_pseudo = getbyte();
				d->d_size = getoff();
				d->d_fragmnr = getshort();
				d->d_flags1 = getbyte();
				break;
			case MARK_OBJ:
				obj = *op = newobject();
				op = &obj->o_next;
				obj->o_dblock = d;
				obj->o_id = getshort();
				assert(obj->o_id >0);
				assert(obj->o_id <= olength);
				omap[obj->o_id] = obj;
				obj->o_size = getoff();
				obj->o_off = getoff();
				break;
			case MARK_ARG:
				arg = *ap = newarg(ARGOFF);
				ap = &arg->a_next;
				arg->a_a.a_offset = getoff();
				break;
			default:
				assert(FALSE);
		}
	}
	OUTTRACE("have read data table, %d objects",olength);
	return head;
}



/* getbblocks */

STATIC argstring(length,abp)
	short  length;
	register argb_p abp;
{

	while (length--) {
		if (abp->ab_index == NARGBYTES)
			abp = abp->ab_next = newargb();
		abp->ab_contents[abp->ab_index++] = getbyte();
	}
}



STATIC arg_p readargs()
{
	/* Read a list of arguments and allocate structures
	 * for them. Return a pointer to the head of the list.
	 */

	arg_p head, arg, *ap;
	byte  t;
	short length;

	ap = &head;
	for (;;) {
		/* every argument list is terminated by an
		 * ARGCEND byte in Intermediate Code.
		 */
		t = getbyte();
		if (t == (byte) ARGCEND) {
			return head;
		}
		arg = *ap = newarg(t);
		ap = &arg->a_next;
		switch((short) t) {
			case ARGOFF:
				arg->a_a.a_offset = getoff();
				break;
			case ARGINSTRLAB:
				arg->a_a.a_instrlab = getshort();
				break;
			case ARGOBJECT:
				arg->a_a.a_obj = omap[getshort()];
				/* Read an object identifier (o_id)
				 * and use the omap table to obtain
				 * a pointer to the rigth obj struct.
				 */
				break;
			case ARGPROC:
				arg->a_a.a_proc = pmap[getshort()];
				/* Read a procedure identifier (p_id) */
				break;
			case ARGSTRING:
				length = getshort();
				argstring(length, &arg->a_a.a_string);
				break;
			case ARGICN:
			case ARGUCN:
			case ARGFCN:
				length = getshort();
				arg->a_a.a_con.ac_length = length;
				/* size of the constant */
				argstring(getshort(),
					  &arg->a_a.a_con.ac_con);
				break;
			default:
				assert(FALSE);
		}
	}
}


STATIC line_p read_line(p_out)
	proc_p *p_out;
{
	/* Read a line of EM code (i.e. one instruction)
	 * and its arguments (if any).
	 * In Intermediate Code, the first byte is the
	 * instruction code and the second byte denotes the kind
	 * of operand(s) that follow.
	 */

	line_p lnp;
	byte   instr;

	instr = getbyte();
	if (feof(f)) return (line_p) 0;
	lnp = newline(getbyte());
	linecount++;
	lnp->l_instr = instr;
	switch(TYPE(lnp)) {
		/* read the operand(s) */
		case OPSHORT:
			SHORT(lnp) = getshort();
			break;
		case OPOFFSET:
			OFFSET(lnp) = getoff();
			break;
		case OPINSTRLAB:
			INSTRLAB(lnp) = getshort();
			if (instr == op_lab) {
				/* defining occurrence of an
				 * instruction label.
				 */
				lmap[INSTRLAB(lnp)] = lnp;
			}
			break;
		case OPOBJECT:
			OBJ(lnp) = omap[getshort()];
			break;
		case OPPROC:
			PROC(lnp) = pmap[getshort()];
			if ((instr & BMASK) == ps_pro) {
				/* enter new procedure: allocate a
				 * label map and a label-block map table.
				 */
				*p_out = PROC(lnp);
				llength = (*p_out)->p_nrlabels;
				lmap = (line_p *) newmap(llength);
				/* maps lab_id to line structure */
				lbmap = (bblock_p *) newmap(llength);
				/* maps lab_id to bblock structure */
				lastlabid = llength;
			}
			break;
		case OPLIST:
			ARG(lnp) = readargs();
			break;
		default:
			assert(TYPE(lnp) == OPNO);
	}
	return lnp;
}


STATIC message(lnp)
	line_p lnp;
{
	/* See if  lnp is some useful message.
	 * (e.g. a message telling that a certain local variable
	 * will never be referenced indirectly, so it may be put
	 * in a register. If so, add it to the mesregs set.)
	 */

	assert(ARG(lnp)->a_type == ARGOFF);
	switch((int) aoff(ARG(lnp),0)) {
		case ms_reg:
			if (ARG(lnp)->a_next != (arg_p) 0) {
				/* take only "mes 3" with further arguments */
				Ladd(lnp,&mesregs);
			}
			break;
		case ms_err:
			error("ms_err encountered");
		case ms_opt:
			error("ms_opt encountered");
		case ms_emx:
			ws = aoff(ARG(lnp),1);
			ps = aoff(ARG(lnp),2);
			break;
	}
}



STATIC line_p getlines(lf,n,p_out,collect_mes)
	FILE *lf;
	int n;
	proc_p *p_out;
	bool collect_mes;
{
	/* Read n lines of EM text and doubly link them.
	 * Also process messages.
	 */

	line_p head, *pp, l, lprev;

	f = lf; /* EM input file */
	pp = &head;
	lprev = (line_p) 0;
	while (n--) {
		l = *pp = read_line(p_out);
		PREV(l) = lprev;
		pp = &l->l_next;
		lprev = l;
		if (collect_mes && INSTR(l) == ps_mes) {
			message(l);
		}
	}
	*pp = (line_p) 0;
	return head;
}



bool getunit(gf,lf,kind_out,g_out,l_out,p_out,collect_mes)
	FILE   *gf,*lf;
	short  *kind_out;
	bblock_p *g_out;
	line_p *l_out;
	proc_p *p_out;
	bool   collect_mes;
{
	/* Read control flow graph (gf) and EM text (lf) of the next procedure.
	 * A pointer to the proctable entry of the read procedure is
	 * returned via p_out.
	 * This routine also constructs the bmap and lpmap tables.
	 * Note that we allocate structs for basic blocks and loops
	 * at their first reference rather than at when we read them.
	 */

	int n,i;
	bblock_p head, *pp, b;
	loop_p lp;

	f = gf;
	blength = getshort(); /* # basic blocks in this procedure */
	if (feof(f)) return FALSE;
	if (blength == 0) {
		/* data unit */
		*kind_out = LDATA;
		n = getshort();
		*l_out = getlines(lf,n,p_out,collect_mes);
		return TRUE;
	}
	*kind_out = LTEXT;
	bmap = (bblock_p *) newmap(blength); /* maps block_id on bblock_p */
	lplength = getshort(); /* # loops in this procedure */
	lpmap = (loop_p *) newmap(lplength); /* maps loop_id on loop_p */

	/* Read the basic blocks and the EM text */
	pp = &head; /* we use a pointer-to-a-pointer to link the structs */
	for (i = 0; i < blength; i++) {
		b = getblock(getshort());
		n = getshort();  /* #instructions in the block */
		b->b_succ = getlset(getblock);
		b->b_pred = getlset(getblock);
		b->b_idom = getblock(getshort());
		b->b_loops = getlset(getloop);
		b->b_flags = getshort();
		b->b_start = getlines(lf,n,p_out,collect_mes);  /* read EM text */
		*pp = b;
		pp = &b->b_next;
		f = gf;
	}
	lastbid = blength; /* last block_id */

	/* read the information about loops */
	curproc->p_loops = Lempty_set();
	for (i = 0; i < lplength; i++) {
		lp = getloop(getshort());
		lp->lp_level = getshort(); /* nesting level */
		lp->lp_entry = getblock(getshort()); /* entry block of the loop */
		lp->lp_end = getblock(getshort()); /* tail of back edge of loop */
		Ladd(lp,&curproc->p_loops);
	}
	*g_out = head;
	return TRUE;
}


/* The procedure getbblocks is used only by the Control Flow phase.
 * It reads the EM textfile and partitions every procedure into
 * a number of basic blocks.
 */


#define LABEL0		0
#define LABEL		1
#define NORMAL		2
#define JUMP		3
#define END		4
#define AFTERPRO	5
#define INIT		6


/* These global variables are used by getbblocks and nextblock. */

STATIC bblock_p b, *bp;  /* b is the current basic block, bp is
			  * the address where the next block has
			  * to be linked.
			  */
STATIC line_p   lnp, *lp; /* lnp is the current line, lp is
			   * the address where the next line
			   * has to be linked.
			   */
STATIC short state;	/* We use a finite state machine with the
			 * following states:
			 *  LABEL0: after the first (successive)
			 *	    instruction label.
			 *  LABEL1:  after at least two successive
			 *	    instruction labels.
			 *  NORMAL: after a normal instruction.
			 *  JUMP:   after a branch (conditional,
			 *	    unconditional or CSA/CSB).
			 *  END:    after an END pseudo
			 *  AFTERPRO: after we've read a PRO pseudo
			 *  INIT:   initial state
			 */


STATIC nextblock()
{
	/* allocate a new basic block structure and
	 * set b, bp and lp.
	 */

	b = *bp = freshblock();
	bp = &b->b_next;
	b->b_start = lnp;
	b->b_succ = Lempty_set();
	b->b_pred = Lempty_set();
	b->b_extend = newcfbx(); /* basic block extension for CF */
	b->b_extend->bx_cf.bx_bucket = Lempty_set();
	b->b_extend->bx_cf.bx_semi = 0;
	lp = &lnp->l_next;
#ifdef TRACE
	fprintf(stderr,"new basic block, id = %d\n",lastbid);
#endif
}


STATIC short kind(lnp)
	line_p lnp;
{
	/* determine if lnp is a label, branch, end or otherwise */

	short instr;
	byte  flow;

	if ((instr = INSTR(lnp)) == op_lab) return (short) LABEL;
	if (instr == ps_end) return (short) END;
	if (instr > sp_lmnem) return (short) NORMAL; /* pseudo */
	if ((flow = (em_flag[instr-sp_fmnem] & EM_FLO)) == FLO_C ||
	     flow == FLO_T) return (short) JUMP; /* conditional/uncond. jump */
	return (short) NORMAL;
}



bool getbblocks(fp,kind_out,n_out,g_out,l_out)
	FILE *fp;
	short *kind_out;
	short *n_out;
	bblock_p *g_out;
	line_p *l_out;
{
	bblock_p head = (bblock_p) 0;
	line_p headl = (line_p) 0;

	curproc = (proc_p) 0;
	/* curproc will get a value when we encounter a PRO pseudo.
	 * If there is no such pseudo, we're reading only data
	 * declarations or messages (outside any proc.).
	 */
	f = fp;
	lastbid = (block_id) 0;  /* block identier */
	state = INIT;	/* initial state */
	bp = &head;

	for (;;) {
#ifdef TRACE
		fprintf(stderr,"state = %d\n",state);
#endif
		switch(state) {
			case LABEL0:
				nextblock();
				/* Fall through !! */
			case LABEL:
				lbmap[INSTRLAB(lnp)] = b;
				/* The lbmap table contains for each
				 * label_id the basic block of that label.
				 */
				lnp = read_line(&curproc);
				state = kind(lnp);
				if (state != END) {
					*lp = lnp;
					lp = &lnp->l_next;
				}
				break;
			case NORMAL:
				lnp = read_line(&curproc);
				if ( (state = kind(lnp)) == LABEL) {
					/* If we come accross a label
					 * here, it must be the beginning
					 * of a new basic block.
					 */
					state = LABEL0;
				} else {
					if (state != END) {
						*lp = lnp;
						lp = &lnp->l_next;
					}
				}
				break;
			case JUMP:
				lnp = read_line(&curproc);
				/* fall through ... */
			case AFTERPRO:
				switch(state = kind(lnp)) {
					case LABEL:
						state = LABEL0;
						break;
					case JUMP:
					case NORMAL:
						nextblock();
						break;
				}
				break;
			case END:
				*lp = lnp;
#ifdef TRACE
				fprintf(stderr,"at end of proc, %d blocks\n",lastbid);
#endif
				if (head == (bblock_p) 0) {
					*kind_out = LDATA;
					*l_out = headl;
				} else {
					*kind_out = LTEXT;
					*g_out = head;
					*n_out = (short) lastbid;
					/* number of basic blocks */
				}
				return TRUE;
			case INIT:
				lnp = read_line(&curproc);
				if (feof(f)) return FALSE;
				if (INSTR(lnp) == ps_pro) {
					state = AFTERPRO;
				} else {
					state = NORMAL;
					headl = lnp;
					lp = &lnp->l_next;
				}
				break;
		}
	}
}

/* The following routines are only used by the Inline Substitution phase */

call_p getcall(cf)
	FILE *cf;
{
	/* read a call from the call-file */

	call_p c;
	proc_p voided;
	actual_p act,*app;
	short n,m;

	f = cf;
	c = newcall();
	n = getshort(); /* void nesting level */
	if (feof(f)) return (call_p) 0;
	c->cl_caller = pmap[getshort()];
	c->cl_id     = getshort();
	c->cl_proc   = pmap[getshort()];
	c->cl_looplevel = getbyte();
	c->cl_flags = getbyte();
	c->cl_ratio  = getshort();
	app = &c->cl_actuals;
	n = getshort();
	while(n--) {
		act = newactual();
		m = getshort();
		act->ac_size = getoff();
		act->ac_inl = getbyte();
		act->ac_exp = getlines(cf,m,&voided);
		*app = act;
		app = &act->ac_next;
	}
	*app = (actual_p) 0;
	return c;
}



line_p get_text(lf,p_out)
	FILE *lf;
	proc_p *p_out;
{
	/* Read the EM text of one unit
	 * If it is a procedure, set p_out to
	 * the proc. just read. Else set p_out
	 * to 0.
	 */

	line_p dumhead, l, lprev;
	loop_p *oldlpmap = lpmap;
	line_p *oldlmap = lmap;
	short oldllength = llength;
	short oldlastlabid = lastlabid;

	f = lf;
	*p_out = (proc_p) 0;
	dumhead = newline(OPNO);
	/* The list of instructions is preceeded by a dummy
	 * line, to simplify list manipulation
	 */
	dumhead->l_instr = op_nop; /* just for fun */
	lprev = dumhead;
	for (;;) {
		l = read_line(p_out);
		if (feof(f)) return (line_p) 0;
		lprev->l_next = l;
		PREV(l) = lprev;
		if (INSTR(l) == ps_end) break;
		if (INSTR(l) == ps_mes) {
			message(l);
		}
		lprev = l;
	}
	/* The tables that map labels to instructions
	 * and labels to basic blocks are not used.
	 */
	if (*p_out != (proc_p) 0) {
		oldmap(lmap,llength);
		oldmap(lbmap,llength);
		lmap = oldlmap;
		lpmap = oldlpmap;
	}
	llength = oldllength;
	lastlabid = oldlastlabid;
	return dumhead;
}



calcnt_p getcc(ccf,p)
	FILE *ccf;
	proc_p p;
{
	/* Get call-count info of procedure p */

	calcnt_p head,cc,*ccp;
	short i;

	fseek(ccf,p->p_extend->px_il.p_ccaddr,0);
	f = ccf;
	head = (calcnt_p) 0;
	ccp = &head;
	for (i = getshort(); i != (short) 0; i--) {
		cc = *ccp = newcalcnt();
		cc->cc_proc = pmap[getshort()];
		cc->cc_count = getshort();
		ccp = &cc->cc_next;
	}
	return head;
}


/* The following routine is only used by the Compact Assembly generation phase,
 * which does not read basic blocks.
 */

line_p get_ca_lines(lf,p_out)
	FILE *lf;
	proc_p *p_out;
{
	/* Read lines of EM text and link them.
	 * Register messages are outputted immediately after the PRO.
	 */

	line_p head, *pp, l;
	line_p headm, *mp;
	arg_p a;

	f = lf; /* EM input file */
	pp = &head;
	mp = &headm;
	headm = (line_p) 0;
	while (TRUE) {
		l = read_line(p_out);
		if (feof(f)) break;
		assert (l != (line_p) 0);
		if (INSTR(l) == ps_end && INSTR(head) != ps_pro) {
			/* Delete end pseudo after data-unit */
			oldline(l);
			break;
		}
		if (INSTR(l) == ps_mes && l->l_a.la_arg->a_a.a_offset == ms_reg) {
			/* l is a register message */
			if (l->l_a.la_arg->a_next == (arg_p) 0) {
				/* register message without arguments */
				oldline(l);
			} else {
				*mp = l;
				mp = &l->l_next;
			}
		} else {
			*pp = l;
			pp = &l->l_next;
		}
		if (INSTR(l) == ps_end) {
			break;
		}
	}
	*pp = (line_p) 0;
	if (INSTR(head) == ps_pro) {
		/* append register message without arguments to list */
		l = newline(OPLIST);
		l->l_instr = ps_mes;
		a = ARG(l) = newarg(ARGOFF);
		a->a_a.a_offset = ms_reg;
		*mp = l;
		l->l_next = head->l_next;
		head->l_next = headm;
	} else {
		assert(headm == (line_p) 0);
	}
	return head;
}
