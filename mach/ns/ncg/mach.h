/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#define ex_ap(y)        fprintf(codefile,".extern %s\n",y)
#define in_ap(y)        /* nothing */

#define newilb(x)       fprintf(codefile,"%s:\n",x)
#define newdlb(x)       fprintf(codefile,"%s:\n",x)
#define newplb(x)       fprintf(codefile,".align 2\n%s:\n",x)
#define dlbdlb(x,y)     fprintf(codefile,"%s = %s\n",x,y)
#define newlbss(l,x)    fprintf(codefile,".comm\t%s,%ld\n",l,x)

#define cst_fmt         "%ld"
#define off_fmt         "%ld"
#define ilb_fmt         "I%x_%x"
#define dlb_fmt         "_%d"
#define hol_fmt         "hol%d"

#define hol_off         "%ld+hol%d"

#define con_cst(x)      fprintf(codefile,".data4\t%ld\n",x)
#define con_ilb(x)      fprintf(codefile,".data4\t%s\n",x)
#define con_dlb(x)      fprintf(codefile,".data4\t%s\n",x)

#define fmt_id(fr,to)	sprintf(to, "_%s", fr)

#define modhead		".sect .text; .sect .rom; .sect .data; .sect .bss\n"

#define BSS_INIT        0
