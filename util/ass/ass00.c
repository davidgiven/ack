#include		<string.h>
#include		<stddef.h>
#include        "ass00.h"
#include        "assex.h"
#include		"assci.h"
#include		"asscm.h"
#include		"assrl.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

char oflag;
static int memflg;

/* Forward declarations. */
static siz_t* getsizes(char *);
static void getcore(void);
static void argument(char *);
static void flags(char *);
static void skipentry(void);
static void enmd_pro(void);
static void enmd_glo(void);
static void archive(void);
static void finish_up(void);
static void check_def(void);

static void c_print(void);
static void c_dprint(char *, char*);

/* External definitions */
void pass_3(void);
void pass_4(void);
void pass_5(void);

/*
 ** Main routine of EM1-assembler/loader
 */

int main(int argc, char **argv)
{
	/*
	 * Usage: ass [-[d][p][m][u][U]] [-s(s/m/l/x)] [ [file] [flag] ] ...
	 *   The d flag can be repeated several times, resulting in more
	 *        debugging information.
	 */
	char workspace[6000];
	register char *cp;
	register int argno;

	progname = argv[0];
	for (cp = argv[0]; *cp;)
		if (*cp++ == '/')
			progname = cp;
	for (argno = 1; argno < argc; argno++)
	{
		if (argv[argno][0] == '-' && LC(argv[argno][1]) == 's')
		{
			oursize = getsizes(&argv[argno][2]);
			break;
		}
	}
	/* A piece of the interpreter's stack frame is used as
	 free area initially */
	freearea((area_t) workspace, (unsigned) sizeof workspace);
	getcore();
	init_files();
	init_vars();
	while (--argc)
		argument(*++argv);
	finish_up();
	exit(nerrors != 0);
}

static void getcore(void)
{
	register siz_t *p;
	siz_t bytes;
	register unsigned n;
	register char *base;

	/*
	 * xglobs[] should be located in front of mglobs[], see upd_reloc()
	 */

	p = oursize;
	n = 0;
	n += (bytes.n_glab = p->n_glab * (sizeof *xglobs));
	n += (bytes.n_mlab = p->n_mlab * (sizeof *mglobs));
	n += (bytes.n_mproc = p->n_mproc * (sizeof *mprocs));
	n += (bytes.n_xproc = p->n_xproc * (sizeof *xprocs));
	n += (bytes.n_proc = p->n_proc * (sizeof *proctab));
	base = getarea(n);
	memset(base, 0, n);
	xglobs = gbp_cast base;
	base += bytes.n_glab;
	mglobs = gbp_cast base;
	base += bytes.n_mlab;
	mprocs = prp_cast base;
	base += bytes.n_mproc;
	xprocs = prp_cast base;
	base += bytes.n_xproc;
	proctab = ptp_cast base;
	base += bytes.n_proc;
}

static siz_t* getsizes(char *str)
{
	/*
	 * accepts -ss (small), -sm (medium), -sl (large), -sx (extra large)
	 */

	switch (LC(*str))
	{
	default:
		error("bad size option %s", str);
	case 's':
		return &sizes[0];
		break;
	case 'm':
		return &sizes[1];
		break;
	case 'l':
		return &sizes[2];
		break;
	case 'x':
		return &sizes[3];
		break;
	}
}

/*
 * This routine decides what to do with each argument.
 * It recognises flags and modules.
 * Furthermore, it knows a library when it sees it and
 * call archive() to split it apart.
 */
static void argument(char *arg)
{
	register int w;

	if (oflag)
	{
		eout = arg;
		oflag = 0;
		return;
	}
	if (*arg == '-')
	{
		flags(arg);
		return;
	}
	curfile = arg; /* for error messages etc. */
	if ((ifile = fopen(arg, "rb")) == NULL)
	{
		error("can't open %s", arg);
		return;
	}
	inpoff = 2;
	if ((w = getu16()) == sp_magic)
		read_compact();
	else if (w == ARMAG || w == AALMAG)
	{
		archmode = TRUE;
		archive();
		archmode = FALSE;
	}
	else
		error("%s: bad format", arg);
	if (fclose(ifile) == EOF)
	{
	}
}

/*
 ** process flag arguments
 */
static void flags(char *arg)
{
	register char *argp;
	register int on;

	argp = arg;
	while (*++argp)
	{
		switch (LC(*argp))
		{
		case 'd':
			d_flag++;
			break;
		case 'r':
			r_flag++;
			break;
		case 's':
			return; /* s-flag is already scanned */
#ifdef MEMUSE
		case 'm':
			memflg++;
			break;
#endif
		case 'p':
			++procflag;
			break;
#ifdef DUMP
		case 'u':
			++c_flag;
			break;
#endif
		case 'o':
			++oflag;
			break;
		case 'w':
			++wflag;
			break;
#ifdef JOHAN
		case 'j':
			++jflag;
			break;
#endif
		case 'U':
			++Uflag;
			break;
		case '-':
		case '+':
			on = (*argp == '+');
			while (*++argp)
				switch (LC(*argp))
				{
				case 't':
					if (on)
						intflags |= 01;
					else
						intflags &= ~01;
					break;
				case 'p':
					if (on)
						intflags |= 02;
					else
						intflags &= ~02;
					break;
				case 'f':
					if (on)
						intflags |= 04;
					else
						intflags &= ~04;
					break;
				case 'c':
					if (on)
						intflags |= 010;
					else
						intflags &= ~010;
				case 'e':
					if (on)
						intflags |= 040;
					else
						intflags &= ~040;
					break;
				default:
					error("bad interpreter option %s", argp);
				}
			--argp;
			break;
		default:
			error("bad flag %s", argp);
			break;
		}
	}
}

void do_proc(void)
{
	/* One procedure has been read and will be processed.
	 *
	 * NOTE: The numbers of the passes, 1 3 4 and 5, are a remainder
	 *       of ancient times.
	 */

	dump(1);
	if (memflg > 2)
		memuse();
	pass_3();
	dump(3);
	pass_4();
	dump(4);
	pass_5();
	if (memflg > 2)
		memuse();
	endproc();
	if (memflg > 1)
		memuse();
}

static void archive(void)
{
	register int i;
	register char *p;

	/*
	 * Read a library.
	 * The format of the libary used is that of a UNIX/V7(PDP)-archive.
	 *
	 * NOTE: If it was allowed for an archive to contain
	 *       obligatory modules as well as optionals,
	 *       it would not be possible to speed up things a bit
	 *       by stopping when all references are resolved.
	 *       This is the only reason.
	 */

	for (;;)
	{
		if (unresolved == 0)
		{ /* no use for this library anymore */
			return;
		}
		p = chp_cast &archhdr;
		if ((i = fgetc(ifile)) == EOF)
		{
			return;
		}
		*p++ = i;
		for (i = 1; i < sizeof archhdr.ar_name; i++)
			*p++ = get8();
		for (i = 0; i < 8; i++)
			get8();
		archhdr.ar_size = ((long) get16() << 16);
		archhdr.ar_size += getu16();
		inpoff = 0;
		libeof = archhdr.ar_size;
		/*
		 * UNIX archiveheader is read now, now process the contents
		 * of it. Note that recursive archives are not implemented.
		 *
		 * The variable libeof is used by get8() to check
		 * whether or not we try to pass the library-boundary.
		 */
		if (getu16() == sp_magic)
		{
			read_compact();
		}
		else
			error("bad archive entry");
		skipentry();
		libeof = 0;
	} /* up to the next entry */
}

static void skipentry(void)
{

	/*
	 * for some reason the rest of this library entry needs to be
	 * skipped. Do that now.
	 */
	while (inpoff < libeof)
		get8();
	if (odd(libeof)) /* archive entries are evensized */
		if (fgetc(ifile) == EOF) /* except maybe the last one */
			;
}

void init_vars(void)
{

	/*
	 * A small collection of variables is initialized.
	 * This occurs only for those that couldn't be initialized
	 * at compile-time.
	 */

}

void init_files(void)
{

	/*
	 * The temporary files on which text and data are kept
	 * during assembly are set up here.
	 */

	/*
	 * The function tmpfil() returns a file-descriptor
	 * of a file that is valid for reading and writing.
	 * It has the nice property of generating truly unique names.
	 */

	tfile = fopen(tmpfil(), "wb+");
	dfile = fopen(tmpfil(), "wb+");
	rtfile = fopen(tmpfil(), "wb+");
	rdfile = fopen(tmpfil(), "wb+");
}

void initproc(void)
{

	/*
	 * Called at the start of assembly of every procedure.
	 */

	stat_t *prevstate;

	prevstate = pst_cast getarea(sizeof pstate);
	*prevstate = pstate;
	pstate.s_prevstat = prevstate;
	pstate.s_curpro = prp_cast 0;
	pstate.s_fline = lnp_cast 0;
	pstate.s_fdata = l_data;
	pstate.s_locl = (locl_t (*)[]) getarea(
	LOCLABSIZE * sizeof((*(pstate.s_locl))[0]));
	memset(chp_cast pstate.s_locl, 0,
			LOCLABSIZE * (unsigned) sizeof((*(pstate.s_locl))[0]));
	if (memflg > 2)
		memuse();
}

void endproc(void)
{
	/* Throw the contents of the line and local label table away */
	register line_t *lnp1;
	register locl_t *lbhead, *lbp, *lbp_next;
	register int kind;
	register stat_t *prevstate;

	while ((lnp1 = pstate.s_fline) != NULL)
	{
		pstate.s_fline = lnp1->l_next;
		kind = lnp1->type1;
		if (kind > VALLOW)
			kind = VALLOW;
		freearea((area_t) lnp1, (unsigned) linesize[kind]);
	}
	prevstate = pstate.s_prevstat;
	if (prevstate != pst_cast 0)
	{
		for (lbhead = *pstate.s_locl; lbhead < &(*pstate.s_locl)[LOCLABSIZE];
				lbhead++)
		{
			for (lbp = lbhead->l_chain; lbp != lbp_cast 0; lbp = lbp_next)
			{
				lbp_next = lbp->l_chain;
				freearea((area_t) lbp, (unsigned) sizeof *lbp);
			}
		}
		freearea((area_t) (*pstate.s_locl),
		LOCLABSIZE * (sizeof((*pstate.s_locl)[0])));
		pstate = *prevstate;
		freearea((area_t) prevstate, (unsigned) sizeof *prevstate);
	}
}

void init_module(void)
{

	/*
	 * Called at the start of every module.
	 */

	holbase = 0;
	line_num = 1;
	mod_sizes = 0;
}

void end_module(void)
{

	/*
	 * Finish a module.
	 * Work to be done is mainly forgetting of local names,
	 * and remembering of those that will live during assembly.
	 */

	align(wordsize);
	set_mode(DATA_NUL);
	dump(100);
	enmd_pro();
	enmd_glo();
	if (memflg)
		memuse();
}

static void enmd_pro(void)
{
	register proc_t *p, *limit;

	/*
	 * Check that all local procedures have been defined,
	 * and forget them immediately thereafter.
	 */

	limit = &mprocs[oursize->n_mproc];
	for (p = mprocs; p < limit; p++)
	{
		if (p->p_name == 0)
			continue;
		if ((p->p_status & DEF) == 0)
			error("undefined local procedure '%s'", p->p_name);
	}
	memset(chp_cast mprocs, 0, (limit - mprocs) * (unsigned ) sizeof *mprocs);

	/* Clobber all flags indicating that external procedures
	 * were used in this module.
	 */

	limit = &xprocs[oursize->n_xproc];
	for (p = xprocs; p < limit; p++)
	{
		p->p_status &= ~EXT;
	}
}

static void enmd_glo(void)
{
	register glob_t *mg, *xg, *limit;

	/*
	 * Tougher then enmd_pro().
	 * Check all the symbols used in this module that are
	 * not to be forgotten immediately.
	 * A difficulty arises here:
	 *      In the tables textreloc[] and datareloc[]
	 *      pointers are used to identify the symbols concerned.
	 *      These pointers point into mglobs[].
	 *      Since at the end of assembly only the value of xglobs[]
	 *      is defined, these pointers have to be changed.
	 *      upd_reloc() takes care of this.
	 */

	limit = &mglobs[oursize->n_mlab];
	for (mg = mglobs; mg < limit; mg++)
	{
		if (mg->g_name == 0)
			continue;
		if ((mg->g_status & (EXT | DEF)) == 0)
			error("undefined local symbol '%s'", glostring(mg));
		if ((mg->g_status & EXT) == 0)
			continue;
		xg = xglolookup(mg->g_name, ENTERING);
		switch (xg->g_status & (EXT | DEF))
		{
		case 0: /* new symbol */
			if ((mg->g_status & DEF) == 0)
				++unresolved;
			break;
		case EXT: /* already used but not defined */
			if (mg->g_status & DEF)
			{
				--unresolved;
			}
			break;
		}
		xg->g_status |= mg->g_status;
		if (mg->g_status & DEF)
			xg->g_val.g_addr = mg->g_val.g_addr;
		else
			mg->g_val.g_gp = xg; /* used by upd_reloc */
	} /* up to the next symbol */
	upd_reloc();
	memset(chp_cast mglobs, 0, (limit - mglobs) * (unsigned ) sizeof *mglobs);
}

static void finish_up(void)
{
	/*
	 * Almost done. Check for unresolved references,
	 * make the e.out file and stop.
	 */

#ifdef DUMP
	c_print();
#endif
	check_def();
	if (nerrors == 0)
		copyout();
}

#ifdef DUMP
static void c_print(void)
{
	if (!c_flag)
		return;
	c_dprint("primary", opcnt1);
	c_dprint("secondary", opcnt2);
	c_dprint("extra long", opcnt3);
}

static void c_dprint(char *str, char* cnt)
{
	register int first, curr;
	printf("unused %s opcodes\n", str);
	for (first = -1, curr = 0; curr <= 256; curr++)
	{
		if (curr == 256 || cnt[curr])
		{
			if (first != -1)
			{
				if (first + 1 == curr)
				{
					printf("%3d\n", first);
				}
				else
				{
					printf("%3d..%3d\n", first, curr - 1);
				}
				first = -1;
			}
		}
		else
		{
			if (first == -1)
				first = curr;
		}
	}
}
#endif

static void check_def(void)
{
	register proc_t *p;
	register glob_t *g;
	register int count;

	/*
	 * Check for unresolved references.
	 * NOTE: The occurring of unresolved references is not fatal,
	 *       although the use of the e.out file after this
	 *       occurring must be strongly discouraged.
	 *       Every use of the symbols concerned is undefined.
	 */

	if (unresolved)
	{
		printf("Unresolved references\n  Procedures:\n");
		count = oursize->n_xproc;
		for (p = xprocs; count--; p++)
			if (p->p_name && (p->p_status & DEF) == 0)
				printf("    %s\n", p->p_name);
		printf("  Data:\n");
		count = oursize->n_glab;
		for (g = xglobs; count--; g++)
			if (g->g_name && (g->g_status & DEF) == 0)
				printf("    %s\n", glostring(g));
		if (!Uflag)
			nerrors++;
	}
}

void ertrap(void)
{ /* trap routine to drain input in case of compile errors */

	if (ifile == stdin)
		while (fgetc(ifile) != EOF)
			;
	exit(EXIT_FAILURE);
}
