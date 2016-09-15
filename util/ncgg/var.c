/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
static char rcsid[]= "$Id$";
#endif

#include "param.h"
#include "reg.h"
#include "property.h"
#include "token.h"
#include "set.h"
#include "instruct.h"
#include "lookup.h"
#include "extern.h"
#include <cgg_cg.h>

int wordsize;
int pointersize;
int nregs=1;
int nprops;
int ntokens=1;
int nsets;
int ninstr;
int codeindex;
int empatlen,emrepllen,emmnem[EMPATMAX];
int maxempatlen, maxemrepllen;
int empatexpr;
int tokpatlen,tokrepllen,tokpatset[TOKPATMAX],tokpatro[TOKPATMAX];
int maxtokpatlen, maxtokrepllen;
int nallreg,allreg[MAXALLREG];
int cursetno = -1;
int allsetno;
int inproc=0;	/* scanning "procedure" */
int callproc=0;
int fc1=1,fc2=1,fc3=1,fc4=1;
int maxmembers=0;
int regclass=1;
int maxtokensize=0;
int rvused=0;
int nregvar[4];
int rvsize[4];
int rvnumbers[4][MAXREGVAR];
int procarg[MAXPROCARG];
int maxprocargs, nprocargs;

reginfo l_regs[MAXREGS];
propinfo l_props[MAXPROPS];
token_p l_tokens[MAXTOKENS];
set_t l_sets[MAXSETS];
instr_t l_instr[MAXINSTR];
symbol *symhash[NSYMHASH];
