#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "parser.h"
#include "Lpars.h"

File *ofile;

outputnopt()
{
	if(!sys_open("dfa.c",OP_WRITE,&ofile)) {
		fprint(STDERR,"Couldn't open dfa.c for output\n");
		sys_stop(S_EXIT);
	}
	outputheaders();
	outputtables();
	outputdfa();
	outputdodefault();
	outputdotrans();
	outputincalls();
}

PRIVATE
outputheaders()
{
	fprint(ofile,"#include \"nopt.h\"\n");
	fprint(ofile,"\n");
}

PRIVATE
outputtables()
{
	int s;
	fprint(ofile,"struct defact {\n");
	fprint(ofile,"\tint numoutput;\n");
	fprint(ofile,"\tint numcopy;\n");
	fprint(ofile,"\tint nextstate;\n");
	fprint(ofile,"} defaultactions[] = {\n");
	for(s=0;s<=higheststate;s++) {
		findfail(s);
		if(s%4==3)
			fprint(ofile,"\n");
	}
	fprint(ofile,"};\n");
	fprint(ofile,"\n");
}

PRIVATE
outputdfa()
{
	int s;
	struct idf *op;
	struct state *p;
	fprint(ofile,"int state = 0;\n");
	fprint(ofile,"\n");
	fprint(ofile,"dfa(last) int last; {\n");
	fprint(ofile,"\twhile(last) {\n");
	fprint(ofile,"\t\tswitch(last) {\n");
	for(op=ops;op!=(struct idf *)NULL;op=op->id_nextidf) {
		if(!op->id_used)
			continue;
		fprint(ofile,"\t\tcase op_%s:\n",op->id_text);
		fprint(ofile,"\t\t\tswitch(state) {\n");
		if(!op->id_startpatt) {
				fprint(ofile,"\t\t\tcase 0: ");
				fprint(ofile,"output(*--nextpatt); ");
				fprint(ofile,"break;\n");
		}
		for(s=0;s<=higheststate;s++)
			for(p=states[s];p!=(struct state *)NULL;p=p->next) {
				if(p->op==op) {
					fprint(ofile,"\t\t\tcase %d: ",s);
					if(actions[p->goto_state]==(struct action *)NULL)
						fprint(ofile,"state = %d; ",p->goto_state);
					else fprint(ofile,"dotrans(%d); ",p->goto_state);
					fprint(ofile,"break;\n");
				}
			}
		fprint(ofile,"\t\t\tdefault: defaultaction(); break;\n");
		fprint(ofile,"\t\t\t}\n");
		fprint(ofile,"\t\t\tbreak;\n");
	}
	fprint(ofile,"\t\tdefault:\n");
	fprint(ofile,"\t\t\tif(state) defaultaction();\n");
	fprint(ofile,"\t\t\telse {\n");
	fprint(ofile,"\t\t\t\tflush();\n");
	fprint(ofile,"\t\t\t\tmkcalls(*--nextpatt);\n");
	fprint(ofile,"\t\t\t\tmyfree(*nextpatt);\n");
	fprint(ofile,"\t\t\t}\n");
	fprint(ofile,"\t\t\tbreak;\n");
	fprint(ofile,"\t\tcase OTHER:\n");
	fprint(ofile,"\t\t\tif(state) defaultaction();\n");
	fprint(ofile,"\t\t\telse {\n");
	fprint(ofile,"\t\t\t\tflush();\n");
	fprint(ofile,"\t\t\t\tmyfree(*--nextpatt);\n");
	fprint(ofile,"\t\t\t}\n");
	fprint(ofile,"\t\t\tbreak;\n");
	fprint(ofile,"\t\t}\n");
	fprint(ofile,"\tlast = nextem();\n");
	fprint(ofile,"\t}\n");
	fprint(ofile,"}\n");
}

PRIVATE
outputmnems(l,state)
	struct mnems l;
	int state;
{
	int i;
	for(i=1;i<=l.m_len;i++) 
		fprint(ofile,"%s ",l.m_elems[i-1]->op_code->id_text);
}

PRIVATE
outputdotrans()
{
	int s;
	struct state *p;
	struct action *a;
	int seennontested;
	if(!sys_open("trans.c",OP_WRITE,&ofile)) {
		fprint(STDERR,"Fatal Error: cannot open output file trans.c\n");
		sys_stop(S_EXIT);
	}
	fprint(ofile,"#include \"nopt.h\"\n\n");
	fprint(ofile,"\ndotrans(s) int s; {\n");
	fprint(ofile,"\tswitch(state=s) {\n");
	fprint(ofile,"\tdefault: return;\n");
	for(s=0;s<=higheststate;s++)
		if(actions[s]!=(struct action *)NULL) {
			fprint(ofile,"\tcase %d: /*",s);
			outputmnems(patterns[s],s);
			fprint(ofile," */\n");
			seennontested=0;
			for(a=actions[s];a!=(struct action *)NULL;a=a->next) {
				if(a->test!=(struct exp_node *)NULL) {
					fprint(ofile,"\t\tif(");
					outputexp(a->test,s);
					/*
					/*fprint(ofile,"dotest(%d)",a->linenum);
					*/
					fprint(ofile,") {\n");
					/*
					/*fprint(ofile,"\t\t\tdoaction(%d);\n",a->linenum);
					*/
					outputoneaction(s,a);
					fprint(ofile,"\t\t\tnfree(%d);\n",patterns[s].m_len);
					fprint(ofile,"\t\t\tstate=0;\n");
					fprint(ofile,"\t\t\tbreak;\n");
					fprint(ofile,"\t\t}\n");
				}
				else {
					if(seennontested) {
						fprint(STDERR,"parser: more than one untested action on state %d\n",s);
						nerrors++;
					}
					seennontested++;
					/*
					/*fprint(ofile,"\t\t\tdoaction(%d);\n",a->linenum);
					*/
					outputoneaction(s,a);
					fprint(ofile,"\t\tnfree(%d);\n",patterns[s].m_len);
					fprint(ofile,"\t\tstate=0;\n");
					fprint(ofile,"\t\tbreak;\n");
				}
			}
			fprint(ofile,"\t\tbreak;\n");
		}
	fprint(ofile,"\t}\n");
	fprint(ofile,"}\n");
	fprint(ofile,"\n");
}

PRIVATE
outputdodefault()
{
	fprint(ofile,"\nstatic defaultaction() {\n");
	fprint(ofile,"\tregister struct defact *p = &defaultactions[state];\n");
	fprint(ofile,"\tpushback(*--nextpatt);\n");
	fprint(ofile,"\tdodefault(p->numoutput,p->numcopy);\n");
	fprint(ofile,"\tdotrans(p->nextstate);\n");
	fprint(ofile,"}\n");
}

PRIVATE
outputoneaction(s,a)
	int s;
	struct action *a;
{
	fprint(ofile,"\t\t/* ");
	fprint(ofile," -> ");
	outputmnems(a->replacement,s);
	fprint(ofile," */\n");
	outputrepl(s,patterns[s],a->replacement);
	findworst(s,a->replacement);
}

PRIVATE
outputrepl(state,patt,repl)
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
			fprint(ofile,"\t\toutop(op_%s);\n",mnem);
			break;
		case CST:
		case CSTOPT:
			fprint(ofile,"\t\toutcst(op_%s,",mnem);
			outputexp(ri->arg,state);
			fprint(ofile,");\n");
			break;
		case LAB:
			fprint(ofile,"\t\toutlab(op_%s,",mnem);
			outputexp(ri->arg,state);
			fprint(ofile,");\n");
			break;
		case DEFILB:
			fprint(ofile,"\t\toutdefilb(op_%s,",mnem);
			outputexp(ri->arg,state);
			fprint(ofile,");\n");
			break;
		case PNAM:
			fprint(ofile,"\t\toutpnam(op_%s,",mnem);
			outputexp(ri->arg,state);
			fprint(ofile,");\n");
			break;
		case EXT:
			fprint(ofile,"\t\toutext(op_%s,",mnem);
			outputexp(ri->arg,state);
			fprint(ofile,");\n");
			break;
		}
	}
}

PRIVATE
outputexp(e,state)
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
		fprint(ofile,"(");
		outputexp(e->exp_left,state);
		outputop(e->node_type);
		outputexp(e->exp_right,state);
		fprint(ofile,")");
		break;
	case NOT:
	case COMP:
	case UPLUS:
	case UMINUS:
		fprint(ofile,"(");
		outputop(e->node_type);
		outputexp(e->exp_left,state);
		fprint(ofile,")");
		break;
	case DEFINED:
		fprint(ofile,"(patternqueue[%d]->argtype!=none_ptyp)",e->leaf_val-1);
		break;
	case UNDEFINED:
		fprint(ofile,"(patternqueue[%d]->argtype==none_ptyp)",e->leaf_val-1);
		break;
	case COMMA:
		outext(e->exp_left);
		fprint(ofile,","); outputexp(e->exp_right,state);
		break;
	case SAMESIGN:
	case SFIT:
	case UFIT:
	case ROTATE:
		outputop(e->node_type);
		outputexp(e->exp_left,state);
		fprint(ofile,",");
		outputexp(e->exp_right,state);
		fprint(ofile,")");
		break;
	case SAMEEXT:
	case SAMENAM:
		outputop(e->node_type);
		outext(e->exp_left);
		fprint(ofile,",");
		outext(e->exp_right,state);
		fprint(ofile,")");
		break;
	case PATARG:
		switch(patterns[state].m_elems[e->leaf_val-1]->op_code->id_argfmt) {
		case NOARG:
			fprint(STDERR,"error: mnem %d has no argument\n",e->leaf_val);
			nerrors++;
			break;
		case CST:
		case CSTOPT:
			fprint(ofile,"CST(patternqueue[%d])",e->leaf_val-1);
			break;
		case LAB:
			fprint(ofile,"LAB(patternqueue[%d])",e->leaf_val-1);
			break;
		case DEFILB:
			fprint(ofile,"DEFILB(patternqueue[%d])",e->leaf_val-1);
			break;
		case PNAM:
			fprint(ofile,"PNAM(patternqueue[%d])",e->leaf_val-1);
			break;
		case EXT:
			fprint(ofile,"offset(patternqueue[%d])",e->leaf_val-1);
			break;
		}
		break;
	case PSIZE:
		fprint(ofile,"PSIZE"); break;
	case WSIZE:
		fprint(ofile,"WSIZE"); break;
	case INT:
		fprint(ofile,"%d",e->leaf_val); break;
	}
}

PRIVATE
outext(e)
	struct exp_node *e;
{
	if(e->node_type!=PATARG) {
		fprint(STDERR,"Internal error in outext of parser\n");
		nerrors++;
	}
	fprint(ofile,"patternqueue[%d]",e->leaf_val-1);
}

PRIVATE
outputop(op)
	int op;
{
	switch(op) {
	case LOGAND:	fprint(ofile,"&&");	break;
	case LOGOR:	fprint(ofile,"||");	break;
	case BITAND:	fprint(ofile,"&");	break;
	case BITOR:	fprint(ofile,"|");	break;
	case XOR:	fprint(ofile,"^");	break;
	case MINUS:	fprint(ofile,"-");	break;
	case PLUS:	fprint(ofile,"+");	break;
	case TIMES:	fprint(ofile,"*");	break;
	case DIV:	fprint(ofile,"/");	break;
	case MOD:	fprint(ofile,"%%");	break;
	case EQ:	fprint(ofile,"==");	break;
	case NE:	fprint(ofile,"!=");	break;
	case LT:	fprint(ofile,"<");	break;
	case LE:	fprint(ofile,"<=");	break;
	case GT:	fprint(ofile,">");	break;
	case GE:	fprint(ofile,">=");	break;
	case LSHIFT:	fprint(ofile,"<<");	break;
	case RSHIFT:	fprint(ofile,">>");	break;
	case NOT:	fprint(ofile,"!");	break;
	case COMP:	fprint(ofile,"~");	break;
	case UPLUS:	fprint(ofile,"+");	break;
	case UMINUS:	fprint(ofile,"-");	break;
	case SAMESIGN:	fprint(ofile,"samesign(");	break;
	case SFIT:	fprint(ofile,"sfit(");	break;
	case UFIT:	fprint(ofile,"ufit(");	break;
	case ROTATE:	fprint(ofile,"rotate(");	break;
	case SAMEEXT:	fprint(ofile,"sameext(");	break;
	case SAMENAM:	fprint(ofile,"samenam(");	break;
	}
}
