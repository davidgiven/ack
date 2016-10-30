/*	fcc
	Driver for fast cc-compatible ACK C compiler.

	Derived from the C compiler driver from Minix.

	Compile this file with
		cc -O -I<EM home dir>/config driver.c
	Install the resulting binaries in the EM bin directory.
	Suggested name: fcc
*/

#ifdef sun3
#define MACHNAME	"m68020"
#define SYSNAME		"sun3"
#endif

#ifdef vax4
#define MACHNAME	"vax4"
#define SYSNAME		"vax4"
#endif

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <em_path.h>
#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif


/*
	Version producing cc-compatible .o files in one pass.
*/
#define MAXARGC	256	/* maximum number of arguments allowed in a list */
#define USTR_SIZE	128	/* maximum length of string variable */

typedef char USTRING[USTR_SIZE];

struct arglist {
	int al_argc;
	char *al_argv[MAXARGC];
};

#define CPP_NAME	"$H/lib.bin/cpp"
#define LD_NAME		"/bin/ld"
#define AS_NAME		"/bin/as"
#define SHELL		"/bin/sh"

char	*CPP;
char	*COMP;

int kids =  -1;
int ecount = 0;

struct arglist CPP_FLAGS = {
	7,
	{
		"-Dunix",
		"-D_EM_WSIZE=4",
		"-D_EM_PSIZE=4",
		"-D_EM_SSIZE=2",
		"-D_EM_LSIZE=4",
		"-D_EM_FSIZE=4",
		"-D_EM_DSIZE=8",
	}
};

struct arglist LD_HEAD = {
#ifdef sun3
	8,
	{
		"-dc",
		"-dp",
		"-e",
		"start",
		"-X",
		"-L/usr/lib/fsoft",
		"/usr/lib/crt0.o",
		"/usr/lib/Fcrt1.o"
	}
#endif
#ifdef vax4
	2,
	{
		"-X",
		"/lib/crt0.o"
	}
#endif
};

struct arglist LD_TAIL = {
	2,
	{
		"$H/lib/$S/tail_ext",
		"-lc"
	}
};

struct arglist LD_FLAGS;

struct arglist COMP_FLAGS;

char *o_FILE = "a.out"; /* default name for executable file */

#define remove(str)	((noexec || unlink(str)), (str)[0] = '\0')
#define cleanup(str)		(str && str[0] && remove(str))
#define init(al)		((al)->al_argc = 1)

char ProgCall[128];

struct arglist SRCFILES;
struct arglist LDFILES;

int RET_CODE = 0;

struct arglist CALL_VEC;

int o_flag = 0;
int c_flag = 0;
int v_flag = 0;
int O_flag = 0;

#if __STDC__
char *mkstr(char *, ...);
#else
char *mkstr();
#endif
char *malloc();
char *alloc();
char *extension();
char *expand_string();

USTRING ofile;
USTRING BASE;
USTRING tmp_file;

int noexec = 0;

trapcc(sig)
	int sig;
{
	signal(sig, SIG_IGN);
	if (kids != -1) kill(kids, sig);
	cleanup(ofile);
	cleanup(tmp_file);
	exit(1);
}

#define lang_suffix()	"c"
#define comp_name()	"$H/lib.bin/c_cccompat"

int
lang_opt(str)
	char *str;
{
	switch(str[1]) {
	case '-':	/* debug options */
	case 'R':	/* strict K&R */
	case 'w':	/* disable warnings */
		append(&COMP_FLAGS, str);
		return 1;
	}
	return 0;
}

main(argc, argv)
	char *argv[];
{
	char *str;
	char **argvec;
	int count;
	char *ext;
	register struct arglist *call = &CALL_VEC;
	char *file;
	char *ldfile;
	int compile_cnt = 0;

	setbuf(stdout, (char *) 0);
	basename(*argv++,ProgCall);

	COMP = expand_string(comp_name());
	CPP = expand_string(CPP_NAME);

#ifdef vax4
	append(&CPP_FLAGS, "-Dvax");
#endif
#ifdef sun3
	append(&CPP_FLAGS, "-Dsun");
	append(&CPP_FLAGS, "-Dmc68020");
	append(&CPP_FLAGS, "-Dmc68000");
#endif

	if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
		signal(SIGHUP, trapcc);
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, trapcc);
	if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
		signal(SIGQUIT, trapcc);
	while (--argc > 0) {
		if (*(str = *argv++) != '-') {
			append(&SRCFILES, str);
			continue;
		}

		if (lang_opt(str)) {
		}
		else switch (str[1]) {

		case 'c':	/* stop after producing .o files */
			c_flag = 1;
			break;
		case 'D':	/* preprocessor #define */
		case 'U':	/* preprocessor #undef */
			append(&CPP_FLAGS, str);
			break;
		case 'I':	/* include directory */
			append(&CPP_FLAGS, str);
			break;
		case 'g':	/* debugger support */
			append(&COMP_FLAGS, str);
			break;
		case 'o':	/* target file */
			if (argc-- >= 0) {
				o_flag = 1;
				o_FILE = *argv++;
				ext = extension(o_FILE);
				if (ext != o_FILE && ! strcmp(ext, lang_suffix())
				) {
					error("-o would overwrite %s", o_FILE);
				}
			}
			break;
		case 'u':	/* mark identifier as undefined */
			append(&LD_FLAGS, str);
			if (argc-- >= 0)
				append(&LD_FLAGS, *argv++);
			break;
		case 'O':	/* use built in peephole optimizer */
			O_flag = 1;
			break;
		case 'v':	/* verbose */
			v_flag++;
			if (str[2] == 'n')
				noexec = 1;
			break;
		case 'l':	/* library file */
			append(&SRCFILES, str);
			break;
		case 't':	/* -target? */
			if (! strcmp(str, "-target")) {
				if (argc-- >= 0) argv++;
				break;
			}
			warning("%s flag ignored", str);
			break;
		case 'M':	/* use other compiler (for testing) */
			strcpy(COMP, str+2);
			break;
		case 's':	/* strip, -sun3? */
			if (! strcmp(str, "-sun3")) {
				break;
			}
			/* fall through */
		case 'n':	/* text not read-only */
		case 'N':	/* text read-only */
		case 'r':	/* relocation produced */
		case 'S':	/* strip, but leave locals and globals */
			if (str[2] == '\0') {
				append(&LD_FLAGS, str);
				break;
			}
			/* fall through */
		default:
			warning("%s flag ignored", str);
			break;
		}
	}

	if (ecount) exit(1);

	count = SRCFILES.al_argc;
	argvec = &(SRCFILES.al_argv[0]);
	while (count-- > 0) {
		ext = extension(*argvec);
		if (*argvec[0] != '-' && 
		    ext != *argvec++ && (! strcmp(ext, lang_suffix())
		)) {
			compile_cnt++;
		}
	}

	if (compile_cnt > 1 && c_flag && o_flag) {
		warning("-o flag ignored");
		o_flag = 0;
	}

	append(&COMP_FLAGS, "-L");
	count = SRCFILES.al_argc;
	argvec = &(SRCFILES.al_argv[0]);
	while (count-- > 0) {
		register char *f;
		basename(file = *argvec++, BASE);

		ext = extension(file);

		if (file[0] != '-' &&
		    ext != file && (!strcmp(ext, lang_suffix())
		)) {
			if (compile_cnt > 1) printf("%s\n", file);

			ldfile = c_flag ? ofile : alloc((unsigned)strlen(BASE)+3);
			if (
			    !strcmp(ext, "s") &&
			    needsprep(file)) {
				strcpy(tmp_file, TMP_DIR);
				strcat(tmp_file, "/F_XXXXXX");
				mktemp(tmp_file);
				init(call);
				append(call, CPP);
				concat(call, &CPP_FLAGS);
				append(call, file);
				if (runvec(call, tmp_file)) {
					file = tmp_file;
				}
				else {
					remove(tmp_file);
					tmp_file[0] = '\0';
					continue;
				}
			}
			init(call);
			if (o_flag && c_flag) {
				f = o_FILE;
			}
			else	f = mkstr(ldfile, BASE, ".", "o", (char *)0);
			if (strcmp(ext, "s")) {
				append(call, COMP);
				concat(call, &CPP_FLAGS);
				concat(call, &COMP_FLAGS);
				append(call, file);
				append(call, f);
			}
			else {
				append(call, AS_NAME);
				append(call, "-o");
				append(call, f);
#ifdef sun3
				append(call, "-mc68020");
#endif
				append(call, file);
			}
			if (runvec(call, (char *) 0)) {
				file = f;
			}
			else {
				remove(f);
				continue;
			}
			cleanup(tmp_file);
			tmp_file[0] = '\0';
		}

		else if (file[0] != '-' &&
			 strcmp(ext, "o") && strcmp(ext, "a")) {
			warning("file with unknown suffix (%s) passed to the loader", ext);
		}

		if (c_flag)
			continue;

		append(&LDFILES, file);
	}

	/* *.s to a.out */
	if (RET_CODE == 0 && LDFILES.al_argc > 0) {
		init(call);
		expand(&LD_HEAD);
		expand(&LD_TAIL);
		append(call, expand_string(LD_NAME));
		concat(call, &LD_FLAGS);
		append(call, "-o");
		append(call, o_FILE);
		concat(call, &LD_HEAD);
		concat(call, &LDFILES);
		concat(call, &LD_TAIL);
		if (! runvec(call, (char *) 0)) {
			exit(RET_CODE);
		}
	}
	exit(RET_CODE);
}

needsprep(name)
	char *name;
{
	int file;
	char fc;

	file = open(name,0);
	if (file < 0) return 0;
	if (read(file, &fc, 1) != 1) fc = 0;
	close(file);
	return fc == '#';
}

char *
alloc(u)
	unsigned u;
{
	char *p = malloc(u);

	if (p == 0)
		panic("no space");
	return p;
}

char *
expand_string(s)
	char	*s;
{
	char	buf[1024];
	register char	*p = s;
	register char	*q = &buf[0];
	int expanded = 0;

	if (!p) return p;
	while (*p) {
		if (*p == '$') {
			p++;
			expanded = 1;
			switch(*p++) {
			case 'H':
				strcpy(q, EM_DIR);
				break;
			case 'M':
				strcpy(q, MACHNAME);
				break;
			case 'S':
				strcpy(q, SYSNAME);
				break;
			default:
				panic("internal error");
				break;
			}
			while (*q) q++;
		}
		else *q++ = *p++;
	}
	if (! expanded) return s;
	*q++ = '\0';
	p = alloc((unsigned int) (q - buf));
	return strcpy(p, buf);
}

append(al, arg)
	register struct arglist *al;
	char *arg;
{
	if (!arg || !*arg) return;
	if (al->al_argc >= MAXARGC)
		panic("argument list overflow");
	al->al_argv[(al->al_argc)++] = arg;
}

expand(al)
	register struct arglist *al;
{
	register int i = al->al_argc;
	register char **p = &(al->al_argv[0]);

	while (i-- > 0) {
		*p = expand_string(*p);
		p++;
	}
}

concat(al1, al2)
	struct arglist *al1, *al2;
{
	register i = al2->al_argc;
	register char **p = &(al1->al_argv[al1->al_argc]);
	register char **q = &(al2->al_argv[0]);

	if ((al1->al_argc += i) >= MAXARGC)
		panic("argument list overflow");
	while (i-- > 0) {
		*p++ = *q++;
	}
}

#if __STDC__
/*VARARGS*/
char *
mkstr(char *dst, ...)
{
	va_list ap;

	va_start(ap, dst);
	{
		register char *p;
		register char *q;

		q = dst;
		p = va_arg(ap, char *);

		while (p) {
			while (*q++ = *p++);
			q--;
			p = va_arg(ap, char *);
		}
	}
	va_end(ap);

	return dst;
}
#else
/*VARARGS*/
char *
mkstr(va_alist)
	va_dcl
{
	va_list ap;
	char *dst;

	va_start(ap);
	{
		register char *p;
		register char *q;

		dst = q = va_arg(ap, char *);
		p = va_arg(ap, char *);

		while (p) {
			while (*q++ = *p++);
			q--;
			p = va_arg(ap, char *);
		}
	}
	va_end(ap);

	return dst;
}
#endif

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
	while (*p1 != '.' && p1 >= p2) p1--;
	if (p1 >= p2) {
		*p1 = '\0';
		while (*dst++ = *p2++);
		*p1 = '.';
	}
	else
		while (*dst++ = *p2++);
}

char *
extension(fn)
	char *fn;
{
	register char *c = fn;

	while (*c++) ;
	while (*--c != '.' && c >= fn) { }
	if (c++ < fn || !*c) return fn;
	return c;
}

runvec(vec, outp)
	struct arglist *vec;
	char *outp;
{
	int pid, status;

	if (v_flag) {
		pr_vec(vec);
		putc('\n', stderr);
	}
	if ((pid = fork()) == 0) {	/* start up the process */
		if (outp) {	/* redirect standard output	*/
			close(1);
			if (creat(outp, 0666) != 1)
				panic("cannot create output file");
		}
		ex_vec(vec);
	}
	if (pid == -1)
		panic("no more processes");
	kids = pid;
	wait(&status);
	if (status) switch(status & 0177) {
	case SIGHUP:
	case SIGINT:
	case SIGQUIT:
	case SIGTERM:
	case 0:
		break;
	default:
		error("%s died with signal %d\n", vec->al_argv[1], status&0177);
	}
	kids = -1;
	return status ? ((RET_CODE = 1), 0) : 1;
}

/*VARARGS1*/
error(str, s1, s2)
	char *str, *s1, *s2;
{
	fprintf(stderr, "%s: ", ProgCall);
	fprintf(stderr, str, s1, s2);
	putc('\n', stderr);
	ecount++;
}

/*VARARGS1*/
warning(str, s1, s2)
	char *str, *s1, *s2;
{
	fprintf(stderr, "%s: (warning) ", ProgCall);
	fprintf(stderr, str, s1, s2);
	putc('\n', stderr);
}

panic(str)
	char *str;
{
	error(str);
	trapcc(SIGINT);
}

pr_vec(vec)
	register struct arglist *vec;
{
	register char **ap = &vec->al_argv[1];

	vec->al_argv[vec->al_argc] = 0;
	fprintf(stderr, "%s", *ap);
	while (*++ap) {
		fprintf(stderr, " %s", *ap);
	}
}

extern int errno;

ex_vec(vec)
	register struct arglist *vec;
{
	if (noexec)
		exit(0);
	vec->al_argv[vec->al_argc] = 0;
	execv(vec->al_argv[1], &(vec->al_argv[1]));
	if (errno == ENOEXEC) { /* not an a.out, try it with the SHELL */
		vec->al_argv[0] = SHELL;
		execv(SHELL, &(vec->al_argv[0]));
	}
	if (access(vec->al_argv[1], 1) == 0) {
		/* File is executable. */
		error("cannot execute %s", vec->al_argv[1]);
	} else {
		error("%s is not executable", vec->al_argv[1]);
	}
	exit(1);
}
