#ifndef NORCSID
static char rcsidp5[] = "$Header$";
#endif

#include "parser.h"
#include "Lpars.h"

FILE *ofile;

outputnopt()
{
	if((ofile=fopen("dfa.c","w"))==NULL) {
		fprintf(stderr,"Couldn't open dfa.c for output\n");
		sys_stop(S_EXIT);
	}
	outheaders();
	outtables();
	outdfa();
	outdodefault();
	outdotrans();
	outputincalls();
}

PRIVATE
outheaders()
{
	fprintf(ofile,"#include \"nopt.h\"\n");
	fprintf(ofile,"\n");
	fprintf(ofile,"int OO_maxpattern = %d;\n", longestpattern);
	fprintf(ofile,"\n");
}

PRIVATE
outtables()
{
	int s;
	fprintf(ofile,"static struct defact {\n");
	fprintf(ofile,"\tint numoutput;\n");
	fprintf(ofile,"\tint numcopy;\n");
	fprintf(ofile,"\tint nextstate;\n");
	fprintf(ofile,"} defaultactions[] = {\n");
	for(s=0;s<=higheststate;s++) {
		findfail(s);
		if(s%4==3)
			fprintf(ofile,"\n");
	}
	fprintf(ofile,"};\n");
	fprintf(ofile,"\n");
}

PRIVATE
outdfa()
{
	int s;
	struct idf *op;
	struct state *p;
	fprintf(ofile,"int OO_state = 0;\n");
	fprintf(ofile,"\n");
	fprintf(ofile,"OO_dfa(last) register int last; {\n");
	fprintf(ofile,"\tfor(;;) {\n");
	fprintf(ofile,"\t\tswitch(last) {\n");
	for(op=ops;op!=(struct idf *)NULL;op=op->id_nextidf) {
		if(!op->id_used)
			continue;
		fprintf(ofile,"\t\tcase op_%s:\n",op->id_text);
		fprintf(ofile,"\t\t\tswitch(OO_state) {\n");
		if(!op->id_startpatt) {
				fprintf(ofile,"\t\t\tcase 0: ");
				fprintf(ofile,"OO_out(*--OO_nxtpatt); ");
				fprintf(ofile,"break;\n");
		}
		for(s=0;s<=higheststate;s++)
			for(p=states[s];p!=(struct state *)NULL;p=p->next) {
				if(p->op==op) {
					fprintf(ofile,"\t\t\tcase %d: ",s);
					if(actions[p->goto_state]==(struct action *)NULL)
						fprintf(ofile,"OO_state = %d; ",p->goto_state);
					else fprintf(ofile,"OO_dotrans(%d); ",p->goto_state);
					fprintf(ofile,"break;\n");
				}
			}
		fprintf(ofile,"\t\t\tdefault: dodefaultaction(); break;\n");
		fprintf(ofile,"\t\t\t}\n");
		fprintf(ofile,"\t\t\tbreak;\n");
	}
	fprintf(ofile,"\t\tdefault:\n");
	fprintf(ofile,"\t\t\tif(OO_state) dodefaultaction();\n");
	fprintf(ofile,"\t\t\telse {\n");
	fprintf(ofile,"\t\t\t\tOO_flush();\n");
	fprintf(ofile,"\t\t\t\tEM_mkcalls(*--OO_nxtpatt);\n");
	fprintf(ofile,"\t\t\t\tOO_free(*OO_nxtpatt);\n");
	fprintf(ofile,"\t\t\t}\n");
	fprintf(ofile,"\t\t\tbreak;\n");
	fprintf(ofile,"\t\tcase OTHER:\n");
	fprintf(ofile,"\t\t\tif(OO_state) dodefaultaction();\n");
	fprintf(ofile,"\t\t\telse {\n");
	fprintf(ofile,"\t\t\t\tOO_flush();\n");
	fprintf(ofile,"\t\t\t\t--OO_nxtpatt;\n");
	fprintf(ofile,"\t\t\t}\n");
	fprintf(ofile,"\t\t\tbreak;\n");
	fprintf(ofile,"\t\t}\n");
	fprintf(ofile,"\t\tif (OO_nxtbackup==OO_bkupqueue)\n");
	fprintf(ofile,"\t\t\treturn;\n");
	fprintf(ofile,"\t\tlast = ((*OO_nxtpatt++ = *(--OO_nxtbackup))->em_opcode);\n");
	fprintf(ofile,"\t}\n");
	fprintf(ofile,"}\n");
}

PRIVATE
outmnems(l)
	struct mnems l;
{
	int i;
	for(i=1;i<=l.m_len;i++) 
		fprintf(ofile,"%s ",l.m_elems[i-1]->op_code->id_text);
}

PRIVATE
outdotrans()
{
	int s;
	int i;
	struct state *p;
	struct action *a;
	int seennontested;
	if((ofile=fopen("trans.c","w"))==NULL) {
		fprintf(stderr,"Fatal Error: cannot open output file trans.c\n");
		sys_stop(S_EXIT);
	}
	fprintf(ofile,"#include \"nopt.h\"\n\n");
	fprintf(ofile,"\nOO_dotrans(s) int s; {\n");
	fprintf(ofile,"\tregister p_instr *patt = OO_patternqueue;\n");
	fprintf(ofile,"\tswitch(OO_state=s) {\n");
	fprintf(ofile,"\tdefault: return;\n");
	for(s=0;s<=higheststate;s++)
		if(actions[s]!=(struct action *)NULL) {
			fprintf(ofile,"\tcase %d: /*",s);
			outmnems(patterns[s]);
			fprintf(ofile," */\n");
			seennontested=0;
			for(a=actions[s];a!=(struct action *)NULL;a=a->next) {
				if(a->test!=(struct exp_node *)NULL) {
					fprintf(ofile,"\t\tif(");
					outexp(a->test,s);
					fprintf(ofile,") {\n");
					outoneaction(s,a);
					fprintf(ofile,"\t\t\tgoto free%d;\n",patterns[s].m_len);
					fprintf(ofile,"\t\t}\n");
				}
				else {
					if(seennontested) {
						fprintf(stderr,"parser: more than one untested action on state %d\n",s);
						nerrors++;
					}
					seennontested++;
					outoneaction(s,a);
					fprintf(ofile,"\t\t\tgoto free%d;\n",patterns[s].m_len);
				}
			}
			if(!seennontested)
				fprintf(ofile,"\t\treturn;\n");
		}
	fprintf(ofile,"\t}\n");
	for(i=longestpattern;i>0;i--)
		fprintf(ofile," free%d: OO_free(*--OO_nxtpatt);\n",i);
	fprintf(ofile," free0: ;\n");
	fprintf(ofile,"\tOO_state=0;\n");
	fprintf(ofile,"}\n");
	fprintf(ofile,"\n");
}

PRIVATE
outdodefault()
{
	fprintf(ofile,"\nstatic dodefaultaction() {\n");
	fprintf(ofile,"\tregister struct defact *p = &defaultactions[OO_state];\n");
	fprintf(ofile,"\tOO_pushback(*--OO_nxtpatt);\n");
	fprintf(ofile,"\tOO_dodefault(p->numoutput,p->numcopy);\n");
	fprintf(ofile,"\tOO_dotrans(p->nextstate);\n");
	fprintf(ofile,"}\n");
}

PRIVATE
outoneaction(s,a)
	int s;
	struct action *a;
{
	fprintf(ofile,"\t\t/* ");
	fprintf(ofile," -> ");
	outmnems(a->replacement);
	fprintf(ofile," */\n");
	fprintf(ofile,"#ifdef STATS\n");
	fprintf(ofile,"\t\t\tif(OO_wrstats) fprintf(stderr,\"%d\\n\");\n",a->linenum);
	fprintf(ofile,"#endif\n");
	outrepl(s,patterns[s],a->replacement);
	findworst(a->replacement);
}

PRIVATE
outrepl(state,patt,repl)
	int state;
	struct mnems patt,repl;
{
	/*
	/* Contruct <repl>=r1 r2 ... rn and put on output queue.
	*/
	int n = repl.m_len;
	int m = patt.m_len;
	int i,j,count;
	for(i=1;i<=n;i++) {
		struct mnem_elem *ri = repl.m_elems[i-1];
		char *mnem = ri->op_code->id_text;
		switch(ri->op_code->id_argfmt) {
		case NOARG:
			fprintf(ofile,"\t\t\tOO_outop(op_%s);\n",mnem);
			break;
		case CST:
		case CSTOPT:
			fprintf(ofile,"\t\t\tOO_outcst(op_%s,",mnem);
			outexp(ri->arg,state);
			fprintf(ofile,");\n");
			break;
		case LAB:
			fprintf(ofile,"\t\t\tOO_outlab(op_%s,",mnem);
			outexp(ri->arg,state);
			fprintf(ofile,");\n");
			break;
		case DEFILB:
			fprintf(ofile,"\t\t\tOO_outdefilb(op_%s,",mnem);
			outexp(ri->arg,state);
			fprintf(ofile,");\n");
			break;
		case PNAM:
			fprintf(ofile,"\t\t\tOO_outpnam(op_%s,",mnem);
			outexp(ri->arg,state);
			fprintf(ofile,");\n");
			break;
		case EXT:
			fprintf(ofile,"\t\t\tOO_outext(op_%s,",mnem);
			outexp(ri->arg,state);
			fprintf(ofile,");\n");
			break;
		}
	}
}

PRIVATE
outexp(e,state)
	struct exp_node *e;
	int state;
{
	switch(e->node_type) {
	case LOGAND:
	case LOGOR:
	case BITAND:
	case BITOR:
	case XOR:
	case MINUS:
	case PLUS:
	case TIMES:
	case DIV:
	case MOD:
	case EQ:
	case NE:
	case LT:
	case LE:
	case GT:
	case GE:
	case LSHIFT:
	case RSHIFT:
		fprintf(ofile,"(");
		outexp(e->exp_left,state);
		outop(e->node_type);
		outexp(e->exp_right,state);
		fprintf(ofile,")");
		break;
	case NOT:
	case COMP:
	case UPLUS:
	case UMINUS:
		fprintf(ofile,"(");
		outop(e->node_type);
		outexp(e->exp_left,state);
		fprintf(ofile,")");
		break;
	case DEFINED:
		fprintf(ofile,"(patt[%d]->em_argtype)",e->leaf_val-1);
		break;
	case UNDEFINED:
		fprintf(ofile,"(patt[%d]->em_argtype==0)",e->leaf_val-1);
		break;
	case COMMA:
		outext(e->exp_left);
		fprintf(ofile,","); outexp(e->exp_right,state);
		break;
	case SAMESIGN:
	case SFIT:
	case UFIT:
	case ROTATE:
		outop(e->node_type);
		outexp(e->exp_left,state);
		fprintf(ofile,",");
		outexp(e->exp_right,state);
		fprintf(ofile,")");
		break;
	case SAMEEXT:
	case SAMENAM:
		outop(e->node_type);
		outext(e->exp_left);
		fprintf(ofile,",");
		outext(e->exp_right,state);
		fprintf(ofile,")");
		break;
	case PATARG:
		switch(patterns[state].m_elems[e->leaf_val-1]->op_code->id_argfmt) {
		case NOARG:
			fprintf(stderr,"error: mnem %d has no argument\n",e->leaf_val);
			nerrors++;
			break;
		case CST:
		case CSTOPT:
			fprintf(ofile,"CST(patt[%d])",e->leaf_val-1);
			break;
		case LAB:
			fprintf(ofile,"LAB(patt[%d])",e->leaf_val-1);
			break;
		case DEFILB:
			fprintf(ofile,"DEFILB(patt[%d])",e->leaf_val-1);
			break;
		case PNAM:
			fprintf(ofile,"PNAM(patt[%d])",e->leaf_val-1);
			break;
		case EXT:
			fprintf(ofile,"OO_offset(patt[%d])",e->leaf_val-1);
			break;
		}
		break;
	case PSIZE:
		fprintf(ofile,"OO_PSIZE"); break;
	case WSIZE:
		fprintf(ofile,"OO_WSIZE"); break;
	case INT:
		fprintf(ofile,"%d",e->leaf_val); break;
	}
}

PRIVATE
outext(e)
	struct exp_node *e;
{
	if(e->node_type!=PATARG) {
		fprintf(stderr,"Internal error in outext of parser\n");
		nerrors++;
	}
	fprintf(ofile,"patt[%d]",e->leaf_val-1);
}

PRIVATE
outop(op)
	int op;
{
	switch(op) {
	case LOGAND:	fprintf(ofile,"&&");	break;
	case LOGOR:	fprintf(ofile,"||");	break;
	case BITAND:	fprintf(ofile,"&");	break;
	case BITOR:	fprintf(ofile,"|");	break;
	case XOR:	fprintf(ofile,"^");	break;
	case MINUS:	fprintf(ofile,"-");	break;
	case PLUS:	fprintf(ofile,"+");	break;
	case TIMES:	fprintf(ofile,"*");	break;
	case DIV:	fprintf(ofile,"/");	break;
	case MOD:	fprintf(ofile,"%%");	break;
	case EQ:	fprintf(ofile,"==");	break;
	case NE:	fprintf(ofile,"!=");	break;
	case LT:	fprintf(ofile,"<");	break;
	case LE:	fprintf(ofile,"<=");	break;
	case GT:	fprintf(ofile,">");	break;
	case GE:	fprintf(ofile,">=");	break;
	case LSHIFT:	fprintf(ofile,"<<");	break;
	case RSHIFT:	fprintf(ofile,">>");	break;
	case NOT:	fprintf(ofile,"!");	break;
	case COMP:	fprintf(ofile,"~");	break;
	case UPLUS:	fprintf(ofile,"+");	break;
	case UMINUS:	fprintf(ofile,"-");	break;
	case SAMESIGN:	fprintf(ofile,"OO_signsame(");	break;
	case SFIT:	fprintf(ofile,"OO_sfit(");	break;
	case UFIT:	fprintf(ofile,"OO_ufit(");	break;
	case ROTATE:	fprintf(ofile,"OO_rotate(");	break;
	case SAMEEXT:	fprintf(ofile,"OO_extsame(");	break;
	case SAMENAM:	fprintf(ofile,"OO_namsame(");	break;
	}
}
