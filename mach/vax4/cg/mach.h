/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
#define ex_ap(x)	fprintf(codefile,".extern\t%s\n",x)
#define in_ap(x)	/* nothing */

#define newilb(x)	fprintf(codefile,"%s:\n",x)
#define newdlb(x)	fprintf(codefile,"%s:\n",x)
#define newplb(x)	fprintf(codefile,".align 1\n%s:\n",x)
#define	dlbdlb(s1,s2)	fprintf(codefile,"%s = %s\n",s1, s2)
#define	newlbss(l,x)	fprintf(codefile,".comm\t%s,%ld\n",l,x);

#define cst_fmt		"%ld"
#define	off_fmt		"%ld"
#define ilb_fmt		"I%x_%x"
#define dlb_fmt		"I_%d"
#define	hol_fmt		"hol%d"

#define fmt_id(fr,to)	sprintf(to,"_%s",fr)

#define hol_off		"%ld+hol%d"

#define con_cst(w)	fprintf(codefile,".data4\t%ld\n",w)
#define con_ilb(x)	fprintf(codefile,".data4\t%s\n",x)
#define con_dlb(x)	fprintf(codefile,".data4\t%s\n",x)

#define BSS_INIT	0
#define MACH_OPTIONS

#define modhead		".sect .text\n.sect .rom\n.sect .data\n.sect .bss\n"
