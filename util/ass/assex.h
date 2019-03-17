/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * global data
 */

#include <stdio.h>

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

/* Generate temporary filename. Fatal error in case of error. */
extern char *tmpfil(void);


/* Read next byte from "af" file, fatal error if there isn't one. */
extern int xgetc(register FILE *af);
/* Read a value of length "l" bytes from file "f",
 * fatal error if cannot be read.
 */
extern cons_t xgetarb(int l,FILE *f);
/* Read the pointer constant a from file "f".
 * NOTE: The bytes were written low to high (little-endian).
 */
extern cons_t xgeta(FILE* f);



/* Output one byte into file "af" and fatal error if it gives an error */
extern void xputc(int c,register FILE *af);
/* Output a 16-bit value into file "f" in little-endian, fatal error if it gives an error. */
extern void xput16(int w,FILE *f);
/* Output a value of "l" bytes into file "f" and fatal error if it gives an error. */
extern void	xputarb(int l,cons_t w, FILE* f);
/* Assemble the pointer constant a into file "f".
 * NOTE: The bytes are written low to high (little-endian).
 */
extern void xputa(cons_t a,FILE* f);





/* Write a byte value into the code temporary file. */
extern void put8(int n);
/* Write a 16-bit value into the code temporary file. */
extern void put16(int n);
/* Write a 32-bit value into the code temporary file. */
extern void put32(cons_t n);
/* Write a 64-bit value into the code temporary file. */
extern void put64(cons_t n);


/* Read a byte from the input file, return EOF upon error of EOF */
extern int xget8(void);
/* Read a byte from the input file, fatal error upon error or EOF. */
extern unsigned int get8(void);
/* Read a signed 16-bit value from the input file. Raise
 * a fatal error upon error or end of stream.
 */
extern int get16(void);
/* Read an unsigned 16-bit value from the input file. Raise
 * a fatal error upon error or end of stream.
 */
extern int getu16(void);
/* Read a 32-bit value from the input file. Raise
 * a fatal error upon error or end of stream.
 */
extern cons_t get32(void);



/* Write a byte to the data file. */
extern void ext8(int b);
/* Write a 16-bit value to the data file.
 * The value is written from low to high (little-endian)
 */
extern void extword(cons_t w);
/* Write "value" of "size" bytes to the data file. The bytes
 * are written low to high.
 */
extern void extarb(int size, long value);
/* Write pointer "a". The bytes are
 * written from low to high to the data file.
 */
extern void extadr(cons_t a);



/* Returns the opcode length in bytes */
extern	int 	oplength(int);
extern	void 	genop(char *,cons_t,int );

extern	void 	putblk(register FILE *stream,register char *from, register int amount);
extern	int getblk(register FILE *stream, register char *from, register int amount);


extern	void 	set_mode(int mode);

/* Dump current information to screen if dump flag is enabled. */
void dump(int n);

/*
extern  char    *findnop();
extern  char    *findfit();

*/
extern glob_t *glolookup(char *name,int status,glob_t *table, int size);
extern proc_t *searchproc(char *name,proc_t *table,int size);
extern glob_t *glo2lookup(char *name ,int status);
extern glob_t *xglolookup(char *name,int status);
extern proc_t *prolookup(char *name,int status);
extern locl_t *loclookup(unsigned int an,int status);
extern proc_t *enterproc(char *name,int status,proc_t *place);
extern cons_t parval(line_t *lnp,char *defined);


extern void determine_props(line_t *lnp, int *min_len, int *max_len);
extern int opfit(int flag,int number,cons_t val,int i_flag);

extern void initproc(void);
extern void endproc(void);
extern void init_files(void);
extern void init_module(void);
extern void end_module(void);
extern void do_proc(void);
extern void ertrap(void);
extern void init_vars(void);

extern void error(const char* string1, ...);
extern void werror(const char* string1, ...);
extern void fatal(char *s);


