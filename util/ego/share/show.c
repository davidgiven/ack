/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  S H O W . C  */

/* This program can be used to make the output of the 'cf' pass
 * human readable. It will display either the procedure table,
 * the datablock table, the basic block table or the EM text,
 * depending on the flag that is passed as first argument.
 */



#include <stdio.h>
#include <em_spec.h>
#include <em_flag.h>
#include <em_pseu.h>
#include "types.h"
#include "def.h"
#include "global.h"


#define BMASK 0377






extern byte em_flag[];

#define space1()	printf("	")
char format[] = "	%-11s%d\n";
char lformat[] = "	%-11s%ld\n";
char sformat[] = "	%-10s%s\n";
char dformat[] = "		%-11s%d\n";
char oformat[] = "		%-11s%ld\n";



FILE *f;	/* input file */


#define getbyte()	getc(f)

short getshort()
{
	register n;

	n = getbyte();
	n |= getbyte() << 8;
	return n;
}

offset getoff()
{
	register offset n;

	n = (unsigned) getshort();
	n |= ((offset) getshort() ) << 16;
	return n;
}


int getint()
{
	/* Read an integer from the input file. This routine is
	 * only used when reading a bitvector-set. We expect  an
	 * integer to be either a short or a long.
	 */

	if (sizeof(int) == sizeof(short)) {
		return getshort();
	} else {
		return getoff();
	}
}


/* VARARGS 1 */
error(s,a) char *s,*a; {

	fprintf(stderr,"error");
	fprintf(stderr,": ");
	fprintf(stderr,s,a);
	fprintf(stderr,"\n");
	exit(-1);
}

main(argc, argv)
	int argc;
	char *argv[];
{
	if (argc != 3 || argv[1][0] != '-') {
		error("usage: %s -[ldpbc] filename",argv[0]);
	}
	if ((f = fopen(argv[2], "r")) == NULL) {
		error("cannot open %s", argv[2]);
	}
	switch(argv[1][1]) {
		case 'l':
			showl();
			break;
		case 'd':
			showd();
			break;
		case 'p':
			showp();
			break;
		case 'b':
			showb();
			break;
		case 'c':
			showc();
			break;
		default:
			error("bad flag");
	}

	fclose(f);
	exit(0);
}


showcset()
{
	/* print a compact (bitvector) set */

	short size;
	register short i,j;
	int w, mask;

	size = getshort();
	/* # significant bits in bitvector */
	i = 1;
	printf(" { ");
	if (size == 0) {
		printf("}\n");
		return;
	}
	for (;;) {
		w = getint();
		mask = 1 ;
		for (j = 1; j <= sizeof(int)*8; j++) {
			if (w & mask) {
				printf("%d ",i);
			}
			if (i++ == size) {
				printf ("}\n");
				return;
			}
			mask <<=  1;
		}
	}
}



showp()
{
	byte b;
	short n;
	short all;
	printf("total number of procs: %d\n\n",getshort());
	all = getshort();
	while (TRUE) {
		n = getshort();
		if (feof(f)) break;
		printf("PROC\n");
		printf(format,"id =",n);
		printf(format,"flags1 =",b = getbyte());
		if (b & PF_BODYSEEN) {
			printf(format,"# labels =",getshort());
			printf(lformat,"# locals =",getoff());
			printf(lformat,"# formals =",getoff());
			if (all == 1) {
				printf("	changed ="); showcset();
				printf(format,"c_flags =",getshort());
				printf(format,"u_flags =",getshort());
				printf("	calling ="); showcset();
			}
		} else {
			printf("	body not available\n");
		}
	}
}


char *pseudo[5] = {"hol", "bss", "rom", "con", "unknown" };

showd()
{
	short n;
	printf("total number of objects: %d\n\n",getshort());
	while (TRUE) {
		n = getbyte();
		if (feof(f)) break;
		switch(n) {
			case MARK_DBLOCK:
				printf("DBLOCK\n");
				printf(format,"id =",getshort());
				printf(sformat,"pseudo =",
					pseudo[(short) getbyte()]);
				printf(lformat,"size =",getoff());
				printf(format,"fragment =",getshort());
				printf(format,"flags1 =",
					(short) getbyte());
				break;
			case MARK_OBJ:
				printf("	OBJ\n");
				space1();
				printf(format,"id =",getshort());
				space1();
				printf(lformat,"size =",getoff());
				space1();
				printf(lformat,"offset =",getoff());
				break;
			case MARK_ARG:
				printf("	VALUE\n");
				space1();
				printf(lformat,"offset =",getoff());
				break;
		}
	}
}


/* The mnemonics of the EM instructions and pseudos */


extern char em_mnem[];
extern char em_pseu[];
char lab_mnem[] = "instrlab";
char sym_mnem[] = "datalab";

showinstr()
{
	short instr;
	char *s;

	instr = (short) getbyte();
	if (feof(f)) return FALSE;
	if (instr >= sp_fmnem && instr <= sp_lmnem) {
		s = &(em_mnem[(instr-sp_fmnem) *4]);
	} else {
		if (instr == op_lab) {
			s = lab_mnem;
		} else {
			if (instr == ps_sym) {
				s = sym_mnem;
			} else {
				s = &(em_pseu[(instr-sp_fpseu)*4]);
			}
		}
	}
	printf("%s",s);
	switch((short) getbyte()) {
		case OPSHORT:
		case OPOBJECT:
			printf(" %d", getshort());
			break;
		case OPPROC:
			printf(" $%d",getshort());
			break;
		case OPINSTRLAB:
			printf(" *%d",getshort());
			break;
		case OPOFFSET:
			printf(" %ld", getoff());
			break;
		case OPLIST:
			arglist();
			break;
	}
	printf("\n");
	return TRUE;
}


showl()
{
	while (showinstr());
}



arglist()
{
	short length;
	for (;;) {
		switch((short) getbyte()) {
			case ARGOBJECT:
				printf(" %d", getshort());
				break;
			case ARGPROC:
				printf(" $%d",getshort());
				break;
			case ARGINSTRLAB:
				printf(" *%d",getshort());
				break;
			case ARGOFF:
				printf(" %ld", getoff());
				break;
			case ARGICN:
			case ARGUCN:
			case ARGFCN:
				printf(" %d",getshort());
				/* Fall through !! */
			case ARGSTRING:
				length = getshort();
				putchar(' ');
				putchar('"');
				while (length--) {
					putchar(getbyte());
				}
				putchar('"');
				break;
			case ARGCEND:
				return;
		}
	}
}



showlset()
{
	register short x;

	printf("{ ");
	while (x = getshort()) {
		printf("%d ",x);
	}
	printf("}\n");
}




showb()
{
	/* basic block file */

	short n,m;

	while (TRUE) {
		n = getshort();
		if (feof(f)) break;
		if (n == 0) {
			printf("Declaration Unit:\n");
			printf(dformat,"#instrs =",getshort());
			printf("\n");
			continue;
		}
		printf("Control Flow Graph:\n");
		printf("number of basic blocks: %d\n",n);
		m = getshort(); /* #loops */
		while (n--) {
			printf("	BASIC BLOCK\n");
			printf(dformat,"id =",getshort());
			printf(dformat,"# instrs =",getshort());
			printf("		succ =");
			showlset();
			printf("		pred =");
			showlset();
			printf(dformat,"idom =",getshort());
			printf("		loops =");
			showlset();
			printf(dformat,"flags =",getshort());
		}
		printf("number of loops: %d\n",m);
		while (m--) {
			printf("	LOOP\n");
			printf(dformat,"id =",getshort());
			printf(dformat,"level =",getshort());
			printf(dformat,"entry =",getshort());
			printf(dformat,"end =",getshort());
		}
		printf("\n");
	}
}


showc()
{
	int n,m,cnt,t;

	cnt = 1;
	while(TRUE) {
		t = getshort();
		if (feof(f)) break;
		printf("CALL %d\n",cnt++);
		printf(format,"nestlevel =",t);
		printf(format,"calling p. =",getshort());
		printf(format,"call_id =",getshort());
		printf(format,"called p. =",getshort());
		printf(format,"looplevel =",getbyte());
		printf(format,"flags =",getbyte());
		printf(format,"ratio =",getshort());
		printf("	actuals:");
		n = getshort();
		if (n == 0) {
			printf("  ---\n");
		} else {
			while (n--) {
				printf("\n");
				m = getshort();
				printf(oformat,"size =",getoff());
				printf(dformat,"inl =",getbyte());
				while (m--) {
					printf("		");
					showinstr();
				}
			}
		}
	}
}
