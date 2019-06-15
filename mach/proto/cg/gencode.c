/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include "assert.h"
#include <stdio.h>
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cg_pattern.h>
#include "data.h"
#include "result.h"
#include "extern.h"
#include "subr.h"
#include "gencode.h"
#include "fillem.h"

FILE *codefile;


void out_init(char *filename)
{

#ifndef NDEBUG

	if (Debug)
	{
		codefile = stderr;
	}
	else
	{
#endif
		if (filename == (char *) 0)
			codefile = stdout;
		else if ((codefile = freopen(filename, "w", stdout)) == NULL)
			error("Can't create %s", filename);
#ifndef NDEBUG
	}
#endif
}

void out_finish(void) {

#ifndef NDEBUG
	if (Debug)
		fflush(stderr);
	else
#endif
		fclose(codefile);
}

void tstoutput(void) {

	if (ferror(codefile))
		error("Write error on output");
}

void gencode(register char *code)
{
	register int c;
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

void genexpr(int nodeno)
{
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

void gennl(void)
{
	fputc('\n',codefile);
}

void prtoken(token_p tp)
{
	register int c;
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
