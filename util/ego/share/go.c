/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  S H A R E D    F I L E
 *
 *  G O . C
 *
 */


#include <stdio.h>
#include "types.h"
#include "debug.h"
#include "global.h"
#include "files.h"
#include "get.h"
#include "put.h"
#include "lset.h"
#include "map.h"
#include "alloc.h"
#include "go.h"


STATIC bool report_flag = FALSE;  /* report #optimizations found? */
#ifdef DEBUG
STATIC bool core_flag = FALSE;    /* report core usage? */
#endif


STATIC mach_init(machfile,phase_machinit)
	char *machfile;
	int (*phase_machinit)();
{
	/* Read target machine dependent information */

	FILE *f;

	f = openfile(machfile,"r");
	fscanf(f,"%d",&ws);
	fscanf(f,"%d",&ps);
	if (ws != ps && ps != 2*ws) error("illegal pointer size");
	(*phase_machinit)(f);
	fclose(f);
}



go(argc,argv,initialize,optimize,phase_machinit,proc_flag)
	int argc;
	char *argv[];
	int (*initialize)();
	int (*optimize)();
	int (*phase_machinit)();
	int (*proc_flag)();
{
	FILE *f, *gf, *f2, *gf2;  /* The EM input and output and
				 * the basic block graphs input and output
				 */
	bblock_p g;
	line_p l;
	short kind;
	int i;
	char *p;
	bool time_opt = TRUE;

	linecount = 0;
	for (i = ARGSTART; i < argc; i++) {
		p = argv[i];
		if (*p++ != '-') error("illegal argument");
		switch(*p) {
			case 'S':
				time_opt = FALSE;
				break;
			case 'T':
				time_opt = TRUE;
				break;
			case 'M':
				p++;
				mach_init(p,phase_machinit);
				break;
			case 'C':
#ifdef DEBUG
				core_flag = TRUE;
#endif
				break;
			case 'Q':
				report_flag = TRUE;
				break;
			case 'V':
				verbose_flag = TRUE;
				break;
			default:
				(*proc_flag)(p);
				break;
		}
	}
	time_space_ratio = (time_opt ? 100 : 0);
	fproc = getptable(pname); /* proc table */
	fdblock = getdtable(dname);  /* data block table */
	(*initialize)();
	if (optimize == no_action) return;
	f   = openfile(lname,"r");
	gf  = openfile(bname,"r");
	f2  = openfile(lname2,"w");
	gf2 = openfile(bname2,"w");
	mesregs = Lempty_set();
	while (getunit(gf,f,&kind,&g,&l,&curproc,TRUE)) {
		/* Read the control flow graph and EM text of
		 * one procedure and optimize it.
		 */
		if (kind == LDATA) {
			putunit(LDATA, (proc_p) 0, l, gf2, f2);
			continue;
		}
		OUTTRACE("flow graph of proc %d read",curproc->p_id);
		curproc->p_start = g;
		/* The global variable curproc points to the
		 * current procedure. It is set by getgraph
		 */
		(*optimize)(curproc);
		putunit(LTEXT,curproc,(line_p) 0,gf2,f2);
		/* output control flow graph + text */
		OUTTRACE("graph of proc %d outputted",curproc->p_id);
		Ldeleteset(mesregs);
		mesregs = Lempty_set();
	}
	fclose(f);
	fclose(f2);
	fclose(gf);
	fclose(gf2);
	f = openfile(dname2,"w");
	putdtable(fdblock,f);
	/* fclose(f); done by putdtable */
	f = openfile(pname2,"w");
	putptable(fproc,f,TRUE);
	/* fclose(f); done by putptable */
	core_usage();
}


no_action() { }

core_usage()
{
#ifdef DEBUG
	if (core_flag) {
		coreusage();
	}
#endif
}

report(s,n)
	char *s;
	int n;
{
	/* Report number of optimizations found, if report_flag is set */

	if (report_flag) {
		fprintf(stderr,"%s:  %d\n",s,n);
	}
}
