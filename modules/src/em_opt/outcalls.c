#ifndef NORCSID
static char rcsidp4[] = "$Id$";
#endif

#include "parser.h"

outputincalls()
{
	struct idf *op;
	char *s;
	for(op=ops;op!=(struct idf *)NULL;op=op->id_nextidf) {
		s = op->id_text;
		switch(op->id_argfmt) {
		case NOARG:
			fprintf(ofile,"%s\t|\t|\n",s);
			if(op->id_used) {
				fprintf(ofile,"\tEM_Nop(op_%s);\n",s);
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
				fprintf(ofile,"\tEM_Nnarg(op_%s);\n",s);
				fprintf(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprintf(ofile,"\tFLUSHDFA();\n");
				fprintf(ofile,"\tC_%s_narg();\n",s);
			}
			/* fall thru */
		case CST:
			fprintf(ofile,"%s\t| arith:n\t|\n",s);
			if(op->id_used) {
				fprintf(ofile,"\tEM_Ncst(op_%s,n);\n",s);
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
				fprintf(ofile,"\tEM_Ndefilb(op_%s,l);\n",s);
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
				fprintf(ofile,"\tEM_Npro(op_%s,s);\n",s);
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
				fprintf(ofile,"\tEM_Nilb(op_%s,l);\n",s);
				fprintf(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprintf(ofile,"\tFLUSHDFA();\n");
				fprintf(ofile,"\tC_%s(l);\n",s);
			}
			break;
		case EXT:
			fprintf(ofile,"%s\t| arith:n\t|\n",s);
			if(op->id_used) {
				fprintf(ofile,"\tEM_Ncst(op_%s,n);\n",s);
				fprintf(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprintf(ofile,"\tFLUSHDFA();\n");
				fprintf(ofile,"\tC_%s(n);\n",s);
			}
			fprintf(ofile,"%s_dnam\t| char *:s arith:n\t|\n",s);
			if(op->id_used) {
				fprintf(ofile,"\tEM_Nsof(op_%s,s,n);\n",s);
				fprintf(ofile,"\tOO_dfa(op_%s);\n",s);
			}
			else {
				fprintf(ofile,"\tFLUSHDFA();\n");
				fprintf(ofile,"\tC_%s_dnam(s,n);\n",s);
			}
			fprintf(ofile,"%s_dlb\t| label:l arith:n\t|\n",s);
			if(op->id_used) {
				fprintf(ofile,"\tEM_Nnof(op_%s,l,n);\n",s);
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
