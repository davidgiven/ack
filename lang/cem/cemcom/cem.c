/*	$Header$	*/
/*
	Driver for the CEMCOM compiler: works like /bin/cc and accepts the
	options accepted by /bin/cc and /usr/em/bin/ack.
	Date written: dec 4, 1985
	Author: Erik Baalbergen
*/
	
#include "string.h"
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
char *CEM = "/user1/erikb/bin/cemcom";
char *AS_FIX = "/user1/erikb/bin/mcomm";
char *ENCODE = "/usr/em/lib/em_encode";
char *DECODE = "/usr/em/lib/em_decode";
char *OPT = "/usr/em/lib/em_opt";
char *CG = "/usr/em/lib/vax4/cg";
char *AS = "/bin/as";
char *LD = "/bin/ld";
char *SHELL = "/bin/sh";

char *LIBDIR = "/user1/cem/lib";

char *V_FLAG = "-Vs2.2w4.4i4.4l4.4f4.4d8.4p4.4";

struct arglist LD_HEAD = {
	2,
	{
		"/usr/em/lib/vax4/head_em",
		"/usr/em/lib/vax4/head_cc"
	}
};

struct arglist LD_TAIL = {
	3,
	{
		"/user1/cem/lib/stb.o",
		"/usr/em/lib/vax4/tail_mon",
		"/usr/em/lib/vax4/tail_em"
	}
};

char *o_FILE = "a.out";

#define remove(str)	(((t_flag == 0) && unlink(str)), (str)[0] = '\0')
#define cleanup(str)		(str && remove(str))
#define mkname(dst, s1, s2)	mkstr(dst, (s1), (s2), 0)
#define init(al)		(al)->al_argc = 1
#define library(nm) \
	mkstr(alloc((unsigned int)strlen(nm) + strlen(LIBDIR) + 7), \
		LIBDIR, "/lib", nm, ".a", 0)

char *ProgCall = 0;

struct arglist SRCFILES;
struct arglist LDFILES;
struct arglist GEN_LDFILES;

struct arglist PP_FLAGS;
struct arglist CEM_FLAGS;

int debug = 0;
int exec = 1;

int RET_CODE = 0;

struct arglist OPT_FLAGS;
struct arglist DECODE_FLAGS;
struct arglist ENCODE_FLAGS;
struct arglist CG_FLAGS;
struct arglist AS_FLAGS;
struct arglist LD_FLAGS;
struct arglist O_FLAGS;
struct arglist DEBUG_FLAGS;

struct arglist CALL_VEC;

int e_flag = 0;
int E_flag = 0;
int c_flag = 0;
int k_flag = 0;
int m_flag = 0;
int o_flag = 0;
int S_flag = 0;
int t_flag = 0;
int v_flag = 0;
int P_flag = 0;

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
	{ 0,		0,		0		}
};

int trap();
char *mkstr();
char *alloc();
long sizeof_file();

main(argc, argv)
	char *argv[];
{
	char *str;
	char **argvec;
	int count;
	int ext;
	char Nfile[USTR_SIZE];
	char kfile[USTR_SIZE];
	char sfile[USTR_SIZE];
	char mfile[USTR_SIZE];
	char ofile[USTR_SIZE];
	register struct arglist *call = &CALL_VEC;
	char BASE[USTR_SIZE];
	char *file;
	char *ldfile = 0;

	set_traps(trap);

	ProgCall = *argv++;

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
				E_flag = 1;
				append(&PP_FLAGS, str);
				PP = CEM;
				P_flag = (str[2] == 'P');
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
			E_flag = 1;
			append(&PP_FLAGS, str);
			P_flag = (str[1] == 'P');
			break;
		case 'c':
			if (str[2] == '.') {
				switch (str[3]) {

				case 's':
					S_flag = 1;
					break;
				case 'k':
					k_flag = 1;
					break;
				case 'o':
					c_flag = 1;
					break;
				case 'm':
					m_flag = 1;
					break;
				case 'e':
					e_flag = 1;
					break;
				default:
					bad_option(str);
				}
			}
			else
			if (str[2] == '\0')
				c_flag = 1;
			else
				bad_option(str);
			break;
		case 'D':
		case 'I':
		case 'U':
			append(&PP_FLAGS, str);
			break;
		case 'k':
			k_flag = 1;
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
			m_flag = 1;
			break;
		case 'o':
			o_flag = 1;
			if (argc-- < 0)
				bad_option(str);
			else
				o_FILE = *argv++;
			break;
		case 'O':
			append(&O_FLAGS, "-O");
			break;
		case 'p':
			append(&CEM_FLAGS, "-p");
			break;
		case 'R':
			if (str[2] == '\0')
				append(&CEM_FLAGS, str);
			else
				Roption(str);
			break;
		case 'S':
			S_flag = 1;
			break;
		case 't':
			t_flag = 1;
			break;
		case 'v':	/* set debug switches */
			v_flag = 1;
			switch (str[2]) {

			case 'd':
				debug = 1;
				break;
			case 'n':	/* no execute */
				exec = 0;
				break;
			}
			break;
		case 'V':
			V_FLAG = str;
			break;
		case 'e':
		case 'F':
		case 'd':
		case 'n':
		case 'N':
		case 'r':
		case 's':
		case 'u':
		case 'x':
		case 'X':
		case 'z':
			append(&LD_FLAGS, str);
			break;
		default:
			append(&CEM_FLAGS, str);
		}
	}

	if (debug)
		report("Note: debug output");
	if (exec == 0)
		report("Note: no execution");

	count = SRCFILES.al_argc;
	argvec = &(SRCFILES.al_argv[0]);

	Nfile[0] = '\0';

	while (count-- > 0) {
		basename(file = *argvec++, BASE);
		
		if (E_flag) {
			char ifile[USTR_SIZE];

			init(call);
			append(call, PP);
			concat(call, &DEBUG_FLAGS);
			concat(call, &PP_FLAGS);
			append(call, file);
			runvec(call, P_flag ? mkname(ifile, BASE, ".i") : 0);
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

		if (k_flag)
			continue;
		
		/* decode .k or .m */
		if (e_flag && (ext == 'k' || ext == 'm')) {
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

		if (m_flag)
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
				init(call);
				append(call, AS_FIX);
				append(call, Nfile);
				append(call, sfile);
				runvec(call, (char *)0);
				remove(Nfile);
			}
			cleanup(mfile);
			file = sfile;
			ext = 's';
		}
	
		if (S_flag)
			continue;
		
		/* .s to .o */
		if (ext == 's') {
			ldfile = c_flag ?
				ofile :
				alloc((unsigned)strlen(BASE) + 3);
			init(call);
			append(call, AS);
			concat(call, &AS_FLAGS);
			append(call, "-o");
			append(call, mkname(ldfile, BASE, ".o"));
			append(call, file);
			if (runvec(call, (char *)0) == 0)
				continue;
			file = ldfile;
			ext = 'o';
			cleanup(sfile);
		}

		if (c_flag)
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
		append(call, o_FILE);
		concat(call, &LD_HEAD);
		concat(call, &LDFILES);
		append(call, library("c"));
		concat(call, &LD_TAIL);
		if (runvec(call, (char *)0)) {
			register i = GEN_LDFILES.al_argc;

			while (i-- > 0)
				remove(GEN_LDFILES.al_argv[i]);
		}
	}

	exit(RET_CODE);
}


char *
alloc(u)
	unsigned u;
{
#define BUFSIZE  (USTR_SIZE * MAXARGC)
	static char buf[BUFSIZE];
	static char *bufptr = &buf[0];
	register char *p = bufptr;

	if ((bufptr += u) >= &buf[BUFSIZE])
		panic("no space");
	return p;
}

append(al, arg)
	struct arglist *al;
	char *arg;
{
	if (al->al_argc >= MAXARGC)
		panic("argument list overflow");
	al->al_argv[(al->al_argc)++] = arg;
}

concat(al1, al2)
	struct arglist *al1, *al2;
{
	register i = al2->al_argc;
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

		if (bc == '-')	{
			opt = mkstr(alloc((unsigned)strlen(arg) + 2),
								"-", arg, 0);
		}
		
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
	if (*--p1 == '.')
		*p1 = '\0';
	while (*dst++ = *p2++);
	*p1 = '.';
}

int
extension(fn)
	register char *fn;
{
	char c;

	while (*fn++) ;
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
	if (v_flag)
		print_vec(vec);
	if (exec == 0)
		return 1;
	if (fork() == 0) {	/* start up the process */
		extern int errno;

		if (outp) {	/* redirect standard output	*/
			if ((fd = creat(outp, 0666)) < 0)
				panic("cannot create %s", outp);
			if (dup2(fd, 1) == -1)
				panic("dup failure");
			close(fd);
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
