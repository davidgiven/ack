#ifndef NORCSID
static char rcsid[]= "$Header$";
#endif

#include "param.h"
#include "reg.h"
#include "property.h"
#include "token.h"
#include "set.h"
#include "instruct.h"
#include "lookup.h"
#include <cgg_cg.h>

int wordsize;
int pointersize;
int nregs=1;
int nprops;
int ntokens=1;
int nsets;
int ninstr;
int codeindex;
int empatlen,emmnem[EMPATMAX];
int empatexpr;
int tokpatlen,tokpatset[TOKPATMAX],tokpatro[TOKPATMAX];
int nallreg,allreg[MAXALLREG];
int cursetno;
int allsetno;
int inproc=0;	/* scanning "procedure" */
int callproc=0;
int procarg[2];
int fc1=1,fc2=1,fc3=1,fc4=1;
int maxmembers=0;
int regclass=1;
int maxtokensize=0;
int rvused=0;
int nregvar[4];
int rvsize[4];
int rvnumbers[4][MAXREGVAR];

reginfo l_regs[MAXREGS];
propinfo l_props[MAXPROPS];
token_p l_tokens[MAXTOKENS];
set_t l_sets[MAXSETS];
instr_t l_instr[MAXINSTR];
symbol *symhash[NSYMHASH];
