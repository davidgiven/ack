/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <em_path.h>
#include <em_mnem.h>
#include <em_pseu.h>
#include "../share/types.h"
#include "il.h"
#include "../share/debug.h"
#include "../share/alloc.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/files.h"
#include "../share/map.h"
#include "il_aux.h"
#include "il1_anal.h"
#include "il1_aux.h"
#include "il2_aux.h"
#include "il3_change.h"
#include "il3_subst.h"
#include "../share/get.h"
#include "../share/put.h"
#include "../share/go.h"
#include <limits.h>

int calnr;
int complete_program;
calcnt_p cchead; /* call-count info of current proc */
STATIC long space = 0;
STATIC long total_size = 0;

STATIC char cname[NAME_MAX];
STATIC char ccname[NAME_MAX];
STATIC char cname2[NAME_MAX];

/* For debugging only */
STATIC char sname[NAME_MAX];
STATIC int kp_temps = 0;

int Ssubst;
#ifdef VERBOSE
int Senv, Srecursive, Slocals, Sinstrlab, Sparsefails, Spremoved, Scals;
int Sbig_caller, Sdispensable, Schangedcallee, Sbigcallee, Sspace, Szeroratio;
#endif

/* P A S S  1
 *
 * Pass 1 reads and analyses the EM text and the CFG.
 * It determines for every procedure if it may be expanded
 * in line and how it uses its formal parameters.
 * It also collects all calls appearing in the program and
 * recognizes the actual parameters of every call.
 * The call descriptors are put in a file (calfile).
 */

STATIC void pass1(const char *lnam, const char *bnam, const char *cnam)
{
	FILE* f, *gf, *cf, *ccf; /* The EM input, the basic block graph,
				  * the call-list file and the calcnt file.
				  */
	long laddr;
	bblock_p g;
	short kind;
	line_p l;

	f = openfile(lnam, "r");
	gf = openfile(bnam, "r");
	cf = openfile(cnam, "w");
	ccf = openfile(ccname, "w");
	mesregs = Lempty_set();
	apriori(fproc);
	/* use information from the procedure table to
	 * see which calls certainly cannot be expanded.
	 */
	while (TRUE)
	{
		laddr = ftell(f);
		if (!getunit(gf, f, &kind, &g, &l, &curproc, TRUE))
			break;
		/* Read the control flow graph and EM text of
		 * one procedure and analyze it.
		 */
		if (kind == LDATA)
		{
			remunit(LDATA, (proc_p)0, l);
			continue;
		}
		OUTTRACE("flow graph of proc %d read", curproc->p_id);
		assert(INSTR(g->b_start) == ps_pro);
		curproc->p_start = g;
		curproc->P_LADDR = laddr;
		/* address of em text in em-file */
		/* address of graph in basic block file */
		curproc->P_SIZE = proclength(curproc); /* #instructions */
		total_size += curproc->P_SIZE;
		if (BIG_PROC(curproc))
		{
			/* curproc is too large to be expanded in line */
			UNSUITABLE(curproc);
		}
		calnr = 0;
		anal_proc(curproc, cf, ccf);
		OUTTRACE("proc %d processed", curproc->p_id);
		remunit(LTEXT, curproc, (line_p)0);
		/* remove control flow graph + text */
		OUTTRACE("graph of proc %d removed", curproc->p_id);
		Ldeleteset(mesregs);
		mesregs = Lempty_set();
	}
	fclose(f);
	fclose(gf);
	fclose(cf);
	fclose(ccf);
}

/* P A S S  2
 *
 * Pass 2 reads the calfile and determines which calls should
 * be expanded in line. It does not use the EM text.
 */

STATIC void pass2(const char *cnam, long space)
{
	FILE* cf, *cf2, *ccf;
	call_p c, a;

	cf = openfile(cnam, "r");
	cf2 = openfile(cname2, "w");
	ccf = openfile(ccname, "r");
	while ((c = getcall(cf)) != (call_p)0)
	{
		/* process all calls */
		if (SUITABLE(c->cl_proc) && anal_params(c))
		{
			/* called proc. may be put in line */
			/* see which parameters may be put in line */
			assign_ratio(c); /* assign a rank */
			a = abstract(c); /* abstract essential info */
			append_abstract(a, a->cl_caller);
			/* put it in call-list of calling proc. */
			putcall(c, cf2, (short)0);
		}
		else
		{
			rem_call(c);
		}
	}
	select_calls(fproc, ccf, space);
	fclose(cf);
	if (!kp_temps)
		unlink(cnam);
	fclose(cf2);
	fclose(ccf);
	if (!kp_temps)
		unlink(ccname);
	cf2 = openfile(cname2, "r");
	add_actuals(fproc, cf2);
	cleancals(fproc); /* remove calls that were not selected */
	/* add actual parameters to each selected call */
	fclose(cf2);
	if (!kp_temps)
		unlink(cname2);
}

/* P A S S  3
 *
 * pass 3 reads the substitution file and performs all
 * substitutions described in that file. It reads the
 * original EM text and produced a new (optimized)
 * EM textfile.
 */

void pass3(const char *lnam, const char *lnam2)
{
	bool verbose = TRUE;
	FILE* lfile, *lfilerand, *lfile2, *sfile;
	call_p c, next;
	line_p l, startscan, cal;
	short lastcid; /* last call-id seen */

	lfile = openfile(lnam, "r");
	lfilerand = openfile(lnam, "r");
	lfile2 = openfile(lnam2, "w");
	if (verbose)
	{
		sfile = openfile(sname, "w");
	}
	mesregs = Lempty_set();
	while ((l = get_text(lfile, &curproc)) != (line_p)0)
	{
		if (curproc == (proc_p)0)
		{
			/* Just a data-unit; no real instructions */
			putlines(l->l_next, lfile2);
			oldline(l);
			continue;
		}
		if (IS_DISPENSABLE(curproc))
		{
			liquidate(curproc, l->l_next);
		}
		else
		{
			startscan = l->l_next;
			lastcid = 0;
			for (c = curproc->P_CALS; c != (call_p)0; c = next)
			{
				next = c->cl_cdr;
				cal = scan_to_cal(startscan, c->cl_id - lastcid);
				assert(cal != (line_p)0);
				startscan = scan_to_cal(cal->l_next, 1);
				/* next CAL */
				lastcid = c->cl_id;
				/* next CAL after current one */
				substitute(lfilerand, c, cal, l->l_next);
				if (verbose)
				{
					putcall(c, sfile, 0);
				}
				else
				{
					rem_call(c);
				}
			}
		}
		putlines(l->l_next, lfile2);
		Ldeleteset(mesregs);
		mesregs = Lempty_set();
		oldline(l);
	}
	fclose(lfile);
	fclose(lfile2);
	if (verbose)
	{
		fclose(sfile);
		if (!kp_temps)
			unlink(sname);
	}
}

STATIC void il_extptab(ptab)
    proc_p ptab;
{
	/* Allocate space for extension of proctable entries.
	 * Also, initialise some of the fields just allocated.
	 */

	register proc_p p;

	for (p = ptab; p != (proc_p)0; p = p->p_next)
	{
		p->p_extend = newilpx();
		p->P_ORGLABELS = p->p_nrlabels;
		p->P_ORGLOCALS = p->p_localbytes;
	}
}

STATIC void il_cleanptab(ptab)
    proc_p ptab;
{
	/* De-allocate space for extensions */

	register proc_p p;

	for (p = ptab; p != (proc_p)0; p = p->p_next)
	{
		oldilpx(p->p_extend);
	}
}

#ifdef VERBOSE
STATIC void Sdiagnostics()
{
	/* print statictical information */

	fprintf(stderr, "STATISTICS:\n");
	fprintf(stderr, "Info about procedures:\n");
	fprintf(stderr, "environment accessed: %d\n", Senv);
	fprintf(stderr, "recursive: %d\n", Srecursive);
	fprintf(stderr, "too many locals: %d\n", Slocals);
	fprintf(stderr, "instr. lab in data block: %d\n", Sinstrlab);
	fprintf(stderr, "procedures removed: %d\n", Spremoved);
	fprintf(stderr, "\nInfo about calls:\n");
	fprintf(stderr, "total number of calls: %d\n", Scals);
	fprintf(stderr, "total number of calls substituted: %d\n", Ssubst);
	fprintf(stderr, "parser failed: %d\n", Sparsefails);
	fprintf(stderr, "caller too big: %d\n", Sbig_caller);
	fprintf(stderr, "caller dispensable: %d\n", Sdispensable);
	fprintf(stderr, "callee is changed: %d\n", Schangedcallee);
	fprintf(stderr, "callee too big: %d\n", Sbigcallee);
	fprintf(stderr, "no space available: %d\n", Sspace);
	fprintf(stderr, "zero ratio: %d\n", Szeroratio);
}
#endif

void il_flags(void *vp)
{
	char *p = vp;

	switch (*p++)
	{
		case 's':
			while (*p != '\0')
			{
				space = 10 * space + *p++ - '0';
			}
			break;
		case 'a':
			complete_program = 1;
			break;
		case 't':
			strcpy(cname, ".");
			strcpy(ccname, ".");
			strcpy(sname, ".");
			strcpy(cname2, ".");
			kp_temps = 1;
			break;
	}
}

int main(argc, argv) int argc;
char* argv[];
{
	struct files* files = findfiles(argc, argv);
	FILE* f;
	char* tmpdir = getenv("TMPDIR");

	go(argc, argv, no_action, no_action, no_action, il_flags);
	il_extptab(fproc); /* add extended data structures */
	if (!tmpdir)
		tmpdir = "/tmp";

	strcpy(cname, tmpdir);
	strcpy(ccname, tmpdir);
	strcpy(sname, tmpdir);
	strcpy(cname2, tmpdir);

	strcat(cname, "/ego.i1.XXXXXX");
	strcat(ccname, "/ego.i2.XXXXXX");
	strcat(sname, "/ego.i3.XXXXXX");
	strcat(cname2, "/ego.i4.XXXXXX");

	close(mkstemp(cname));
	close(mkstemp(ccname));
	close(mkstemp(sname));
	close(mkstemp(cname2));
	pass1(files->lname_in, files->bname_in, cname); /* grep calls, analyse procedures */
	space = total_size * space / 100;
	pass2(cname, space); /* select calls to be expanded */
	pass3(files->lname_in, files->lname_out); /* do substitutions */
	f = openfile(files->dname_out, "w");
	il_cleanptab(fproc); /* remove extended data structures */
	putdtable(fdblock, f);
	f = openfile(files->pname_out, "w");
	putptable(fproc, f, FALSE);
	report("inline substitutions", Ssubst);
#ifdef VERBOSE
	if (verbose_flag)
	{
		Sdiagnostics();
	}
#endif
#ifdef DEBUG
	core_usage();
#endif
	exit(0);
}
