#ifndef NORCSID
static char rcsid[] = "$Id$";
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <em_spec.h>
#include <em_flag.h>
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cgg_cg.h>
#include "data.h"
#include "result.h"
#include "extern.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

#ifndef NDEBUG
#include <stdio.h>
extern char em_mnem[][4];
#endif

static int argtyp(int);

static byte *trypat(byte *bp, int len) {
	int patlen,i;
	result_t result;

	getint(patlen,bp);
	if (len == 3) {
		if (patlen < 3)
			return(0);
	} else {
		if (patlen != len)
			return(0);
	}
	for (i=0;i<patlen;i++)
		if (emp[i].em_instr != (*bp++&BMASK))
			return(0);
	for (i=0;i<patlen;i++)
		if (emp[i].em_optyp==OPNO)
			dollar[i].e_typ=EV_UNDEF;
		else if ((dollar[i].e_typ=argtyp(emp[i].em_instr))==EV_INT)
			dollar[i].e_v.e_con=emp[i].em_u.em_ioper;
		else {
			dollar[i].e_v.e_addr.ea_str=emp[i].em_soper;
			dollar[i].e_v.e_addr.ea_off=0;
		}
	getint(i,bp);
	if (i!=0) {
		struct emline *svp = saveemp;

		saveemp = emp;
		compute(&enodes[i], &result);
		if (result.e_typ != EV_INT || result.e_v.e_con == 0) {
			saveemp = svp;
			return(0);
		}
	}
#ifndef NDEBUG
	if (Debug) {
		fprintf(stderr,"Matched:");
		for (i=0;i<patlen;i++) {
#ifdef USE_TES
			if (emp[i].em_instr == op_lab)
				fprintf(stderr," lab");
			else
#endif
				fprintf(stderr," %3.3s",em_mnem[emp[i].em_instr-sp_fmnem]);
			if (emp[i].em_soper)
				fprintf(stderr," %s",emp[i].em_soper);
		}
		fprintf(stderr,"\n");
	}
#endif
	saveemp = emp;
	emp += patlen;
	return(bp);
}

extern char em_flag[];

static int argtyp(int mn) {

	/* op_lab is a special opcode which represents a label definition. It's
	 * not actually a real EM instruction. Therefore if we try to look it
	 * up in em_flag, we'll get a buffer overrun... */

	if (mn == op_lab)
		return EV_UNDEF;

	switch (em_flag[mn-sp_fmnem]&EM_PAR) {
	case PAR_W:
	case PAR_S:
	case PAR_Z:
	case PAR_O:
	case PAR_N:
	case PAR_L:
	case PAR_F:
	case PAR_R:
	case PAR_C:
		return(EV_INT);
	default:
		return(EV_ADDR);
	}
}

byte *nextem(int toplevel) {
	int i;
	short hash[3];
	byte *bp;
	byte *cp;
	int index;
	struct emline *ep;

	if (toplevel) {
		if (nemlines && emp>emlines) {
			nemlines -= emp-emlines;
			for (i=0,ep=emlines;i<nemlines;i++)
				*ep++ = *emp++;
			emp=emlines;
		}
		fillemlines();
	}
	hash[0] = emp[0].em_instr;
	hash[1] = (hash[0]<<4) ^ emp[1].em_instr;
	hash[2] = (hash[1]<<4) ^ emp[2].em_instr;
	for (i=2;i>=0;i--) {
		index = pathash[hash[i]&BMASK];
		while (index != 0) {
			bp = &pattern[index];
			if ( bp[PO_HASH] == (hash[i]>>8))
				if ((cp=trypat(&bp[PO_MATCH],i+1)) != 0)
					return(cp);
			index = (bp[PO_NEXT]&BMASK) | (bp[PO_NEXT+1]<<8);
		}
	}
	return(0);
}
