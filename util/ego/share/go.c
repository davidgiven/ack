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
#include <unistd.h>
#include "types.h"
#include "debug.h"
#include "global.h"
#include "get.h"
#include "put.h"
#include "lset.h"
#include "map.h"
#include "alloc.h"
#include "go.h"
#include "files.h"
#include "mach.h"

static bool report_flag = false; /* report #optimizations found? */
#ifdef DEBUG
static bool core_flag = false; /* report core usage? */
#endif

static void mach_init(char* machfile, void (*phase_machinit)(config_t*))
{
	/* Read target machine dependent information */

	config_t config;

	config_init(&config);
	if (!config_read_file(&config, machfile))
	{
		/* Keep the diagnostic valid until error() terminates the process. */
		error("%s:%d: %s", machfile, config_error_line(&config), config_error_text(&config));
	}
	ws = mach_int(config_root_setting(&config), "word_size");
	ps = mach_int(config_root_setting(&config), "pointer_size");
	if (ws <= 0 || (ws != ps && ps != 2 * ws))
		error("illegal pointer size");
	(*phase_machinit)(&config);
	config_destroy(&config);
}

void go(
    int argc,
    char* const* argv,
    void (*initialize)(void*),
    void (*optimize)(void*),
    void (*phase_machinit)(config_t*),
    void (*proc_flag)(void*))
{
	struct files* files = findfiles(argc, argv);
	FILE *f, *gf, *f2, *gf2; /* The EM input and output and
	                          * the basic block graphs input and output
	                          */
	bblock_p g;
	line_p l;
	short kind;
	int i;
	char* p;
	bool time_opt = true;
	char* machfile = NULL;

	linecount = 0;
	opterr = 0;
	for (;;)
	{
		int opt = getopt(files->argc, files->argv, "STM:CQV");
		if (opt == -1)
			break;

		switch (opt)
		{
			case 'S':
				time_opt = false;
				break;

			case 'T':
				time_opt = true;
				break;

			case 'M':
				machfile = optarg;
				break;

			case 'C':
#ifdef DEBUG
				core_flag = true;
#endif
				break;

			case 'Q':
				report_flag = true;
				break;

			case 'V':
				verbose_flag = true;
				break;

			case '?':
				proc_flag(argv[optind - 1]);
				break;
		}
	}
	time_space_ratio = (time_opt ? 100 : 0);
	if (machfile)
		mach_init(machfile, phase_machinit);
	fproc = getptable(files->pname_in); /* proc table */
	fdblock = getdtable(files->dname_in); /* data block table */
	(*initialize)(NULL);
	if (optimize == no_action)
		return;
	f = openfile(files->lname_in, "rb");
	gf = openfile(files->bname_in, "rb");
	f2 = openfile(files->lname_out, "wb");
	gf2 = openfile(files->bname_out, "wb");
	mesregs = Lempty_set();
	while (getunit(gf, f, &kind, &g, &l, &curproc, true))
	{
		/* Read the control flow graph and EM text of
		 * one procedure and optimize it.
		 */
		if (kind == LDATA)
		{
			putunit(LDATA, (proc_p)0, l, gf2, f2);
			continue;
		}
		OUTTRACE("flow graph of proc %d read", curproc->p_id);
		curproc->p_start = g;
		/* The global variable curproc points to the
		 * current procedure. It is set by getgraph
		 */
		(*optimize)(curproc);
		putunit(LTEXT, curproc, (line_p)0, gf2, f2);
		/* output control flow graph + text */
		OUTTRACE("graph of proc %d outputted", curproc->p_id);
		Ldeleteset(mesregs);
		mesregs = Lempty_set();
	}
	fclose(f);
	fclose(f2);
	fclose(gf);
	fclose(gf2);
	f = openfile(files->dname_out, "wb");
	putdtable(fdblock, f);
	/* fclose(f); done by putdtable */
	f = openfile(files->pname_out, "wb");
	putptable(fproc, f, true);
	/* fclose(f); done by putptable */
	core_usage();
}

/* ARGSUSED */
void no_action(void* vp)
{
}

void core_usage(void)
{
#ifdef DEBUG
	if (core_flag)
	{
		coreusage();
	}
#endif
}

void report(char* s, int n)
{
	/* Report number of optimizations found, if report_flag is set */

	if (report_flag)
	{
		fprintf(stderr, "%s:  %d\n", s, n);
	}
}

void no_machinit(config_t* config)
{
}
