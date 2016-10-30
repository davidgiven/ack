/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * global data
 */

#define	RCS_EX	"$Id$"

extern  int     wordsize;
extern  int     ptrsize;
extern  cons_t  maxadr;
extern  cons_t  maxint;
extern  cons_t  maxdint;
extern  cons_t  maxunsig;
extern  cons_t  maxdunsig;

/*
 * tables loaded from em_libraries
 */
extern  char    em_flag[];

/*
	The structure containing used for procedure environment stacking
 */
extern  stat_t  pstate ;

/*
 * pointers to not yet allocated storage
 */
extern  glob_t  *mglobs;
extern  glob_t  *xglobs;
extern  proc_t  *mprocs;
extern  proc_t  *xprocs;
extern  ptab_t  *proctab;

extern  FILE    *ifile;
extern  FILE    *tfile;
extern  FILE    *dfile;
extern  FILE    *rtfile;
extern  FILE    *rdfile;
extern  char    string[];

/*
 * some other pointers
 */
extern  glob_t  *lastglosym;
extern  glob_t  *curglosym;
extern  siz_t  *oursize;
extern  relc_t  *f_data;
extern  relc_t  *l_data;
extern  relc_t  *f_text;
extern  relc_t  *l_text;

/*
 * some indices
 */
extern  int     strlngth;
extern  FOFFSET inpoff;
extern  FOFFSET libeof;

/*
 * some other counters
 */
extern  int     procnum;
extern  cons_t  prog_size;
extern  int     max_bytes;
extern  int     pass;
extern  int     line_num;
extern  int     nerrors;
extern  cons_t  textbytes;
extern  cons_t  databytes;
extern  FOFFSET dataoff;
extern  FOFFSET textoff;
extern  FOFFSET lastoff;
extern  int     datamode;
extern  int     datablocks;
extern  relc_t  *lastheader;
extern  cons_t  holbase;
extern  cons_t  holsize;
extern  int     unresolved;
extern  int     sourcelines;
extern  int     intflags;
/*
 * some flags
 */
extern  int     archmode;
extern  int     procflag;
#ifdef  DUMP
extern  int     c_flag;
extern  char    opcnt1[];
extern  char    opcnt2[];
extern  char    opcnt3[];
#endif
extern  int     d_flag;
extern  int     r_flag;
#ifdef JOHAN
extern  int     jflag;
#endif
extern  int     wflag;
extern  int     Uflag;
extern  int     eof_seen;
extern  int     mod_sizes;
/*
 * miscellaneous
 */
extern  cons_t  consiz;
extern  char    *progname;
extern  char    *curfile;
extern  char    *eout;
extern  arch_t  archhdr;
extern  siz_t  sizes[];

extern  char    linesize[];

/*
 * from asstb.c
 */

extern  char    *opindex[] ;
extern  char    opchoice[] ;
extern  int     maxinsl ;

/*
 * types of value returning routines
 */
#ifndef CPM
extern  int     tmpfil();
extern  FILE    *frewind();
#endif
extern  int     xgetc();
extern  unsigned get8();
extern  int     get16();
extern  cons_t  get32();
extern  cons_t  xgeta();
extern  cons_t  parval();
extern  cons_t  valsize();
extern  cons_t  xgetarb();
extern  char    *findnop();
extern  char    *findfit();
extern  glob_t  *glolookup();
extern  glob_t  *glo2lookup();
extern  glob_t  *xglolookup();
extern  locl_t  *loclookup();
extern  proc_t  *prolookup();
extern  proc_t  *enterproc();
extern  proc_t  *searchproc();
extern  relc_t  *text_reloc();
extern  relc_t  *data_reloc();
extern  area_t  getarea();

#define void    int
