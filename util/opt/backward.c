#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "param.h"
#include "types.h"
#include "tes.h"
#include "assert.h"
#include "line.h"
#include "lookup.h"
#include "alloc.h"
#include "proinf.h"
#include <em_spec.h>
#include <em_pseu.h>
#include <em_mnem.h>
#include <em_mes.h>
#include "ext.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

#define local(x)	((((x)->s_flags&SYMKNOWN) == 0 && \
			  ((x)->s_flags &= ~ SYMGLOBAL)),\
			 (x)->s_flags |= SYMSEEN)
#define global(x)	((((x)->s_flags&SYMKNOWN) == 0 && \
			  ((x)->s_flags |= SYMGLOBAL)), \
			 (x)->s_flags |= SYMSEEN)

#define DTYPHOL	1
#define DTYPBSS 2
#define DTYPCON 3
#define DTYPROM 4
byte	curdtyp;
bool	goodrom;
short	curfrag = 3;	/* see also peephole.c */
offset rombuf[MAXROM];
int	rc;

extern offset aoff();

backward() {
	register line_p lnp;
	line_p	next;
	register arg_p ap;
	line_p i,p;
	int n;
	register sym_p sp;

	i = p = (line_p) 0;
	curdtyp=0;
	for (lnp = curpro.lastline; lnp != (line_p) 0; lnp = next) {
		next = lnp->l_next;
		switch(lnp->l_optyp) {
		case OPSYMBOL:
			global(lnp->l_a.la_sp);
			break;
		case OPSVAL:
			global(lnp->l_a.la_sval.lasv_sp);
			break;
		case OPLVAL:
			global(lnp->l_a.la_lval.lalv_sp);
			break;
		case OPLIST:
			ap = lnp->l_a.la_arg;
			while (ap != (arg_p) 0 ) {
				switch(ap->a_typ) {
				case ARGSYM:
					global(ap->a_a.a_sp);
					break;
				case ARGVAL:
					global(ap->a_a.a_val.av_sp);
				}
				ap = ap->a_next;
			}
			break;
		}

		/*
		 * references to symbols are processed now.
		 * for plain instructions nothing else is needed
		 */

		switch(lnp->l_instr&BMASK) {
		/*
		 * count all local occurences for register counts;
		 * op_lal is omitted and not by accident.
		 */
		case op_del:
		case op_inl:
		case op_ldl:
		case op_lil:
		case op_lol:
		case op_sdl:
		case op_sil:
		case op_stl:
		case op_zrl:
			switch(lnp->l_optyp) {
			case OPNO:
			case OPNUMLAB:
			case OPSYMBOL:
			case OPSVAL:
			case OPLVAL:
			case OPLIST:
				break;
			case OPOFFSET:
				incregusage(lnp->l_a.la_offset);
				break;
			case OPSHORT:
				incregusage((offset)lnp->l_a.la_short);
				break;
			default:
				incregusage((offset)(lnp->l_optyp&BMASK)-Z_OPMINI);
				break;
			}
			/* fall through !! */
		default:
			assert((lnp->l_instr&BMASK)<=op_last);
			lnp->l_next = i;
			i = lnp;
			continue;
		case ps_sym:
			sp = lnp->l_a.la_sp;
			local(sp);
			if (curdtyp == DTYPROM && goodrom) {
				sp->s_rom = newrom();
				for (n=0;n<rc;n++)
					sp->s_rom[n] = rombuf[n];
			}
			sp->s_frag = curfrag;
			break;
		case ps_hol:
			curdtyp = DTYPHOL;
			curfrag++;
			break;
		case ps_bss:
			curdtyp = DTYPBSS;
			curfrag++;
			break;
		case ps_con:
			if (curdtyp != DTYPCON) {
				curdtyp = DTYPCON;
				curfrag++;
			}
			break;
		case ps_rom:
			if (curdtyp != DTYPROM) {
				curdtyp = DTYPROM;
				curfrag++;
			}
			ap = lnp->l_a.la_arg;
			rc = 0;
			while (ap != (arg_p) 0 && rc < MAXROM) {
				if (ap->a_typ == ARGOFF) {
					rombuf[rc++] = ap->a_a.a_offset;
					ap = ap->a_next;
				} else
					ap = (arg_p) 0;
			}
			goodrom = (rc >= 2);
			break;
		case ps_mes:
			if (prodepth != 0 &&
			    ((int) aoff(lnp->l_a.la_arg, 0) == ms_std ||
			     (int) aoff(lnp->l_a.la_arg, 0) == ms_stb ||
			     (int) aoff(lnp->l_a.la_arg, 0) == ms_ego)) {
				lnp->l_next = i;
				i = lnp;
				continue;
			}
			break;
		case ps_inp:
		case ps_ina:
			local(lnp->l_a.la_sp);
		case ps_exp:
		case ps_exa:
		case ps_exc:
			oldline(lnp);
			continue;
		}
		lnp->l_next = p;
		p = lnp;
	}
	if (prodepth != 0)
		local(curpro.symbol);
	instrs = i; pseudos = p; curpro.lastline = (line_p) 0;
}
