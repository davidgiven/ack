/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include        "ass00.h"
#include        "assex.h"


/*
 * global data
 */

int     wordsize ;
int     ptrsize ;
cons_t  maxadr ;
cons_t  maxint;
cons_t  maxdint;
cons_t  maxunsig;
cons_t  maxdunsig;

/*
	The structure containing used for procedure environment stacking
*/
stat_t  pstate ;

/*
 * pointers to not yet allocated storage
 */
glob_t  *mglobs;                        /* pointer to module symbols */
glob_t  *xglobs;                        /* pointer to extern symbols */
proc_t  *mprocs;                        /* pointer to local procs */
proc_t  *xprocs;                        /* pointer to external procs */
ptab_t  *proctab;                       /* pointer to proctab[] */

/*
 * some array and structures of known size
 */
FILE    *ifile;                         /* input file buffer */
FILE    *tfile;                         /* code file buffer */
FILE    *dfile;                         /* data file buffer */
FILE    *rtfile;                        /* code file buffer */
FILE    *rdfile;                        /* data file buffer */
char    string[MAXSTRING];

/*
 * some other pointers
 */
glob_t  *lastglosym;                    /* last global symbol */
glob_t  *curglosym;                     /* current global symbol */
relc_t  *f_data = (relc_t *)0 ;         /* first data reloc pointer */
relc_t  *l_data = (relc_t *)0 ;         /* last data reloc pointer */
relc_t  *f_text = (relc_t *)0 ;         /* first text reloc pointer */
relc_t  *l_text = (relc_t *)0 ;         /* last text reloc pointer */

/*
 * some indices
 */
int     strlngth;                       /* index in string[] */
FOFFSET inpoff;                         /* offset in current input file */
FOFFSET libeof;                         /* ceiling for above number */

/*
 * some other counters
 */
int     procnum;                        /* generic for unique proc-descr. */
cons_t  prog_size;                      /* length of current proc */
int     max_bytes;
int     pass;
int     line_num;                       /* line number for error messages */
int     nerrors;                        /* number of nonfatal errors */
cons_t  consiz;                         /* size of U,I or F value */
cons_t  textbytes;                      /* size of code file */
cons_t  databytes;                      /* highwater mark in data */
FOFFSET dataoff;                        /* size of data file */
FOFFSET textoff;                        /* size of text file */
FOFFSET lastoff;                        /* previous size before last block */
int     datamode;                       /* what kind of data */
int     datablocks;                     /* number of datablocks written out */
relc_t *lastheader;                     /* pointer into datareloc */
cons_t  holbase;
cons_t  holsize;
int     unresolved;                     /* # of unresolved references */
int     sourcelines;                    /* number of lines in source program*/
int     intflags        =  1;           /* flags for interpreter */
/*
 * some flags
 */
int     archmode;                       /* reading library ? */
int     procflag;                       /* print "namelist" of procedures */
#ifdef  DUMP
int     c_flag;                         /* print unused opcodes */
char    opcnt1[256];                    /* count primary opcodes */
char    opcnt2[256];                    /* count secondary opcodes */
char    opcnt3[256];                    /* count long opcodes */
#endif
int     d_flag          =  0;           /* don't dump */
int     r_flag          =  0;           /* don't dump relocation tables */
#ifdef JOHAN
int     jflag;
#endif
int     wflag           =  0;           /* don't issue warning messages */
int	Uflag		=  0;		/* exit status 0 on unresolved refs */
int     eof_seen;
int     mod_sizes;                      /* Size info in current module ok? */

#define BASE    (sizeof (struct lines) - sizeof (addr_u))

char    linesize[VALLOW+1] = {
	BASE,                           /* MISSING */
	BASE + sizeof (cons_t),         /* CONST */
	BASE + sizeof prp_cast,         /* PROCNAME */
	BASE + sizeof gbp_cast,         /* GLOSYM */
	BASE + sizeof lbp_cast,         /* LOCSYM */
	BASE + sizeof (struct sad_df),  /* GLOOFF */
	BASE + sizeof (struct sad_ln),  /* LINES */
	BASE                            /* VALLOW */
} ;

/*
 * miscellaneous
 */
char    *progname;                      /* argv[0] */
char    *curfile        =  0;           /* name of current file */
char    *eout           =  "e.out";
arch_t  archhdr;
siz_t  sizes[NDEFAULT] = {
/*   mlab, glab,mproc,xproc, proc */
    { 151,   29,   31,   73,  130 },	/* small */
    { 307,  127,  151,  401,  460 },	/* medium */
    { 601,  251,  151,  401,  600 },	/* large */
    {1601, 1601, 1601, 1601, 2000 }	/* extra large */
};
siz_t  *oursize        =  &sizes[2] ;  /* point to selected sizes */
