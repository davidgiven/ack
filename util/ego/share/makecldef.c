/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*  MAKECLASSDEF
 *
 * This program is used by several phases of the optimizer
 * to make the file classdefs.h. It reads two files:
 *  - the em_mnem,h file, containing the definitions of the
 *    EM mnemonics
 *  - the class-file, containing tuples:
 *    (mnemonic, src_class, res_class)
 *    where src_class and res_class are integers telling how
 *    to compute the number of bytes popped and pushed
 *    by the instruction.
 * The output (standard output) is a C array.
 */


void error(const char *s)
{
	fprintf(stderr,"%s\n",s);
	exit(-1);
}


void convert(FILE *mnemfile, FILE *classfile)
{
	char mnem1[10], mnem2[10],def[10];
	int src,res,opc;
	bool newcl;

	newcl = true;
	printf("struct class classtab[] = {\n");
	printf("\tNOCLASS,\tNOCLASS,\n");
	/* EM mnemonics start at 1, arrays in C at 0 */
	for (;;) {
		fscanf(mnemfile,"%9s%9s%d",def,mnem1,&opc);
		/* read a line like "#define op_aar 1" */
		if (feof(mnemfile)) break;
		if (strcmp(def,"#define") != 0) {
			error("bad mnemonic file, #define expected");
		}
		if (newcl) {
			fscanf(classfile,"%9s%d%d",mnem2,&src,&res);
			/* read a line like "op_loc 8 1" */
		}
		if (feof(classfile) || strcmp(mnem1,mnem2) != 0) {
			/* there is no line for this mnemonic, so
			 * it has no class.
			 */
			printf("\tNOCLASS,\tNOCLASS,\n");
			newcl = false;
		} else {
			printf("\tCLASS%d,\t\tCLASS%d,\n",src,res);
			/* print a line like "CLASS8, CLASS1," */
			newcl = true;
		}
	}
	printf("};\n");
}


int main(int argc, char *argv[])
{
	FILE *f1,*f2;

	if (argc != 3) {
		error("usage: makeclassdef mnemfile classfile");
	}
	if ((f1 = fopen(argv[1],"r")) == NULL) {
		error("cannot open mnemonic file");
	}
	if ((f2 = fopen(argv[2],"r")) == NULL) {
		error("cannot open class file");
	}
	convert(f1,f2);
	exit(0);
}
