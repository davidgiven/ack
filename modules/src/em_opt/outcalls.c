#ifndef NORCSID
static char rcsidp4[] = "$Header$";
#endif

#include "parser.h"

outputincalls()
{
	struct idf *op;
	int opcode;
	char *s;
	if((ofile=fopen("incalls.r","w"))==NULL) {
		fprintf(stderr,"Fatal Error: cannot open output file incalls.r\n");
		sys_stop(S_EXIT);
	}
	for(op=ops;op!=(struct idf *)NULL;op=op->id_nextidf) {
		opcode = op->id_opcode;
		s = op->id_text;
		switch(op->id_argfmt) {
		case NOARG:
			fprintf(ofile,"%s\t|\t|\n",s);
			if(op->id_used) {
				fprintf(ofile,"\tOO_inop(op_%s);\n",s);
				fprintf(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprintf(ofile,"\tFLUSHDFA();\n");
				fprintf(ofile,"\tC_%s();\n",s);
			}
			break;
		case CSTOPT:
			fprintf(ofile,"%s_narg\t|\t|\n",s);
			if(op->id_used) {
				fprintf(ofile,"\tOO_inop(op_%s);\n",s);
				fprintf(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprintf(ofile,"\tFLUSHDFA();\n");
				fprintf(ofile,"\tC_%s_narg();\n",s);
			}
			/* fall thru */
		case CST:
			fprintf(ofile,"%s\t| int:n\t|\n",s);
			if(op->id_used) {
				fprintf(ofile,"\tOO_incst(op_%s,n);\n",s);
				fprintf(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprintf(ofile,"\tFLUSHDFA();\n");
				fprintf(ofile,"\tC_%s(n);\n",s);
			}
			break;
		case DEFILB:
			fprintf(ofile,"df_ilb\t| label:l\t|\n");
			if(op->id_used) {
				fprintf(ofile,"\tOO_indefilb(op_%s,l);\n",s);
				fprintf(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprintf(ofile,"\tFLUSHDFA();\n");
				fprintf(ofile,"\tC_df_ilb(l);\n",s);
			}
			break;
		case PNAM:
			fprintf(ofile,"%s\t| char *:s\t|\n",s);
			if(op->id_used) {
				fprintf(ofile,"\tOO_inpnam(op_%s,s);\n",s);
				fprintf(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprintf(ofile,"\tFLUSHDFA();\n");
				fprintf(ofile,"\tC_%s(s);\n",s);
			}
			break;
		case LAB:
			fprintf(ofile,"%s\t| label:l\t|\n",s);
			if(op->id_used) {
				fprintf(ofile,"\tOO_inlab(op_%s,l);\n",s);
				fprintf(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprintf(ofile,"\tFLUSHDFA();\n");
				fprintf(ofile,"\tC_%s(l);\n",s);
			}
			break;
		case EXT:
			fprintf(ofile,"%s\t| int:n\t|\n",s);
			if(op->id_used) {
				fprintf(ofile,"\tOO_incst(op_%s,n);\n",s);
				fprintf(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprintf(ofile,"\tFLUSHDFA();\n");
				fprintf(ofile,"\tC_%s(n);\n",s);
			}
			fprintf(ofile,"%s_dnam\t| char *:s int:n\t|\n",s);
			if(op->id_used) {
				fprintf(ofile,"\tOO_indnam(op_%s,s,n);\n",s);
				fprintf(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprintf(ofile,"\tFLUSHDFA();\n");
				fprintf(ofile,"\tC_%s_dnam(s,n);\n",s);
			}
			fprintf(ofile,"%s_dlb\t| label:l int:n\t|\n",s);
			if(op->id_used) {
				fprintf(ofile,"\tOO_indlb(op_%s,l,n);\n",s);
				fprintf(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprintf(ofile,"\tFLUSHDFA();\n");
				fprintf(ofile,"\tC_%s_dlb(l,n);\n",s);
			}
			break;
		}
	}
}
