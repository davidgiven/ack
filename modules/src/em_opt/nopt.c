#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "nopt.h"
#define MAXPATTERN	15
#define MAXBACKUP	50
#define MAXOUTPUT	200
#define MAXFREEI	200
#define MAXSTRING	1000

#define GETINSTR() (nextifree>freeiqueue)?*(--nextifree):\
	((struct instr *)Malloc(sizeof(struct instr)))

extern char em_mnem[][4];

struct instr **patternqueue, **nextpatt;
struct instr **backupqueue, **nextbackup, **lastbackup;
struct instr **outputqueue, **nextoutput, **lastoutput;
struct instr **freeiqueue,  **nextifree, **lastifree;
char *strqueue, *nextstr, *laststr;

int noutput;	/* number of instructions in output queue */
int WSIZE;	/* wordlength */
int PSIZE;	/* pointer length */

#ifdef STATS
int sflag = 1;			/* pattern statistics output */
#endif
#ifdef COLLECT
int cflag = 0;			/* internal statistics output */
#define UPDATEMAX(oldmax,n) if(cflag&&n>oldmax) oldmax=n
int numwrites = 0;
int numnextems = 0;
int numpushs = 0;
int numbackups = 0;
int numflushes = 0;
int numfrees = 0;
int numdefaults = 0;
int highestbackup = 0, totalbackup = 0;
int highestoutput = 0, totaloutput = 0;
int highestfreei = 0, totalfreei = 0;
int higheststr = 0, totalstr = 0;
#endif

C_init(wsize,psize)
	arith wsize, psize;
{
	allocmem();
	O_init(wsize,psize);
	WSIZE = wsize;
	PSIZE = psize;
}

C_open(fname)
	char *fname;
{
	return(O_open(fname));
}

C_magic()
{
	O_magic();
}

C_close()
{
#ifdef COLLECT
	if(cflag)
		outputstats();
#endif
	O_close();
}

PRIVATE
allocmem()
{
	/* Allocate memory for queues on heap */
	nextpatt = patternqueue =
		(struct instr **)Malloc(MAXPATTERN*sizeof(struct instr *));
	nextbackup = backupqueue =
		(struct instr **)Malloc(MAXBACKUP*sizeof(struct instr *));
	lastbackup = backupqueue + MAXBACKUP - 1;
	nextoutput = outputqueue =
		(struct instr **)Malloc(MAXOUTPUT*sizeof(struct instr *));
	lastoutput = outputqueue + MAXOUTPUT - 1;
	noutput = 0;
	nextifree = freeiqueue =
		(struct instr **)Malloc(MAXFREEI*sizeof(struct instr *));
	lastifree = freeiqueue + MAXFREEI - 1;
	nextstr = strqueue =
		(char *)Malloc(MAXSTRING*sizeof(char));
	laststr = strqueue + MAXSTRING - 1;
}

#ifdef COLLECT
PRIVATE
outputstats()
{
	int i;
	fprint(STDERR,"Total of %d instructions read, %d written\n",numreads,numwrites);
	fprint(STDERR,"Total of %d calls to nextem\n",numnextems);
	fprint(STDERR,"Total of %d calls to flush\n",numflushes);
	fprint(STDERR,"Total of %d calls to myfree\n",numfrees);
	fprint(STDERR,"Total of %d instructions pushed back\n",numpushs-numbackups);
	fprint(STDERR,"Total of %d instructions backed up\n",numbackups);
	fprint(STDERR,"Total of %d calls to dodefault\n",numdefaults);
	fprint(STDERR,"Max of highestbackup\t%d\t(%d)\t",highestbackup,MAXBACKUP);
	printav(totalbackup);
	fprint(STDERR,"Max of highestoutput\t%d\t(%d)\t",highestoutput,MAXOUTPUT);
	printav(totaloutput);
	fprint(STDERR,"Max of highestfreei\t%d\t(%d)\t",highestfreei,MAXFREEI);
	printav(totalfreei);
	fprint(STDERR,"Max of higheststr\t%d\t(%d)\t",higheststr,MAXSTRING);
	printav(totalstr);
}

PRIVATE
printav(n)
	int n;
{
	fprint(STDERR,"Av.\t%d.%d\n",n/numflushes,(n*10)%numflushes);
}
#endif

myfree(p)
	struct instr *p;
{
#ifdef DEBUG
	fprint(STDERR,"about to free ");
	prtinst(p);
	fprint(STDERR,"\n");
#endif
#ifdef COLLECT
	if(cflag)
		numfrees++;
#endif
	if(nextifree > lastifree) {
#ifdef DEBUG
		fprint(STDERR,"Warning: Overflow of freeiqueue-free arg ignored\n");
		printstate("Freea overflow");
#endif
		return;
	}
	*nextifree++ = p;
#ifdef COLLECT
	UPDATEMAX(highestfreei,nextifree-freeiqueue);
#endif
}

nfree(n)
{
	while(n--)
		myfree(*--nextpatt);
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
#ifdef COLLECT
	UPDATEMAX(higheststr,nextstr-strqueue);
#endif
	return(res);
}

nextem()
{
	/*
	/* Return the next instruction from backup queue else 0.
	*/
#ifdef COLLECT
	if(cflag)
		numnextems++;
#endif
	if(nextbackup>backupqueue)
		return((*nextpatt++ = *(--nextbackup))->opcode);
	return(0);
}

flush()
{
	/*
	/* Output all instructions waiting in the output queue and free their
	/* storage including the saved strings.
	*/
	struct instr **p;
#ifdef DEBUG
	printstate("Flush");
#endif
#ifdef COLLECT
	if(cflag) {
		numflushes++;
		totaloutput += nextoutput-outputqueue;
		totalbackup += nextbackup-backupqueue;
		totalfreei += nextifree-freeiqueue;
		totalstr += nextstr-strqueue;
	}
#endif
	if(noutput) {
		for(p=outputqueue;p<nextoutput;p++) {
#ifdef COLLECT
			if(cflag)
				numwrites++;
#endif
			mkcalls(*p);
			myfree(*p);
		}
		nextoutput=outputqueue;
		if(nextbackup==backupqueue)
			nextstr = strqueue;
		noutput = 0;
	}
}

outop(opcode)
	int opcode;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = none_ptyp;
	output(p);
}

inop(opcode)
	int opcode;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = none_ptyp;
	*nextpatt++ = p;
}

outcst(opcode,cst)
	int opcode,cst;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = cst_ptyp;
	p->acst = cst;
	output(p);
}

incst(opcode,cst)
	int opcode,cst;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = cst_ptyp;
	p->acst = cst;
	*nextpatt++ = p;
}

outlab(opcode,lab)
	int opcode,lab;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = cst_ptyp;
	p->acst = lab;
	output(p);
}

inlab(opcode,lab)
	int opcode,lab;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = cst_ptyp;
	p->acst = lab;
	*nextpatt++ = p;
}

outpnam(opcode,pnam)
	int opcode;
	char *pnam;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = pro_ptyp;
	p->apnam = pnam;
	output(p);
}

inpnam(opcode,pnam)
	int opcode;
	char *pnam;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = pro_ptyp;
	p->apnam = freestr(pnam);
	*nextpatt++ = p;
}

outdefilb(opcode,deflb)
	int opcode;
	label deflb;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = lab_ptyp;
	p->alab = deflb;
	output(p);
}

indefilb(opcode,deflb)
	int opcode;
	label deflb;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = lab_ptyp;
	p->alab = deflb;
	*nextpatt++ = p;
}

outext(opcode,arg,soff)
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
	output(p);
}

indnam(opcode,name,off)
	int opcode;
	char *name;
	int off;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = sof_ptyp;
	p->adnam = freestr(name);
	p->asoff = off;
	*nextpatt++ = p;
}

indlb(opcode,lab,off)
	int opcode;
	label lab;
	int off;
{
	register struct instr *p = GETINSTR();
	p->opcode = opcode;
	p->argtype = nof_ptyp;
	p->adlb = lab;
	p->anoff = off;
	*nextpatt++ = p;
}

output(p)
	struct instr *p;
{
	/* Put the instruction p on the output queue */
	if(nextoutput > lastoutput) {
#ifdef DEBUG
		fprint(STDERR,"Overflow of outputqueue - output flushed\n");
		printstate("Output overflow");
#endif
		flush();
	}
	noutput++;
	*nextoutput++ = p;
#ifdef COLLECT
	UPDATEMAX(highestoutput,nextoutput-outputqueue);
#endif
}

pushback(p)
	struct instr *p;
{
	/* push instr. p onto backupqueue */
	if(nextbackup > lastbackup) {
#ifdef DEBUG
		fprint(STDERR,"Warning: Overflow of backupqueue-backup ignored\n");
		printstate("Backup overflow");
#endif
		return;
	}
	*nextbackup++ = p;
#ifdef COLLECT
	UPDATEMAX(highestbackup,nextbackup-backupqueue);
	numpushs++;
#endif
}

backup(n)
	int n;
{
	/* copy (up to) n instructions from output to backup queues */
	while(n-- && nextoutput>outputqueue) {
#ifdef COLLECT
		if(cflag)
			numbackups++;
#endif
		pushback(*(--nextoutput));
		noutput--;
	}
}

dodefault(numout, numcopy)
	int numout, numcopy;
{
	register struct instr **p,**q;
	q = (p = patternqueue) + numout;
	while(numcopy--) {
		if(numout) {
			numout--;
			output(*p);
		}
		*p++ = *q++;
	}
	nextpatt = p;
	while(numout--) output(*p++);
#ifdef COLLECT
	if(cflag)
		numdefaults++;
#endif
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
	p = patternqueue;
	while(p<nextpatt)
		prtinst(*p++);
	fprint(STDERR," |==| ");
	p = backupqueue;
	while(p<nextbackup)
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
