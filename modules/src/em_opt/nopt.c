#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "nopt.h"
extern int maxpattern;	/* Initialized from patterns in dfa.c */
#define MAXBACKUP	50
#define MAXOUTPUT	200
#define MAXFREEI	200
#define MAXSTRING	1000

#define GETINSTR() (nextifree>freeiqueue)?*(--nextifree):\
	((struct instr *)Malloc(sizeof(struct instr)))

extern char em_mnem[][4];

struct instr **OO_patternqueue;
struct instr **OO_nxtpatt;
struct instr **OO_bkupqueue;
struct instr **OO_nxtbackup;

static char *filename;
static struct instr **lastbackup;
static struct instr **outputqueue;
static struct instr **nextoutput;
static struct instr **lastoutput;
static struct instr **freeiqueue;
static struct instr **nextifree;
static struct instr **lastifree;
static char *strqueue;
static char *nextstr;
static char *laststr;

int OO_noutput;	/* number of instructions in output queue */
int OO_WSIZE;	/* wordlength */
int OO_PSIZE;	/* pointer length */

#ifdef STATS
int OO_wrstats = 1;			/* pattern statistics output */
#endif

C_init(wsize,psize)
	arith wsize, psize;
{
	allocmem();
	O_init(wsize,psize);
	OO_WSIZE = wsize;
	OO_PSIZE = psize;
}

C_open(fname)
	char *fname;
{
	filename = fname;
	return(O_open(fname));
}

C_magic()
{
	O_magic();
}

C_close()
{
	O_close();
}

PRIVATE
fatal(s,a)
	char *s;
	int a;
{
	fprint(STDERR, "%s: ", filename ? filename : "standard input");
	fprint(STDERR,s,a);
	fprint(STDERR,"\n");
	sys_stop(S_EXIT);
}

PRIVATE
allocmem()
{
	/* Allocate memory for queues on heap */
	OO_nxtpatt = OO_patternqueue =
		(struct instr **)Malloc(maxpattern*sizeof(struct instr *));
	OO_nxtbackup = OO_bkupqueue =
		(struct instr **)Malloc(MAXBACKUP*sizeof(struct instr *));
	lastbackup = OO_bkupqueue + MAXBACKUP - 1;
	nextoutput = outputqueue =
		(struct instr **)Malloc(MAXOUTPUT*sizeof(struct instr *));
	lastoutput = outputqueue + MAXOUTPUT - 1;
	OO_noutput = 0;
	nextifree = freeiqueue =
		(struct instr **)Malloc(MAXFREEI*sizeof(struct instr *));
	lastifree = freeiqueue + MAXFREEI - 1;
	nextstr = strqueue =
		(char *)Malloc(MAXSTRING*sizeof(char));
	laststr = strqueue + MAXSTRING - 1;
}

OO_free(p)
	struct instr *p;
{
	if(nextifree > lastifree) {
#ifdef DEBUG
		fprint(STDERR,"Warning: Overflow of free intr. queue.\n");
		fprint(STDERR,"Ignored free of ");
		prtinst(p);
		fprint(STDERR,"\n");
		printstate("Freea overflow");
#endif
		return;
	}
	*nextifree++ = p;
}

PRIVATE char *
freestr(s)
	char *s;
{
	char *res = nextstr;
	while(*nextstr++ = *s++);
	if(nextstr > laststr) {
		fprint(STDERR,"string space overflowed!\n");
		sys_stop(S_EXIT);
	}
	return(res);
}

OO_flush()
{
	/*
	/* Output all instructions waiting in the output queue and free their
	/* storage including the saved strings.
	*/
	struct instr **p;
#ifdef DEBUG
	printstate("Flush");
#endif
	if(OO_noutput) {
		for(p=outputqueue;p<nextoutput;p++) {
			OO_mkcalls(*p);
			OO_free(*p);
		}
		nextoutput=outputqueue;
		if(OO_nxtbackup==OO_bkupqueue)
			nextstr = strqueue;
		OO_noutput = 0;
	}
}

OO_outop(opcode)
	int opcode;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = none_ptyp;
	OO_out(p);
}

OO_inop(opcode)
	int opcode;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = none_ptyp;
	*OO_nxtpatt++ = p;
}

OO_outcst(opcode,cst)
	int opcode,cst;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = cst_ptyp;
	p->acst = cst;
	OO_out(p);
}

OO_incst(opcode,cst)
	int opcode,cst;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = cst_ptyp;
	p->acst = cst;
	*OO_nxtpatt++ = p;
}

OO_outlab(opcode,lab)
	int opcode,lab;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = cst_ptyp;
	p->acst = lab;
	OO_out(p);
}

OO_inlab(opcode,lab)
	int opcode,lab;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = cst_ptyp;
	p->acst = lab;
	*OO_nxtpatt++ = p;
}

OO_outpnam(opcode,pnam)
	int opcode;
	char *pnam;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = pro_ptyp;
	p->apnam = pnam;
	OO_out(p);
}

OO_inpnam(opcode,pnam)
	int opcode;
	char *pnam;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = pro_ptyp;
	p->apnam = freestr(pnam);
	*OO_nxtpatt++ = p;
}

OO_outdefilb(opcode,deflb)
	int opcode;
	label deflb;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = lab_ptyp;
	p->alab = deflb;
	OO_out(p);
}

OO_indefilb(opcode,deflb)
	int opcode;
	label deflb;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = lab_ptyp;
	p->alab = deflb;
	*OO_nxtpatt++ = p;
}

OO_outext(opcode,arg,soff)
	int opcode;
	struct instr *arg;
	int soff;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	switch(p->argtype = arg->argtype) {
	case cst_ptyp:
		p->acst = soff;
		break;
	case sof_ptyp:
		p->adnam = arg->adnam;
		p->asoff = soff;
		break;
	case nof_ptyp:
		p->adlb = arg->adlb;
		p->anoff = soff;
		break;
	default:
		fatal("Unexpected type %d in outext",arg->argtype);
	}
	OO_out(p);
}

OO_indnam(opcode,name,off)
	int opcode;
	char *name;
	int off;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = sof_ptyp;
	p->adnam = freestr(name);
	p->asoff = off;
	*OO_nxtpatt++ = p;
}

OO_indlb(opcode,lab,off)
	int opcode;
	label lab;
	int off;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = nof_ptyp;
	p->adlb = lab;
	p->anoff = off;
	*OO_nxtpatt++ = p;
}

OO_out(p)
	struct instr *p;
{
	/* Put the instruction p on the output queue */
	if(nextoutput > lastoutput) {
#ifdef DEBUG
		fprint(STDERR,"Warning: Overflow of outputqueue - output flushed\n");
#endif
		OO_flush();
	}
	OO_noutput++;
	*nextoutput++ = p;
}

OO_pushback(p)
	struct instr *p;
{
	/* push instr. p onto bkupqueue */
	if(OO_nxtbackup > lastbackup) {
#ifdef DEBUG
		fprint(STDERR,"Warning: Overflow of bkupqueue-backup ignored\n");
		printstate("Backup overflow");
#endif
		return;
	}
	*OO_nxtbackup++ = p;
}

OO_backup(n)
	int n;
{
	/* copy (up to) n instructions from output to backup queues */
	while(n-- && nextoutput>outputqueue) {
		OO_pushback(*(--nextoutput));
		OO_noutput--;
	}
}

OO_dodefault(numout, numcopy)
	int numout, numcopy;
{
	register struct instr **p,**q;
	q = (p = OO_patternqueue) + numout;
	while(numcopy--) {
		if(numout) {
			numout--;
			OO_out(*p);
		}
		*p++ = *q++;
	}
	OO_nxtpatt = p;
	while(numout--) OO_out(*p++);
}

#ifdef DEBUG
PRIVATE
printstate(mess)
	char *mess;
{
	struct instr **p;
	fprint(STDERR,"%s - state: ",mess);
	p = outputqueue;
	while(p<nextoutput)
		prtinst(*p++);
	fprint(STDERR," |==| ");
	p = OO_patternqueue;
	while(p<OO_nxtpatt)
		prtinst(*p++);
	fprint(STDERR," |==| ");
	p = OO_bkupqueue;
	while(p<OO_nxtbackup)
		prtinst(*p++);
	fprint(STDERR,"\n");
}

PRIVATE
prtinst(p)
	struct instr *p;
{
	switch(p->opcode) {
	default:
		fprint(STDERR,"%s",em_mnem[p->opcode-sp_fmnem]);
		break;
	case OTHER:
		fprint(STDERR,"OTHER");
		break;
	case op_lab:
		break;
	}
	switch(p->argtype) {
	case none_ptyp:
		fprint(STDERR," ");
		break;
	case cst_ptyp:
		fprint(STDERR," %d ",p->acst);
		break;
	case lab_ptyp:
		fprint(STDERR,"%d: ",p->alab);
		break;
	case nof_ptyp:
		fprint(STDERR," .%d+%d ",p->adlb,p->asoff);
		break;
	case sof_ptyp:
		fprint(STDERR," %s+%d ",p->adnam,p->asoff);
		break;
	case ilb_ptyp:
		fprint(STDERR," *%d ",p->alab);
		break;
	case pro_ptyp:
		fprint(STDERR," $%s ",p->apnam);
		break;
	default:
		fatal(" prtinst - Unregognized arg %d ",p->argtype);
	}
}
#endif
