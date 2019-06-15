#ifndef NORCSID
static char rcsid2[] = "$Id$";
#endif

#include "nopt.h"

extern struct dfa OO_checknext[]; /* Initialized in dfa.c */
extern struct dfa *OO_base[]; /* Initialized in dfa.c */
extern struct dodefault OO_default[]; /* Initialized in dfa.c */
extern int OO_maxpattern; /* Initialized in dfa.c */
extern int OO_maxreplacement; /* Initialized in dfa.c */
extern int (*OO_ftrans[])(); /* Initialized in trans.c */

extern char em_mnem[][4];
extern char em_pseu[][4];

int OO_state = 0;

p_instr OO_buffer;
p_instr OO_patternqueue;
p_instr OO_nxtpatt;
p_instr OO_endbackup;
p_instr OO_nxtrepl;
static p_instr OO_replqueue;

static char *filename;
static char *strqueue;
static char *nextstr;
static char *laststr;

arith OO_WSIZE; /* wordlength */
arith OO_DWSIZE; /* 2*wordlength */
arith OO_PSIZE; /* pointer length */

#ifdef STATS
int OO_wrstats = 1; /* pattern statistics output */
#endif
#ifdef DEBUG
#define printstate(s) dumpstate(s)
#else
#define printstate(s)
#endif /* DEBUG */

/**** WHICH IS FASTER? ****
 #define BTSCPY(pp,qq,i,p,q,n) btscpy(p,q,(n)*sizeof(struct e_instr))
 **************************/
#define BTSCPY(pp,qq,i,p,q,n) for(pp=(p),qq=(q),i=(n);i--;*pp++ = *qq++)

PRIVATE void allocmem(void);

void O_init(arith wsize, arith psize)
{
	allocmem();
	C_init(wsize, psize);
	OO_WSIZE = wsize;
	OO_DWSIZE = 2 * wsize;
	OO_PSIZE = psize;
}

int O_open(char *fname)
{
	filename = fname;
	return (C_open(fname));
}

void O_magic(void)
{
	C_magic();
}

void O_close(void)
{
	C_close();
}

void OO_dfa(register int last)
{
	register struct dfa *b;
	register struct dodefault *d;
	register int (*f)();
	for (;;)
	{
		printstate("OO_dfa");
		if ((b = OO_base[OO_state]) && ((b += last)->check == OO_state))
		{
			if ((f = OO_ftrans[OO_state = b->next]))
				(*f)();
		}
		else if (OO_state)
		{
			/* consult default entry */
			d = &OO_default[OO_state];
			if (!OO_endbackup)
				OO_endbackup = OO_nxtpatt;
			OO_nxtpatt--;
			OO_patternqueue += d->numout;
			if ((f = OO_ftrans[OO_state = d->next]))
				(*f)();
		}
		else
			OO_flush();
		if (!OO_endbackup)
			return;
		last = (OO_nxtpatt++)->em_opcode;
		if (OO_nxtpatt >= OO_endbackup)
			OO_endbackup = 0;
	}
}

PRIVATE void fatal(s, a)
	char *s;int a;
{
	fprint(STDERR, "%s: ", filename ? filename : "standard input");
	fprint(STDERR, s, a);
	fprint(STDERR, "\n");
	sys_stop(S_EXIT);
}

PRIVATE void allocmem(void)
{
	/* Allocate memory for queues on heap */
	OO_buffer = (p_instr) Malloc(
			(unsigned) (MAXBUFFER * sizeof(struct e_instr)));
	OO_patternqueue = OO_nxtpatt = OO_buffer;
	OO_replqueue = (p_instr) Malloc(
			(unsigned) OO_maxreplacement * sizeof(struct e_instr));
	OO_nxtrepl = OO_replqueue;
	nextstr = strqueue = (char *) Malloc(MAXSTRING * sizeof(char));
	laststr = strqueue + MAXSTRING - 1;
}

char * OO_freestr(char *str)
{
	register char *s = str;
	register char *res;
	while (*s++)
		;
	again: if ((s - str) > (laststr - nextstr))
	{
		unsigned newsize = (laststr - strqueue + 1) * 2;
		res = Realloc(strqueue, newsize);
		laststr = res + newsize - 1;
		nextstr = res + (nextstr - strqueue);
		strqueue = res;
#ifdef DEBUG
		fprintf(stderr,"Warning: Reallocated string area.");
		fprintf(stderr,"New size is %d bytes\n", newsize);
#endif
		goto again;
	}
	res = nextstr;
	for (s = str; (*nextstr++ = *s++);)
		;
	return (res);
}

void OO_flush(void)
{
	/*
	 	 Output all instructions waiting in the output queue and free their
	 	 storage including the saved strings.
	 */
	register p_instr p, q;
	register int i, n;
	printstate("Flush");
	for (p = OO_buffer; p < OO_patternqueue; p++)
		C_out(p);
	if (p->em_opcode != OTHER)
		C_out(p);
	if (OO_endbackup)
	{
		n = OO_endbackup - OO_nxtpatt;
		BTSCPY(p, q, i, OO_buffer, OO_nxtpatt, n);
		OO_endbackup = OO_buffer + n;
	}
	else
		nextstr = strqueue;
	OO_patternqueue = OO_nxtpatt = OO_buffer;
}

p_instr OO_halfflush(void)
{
	/*
	 	 Called when buffer full, flush half the buffer and move the
	 	 the pattern pointers to the new positions. Return a pointer
	 	 to the new nxtpatt position and increment it.
	 	 Note that OO_endbackup is always NIL (i.e. there are no
	 	 instructions on the backup queue) when this is invoked.
	 */
	register int i, n;
	register p_instr p, q;
	printstate("Half flush");
	n = MAXBUFFER / 2;
	for (p = OO_buffer, i = n; i--;)
		C_out(p++);
	/* now copy the rest of buffer and pattern back */
	BTSCPY(p, q, i, OO_buffer, OO_buffer + n, (OO_nxtpatt - OO_buffer) - n);
	OO_patternqueue -= n;
	OO_nxtpatt -= n;
	printstate("after Half flush");
	return (OO_nxtpatt++);
}

void OO_mkext(register p_instr p, int opcode, p_instr arg, arith off)
{
	switch (arg->em_argtype)
	{
		case cst_ptyp:
			EM_mkcst(p, opcode, off);
			break;
		case sof_ptyp:
			EM_mksof(p, opcode, arg->em_dnam, off);
			break;
		case nof_ptyp:
			EM_mknof(p, opcode, arg->em_dlb, off);
			break;
		default:
			fatal("Unexpected type %d in outext", arg->em_argtype);
	}
}

void OO_mkrepl(int lrepl, int diff, int numbkup)
{
	/* copy the replacement queue into the buffer queue */
	/* then move the pattern queue back n places */
	register p_instr p, q;
	register int i;
	printstate("Before backup");
	if (OO_endbackup)
	{
		/* move the region between OO_nxtpatt and OO_endbackup */
		if (diff > 0)
		{
			/* move left by diff */
			BTSCPY(p, q, i, OO_nxtpatt - diff, OO_nxtpatt,
					OO_endbackup - OO_nxtpatt);
			OO_nxtpatt -= diff;
			OO_endbackup -= diff;
		}
		else if (diff < 0)
		{
			/* move right by diff */
			/* careful of overflowing buffer!! */
			if ((OO_nxtpatt - diff) > (OO_buffer + MAXBUFFER))
				OO_halfflush();
			/* cannot use btscpy as strings may overlap */
			p = (q = OO_endbackup - 1) - diff;
			while (q >= OO_nxtpatt)
				*p-- = *q--;
			OO_nxtpatt -= diff;
			OO_endbackup -= diff;
		}
	}
	/* copy the replacement */
	if (lrepl)
	{
		BTSCPY(p, q, i, OO_patternqueue, OO_replqueue, lrepl);
		OO_nxtrepl = OO_replqueue;
		OO_patternqueue += lrepl;
	}
	/* now move the position of interest back nunbkup instructions */
	if ((OO_patternqueue - OO_buffer) < numbkup)
		numbkup = (OO_patternqueue - OO_buffer);
	OO_nxtpatt = OO_patternqueue -= numbkup;
	if (!OO_endbackup && numbkup)
		OO_endbackup = OO_patternqueue + numbkup;
	OO_state = 0;
	printstate("After backup");
}

#ifdef DEBUG
void
dumpstate(char *mess)
{
	p_instr p;
	fprintf(stderr,"%s - state(%d): ",mess,OO_state);
	p = OO_buffer;
	while(p<OO_patternqueue)
	prtinst(p++);
	fprintf(stderr," |==| ");
	while(p<OO_nxtpatt)
	prtinst(p++);
	fprintf(stderr," |==| ");
	if(OO_endbackup)
	{
		while(p<OO_endbackup)
		prtinst(p++);
	}
	fprintf(stderr,"\n");
}

void
prtinst(p_instr p)
{
	switch(p->em_type)
	{
		case EM_MNEM:
		fprintf(stderr,"%s ",em_mnem[p->em_opcode-sp_fmnem]);
		break;
		case EM_PSEU:
		fprintf(stderr,"%s ",em_pseu[p->em_opcode-sp_fpseu]);
		break;
		case EM_STARTMES:
		case EM_MESARG:
		case EM_ENDMES:
		fprintf(stderr,"MES ");
		break;
		case EM_DEFILB:
		fprintf(stderr,"%ld ", (long)p->em_ilb);
		return;
		case EM_DEFDLB:
		fprintf(stderr,"%ld ", (long)p->em_dlb);
		return;
		case EM_DEFDNAM:
		fprintf(stderr,"%d ", p->em_dnam);
		return;
		case EM_ERROR:
		case EM_FATAL:
		case EM_EOF:
		return;
	}
	switch(p->em_argtype)
	{
		case 0:
		break;
		case cst_ptyp:
		fprintf(stderr,"%d ",p->em_cst);
		break;
		case nof_ptyp:
		fprintf(stderr,".%d+%d ",p->em_dlb,p->em_off);
		break;
		case sof_ptyp:
		fprintf(stderr,"%s+%d ",p->em_dnam,p->em_off);
		break;
		case ilb_ptyp:
		fprintf(stderr,"*%d ",p->em_ilb);
		break;
		case pro_ptyp:
		fprintf(stderr,"$%s ",p->em_pnam);
		break;
		case str_ptyp:
		case ico_ptyp:
		case uco_ptyp:
		fprintf(stderr,"\"%s\"",p->em_string);
		break;
		default:
		fatal(" prtinst - Unregognized arg %d ",p->em_argtype);
	}
}
#endif
