/* $Header$ */

#define ex_ap(y)        fprintf(codefile,".extern %s\n",y)
#define in_ap(y)        /* nothing */

#define newilb(x)       fprintf(codefile,"%s:\n",x)
#define newdlb(x)       fprintf(codefile,"%s:\n",x)
#define dlbdlb(x,y)     fprintf(codefile,"%s = %s\n",x,y)
#define newlbss(l,x)    fprintf(codefile,"%s: .space\t%d\n",l,x);

#define cst_fmt         "%d"
#define off_fmt         "%d"
#define ilb_fmt         "I%03x%x"
#define dlb_fmt         "_%d"
#define hol_fmt         "hol%d"

#define hol_off         "%d+hol%d"

#define con_cst(x)      fprintf(codefile,".word\t%d\n",x)
#define con_ilb(x)      fprintf(codefile,".word\t%s\n",x)
#define con_dlb(x)      fprintf(codefile,".word\t%s\n",x)

#define modhead         ""

#define id_first        '_'
#define BSS_INIT        0
