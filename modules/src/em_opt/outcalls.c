#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "parser.h"

outputincalls()
{
	struct idf *op;
	int opcode;
	char *s;
	if(!sys_open("incalls.c",OP_WRITE,&ofile)) {
		fprint(STDERR,"Fatal Error: cannot open output file incalls.c\n");
		sys_stop(S_EXIT);
	}
	fprint(ofile,"#include \"nopt.h\"\n\n");
	for(op=ops;op!=(struct idf *)NULL;op=op->id_nextidf) {
		opcode = op->id_opcode;
		s = op->id_text;
		switch(op->id_argfmt) {
		case NOARG:
			fprint(ofile,"\nC_%s() {\n",s);
			if(op->id_used) {
				fprint(ofile,"\tinop(op_%s);\n",s);
				fprint(ofile,"\tdfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s();\n",s);
			}
			fprint(ofile,"}\n",s);
			break;
		case CSTOPT:
			fprint(ofile,"\nC_%s_narg() {\n",s);
			if(op->id_used) {
				fprint(ofile,"\tinop(op_%s);\n",s);
				fprint(ofile,"\tdfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s_narg();\n",s);
			}
			fprint(ofile,"}\n",s);
			/* fall thru */
		case CST:
			fprint(ofile,"\nC_%s(n) int n; {\n",s);
			if(op->id_used) {
				fprint(ofile,"\tincst(op_%s,n);\n",s);
				fprint(ofile,"\tdfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s(n);\n",s);
			}
			fprint(ofile,"}\n",s);
			break;
		case DEFILB:
			fprint(ofile,"\nC_df_ilb(l) label l; {\n");
			if(op->id_used) {
				fprint(ofile,"\tindefilb(op_%s,l);\n",s);
				fprint(ofile,"\tdfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_df_ilb(l);\n",s);
			}
			fprint(ofile,"}\n",s);
			break;
		case PNAM:
			fprint(ofile,"\nC_%s(s) char *s; {\n",s);
			if(op->id_used) {
				fprint(ofile,"\tinpnam(op_%s,s);\n",s);
				fprint(ofile,"\tdfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s(s);\n",s);
			}
			fprint(ofile,"}\n",s);
			break;
		case LAB:
			fprint(ofile,"\nC_%s(l) label l; {\n",s);
			if(op->id_used) {
				fprint(ofile,"\tinlab(op_%s,l);\n",s);
				fprint(ofile,"\tdfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s(l);\n",s);
			}
			fprint(ofile,"}\n",s);
			break;
		case EXT:
			fprint(ofile,"\nC_%s(n) int n; {\n",s);
			if(op->id_used) {
				fprint(ofile,"\tincst(op_%s,n);\n",s);
				fprint(ofile,"\tdfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s(n);\n",s);
			}
			fprint(ofile,"}\n",s);
			fprint(ofile,"\nC_%s_dnam(s,n) char *s; int n; {\n",s);
			if(op->id_used) {
				fprint(ofile,"\tindnam(op_%s,s,n);\n",s);
				fprint(ofile,"\tdfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s_dnam(s,n);\n",s);
			}
			fprint(ofile,"}\n",s);
			fprint(ofile,"\nC_%s_dlb(l,n) label l; int n; {\n",s);
			if(op->id_used) {
				fprint(ofile,"\tindlb(op_%s,l,n);\n",s);
				fprint(ofile,"\tdfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s_dlb(l,n);\n",s);
			}
			fprint(ofile,"}\n",s);
			break;
		}
	}
}
