#ifndef m68k4
#define m68020
#endif
			/* m68020 should be used for a m68020 cg, and it should
			 * not be used for a m68k4 cg
			 */

#define ex_ap(y)	fprintf(codefile,".extern %s\n",y)
#define in_ap(y)	/* nothing */

#define newilb(x)	fprintf(codefile,"%s:\n",x)
#define newdlb(x)	fprintf(codefile,"%s:\n",x)
#define	dlbdlb(x,y)	fprintf(codefile,"%s = %s\n",x,y)
#define newlbss(l,x)	fprintf(codefile,"%s:.space\t%ld\n",l,x);

#define	pop_fmt		"(sp)+"
#define cst_fmt		"%ld"
#define	off_fmt		"%ld"
#define ilb_fmt		"I%03x%x"
#define dlb_fmt		"_%d"
#define	hol_fmt		"hol%d"

#ifdef m68020
#define loc_off		"(%d,a6)"
#define arg_off		"(8+%d,a6)"
#else
#define loc_off		"%d(a6)"
#define arg_off		"8+%d(a6)"
#endif
#define hol_off		"%ld+hol%d"

#define con_cst(x)	fprintf(codefile,".data4\t%ld\n",x)
#define con_ilb(x)	fprintf(codefile,".data4\t%s\n",x)
#define con_dlb(x)	fprintf(codefile,".data4\t%s\n",x)

#define modhead		".sect .text\n.sect .rom\n.sect .data\n.sect .bss\n"

#define id_first	'_'
#define BSS_INIT	0
