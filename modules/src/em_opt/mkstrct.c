#ifndef NORCSID
static char rcsid3[] = "$Header$";
#endif

#include "nopt.h"

OO_inop(opcode)
	int opcode;
{
	register p_instr p = GETINSTR();
	p->em_type = EM_MNEM;
	p->em_opcode = opcode;
	p->em_argtype = 0;
	*OO_nxtpatt++ = p;
}

OO_incst(opcode,cst)
	int opcode,cst;
{
	register p_instr p = GETINSTR();
	p->em_type = EM_MNEM;
	p->em_opcode = opcode;
	p->em_argtype = cst_ptyp;
	p->em_cst = cst;
	*OO_nxtpatt++ = p;
}

OO_inlab(opcode,lab)
	int opcode,lab;
{
	register p_instr p = GETINSTR();
	p->em_type = EM_MNEM;
	p->em_opcode = opcode;
	p->em_argtype = ilb_ptyp;
	p->em_ilb = lab;
	*OO_nxtpatt++ = p;
}

OO_inpnam(opcode,pnam)
	int opcode;
	char *pnam;
{
	register p_instr p = GETINSTR();
	p->em_type = EM_MNEM;
	p->em_opcode = opcode;
	p->em_argtype = pro_ptyp;
	p->em_pnam = OO_freestr(pnam);
	*OO_nxtpatt++ = p;
}

OO_indefilb(opcode,deflb)
	int opcode;
	label deflb;
{
	register p_instr p = GETINSTR();
	p->em_type = EM_DEFILB;
	p->em_opcode = opcode;
	p->em_argtype = 0;
	p->em_ilb = deflb;
	*OO_nxtpatt++ = p;
}

OO_indnam(opcode,name,off)
	int opcode;
	char *name;
	int off;
{
	register p_instr p = GETINSTR();
	p->em_type = EM_MNEM;
	p->em_opcode = opcode;
	p->em_argtype = sof_ptyp;
        p->em_dnam = OO_freestr(name);
	p->em_off = off;
	*OO_nxtpatt++ = p;
}

OO_indlb(opcode,lab,off)
	int opcode;
	label lab;
	int off;
{
	register p_instr p = GETINSTR();
	p->em_type = EM_MNEM;
	p->em_opcode = opcode;
	p->em_argtype = nof_ptyp;
	p->em_dlb = lab;
	p->em_off = off;
	*OO_nxtpatt++ = p;
}

