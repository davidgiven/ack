#ifndef NORCSID
static char rcsidp2[] = "$Header$";
#endif

#include <em_mnem.h>
#include <em_flag.h>
#include <em_spec.h>
#include "parser.h"
#define op_lab sp_fpseu	/* if you change this change nopt.h also */

#include <idf_pkg.body>

extern char em_flag[];
extern char em_mnem[][4];

PRIVATE idinit();

initlex()
{
	register int i,j;
	init_idf();
	idinit("lab",op_lab,DEFILB);
	for(i=sp_fmnem;i<=sp_lmnem;i++) {
		j=i-sp_fmnem;
		switch(em_flag[j] & EM_PAR) {
		case PAR_NO:
			idinit(em_mnem[j],i,NOARG); break;
		case PAR_C:
		case PAR_D:
		case PAR_F:
		case PAR_L:
		case PAR_N:
		case PAR_O:
		case PAR_R:
		case PAR_S:
		case PAR_Z:
			idinit(em_mnem[j],i,CST); break;
		case PAR_W:
			idinit(em_mnem[j],i,CSTOPT); break;
		case PAR_P:
			idinit(em_mnem[j],i,PNAM); break;
		case PAR_B:
			idinit(em_mnem[j],i,LAB); break;
		case PAR_G:
			idinit(em_mnem[j],i,EXT); break;
		}
	}
}

PRIVATE
idinit(tag,opcode,argfmt)
	char *tag;
	int opcode;
	int argfmt;
{
	struct idf *p;
	p = str2idf(tag,0);
	p->id_nextidf = ops;		/* chain into all ops */
	ops = p;
	p->id_used = 0;
	p->id_startpatt = 0;
	p->id_opcode  = opcode;
	p->id_argfmt = argfmt;
}
