/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	$Id$	*/
/*
	Driver for the CEMCOM compiler: works like /bin/cc and accepts
	most of the options accepted by /bin/cc and /usr/em/bin/ack.
	Date written: dec 4, 1985
	Adapted for 68000 (Aug 19, 1986)
	Merged the vax and mantra versions (Nov 10, 1986)
	Author: Erik Baalbergen
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>

#define MAXARGC	256	/* maximum number of arguments allowed in a list */
#define USTR_SIZE	1024	/* maximum length of string variable */

struct arglist {
	int al_argc;
	char *al_argv[MAXARGC];
};

/* some system-dependent variables	*/
char *PP = "/lib/cpp";
char *CEM = "/usr/em/lib/em_cemcom";
char *ENCODE = "/usr/em/lib/em_encode";
char *DECODE = "/usr/em/lib/em_decode";
char *OPT = "/usr/em/lib/em_opt";
char *SHELL = "/bin/sh";

#ifndef MANTRA
char *CG = "/usr/em/lib/vax4/cg";
char *AS = "/bin/as";
char *AS_FIX = "/user1/erikb/bin/mcomm";
char *LD = "/bin/ld";
char *LIBDIR = "/user1/cem/lib";
char *V_FLAG = "-Vs2.2w4.4i4.4l4.4f4.4d8.4p4.4";
#else MANTRA
char *CG = "/usr/em/lib/m68k2/cg";
char *AS = "/usr/em/lib/m68k2/as";
char *LD = "/usr/em/lib/em_led";
char *CV = "/usr/em/lib/m68k2/cv";
char *LIBDIR = "/usr/em/lib/m68k2";
char *V_FLAG = "-Vs2.2w2.2i2.2l4.2f4.2d8.2p4.2";
#endif MANTRA

struct arglist LD_HEAD = {
	2,
	{
#ifndef MANTRA
		"/usr/em/lib/vax4/head_em",
		"/usr/em/lib/vax4/head_cc"
#else MANTRA
		"/usr/em/lib/m68k2/head_em",
		"/usr/em/lib/m68k2/head_cc"
#endif MANTRA
	}
};

struct arglist LD_TAIL = {
#ifndef MANTRA
	4,
	{
		"/user1/cem/lib/libc.a",
		"/user1/cem/lib/stb.o",
		"/usr/em/lib/vax4/tail_mon",
		"/usr/em/lib/vax4/tail_em"
	}
#else MANTRA
	7,
	{
		"/usr/em/lib/m68k2/tail_cc.1s",
		"/usr/em/lib/m68k2/tail_cc.2g",
		"/usr/em/lib/m68k2/tail_cem",
		"/usr/em/lib/m68k2/tail_fp.a",
		"/usr/em/lib/m68k2/tail_em.rt",
		"/usr/em/lib/m68k2/tail_mon",
		"/usr/em/lib/m68k2/end_em"
	}
#endif MANTRA
};

char *o_FILE = "a.out";
#ifdef MANTRA
char *cv_FILE = "cv.out";
#endif MANTRA

#define remove(str)	(((FLAG(t) == 0) && unlink(str)), (str)[0] = '\0')
#define cleanup(str)		(str && remove(str))
#define mkname(dst, s1, s2)	mkstr(dst, (s1), (s2), 0)
#define init(al)		(al)->al_argc = 1
#define library(nm) \
	mkstr(alloc((unsigned int)strlen(nm) + strlen(LIBDIR) + 7), \
		LIBDIR, "/lib", nm, ".a", 0)

struct arglist SRCFILES, LDFILES, GEN_LDFILES, PP_FLAGS, CEM_FLAGS,
	OPT_FLAGS, DECODE_FLAGS, ENCODE_FLAGS, CG_FLAGS, AS_FLAGS,
	O_FLAGS, DEBUG_FLAGS, CALL_VEC;

#ifndef MANTRA
struct arglist LD_FLAGS;
#else MANTRA
struct arglist LD_FLAGS = {
	5,
	{
		"-b0:0x80000",
		"-a0:2",
		"-a1:2",
		"-a2:2",
		"-a3:2"
	}
};
struct arglist CV_FLAGS;
int Nc_flag = 0;
#endif MANTRA

/* option naming */
#define NAME(chr)	chr
#define FLAG(chr)	NAME(chr)_flag
int E_flag, P_flag, S_flag, c_flag, e_flag, k_flag, 
	m_flag, o_flag, t_flag, v_flag;

/* various passes */
struct prog {
	char *p_name;
	char **p_task;
	struct arglist *p_flags;
} ProgParts[] = {
	{ "cpp",	&PP,		&PP_FLAGS	},
	{ "cem",	&CEM,		&CEM_FLAGS	},
	{ "opt",	&OPT,		&OPT_FLAGS	},
	{ "decode",	&DECODE,	&DECODE_FLAGS	},
	{ "encode",	&ENCODE,	&ENCODE_FLAGS	},
	{ "be",		&CG,		&CG_FLAGS	},
	{ "cg",		&CG,		&CG_FLAGS	},
	{ "as",		&AS,		&AS_FLAGS	},
	{ "ld",		&LD,		&LD_FLAGS	},
#ifdef MANTRA
	{ "cv",		&CV,		&CV_FLAGS	},
#endif MANTRA
	{ 0,		0,		0		}
};

/* various forward declarations */
int trap();
char *mkstr();
char *alloc();
long sizeof_file();

/* various globals */
char *ProgCall = 0;
int debug = 0;
int exec = 1;
int RET_CODE = 0;

main(argc, argv)
	char *argv[];
{
	char *str, **argvec, *file, *ldfile = 0;
	int count, ext;
	char Nfile[USTR_SIZE], kfile[USTR_SIZE], sfile[USTR_SIZE],
		mfile[USTR_SIZE], ofile[USTR_SIZE], BASE[USTR_SIZE];
	register struct arglist *call = &CALL_VEC;

	set_traps(trap);
	ProgCall = *argv++;
	append(&CEM_FLAGS, "-L");
	while (--argc > 0) {
		if (*(str = *argv++) != '-') {
			append(&SRCFILES, str);
			continue;
		}
		switch (str[1]) {
		case '-':
			switch (str[2]) {
			case 'C':
			case 'E':
			case 'P':
				FLAG(E) = 1;
				append(&PP_FLAGS, str);
				PP = CEM;
				FLAG(P) = (str[2] == 'P');
				break;
			default:
				append(&DEBUG_FLAGS, str);
				break;
			}
			break;
		case 'B':
			PP = CEM = &str[2];
			break;
		case 'C':
		case 'E':
		case 'P':
			FLAG(E) = 1;
			append(&PP_FLAGS, str);
			FLAG(P) = (str[1] == 'P');
			break;
		case 'c':
			if (str[2] == '.') {
				switch (str[3]) {
				case 's':
					FLAG(S) = 1;
					break;
				case 'k':
					FLAG(k) = 1;
					break;
				case 'o':
					FLAG(c) = 1;
					break;
				case 'm':
					FLAG(m) = 1;
					break;
				case 'e':
					FLAG(e) = 1;
					break;
				default:
					bad_option(str);
				}
			}
			else
			if (str[2] == '\0')
				FLAG(c) = 1;
			else
				bad_option(str);
			break;
		case 'D':
		case 'I':
		case 'U':
			append(&PP_FLAGS, str);
			break;
		case 'k':
			FLAG(k) = 1;
			break;
		case 'l':
			if (str[2] == '\0')	/* no standard libraries */
				LD_HEAD.al_argc = LD_TAIL.al_argc = 0;
			else	/* use library from library directory */
				append(&SRCFILES, library(&str[2]));
			break;
		case 'L':	/* change default library directory */
			LIBDIR = &str[2];
			break;
		case 'm':
			FLAG(m) = 1;
			break;
#ifdef MANTRA
		case 'N':
			switch (str[2]) {
			case 'c': /* no a.out conversion */
				Nc_flag = 1;
				break;
			case 'l': /* no default options to led */
				LD_FLAGS.al_argc = 0;
				break;
			default:
				bad_option(str);
			}
			break;
#endif MANTRA
		case 'o':
			FLAG(o) = 1;
			if (argc-- < 0)
				bad_option(str);
			else
				o_FILE = *argv++;
			break;
		case 'O':
			append(&O_FLAGS, "-O");
			break;
		case 'R':
			if (str[2] == '\0')
				append(&CEM_FLAGS, str);
			else
				Roption(str);
			break;
		case 'S':
			FLAG(S) = 1;
			break;
		case 't':
			FLAG(t) = 1;
			break;
		case 'v':	/* set debug switches */
			FLAG(v) = 1;
			switch (str[2]) {
			case 'd':
				debug = 1;
				break;
			case 'n':	/* no execute */
				exec = 0;
				break;
			case '\0':
				break;
			default:
				bad_option(str);
			}
			break;
		case 'V':
			V_FLAG = str;
			break;
		default:
			append(&LD_FLAGS, str);
		}
	}
	if (debug) report("Note: debug output");
	if (exec == 0)
		report("Note: no execution");
	count = SRCFILES.al_argc;
	argvec = &(SRCFILES.al_argv[0]);
	Nfile[0] = '\0';
	while (count-- > 0) {
		basename(file = *argvec++, BASE);
		if (FLAG(E)) {
			char ifile[USTR_SIZE];

			init(call);
			append(call, PP);
			concat(call, &DEBUG_FLAGS);
			concat(call, &PP_FLAGS);
			append(call, file);
			runvec(call, FLAG(P) ? mkname(ifile, BASE, ".i") : 0);
			continue;
		}
		ext = extension(file);
		/* .c to .k and .N	*/
		if (ext == 'c' || ext == 'i') {
			init(call);
			append(call, CEM);
			concat(call, &DEBUG_FLAGS);
			append(call, V_FLAG);
			concat(call, &CEM_FLAGS);
			concat(call, &PP_FLAGS);
			append(call, file);
			append(call, mkname(kfile, BASE, ".k"));
			append(call, mkname(Nfile, BASE, ".N"));
			if (runvec(call, (char *)0)) {
				file = kfile;
				ext = 'k';
				if (sizeof_file(Nfile) <= 0L)
					remove(Nfile);
			}
			else {
				remove(kfile);
				remove(Nfile);
				continue;
			}
		}
		/* .e to .k */
		if (ext == 'e') {
			init(call);
			append(call, ENCODE);
			concat(call, &ENCODE_FLAGS);
			append(call, file);
			append(call, mkname(kfile, BASE, ".k"));
			if (runvec(call, (char *)0) == 0)
				continue;
			file = kfile;
			ext = 'k';
		}
		if (FLAG(k))
			continue;
		/* decode .k or .m */
		if (FLAG(e) && (ext == 'k' || ext == 'm')) {
			char efile[USTR_SIZE];
			init(call);
			append(call, DECODE);
			concat(call, &DECODE_FLAGS);
			append(call, file);
			append(call, mkname(efile, BASE, ".e"));
			runvec(call, (char *)0);
			cleanup(kfile);
			continue;
		}
		/* .k to .m */
		if (ext == 'k') {
			init(call);
			append(call, OPT);
			concat(call, &OPT_FLAGS);
			append(call, file);
			if (runvec(call, mkname(mfile, BASE, ".m")) == 0)
				continue;
			file = mfile;
			ext = 'm';
			cleanup(kfile);
		}
		if (FLAG(m))
			continue;
		/* .m to .s */
		if (ext == 'm') {
			init(call);
			append(call, CG);
			concat(call, &CG_FLAGS);
			append(call, file);
			append(call, mkname(sfile, BASE, ".s"));
			if (runvec(call, (char *)0) == 0)
				continue;
			if (Nfile[0] != '\0') {
#ifndef MANTRA
				init(call);
				append(call, AS_FIX);
				append(call, Nfile);
				append(call, sfile);
				runvec(call, (char *)0);
#endif MANTRA
				remove(Nfile);
			}
			cleanup(mfile);
			file = sfile;
			ext = 's';
		}
		if (FLAG(S))
			continue;
		/* .s to .o */
		if (ext == 's') {
			ldfile = FLAG(c) ?
				ofile :
				alloc((unsigned)strlen(BASE) + 3);
			init(call);
			append(call, AS);
			concat(call, &AS_FLAGS);
#ifdef MANTRA
			append(call, "-");
#endif MANTRA
			append(call, "-o");
			append(call, mkname(ldfile, BASE, ".o"));
			append(call, file);
			if (runvec(call, (char *)0) == 0)
				continue;
			file = ldfile;
			ext = 'o';
			cleanup(sfile);
		}
		if (FLAG(c))
			continue;
		append(&LDFILES, file);
		if (ldfile) {
			append(&GEN_LDFILES, ldfile);
			ldfile = 0;
		}
	}
	/* *.o to a.out */
	if (RET_CODE == 0 && LDFILES.al_argc > 0) {
		init(call);
		append(call, LD);
		concat(call, &LD_FLAGS);
		append(call, "-o");
#ifndef MANTRA
		append(call, o_FILE);
#else MANTRA
		append(call, Nc_flag ? o_FILE : cv_FILE);
#endif MANTRA
		concat(call, &LD_HEAD);
		concat(call, &LDFILES);
		concat(call, &LD_TAIL);
		if (runvec(call, (char *)0)) {
			register i = GEN_LDFILES.al_argc;

			while (i-- > 0)
				remove(GEN_LDFILES.al_argv[i]);
#ifdef MANTRA
			/* convert to local a.out format */
			if (Nc_flag == 0) {
				init(call);
				append(call, CV);
				concat(call, &CV_FLAGS);
				append(call, cv_FILE);
				append(call, o_FILE);
				if (runvec(call, (char *)0))
					remove(cv_FILE);
			}
#endif MANTRA
		}
	}
	exit(RET_CODE);
}

#define BUFSIZE  (USTR_SIZE * MAXARGC)
char alloc_buf[BUFSIZE];

char *
alloc(u)
	unsigned u;
{
	static char *bufptr = &alloc_buf[0];
	register char *p = bufptr;

	if ((bufptr += u) >= &alloc_buf[BUFSIZE])
		panic("no space");
	return p;
}

append(al, arg)
	register struct arglist *al;
	char *arg;
{
	if (al->al_argc >= MAXARGC)
		panic("argument list overflow");
	al->al_argv[(al->al_argc)++] = arg;
}

concat(al1, al2)
	struct arglist *al1, *al2;
{
	register int i = al2->al_argc;
	register char **p = &(al1->al_argv[al1->al_argc]);
	register char **q = &(al2->al_argv[0]);

	if ((al1->al_argc += i) >= MAXARGC)
		panic("argument list overflow");
	while (i-- > 0)
		*p++ = *q++;
}

/*	The next function is a dirty old one, taking a variable number of
	arguments.
	Take care that the last argument is a null-valued pointer!
*/
/*VARARGS1*/
char *
mkstr(dst, arg)
	char *dst, *arg;
{
	char **vec = (char **) &arg;
	register char *p;
	register char *q = dst;

	while (p = *vec++) {
		while (*q++ = *p++);
		q--;
	}
	return dst;
}

Roption(str)
	char *str;	/* of the form "prog=/-arg"	*/
{
	char *eq;
	char *prog, *arg;
	char bc;
	char *cindex();

	prog = &str[2];
	if (eq = cindex(prog, '='))
		bc = '=';
	else
	if (eq = cindex(prog, '-'))
		bc = '-';
	else {
		bad_option(str);
		return;
	}
	*eq++ = '\0';
	if (arg = eq) {
		char *opt = 0;
		struct prog *pp = &ProgParts[0];

		if (bc == '-')
			opt = mkstr(alloc((unsigned)strlen(arg) + 2),
								"-", arg, 0);
		while (pp->p_name) {
			if (strcmp(prog, pp->p_name) == 0) {
				if (opt)
					append(pp->p_flags, opt);
				else
					*(pp->p_task) = arg;
				return;
			}
			pp++;
		}
	}
	bad_option(str);
}

basename(str, dst)
	char *str;
	register char *dst;
{
	register char *p1 = str;
	register char *p2 = p1;

	while (*p1)
		if (*p1++ == '/')
			p2 = p1;
	p1--;
	if (*--p1 == '.') {
		*p1 = '\0';
		while (*dst++ = *p2++) {}
		*p1 = '.';
	}
	else
		while (*dst++ = *p2++) {}
}

int
extension(fn)
	register char *fn;
{
	char c;

	while (*fn++) {}
	fn--;
	c = *--fn;
	return (*--fn == '.') ? c : 0;
}

long
sizeof_file(nm)
	char *nm;
{
	struct stat stbuf;

	if (stat(nm, &stbuf) == 0)
		return stbuf.st_size;
	return -1;
}

char * sysmsg[]  = {
	0,
	"Hangup",
	"Interrupt",
	"Quit",
	"Illegal instruction",
	"Trace/BPT trap",
	"IOT trap",
	"EMT trap",
	"Floating exception",
	"Killed",
	"Bus error",
	"Memory fault",
	"Bad system call",
	"Broken pipe",
	"Alarm call",
	"Terminated",
	"Signal 16"
};

runvec(vec, outp)
	struct arglist *vec;
	char *outp;
{
	int status, fd;
	char *task = vec->al_argv[1];

	vec->al_argv[vec->al_argc] = 0;
	if (FLAG(v))
		print_vec(vec);
	if (exec == 0)
		return 1;
	if (fork() == 0) {	/* start up the process */
		extern int errno;
		if (outp) {	/* redirect standard output	*/
			close(1);
			if ((fd = creat(outp, 0666)) < 0)
				panic("cannot create %s", outp);
			if (fd != 1)
				panic("illegal redirection");
		}
		if (debug) report("exec %s", task);
		execv(task, &(vec->al_argv[1]));
		/* not an a.out file, let's try it with the SHELL */
		if (debug) report("try it with %s", SHELL);
		if (errno == ENOEXEC) {
			vec->al_argv[0] = SHELL;
			execv(SHELL, &(vec->al_argv[0]));
		}
		/* failed, so ... */
		panic("cannot execute %s", task);
		exit(1);
	}
	else {
		int loworder, highorder, sig;

		wait(&status);
		loworder = status & 0377;
		highorder = (status >> 8) & 0377;
		if (loworder == 0) {
			if (highorder)
				report("%s: exit status %d", task, highorder);
			return highorder ? ((RET_CODE = 1), 0) : 1;
		}
		else {
			sig = loworder & 0177;
			if (sig == 0177)
				report("%s: stopped by ptrace", task);
			else
			if (sysmsg[sig])
				report("%s: %s%s", task, sysmsg[sig],
					(loworder & 0200)
						? " - core dumped"
						: "");
			RET_CODE = 1;
			return 0;
		}
	}
	/*NOTREACHED*/
}

bad_option(str)
	char *str;
{
	report("bad option %s", str);
}

/*VARARGS1*/
report(fmt, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)
	char *fmt;
{
	fprintf(stderr, "%s: ", ProgCall);
	fprintf(stderr, fmt, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	fprintf(stderr, "\n");
}

/*VARARGS1*/
panic(fmt, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)
	char *fmt;
{
	fprintf(stderr, "%s: ", ProgCall);
	fprintf(stderr, fmt, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	fprintf(stderr, "\n");
	exit(1);
}

set_traps(f)
	int (*f)();
{
	signal(SIGHUP, f);
	signal(SIGINT, f);
	signal(SIGQUIT, f);
	signal(SIGALRM, f);
	signal(SIGTERM, f);
}

/*ARGSUSED*/
trap(sig)
{
	set_traps(SIG_IGN);
	panic("Trapped");
}

print_vec(vec)
	struct arglist *vec;
{
	register i;

	for (i = 1; i < vec->al_argc; i++)
		printf("%s ", vec->al_argv[i]);
	printf("\n");
}

char *
cindex(s, c)
	char *s, c;
{
	while (*s)
		if (*s++ == c)
			return s - 1;
	return (char *) 0;
}
