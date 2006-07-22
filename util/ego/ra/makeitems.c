/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
 
#include <stdlib.h>
#include <stdio.h>

/*  MAKE ITEMS TABLE
 *
 * This program is used by the register allocation phase of the optimizer
 * to make the file itemtab.h. It reads two files:
 *  - the em_mnem.h file, containing the definitions of the
 *    EM mnemonics
 *  - the item-file, containing tuples:
 *    (mnemonic, item_type)
 * The output (standard output) is a C array.
 */


#define TRUE  1
#define FALSE 0

convert(mnemfile,itemfile)
	FILE *mnemfile, *itemfile;
{
	char mnem1[20], mnem2[20],def[20],itemtype[20];
	int newcl,opc,index;

	newcl = TRUE;
	printf("struct item_descr itemtab[] = {\n");
	for (;;) {
		fscanf(mnemfile,"%s%s%d",def,mnem1,&opc);
		/* read a line like "#define op_aar 1" */
		if (feof(mnemfile)) break;
		if (strcmp(def,"#define") != 0) {
			error("bad mnemonic file, #define expected");
		}
		if (newcl) {
			fscanf(itemfile,"%s%s%d",mnem2,itemtype,&index);
			/* read a line like "op_loc CONST 4" */
		}
		if (feof(itemfile) || strcmp(mnem1,mnem2) != 0) {
			/* there is no line for this mnemonic, so
			 * it has no type.
			 */
			printf("{NO_ITEM,0}, /* %s */\n", mnem1);
			newcl = FALSE;
		} else {
			printf("{%s,%d}, /* %s */\n",itemtype,index, mnem1);
			newcl = TRUE;
		}
	}
	printf("};\n");
}



error(s)
	char *s;
{
	fprintf(stderr,"%s\n",s);
	exit(-1);
}


main(argc,argv)
	int argc;
	char *argv[];
{
	FILE *f1,*f2;

	if (argc != 3) {
		error("usage: makeitems mnemfile itemfile");
	}
	if ((f1 = fopen(argv[1],"r")) == NULL) {
		error("cannot open mnemonic file");
	}
	if ((f2 = fopen(argv[2],"r")) == NULL) {
		error("cannot open item file");
	}
	convert(f1,f2);
	exit(0);
}
