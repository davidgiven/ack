/* $Header$ */

#define ex_ap(y)	fprintf(codefile,".globl %s\n",y)
#define in_ap(y)	/* nothing */

#define newplb(x)	fprintf(codefile,"%s:\n",x)
#define newilb(x)	fprintf(codefile,"%s:\n",x)
#define newdlb(x)	fprintf(codefile,"%s:\n",x)
#define newlbss(l,x)	fprintf(codefile,"%s:.=.+0%o\n",l,x);

#define cst_fmt		"$0%o"
#define off_fmt		"0%o"
#define ilb_fmt		"I%03x%x"
#define dlb_fmt		"_%d"
#define	hol_fmt		"hol%d"

#define hol_off		"0%o+hol%d"

#define con_cst(x)	fprintf(codefile,"0%o\n",x)
#define con_ilb(x)	fprintf(codefile,"%s\n",x)
#define con_dlb(x)	fprintf(codefile,"%s\n",x)

#define id_first	'_'
#define BSS_INIT	0
