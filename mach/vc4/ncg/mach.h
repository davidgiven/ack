/*
 * VideoCore IV code generator for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#define ex_ap(y)        fprintf(codefile,".extern %s\n",y)
#define in_ap(y)        /* nothing */

#define newilb(x)       fprintf(codefile,"%s:\n",x)
#define newdlb(x)       fprintf(codefile,"%s:\n",x)
#define dlbdlb(x,y)     fprintf(codefile,"%s = %s\n",x,y)
#define newlbss(l,x)       fprintf(codefile,".comm %s,%ld\n",l,x);

#define cst_fmt         "%ld"
#define off_fmt         "%ld"
#define ilb_fmt         "I%x_%x"
#define dlb_fmt         "_%d"
#define hol_fmt         "hol%d"

#define hol_off         "%ld+hol%d"

#define con_cst(x)      fprintf(codefile,".data4\t%ld\n",x)
#define con_ilb(x)      fprintf(codefile,".data4\t%s\n",x)
#define con_dlb(x)      fprintf(codefile,".data4\t%s\n",x)

#define fmt_id(sf, st)	sprintf(st,"_%s",sf)

#define modhead	".sect .text; .sect .rom; .sect .data; .sect .bss\n"

#define BSS_INIT        0
