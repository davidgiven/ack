/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdio.h>
#include <em_spec.h>
#include <em_pseu.h>
#include <em_mnem.h>
#include <em_flag.h>
#include <em_mes.h>
#include "../share/types.h"
#include "ca.h"
#include "../share/debug.h"
#include "../share/def.h"
#include "../share/map.h"
#include "../share/alloc.h"

#define outbyte(b) putc(b,outfile)

FILE *outfile;

STATIC proc_p thispro;

STATIC outinst(m) {

	outbyte( (byte) m );
}

STATIC coutshort(i) short i; {

	outbyte( (byte) (i&BMASK) );
	outbyte( (byte) (i>>8) );
}

STATIC coutint(i) short i; {

	if (i>= -sp_zcst0 && i< sp_ncst0-sp_zcst0)
		outbyte( (byte) (i+sp_zcst0+sp_fcst0) );
	else {
		outbyte( (byte) sp_cst2) ;
		coutshort(i);
	}
}

STATIC coutoff(off) offset off; {

	if ((short) off == off)
		coutint((short) off);
	else {
		outbyte( (byte) sp_cst4) ;
		coutshort( (short) (off&0177777L) );
		coutshort( (short) (off>>16) );
	}
}


STATIC outsym(s,t)
	char *s;
	int t;
{
	register byte *p;
	register unsigned num;

	if (s[0] == '.') {
		num = atoi(&s[1]);
		if (num < 256) {
			outbyte( (byte) sp_dlb1) ;
			outbyte( (byte) (num) );
		} else {
			outbyte( (byte) sp_dlb2) ;
			coutshort((short) num);
		}
	} else {
		p= s;
		while (*p && p < &s[IDL])
			p++;
		num = p - s;
		outbyte( (byte) t);
		coutint((short) num);
		p = s;
		while (num--)
			outbyte( (byte) *p++ );
	}
}


STATIC outdsym(dbl)
	dblock_p dbl;
{
	if (dnames[dbl->d_id]) outsym(dnames[dbl->d_id],sp_dnam);
}


STATIC outpsym(p)
	proc_p p;
{
	outsym(pnames[p->p_id],sp_pnam);
}


STATIC outddef(id) short id; {

	dblock_p dbl;

	dbl = dmap[id];
	dbl->d_flags2 |= DF_SYMOUT;
	if (dbl->d_flags1 & DF_EXTERNAL) {
		outinst(ps_exa);
		outdsym(dbl);
	}
}

STATIC outpdef(p) proc_p p; {
	p->p_flags2 |= PF_SYMOUT;
	if (p->p_flags1 & PF_EXTERNAL) {
		outinst(ps_exp);
		outpsym(p);
	}
}


STATIC outdocc(obj) obj_p obj; {
	dblock_p dbl;

	dbl = obj->o_dblock;
	if ((dbl->d_flags2 & DF_SYMOUT) == 0) {
		dbl->d_flags2 |= DF_SYMOUT;
		if (dnames[dbl->d_id] != 0 && 
		    (dbl->d_flags1 & DF_EXTERNAL) == 0) {
			outinst(ps_ina);
			outdsym(dbl);
		}
	}
}


STATIC outpocc(p) proc_p p; {
	if ((p->p_flags2 & PF_SYMOUT) == 0) {
		p->p_flags2 |= PF_SYMOUT;
		if ((p->p_flags1 & PF_EXTERNAL) == 0) {
			outinst(ps_inp);
			outpsym(p);
		}
	}
}


STATIC coutobject(obj)
	obj_p obj;
{
	/* In general, an object is defined by a global data
	 * label and an offset. There are two special cases:
	 * the label is omitted if the object is part of the current
	 * hol block; the offset is omitted if it is 0 and the label
	 * was not omitted.
	 */
	if (dnames[obj->o_dblock->d_id] == 0) {
		coutoff(obj->o_off);
	} else {
		if (obj->o_off == 0) {
			outdsym(obj->o_dblock);
		} else {
			outbyte((byte) sp_doff);
			outdsym(obj->o_dblock);
			coutoff(obj->o_off);
		}
	}
}


STATIC cputstr(abp) register argb_p abp; {
	register argb_p tbp;
	register length;

	length = 0;
	tbp = abp;
	while (tbp!= (argb_p) 0) {
		length += tbp->ab_index;
		tbp = tbp->ab_next;
	}
	coutint(length);
	while (abp != (argb_p) 0) {
		for (length=0;length<abp->ab_index;length++)
			outbyte( (byte) abp->ab_contents[length] );
		abp = abp->ab_next;
	}
}


STATIC outnum(n)
	int n;
{
	if (n < 256) {
		outbyte((byte) sp_ilb1);
		outbyte((byte) n);
	} else {
		outbyte((byte) sp_ilb2);
		coutshort((short) n);
	}
}


STATIC numlab(n)
	int n;
{
	if (n < sp_nilb0) {
		outbyte((byte) (n + sp_filb0));
	} else {
		outnum(n);
	}
}


STATIC cputargs(lnp)
	line_p lnp;
{
	register arg_p ap;
	int cnt = 0;
	ap = ARG(lnp);
	while (ap != (arg_p) 0) {
		switch(ap->a_type) {
			case ARGOFF:
				coutoff(ap->a_a.a_offset);
				break;
			case ARGOBJECT:
				coutobject(ap->a_a.a_obj);
				break;
			case ARGPROC:
				outpsym(ap->a_a.a_proc);
				break;
			case ARGINSTRLAB:
				outnum(ap->a_a.a_instrlab);
				break;
			case ARGSTRING:
				outbyte((byte) sp_scon);
				cputstr(&ap->a_a.a_string);
				break;
			case ARGICN:
				outbyte((byte) sp_icon);
				goto casecon;
			case ARGUCN:
				outbyte((byte) sp_ucon);
				goto casecon;
			case ARGFCN:
				outbyte((byte) sp_fcon);
			casecon:
				coutint(ap->a_a.a_con.ac_length);
				cputstr(&ap->a_a.a_con.ac_con);
				break;
			default:
				assert(FALSE);
		}
		ap = ap->a_next;
		/* Avoid generating extremely long CON or ROM statements */
		if (cnt++ > 10 && ap != (arg_p) 0 && 
		    (INSTR(lnp) == ps_con || INSTR(lnp) == ps_rom)) {
			cnt = 0;
			outbyte((byte) sp_cend);
			outinst(INSTR(lnp));
		}
	}
}



STATIC outoperand(lnp)
	line_p lnp;
{
	/* Output the operand of instruction lnp */

	switch(TYPE(lnp)) {
		case OPNO:
			if (INSTR(lnp) <= sp_lmnem &&
			    (em_flag[INSTR(lnp)-sp_fmnem]&EM_PAR) != PAR_NO) {
				outbyte((byte) sp_cend);
			}
			break;
		case OPSHORT:
			if (INSTR(lnp) == ps_sym) {
				outsym(dnames[SHORT(lnp)],sp_dnam);
			} else {
				coutint(SHORT(lnp));
			}
			break;
		case OPOFFSET:
			coutoff(OFFSET(lnp));
			break;
		case OPINSTRLAB:
			if (INSTR(lnp) == op_lab) {
				numlab(INSTRLAB(lnp));
			} else {
				if (INSTR(lnp) < sp_fpseu) {
					coutint(INSTRLAB(lnp));
				} else {
					numlab(INSTRLAB(lnp));
				}
			}
			break;
		case OPOBJECT:
			coutobject(OBJ(lnp));
			break;
		case OPPROC:
			outpsym(PROC(lnp));
			break;
		case OPLIST:
			cputargs(lnp);
			switch(INSTR(lnp)) {
				case ps_con:
				case ps_rom:
				case ps_mes:
					outbyte((byte) sp_cend);
					/* list terminator */
					break;
			}
			break;
		default:
			assert(FALSE);
	}
}


STATIC outvisibility(lnp)
	line_p lnp;
{
	/* In EM names of datalabels and procedures can be made
	 * externally visible, so they can be used in other files.
	 * There are special EM pseudo-instructions to state
	 * explicitly that a certain identifier is externally
	 * visible (ps_exa,ps_exp) or invisible (ps_ina,ps_inp).
	 * If there is no such pseudo for a certain identifier,
	 * the identifier is external only if its first use
	 * in the current file is an applied occurrence.
	 * Unfortunately the global optimizer may change the
	 * order of defining and applied occurrences.
	 * In the first optimizer pass (ic) we record for each identifier
	 * whether it is external or not. If necessary we generate
	 * pseudo instructions here.
	 */

	 arg_p ap;
	 short instr;

	 instr = INSTR(lnp);
	 switch(TYPE(lnp)) {
		case OPOBJECT:
			outdocc(OBJ(lnp));
			/* applied occurrence of a data label */
			break;
		case OPSHORT:
			if (instr == ps_sym) {
				outddef(SHORT(lnp));
				/* defining occ. data label */
			}
			break;
		case OPPROC:
			if (instr == ps_pro) {
				outpdef(PROC(lnp));
				/* defining occ. procedure */
			} else {
				outpocc(PROC(lnp));
			}
			break;
		case OPLIST:
			for (ap =  ARG(lnp); ap != (arg_p) 0; ap = ap->a_next) {
				switch(ap->a_type) {
					case ARGOBJECT:
					   outdocc(ap->a_a.a_obj);
					   break;
					case ARGPROC:
					   outpocc(ap->a_a.a_proc);
					   break;
				}
			}
			break;
	}
}


cputlines(l,lf)
	line_p l;
	FILE *lf;
{
	/* Output the lines in Campact assembly language
	 * format.
	 */

	line_p next,lnp;

	outfile = lf;
	for (lnp = l; lnp != (line_p) 0; lnp = next) {
		next = lnp->l_next;
		outvisibility(lnp); /* take care of visibiltity rules */
		if (INSTR(lnp) != ps_sym && INSTR(lnp) != op_lab) {
			outinst(INSTR(lnp));
		}
		outoperand(lnp);
		switch(INSTR(lnp)) {
			case ps_pro:
				thispro = PROC(lnp);
				/* fall through ... */
			case ps_end:
				coutoff(thispro->p_localbytes);
		}
		oldline(lnp);
	}
	if (lmap != (line_p *) 0) {
		oldmap(lmap,llength);
		lmap = (line_p *) 0;
	}
}

cputmagic(lf)
	FILE *lf;
{
	/* write the magic number */

	outfile = lf;
	coutshort(sp_magic);
}
