/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N T E R M E D I A T E   C O D E
 *
 *  C O R E   A L L O C A T I O N   A N D   D E A L L O C A T I O N
 */

#ifdef DEBUG
extern char *newcore();
extern oldcore();
#else
extern char *myalloc();
#define newcore(size) myalloc(size)
#define oldcore(p,size) free((char *)p)
#endif

#define newstruct(t)	((struct t *) newcore (sizeof (struct t)))
#define oldstruct(t,p)	oldcore((char *) p,sizeof (struct t))

extern line_p	newline();		/* (byte optype) */
extern arg_p	newarg();		/* (byte argtype) */
extern short    **newmap();		/* (short length)	*/
extern cset	newbitvect();		/* (short nrbytes)	*/
extern cond_p 	newcondtab();


extern oldline() ;
extern oldargs() ;
extern oldargb() ;
extern oldobjects() ;
extern olddblock() ;
extern oldmap();
extern oldbitvect();			/* (cset s, short nrbytes)	*/
extern oldcondtab();

extern short *newtable();
extern oldtable();

#define newdblock()	(dblock_p) newstruct(dblock)
#define newobject()	(obj_p) newstruct(obj)
#define newproc()	(proc_p) newstruct(proc)
#define newargb()	(argb_p) newstruct(argbytes)
#define newbblock()	(bblock_p) newstruct(bblock)
#define newelem()	(elem_p) newstruct(elemholder)
#define newloop()	(loop_p) newstruct(loop)
#define newuse()	(use_p) newstruct(use)
#define newchange()	(change_p) newstruct(change)
#define newlocal()	(local_p) newstruct(local)

#define oldproc(x)	oldstruct(proc,x)
#define oldbblock(x)	oldstruct(bblock,x)
#define oldelem(x)	oldstruct(elemholder,x)
#define oldloop(x)	oldstruct(loop,x)
#define olduse(x)	oldstruct(use,x)
#define oldchange(x)	oldstruct(change,x)
#define oldlocal(x)	oldstruct(local,x)
