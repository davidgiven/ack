#ifndef NORCSID
static char rcsid2[] = "$Header$";
#endif

#include "nopt.h"
extern int OO_maxpattern;	/* Initialized from patterns in dfa.c */
#define MAXBACKUP	50
#define MAXOUTPUT	200
#define MAXFREEI	200
#define MAXSTRING	1000

extern char	em_mnem[][4];
extern char	em_pseu[][4];

p_instr		*OO_freeiqueue;
p_instr		*OO_nxtifree;
p_instr		*OO_patternqueue;
p_instr		*OO_nxtpatt;
p_instr		*OO_bkupqueue;
p_instr		*OO_nxtbackup;
p_instr		OO_OTHER;

static char	*filename;
static p_instr	*lastbackup;
static p_instr	*outputqueue;
static p_instr	*nextoutput;
static p_instr	*lastoutput;
static p_instr	*lastifree;
static char	*strqueue;
static char	*nextstr;
static char	*laststr;

int		OO_noutput;	/* number of instructions in output queue */
arith		OO_WSIZE;	/* wordlength */
arith		OO_PSIZE;	/* pointer length */

#ifdef STATS
int		OO_wrstats = 1;	/* pattern statistics output */
#endif

O_init(wsize,psize)
	arith wsize, psize;
{
	allocmem();
	C_init(wsize,psize);
	OO_WSIZE = wsize;
	OO_PSIZE = psize;
}

O_open(fname)
	char *fname;
{
	filename = fname;
	return(C_open(fname));
}

O_magic()
{
	C_magic();
}

O_close()
{
	C_close();
}

PRIVATE
fatal(s,a)
	char *s;
	int a;
{
	fprintf(stderr, "%s: ", filename ? filename : "standard input");
	fprintf(stderr,s,a);
	fprintf(stderr,"\n");
	sys_stop(S_EXIT);
}

PRIVATE
allocmem()
{
	/* Allocate memory for queues on heap */
	OO_nxtpatt = OO_patternqueue =
		(p_instr *)Malloc(OO_maxpattern*sizeof(p_instr));
	OO_nxtbackup = OO_bkupqueue =
		(p_instr *)Malloc(MAXBACKUP*sizeof(p_instr));
	lastbackup = OO_bkupqueue + MAXBACKUP - 1;
	nextoutput = outputqueue =
		(p_instr *)Malloc(MAXOUTPUT*sizeof(p_instr));
	lastoutput = outputqueue + MAXOUTPUT - 1;
	OO_noutput = 0;
	OO_nxtifree = OO_freeiqueue =
		(p_instr *)Malloc(MAXFREEI*sizeof(p_instr));
	lastifree = OO_freeiqueue + MAXFREEI - 1;
	nextstr = strqueue =
		(char *)Malloc(MAXSTRING*sizeof(char));
	laststr = strqueue + MAXSTRING - 1;
	/* allocate dummy OTHER data structure */
	OO_OTHER = (p_instr)Malloc(sizeof(struct e_instr));
	OO_OTHER->em_type = EM_MNEM;
	OO_OTHER->em_opcode = OTHER;
	OO_OTHER->em_argtype = 0;
}

OO_free(p)
	p_instr	p;
{
	if(OO_nxtifree > lastifree) {
#ifdef DEBUG
		fprintf(stderr,"Warning: Overflow of free intr. queue.\n");
		fprintf(stderr,"Ignored free of ");
		prtinst(p);
		fprintf(stderr,"\n");
		printstate("Freea overflow");
#endif
		return;
	}
	*OO_nxtifree++ = p;
}

OO_nfree(n)
	register int n;
{
	while(n--) {
		OO_free(*--OO_nxtpatt);
	}
	OO_state = 0;
}

char *
OO_freestr(str)
	char *str;
{
	register char *s = str;
	register char *res;
	while (*s++);
 again:
	if ((s-str) > (laststr-nextstr)) {
		unsigned newsize = (laststr - strqueue + 1)*2;
		res = Realloc(strqueue,newsize);
		laststr = res + newsize - 1;
		nextstr = res + (nextstr - strqueue);
		strqueue = res;
#ifdef DEBUG
		fprintf(stderr,"Warning: Reallocated string area.");
		fprintf(stderr,"New size is %d bytes\n", newsize);
#endif
		goto again;
	}
	res=nextstr;
	for(s=str;*nextstr++ = *s++;);
	return(res);
}

OO_flush()
{
	/*
	/* Output all instructions waiting in the output queue and free their
	/* storage including the saved strings.
	*/
	register int n;
	register p_instr *p;
#ifdef DEBUG
	printstate("Flush");
#endif
	if (n = OO_noutput) {
		for(p=outputqueue;n--;p++) {
			EM_mkcalls(*p);
			OO_free(*p);
		}
		nextoutput=outputqueue;
		if(OO_nxtbackup==OO_bkupqueue)
			nextstr = strqueue;
		OO_noutput = 0;
	}
}

OO_out(p)
	p_instr	p;
{
	/* Put the instruction p on the output queue */
	if(nextoutput > lastoutput) {
#ifdef DEBUG
		fprintf(stderr,"Warning: Overflow of outputqueue - output flushed\n");
#endif
		OO_flush();
	}
	OO_noutput++;
	*nextoutput++ = p;
}

OO_outop(opcode)
	int opcode;
{
	register p_instr p = GETINSTR();
	p->em_type = EM_MNEM;
	p->em_opcode = opcode;
	p->em_argtype = 0;
	OO_out(p);
}

OO_outcst(opcode,cst)
	int opcode;
	arith cst;
{
	register p_instr p = GETINSTR();
	p->em_type = EM_MNEM;
	p->em_opcode = opcode;
	p->em_argtype = cst_ptyp;
	p->em_cst = cst;
	OO_out(p);
}

OO_outlab(opcode,lab)
	int opcode;
	label lab;
{
	register p_instr p = GETINSTR();
	p->em_type = EM_MNEM;
	p->em_opcode = opcode;
	p->em_argtype = ilb_ptyp;
	p->em_ilb = lab;
	OO_out(p);
}

OO_outpnam(opcode,pnam)
	int opcode;
	char *pnam;
{
	register p_instr p = GETINSTR();
	p->em_type = EM_MNEM;
	p->em_opcode = opcode;
	p->em_argtype = pro_ptyp;
	p->em_pnam = pnam;
	OO_out(p);
}

OO_outdefilb(opcode,deflb)
	int opcode;
	label deflb;
{
	register p_instr p = GETINSTR();
	p->em_type = EM_DEFILB;
	p->em_opcode = opcode;
	p->em_argtype = 0;
	p->em_ilb = deflb;
	OO_out(p);
}

OO_outext(opcode,arg,off)
	int opcode;
	p_instr	arg;
	arith off;
{
	register p_instr p = GETINSTR();
	p->em_type = EM_MNEM;
	p->em_opcode = opcode;
	switch(p->em_argtype = arg->em_argtype) {
	case cst_ptyp:
		p->em_cst = off;
		break;
	case sof_ptyp:
		p->em_dnam = arg->em_dnam;
		p->em_off = off;
		break;
	case nof_ptyp:
		p->em_dlb = arg->em_dlb;
		p->em_off = off;
		break;
	default:
		fatal("Unexpected type %d in outext",arg->em_argtype);
	}
	OO_out(p);
}

OO_pushback(p)
	p_instr	p;
{
	/* push instr. p onto bkupqueue */
	if(OO_nxtbackup > lastbackup) {
#ifdef DEBUG
		fprintf(stderr,"Warning: Overflow of bkupqueue-backup ignored\n");
		printstate("Backup overflow");
#endif
		return;
	}
	*OO_nxtbackup++ = p;
}

OO_backup(n)
	register int n;
{
	/* copy (up to) n instructions from output to backup queues */
	while(n-- && nextoutput>outputqueue) {
		OO_pushback(*(--nextoutput));
		OO_noutput--;
	}
}

OO_dodefault(numout, numcopy)
	register int numout, numcopy;
{
	register p_instr *p, *q;
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
	p_instr	*p;
	fprintf(stderr,"%s - state: ",mess);
	p = outputqueue;
	while(p<nextoutput)
		prtinst(*p++);
	fprintf(stderr," |==| ");
	p = OO_patternqueue;
	while(p<OO_nxtpatt)
		prtinst(*p++);
	fprintf(stderr," |==| ");
	p = OO_bkupqueue;
	while(p<OO_nxtbackup)
		prtinst(*p++);
	fprintf(stderr,"\n");
}

PRIVATE
prtinst(p)
	p_instr	p;
{
	switch(p->em_type) {
	case EM_MNEM:
		if(p->em_opcode == OTHER)
			fprintf(stderr,"OTHER");
		else
			fprintf(stderr,"%s",em_mnem[p->em_opcode-sp_fmnem]);
		break;
	case EM_PSEU:
	case EM_STARTMES:
		fprintf(stderr,"%s",em_pseu[p->em_opcode-sp_fpseu]);
		break;
	case EM_MESARG:
	case EM_ENDMES:
		break;
	case EM_DEFILB:
		fprintf(stderr,"%ld", (long)p->em_ilb);
		break;
	case EM_DEFDLB:
		fprintf(stderr,"%ld", (long)p->em_dlb);
		break;
	case EM_DEFDNAM:
		fprintf(stderr,"%d", p->em_dnam);
		break;
	case EM_ERROR:
	case EM_FATAL:
	case EM_EOF:
		break;
	}
	switch(p->em_argtype) {
	case 0:
		fprintf(stderr," ");
		break;
	case cst_ptyp:
		fprintf(stderr," %d ",p->em_cst);
		break;
	case nof_ptyp:
		fprintf(stderr," .%d+%d ",p->em_dlb,p->em_off);
		break;
	case sof_ptyp:
		fprintf(stderr," %s+%d ",p->em_dnam,p->em_off);
		break;
	case ilb_ptyp:
		fprintf(stderr," *%d ",p->em_ilb);
		break;
	case pro_ptyp:
		fprintf(stderr," $%s ",p->em_pnam);
		break;
	default:
		fatal(" prtinst - Unregognized arg %d ",p->em_argtype);
	}
}
#endif
