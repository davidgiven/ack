#ifndef NORCSID
static char rcsid[] = "$Id$";
#endif

#include "assert.h"
#include <stdio.h>
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cg_pattern.h>
#include "data.h"
#include "result.h"
#include "extern.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

FILE *codefile;
extern FILE *freopen();

out_init(filename) char *filename; {

#ifndef NDEBUG
	static char stderrbuff[BUFSIZ];

    if (Debug) {
	codefile = stderr;
	if (!isatty(2))
		setbuf(stderr,stderrbuff);
    } else {
#endif
	if (filename == (char *) 0)
		codefile = stdout;
	else
		if ((codefile=freopen(filename,"w",stdout))==NULL)
			error("Can't create %s",filename);
#ifndef NDEBUG
    }
#endif
}

out_finish() {

#ifndef NDEBUG
	if (Debug)
		fflush(stderr);
	else
#endif
		fclose(codefile);
}

tstoutput() {

	if (ferror(codefile))
		error("Write error on output");
}

gencode(code) register char *code; {
	register c;
	int tokno,fldno,insno,regno,subno;
	register token_p tp;

	swtxt();
	while ((c= *code++)!=0) switch(c) {
	default:
		fputc(c,codefile);
		break;
	case PR_TOK:
		tokno = *code++;
		tp = &fakestack[stackheight-tokno];
		if (tp->t_token==-1)
			fprintf(codefile,"%s",codestrings[machregs[tp->t_att[0].ar].r_repr]);
		else
			prtoken(tp);
		break;
	case PR_TOKFLD:
		tokno = *code++;
		fldno = *code++;
		tp = &fakestack[stackheight-tokno];
		assert(tp->t_token != -1);
		switch(tokens[tp->t_token].t_type[fldno-1]) {
		default:
			assert(FALSE);
		case EV_INT:
			fprintf(codefile,WRD_FMT,tp->t_att[fldno-1].aw);
			break;
		case EV_STR:
			fprintf(codefile,"%s",tp->t_att[fldno-1].as);
			break;
		case EV_REG:
			assert(tp->t_att[fldno-1].ar>0 && tp->t_att[fldno-1].ar<NREGS);
			fprintf(codefile,"%s",codestrings[machregs[tp->t_att[fldno-1].ar].r_repr]);
			break;
		}
		break;
	case PR_EMINT:
		insno = *code++;
		fprintf(codefile,WRD_FMT,dollar[insno-1].e_v.e_con);
		break;
	case PR_EMSTR:
		insno = *code++;
		fprintf(codefile,"%s",dollar[insno-1].e_v.e_str);
		break;
	case PR_ALLREG:
		regno = *code++;
		subno = (*code++)&0377;
		assert(regno>=1 && regno<=nallreg);
		regno = allreg[regno-1];
#if MAXMEMBERS!=0
		if (subno!=255) {
			assert(subno>=1 && subno<=MAXMEMBERS);
			regno = machregs[regno].r_members[subno-1];
			assert(regno!=0);
		}
#endif
		fprintf(codefile,"%s",codestrings[machregs[regno].r_repr]);
		break;
#if MAXMEMBERS!=0
	case PR_SUBREG:
		tokno = *code++;
		subno = *code++;
		tp = &fakestack[stackheight-tokno];
		assert(tp->t_token == -1);
		fprintf(codefile,"%s",codestrings[machregs[machregs[tp->t_att[0].ar].r_members[subno-1]].r_repr]);
		break;
#endif
	}
}

genexpr(nodeno) {
	result_t result;

	result= compute(&enodes[nodeno]);
	switch(result.e_typ) {
	default: assert(FALSE);
	case EV_INT:
		fprintf(codefile,WRD_FMT,result.e_v.e_con);
		break;
	case EV_REG:
		fprintf(codefile,"%s", codestrings[machregs[result.e_v.e_reg].r_repr]);
		break;
	case EV_STR:
		fprintf(codefile,"%s",result.e_v.e_str);
		break;
	}
}

gennl() {
	fputc('\n',codefile);
}

prtoken(tp) token_p tp; {
	register c;
	register char *code;
	register tkdef_p tdp;

	tdp = &tokens[tp->t_token];
	assert(tdp->t_format != -1);
	code = codestrings[tdp->t_format];
	while ((c = *code++) != 0) {
		if (c>=' ' && c<='~')
			fputc(c,codefile);
		else {
			assert(c>0 && c<=TOKENSIZE);
			switch(tdp->t_type[c-1]) {
			default:
				assert(FALSE);
			case EV_INT:
				fprintf(codefile,WRD_FMT,tp->t_att[c-1].aw);
				break;
			case EV_STR:
				fprintf(codefile,"%s",tp->t_att[c-1].as);
				break;
			case EV_REG:
				fprintf(codefile,"%s",codestrings[machregs[tp->t_att[c-1].ar].r_repr]);
				break;
			}
		}
	}
}
