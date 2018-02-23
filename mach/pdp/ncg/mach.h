/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

/* The next define switches between codegeneration for an ACK assembler
 * or for the standard UNIX V7 assembler.
 * If on code is generated for the ACK assembler.
 */
#define ACK_ASS	/* code for ACK assembler */

#ifdef ACK_ASS
#define COMMENTCHAR '!'
#define ex_ap(y)	fprintf(codefile,".extern %s\n",y)
#else
#define COMMENTCHAR '/'
#define ex_ap(y)	fprintf(codefile,".globl %s\n",y)
#endif
#define in_ap(y)	/* nothing */

#define newplb(x)	fprintf(codefile,"%s:\n",x)
#define newilb(x)	fprintf(codefile,"%s:\n",x)
#define newdlb(x)	fprintf(codefile,"%s:\n",x)
#ifdef ACK_ASS
#define newlbss(l,x)	fprintf(codefile,".comm %s,%u\n",l,x);
#else
#define newlbss(l,x)	fprintf(codefile,"%s:.=.+%d\n",l,x);
#endif

#define cst_fmt		"$%d"
#define off_fmt		"%d"
#define ilb_fmt		"I%x_%x"
#define dlb_fmt		"_%d"
#define	hol_fmt		"hol%d"

#define hol_off		"%d+hol%d"

#ifdef ACK_ASS
#define con_cst(x)	fprintf(codefile,".data2 %d\n",x)
#define con_ilb(x)	fprintf(codefile,".data2 %s\n",x)
#define con_dlb(x)	fprintf(codefile,".data2 %s\n",x)
#else
#define con_cst(x)	fprintf(codefile,"%d\n",x)
#define con_ilb(x)	fprintf(codefile,"%s\n",x)
#define con_dlb(x)	fprintf(codefile,"%s\n",x)
#endif

#ifdef ACK_ASS
#define modhead		".sect .text; .sect .rom; .sect .data; .sect .bss\n"
#define fmt_id(f,t)	sprintf(t,"_%s",f)
#else
#define id_first	'_'
#endif

#define BSS_INIT	0
