#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "parser.h"

outputincalls()
{
	struct idf *op;
	int opcode;
	char *s;
	if(!sys_open("incalls.r",OP_WRITE,&ofile)) {
		fprint(STDERR,"Fatal Error: cannot open output file incalls.r\n");
		sys_stop(S_EXIT);
	}
	for(op=ops;op!=(struct idf *)NULL;op=op->id_nextidf) {
		opcode = op->id_opcode;
		s = op->id_text;
		switch(op->id_argfmt) {
		case NOARG:
			fprint(ofile,"%s\t|\t|\n",s);
			if(op->id_used) {
				fprint(ofile,"\tOO_inop(op_%s);\n",s);
				fprint(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s();\n",s);
			}
			break;
		case CSTOPT:
			fprint(ofile,"%s_narg\t|\t|\n",s);
			if(op->id_used) {
				fprint(ofile,"\tOO_inop(op_%s);\n",s);
				fprint(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s_narg();\n",s);
			}
			/* fall thru */
		case CST:
			fprint(ofile,"%s\t| int:n\t|\n",s);
			if(op->id_used) {
				fprint(ofile,"\tOO_incst(op_%s,n);\n",s);
				fprint(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s(n);\n",s);
			}
			break;
		case DEFILB:
			fprint(ofile,"df_ilb\t| label:l\t|\n");
			if(op->id_used) {
				fprint(ofile,"\tOO_indefilb(op_%s,l);\n",s);
				fprint(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_df_ilb(l);\n",s);
			}
			break;
		case PNAM:
			fprint(ofile,"%s\t| char *:s\t|\n",s);
			if(op->id_used) {
				fprint(ofile,"\tOO_inpnam(op_%s,s);\n",s);
				fprint(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s(s);\n",s);
			}
			break;
		case LAB:
			fprint(ofile,"%s\t| label:l\t|\n",s);
			if(op->id_used) {
				fprint(ofile,"\tOO_inlab(op_%s,l);\n",s);
				fprint(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s(l);\n",s);
			}
			break;
		case EXT:
			fprint(ofile,"%s\t| int:n\t|\n",s);
			if(op->id_used) {
				fprint(ofile,"\tOO_incst(op_%s,n);\n",s);
				fprint(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s(n);\n",s);
			}
			fprint(ofile,"%s_dnam\t| char *:s int:n\t|\n",s);
			if(op->id_used) {
				fprint(ofile,"\tOO_indnam(op_%s,s,n);\n",s);
				fprint(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s_dnam(s,n);\n",s);
			}
			fprint(ofile,"%s_dlb\t| label:l int:n\t|\n",s);
			if(op->id_used) {
				fprint(ofile,"\tOO_indlb(op_%s,l,n);\n",s);
				fprint(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprint(ofile,"\tFLUSHDFA();\n");
				fprint(ofile,"\tO_%s_dlb(l,n);\n",s);
			}
			break;
		}
	}
}
