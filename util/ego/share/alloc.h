/*  I N T E R M E D I A T E   C O D E
 *
 *  C O R E   A L L O C A T I O N   A N D   D E A L L O C A T I O N
 */

#ifdef DEBUG
extern short *newcore();
extern oldcore();
#else
extern short *myalloc();
#define newcore(size) myalloc(size)
#define oldcore(p,size) free(p)
#endif

#define newstruct(t)	(newcore (sizeof (struct t)))
#define oldstruct(t,p)	oldcore((short *) p,sizeof (struct t))

extern line_p	newline();		/* (byte optype) */
extern dblock_p newdblock();
extern obj_p	newobject();
extern proc_p	newproc();
extern arg_p	newarg();		/* (byte argtype) */
extern argb_p	newargb();
extern bblock_p	newbblock();
extern short    **newmap();		/* (short length)	*/
extern elem_p	newelem();
extern cset	newbitvect();		/* (short nrbytes)	*/
extern loop_p	newloop();
extern use_p	newuse();
extern change_p newchange();
extern cond_p 	newcondtab();


extern oldline() ;
extern oldargs() ;
extern oldargb() ;
extern oldobjects() ;
extern oldproc() ;
extern olddblock() ;
extern oldbblock();
extern oldmap();
extern oldelem();
extern oldbitvect();			/* (cset s, short nrbytes)	*/
extern oldloop();
extern olduse();
extern oldchange();
extern oldcondtab();

extern sym_p	newsym();
extern prc_p	newprc();
extern num_p	newnum();
extern oldnum() ;
extern oldsym();
extern oldprc();
extern iv_p newiv();
extern oldiv();
extern code_p newcinfo();
extern oldcinfo();
extern call_p newcall();
extern oldcall();
extern actual_p newactual();
extern oldactual();
extern formal_p newformal();
extern oldformal();
extern calcnt_p newcalcnt();
extern oldcalcnt();
extern local_p newlocal();
extern oldlocal();
extern short *newtable();
extern oldtable();
extern char **newnametab();
extern bext_p newcfbx();
extern oldcfbx();
extern lpext_p newcflpx();
extern oldcflpx();
extern lpext_p newsrlpx();
extern oldsrlpx();
extern pext_p newilpx();
extern oldilpx();
extern bext_p newudbx();
extern oldudbx();
extern bext_p newlvbx();
extern oldlvbx();
extern bext_p newrabx();
extern oldrabx();
extern lpext_p newralpx();
extern oldralpx();
