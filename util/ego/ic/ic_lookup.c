/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N T E R M E D I A T E   C O D E
 *
 *  I C _ L O O K U P . C
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <em_spec.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/map.h"
#include "ic.h"
#include "ic_io.h"
#include "ic_lookup.h"
#include "../share/alloc.h"


sym_p symhash[NSYMHASH];
prc_p prochash[NPROCHASH];
num_p numhash[NNUMHASH];
char *lastname;

#define newsym()	(sym_p) newstruct(sym)
#define newprc()	(prc_p) newstruct(prc)
#define newnum()	(num_p) newstruct(num)

#define oldsym(x)	oldstruct(sym,x)
#define oldprc(x)	oldstruct(prc,x)
#define oldnum(x)	oldstruct(num,x)

#define PF_FILE	2
#define DF_FILE	2

/* instr_lab */





lab_id instr_lab(short number)
{
	register num_p *npp, np;

	/* In EM assembly language, a label is an unsigned number,
	 * e.g. 120 in 'BRA *120'. In IC the labels of a procedure
	 * are represented by consecutive integer numbers, called
	 * lab_id. The mapping takes place here.
	 */


	npp = &numhash[number%NNUMHASH];
	while (*npp != (num_p) 0) {
		if ((*npp)->n_number == number) {
			return(*npp)->n_labid;
		} else {
			npp = &(*npp)->n_next;
		}
	}

	/* The label was not found in the hashtable, so
	 * create a new entry for it.
	 */

	*npp = np = newnum();
	np->n_number = number;
	np->n_labid = ++lastlid;
	/* Assign a new label identifier to the num struct.
	 * lastlid is reset to 0 at the beginning of
	 * every new EM procedure (by cleaninstrlabs).
	 */
	return (np->n_labid);
}



/*  symlookup */

STATIC unsigned hash(const char *string) {
	register char *p;
	register unsigned i,sum;

	for (sum=i=0,p=string;*p;i += 3)
		sum ^= (*p++)<<(i&07);
	return(sum);
}

dblock_p symlookup(const char *name, int status)
{
	/* Look up the name of a data block. The name can appear
	 * in either a defining or applied occurrence (status is
	 * DEFINING, OCCURRING resp.), or in a MES ms_ext instruction
	 * as the name of a data block imported by a library module
	 * (status is IMPORTING). Things get complicated,
	 * because a HOL pseudo need not be preceded by a
	 * data label, i.e. a hol block need not have a name.
	 */


	register sym_p *spp,  sp;
	register dblock_p dp;

	if (name == (char *) 0) {
		assert(status == DEFINING);
		dp = newdblock();
	} else {
		spp = &symhash[hash(name)%NSYMHASH];
		while (*spp != (sym_p) 0) {
			/* Every hashtable entry points to a list
			 * of synonyms (i.e. names with the same
			 * hash values). Try to find 'name' in its
			 * list.
			 */
			if (strcmp((*spp)->sy_name, name) == 0) {
				dp = (*spp)->sy_dblock;
				if (status != DEFINING ||
			    	    (dp->d_flags1 & DF_EXTERNAL) == 0) {
					dp->d_flags2 |= DF_FILE;
				}
				if (dp->d_flags2 & DF_FILE) {
					lastname = (*spp)->sy_name;
					return dp;
				}
				break;
			} else {
				spp = &(*spp)->sy_next;
			}
		}
		/* The name is not found, so create a new entry for it.
		 * However, if the status is IMPORTING, we just return 0,
		 * indicating that we don't need this name.
		 */
		if (status == IMPORTING) return (dblock_p) 0;
		sp = newsym();
		sp->sy_next = *spp;
		*spp = sp;
		sp->sy_name = (char *) newcore(strlen(name)+1);
		strcpy(sp->sy_name, name);
		lastname = sp->sy_name;		/* quick hack to get at
						   the name
						*/
		dp = sp->sy_dblock = newdblock();
	}
	if (fdblock == (dblock_p) 0) {
		fdblock = dp;
		/* first data block */
	} else {
		ldblock->d_next = dp; /* link to last dblock */
	}
	ldblock = dp;
	dp->d_pseudo	= DUNKNOWN;	/* clear all fields */
	dp->d_id	= ++lastdid;
	dp->d_size	= 0;
	dp->d_objlist	= (obj_p) 0;
	dp->d_values	= (arg_p) 0;
	dp->d_next	= (dblock_p) 0;
	dp->d_flags1	= 0;
	dp->d_flags2	= 0;
	if (status == OCCURRING) {
		/* This is the first occurrence of the identifier,
		 * so if it is a used occurrence make the
		 * identifier externally visible, else make it
		 * internal.
		 */
		dp->d_flags1 |= DF_EXTERNAL;
	}
	dp->d_flags2 |= DF_FILE;
	return dp;
}



/* getsym */

dblock_p getsym(status)
	int status;
{
	if (table2() != DLBX) {
		error("symbol expected");
	}
	return(symlookup(string,status));
}



/* getproc */

proc_p getproc(status)
	int status;
{
	if (table2() != sp_pnam) {
		error("proc name expected");
	}
	return(proclookup(string,status));
}



/* proclookup */

proc_p proclookup(const char *name, int status)
{
	register prc_p *ppp,  pp;
	register proc_p dp;

	ppp = &prochash[hash(name)%NPROCHASH];
	while (*ppp != (prc_p) 0) {
		/* Every hashtable entry points to a list
		 * of synonyms (i.e. names with the same
		 * hash values). Try to find 'name' in its
		 * list.
		 */
		if (strcmp((*ppp)->pr_name, name) == 0) {
			/* found */
			dp = (*ppp)->pr_proc;
			if (status != DEFINING ||
			    (dp->p_flags1 & PF_EXTERNAL) == 0) {
				dp->p_flags2 |= PF_FILE;
				return dp;
			}
			if (dp->p_flags2 & PF_FILE) return dp;
			break;
		} else {
			ppp = &(*ppp)->pr_next;
		}
	}
	/* The name is not found, so create a new entry for it,
	 * unless the status is IMPORTING, in which case we
	 * return 0, indicating we don't want this proc.
	 */
	if (status == IMPORTING) return (proc_p) 0;
	pp = newprc();
	pp->pr_next = *ppp;
	*ppp = pp;
	pp->pr_name = (char *) newcore(strlen(name)+1);
	strcpy(pp->pr_name, name);
	dp = pp->pr_proc = newproc();
	if (fproc == (proc_p) 0) {
		fproc = dp;  /* first proc */
	} else {
		lproc->p_next = dp;
	}
	lproc = dp;
	dp->p_id	= ++lastpid;	/* create a unique proc_id */
	dp->p_next	= (proc_p) 0;
	dp->p_flags1	= 0;
	dp->p_flags2	= 0;
	if (status == OCCURRING) {
		/* This is the first occurrence of the identifier,
		 * so if it is a used occurrence the make the
		 * identifier externally visible, else make it
		 * internal.
		 */
		dp->p_flags1 |= PF_EXTERNAL;
	}
	dp->p_flags2 |= PF_FILE;
	return dp;
}



/* cleaninstrlabs */

void cleaninstrlabs()
{
	register num_p *npp, np, next;

	for (npp = numhash; npp < &numhash[NNUMHASH]; npp++) {
		for  (np = *npp; np != (num_p) 0; np = next) {
			next = np->n_next;
			oldnum(np);
		}
		*npp = (num_p) 0;
	}
	/* Reset last label id (used by instr_lab). */
	lastlid = (lab_id) 0;
}



/* dump_procnames */

void dump_procnames(hash,n,f)
	prc_p  hash[];
	int    n;
	FILE   *f;
{
	/* Save the names of the EM procedures in file f.
	 * Note that the Optimizer Intermediate Code does not
	 * use identifiers but proc_ids, object_ids etc.
	 * The names, however, can be used after optimization
	 * is completed, to reconstruct Compact Assembly Language.
	 * The output consists of tuples (proc_id, name).
	 * This routine is called once for every input file.
	 * To prevent names of external procedures being written
	 * more than once, the PF_WRITTEN flag is used.
	 */

	register prc_p *pp, ph;
	proc_p p;

#define PF_WRITTEN 01


	for (pp = &hash[0]; pp < &hash[n]; pp++) {
		/* Traverse the entire hash table */
		for (ph = *pp; ph != (prc_p) 0; ph = ph->pr_next) {
			/* Traverse the list of synonyms */
			p = ph->pr_proc;
			if ((p->p_flags2 & PF_WRITTEN) == 0) {
				/* not been written yet */
				fprintf(f,"%d	%s\n",p->p_id, ph->pr_name);
				p->p_flags2 |= PF_WRITTEN;
			}
		}
	}
}

/* cleanprocs */

void cleanprocs(hash,n,mask)
	prc_p hash[];
	int   n,mask;
{
	/* After an EM input file has been processed, the names
	 * of those procedures that are internal (i.e. not visible
	 * outside the file they are defined in) must be removed
	 * from the procedure hash table. This is accomplished
	 * by removing the 'prc struct' from its synonym list.
	 * After the final input file has been processed, all
	 * remaining prc structs are also removed.
	 */

	register prc_p *pp, ph, x, next;

	for (pp = &hash[0]; pp < &hash[n]; pp++) {
		/* Traverse the hash table */
		x = (prc_p) 0;
		for (ph = *pp; ph != (prc_p) 0; ph = next) {
			/* Traverse the synonym list.
			 * x points to the prc struct just before ph,
			 * or is 0 if ph is the first struct of
			 * the list.
			 */
			ph->pr_proc->p_flags2 &= ~PF_FILE;
			next = ph->pr_next;
			if ((ph->pr_proc->p_flags1 & mask) == 0) {
				if (x == (prc_p) 0) {
					*pp = next;
				} else {
					x->pr_next = next;
				}
				oldprc(ph); /* delete the struct */
			} else {
				x = ph;
			}
		}
	}
}



/* dump_dblocknames */

void dump_dblocknames(hash,n,f)
	sym_p  hash[];
	int    n;
	FILE   *f;
{
	/* Save the names of the EM data blocks in file f.
	 * The output consists of tuples (dblock_id, name).
	 * This routine is called once for every input file.
	 */

	register sym_p *sp, sh;
	dblock_p d;

#define DF_WRITTEN 01


	for (sp = &hash[0]; sp < &hash[n]; sp++) {
		/* Traverse the entire hash table */
		for (sh = *sp; sh != (sym_p) 0; sh = sh->sy_next) {
			/* Traverse the list of synonyms */
			d = sh->sy_dblock;
			if ((d->d_flags2 & DF_WRITTEN) == 0) {
				/* not been written yet */
				fprintf(f,"%d	%s\n",d->d_id, sh->sy_name);
				d->d_flags2 |= DF_WRITTEN;
			}
		}
	}
}

/* cleandblocks */

void cleandblocks(hash,n,mask)
	sym_p hash[];
	int   n,mask;
{
	/* After an EM input file has been processed, the names
	 * of those data blocks that are internal must be removed.
	 */

	register sym_p *sp, sh, x, next;

	for (sp = &hash[0]; sp < &hash[n]; sp++) {
		x = (sym_p) 0;
		for (sh = *sp; sh != (sym_p) 0; sh = next) {
			next = sh->sy_next;
			sh->sy_dblock->d_flags2 &= ~DF_FILE;
			if ((sh->sy_dblock->d_flags1 & mask) == 0) {
				if (x == (sym_p) 0) {
					*sp = next;
				} else {
					x->sy_next = next;
				}
				oldsym(sh); /* delete the struct */
			} else {
				x = sh;
			}
		}
	}
}
