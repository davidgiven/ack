/*	fcc/fm2/fpc 
	Driver for fast ACK compilers.

	Derived from the C compiler driver from Minix.

	You can create cc-compatible versions by compiling with
		cc -O '-DFASTDIR="<this directory>"' -DF?? driver.c
	where F?? is either FCC, FM2, or FPC.
	Install resulting binaries in a public place (f.i. /usr/local/bin).
	Suggested names: fcc, fm2, and fpc.

	If you have ACK, you can create ACK-compatible versions by compiling
	this file with
		cc -O -DACK_BIN -I<ACK home directory>/h -DF?? driver.c
	where F?? is either FCC, FPC, or FM2.
	Install the resulting binaries in the EM bin directory.
	Suggested names: afcc, afm2, and afpc.
	The ACK compatible version afpc is not compatible with the 4th
	ACK distribution, and thus is not on the tape.
*/


#ifndef vax
#ifndef sun
In this case, no fast ACK compiler available
#endif
#endif

#if FM2+FPC+FCC > 1
Something wrong here! Only one of FM2, FPC, or FCC must be defined
#endif

#include <errno.h>
#include <signal.h>
#include <varargs.h>
#include <stdio.h>
#ifdef ACK_BIN
#include <em_path.h>
#define M2DEF	"/lib/m2"
#define FASTDIR EM_DIR
#else
#define M2DEF "/def"
#define TMP_DIR	"/tmp"
#endif ACK_BIN


#ifndef ACK_BIN
#define BINDIR	"/bin/"
#define LIBDIR	"/lib/"
#else ACK_BIN
#define BINDIR	"/lib/"
#ifdef sun
#define LIBDIR	"/lib/sun3/"
#endif sun
#ifdef vax
#define LIBDIR	"/lib/vax4/"
#endif vax
#define CCINCL	"/include/_tail_cc"
#endif ACK_BIN
char *ROOT_DIR = FASTDIR;

/*
#ifndef ACK_BIN
	Version producing VAX or SUN .o files in one pass.
#else ACK_BIN
	Version producing ACK .o files in one pass.
#endif ACK_BIN
*/
#define MAXARGC	256	/* maximum number of arguments allowed in a list */
#define USTR_SIZE	128	/* maximum length of string variable */

typedef char USTRING[USTR_SIZE];

USTRING INCLUDE = "-I";

struct arglist {
	int al_argc;
	char *al_argv[MAXARGC];
};

#define LD	"/bin/ld"
#define AS	"/bin/as"
#define CPP	"*cpp"
#if ACK_BIN && !vax
#undef LD
#undef AS
#define LD	"*em_led"
#define CV	"~cv"		/* machine dependent; resides in lib directory */
#endif ACK_BIN && !vax
#define SHELL	"/bin/sh"

/*	flags passed to linker */
#define TO_LD	"dnNrsSi"
#if ACK_BIN && !vax
#undef TO_LD
#define TO_LD	"rs"
#endif ACK_BIN && !vax

int kids =  -1;
int ecount = 0;

struct arglist CPP_FLAGS = {
#ifdef vax
	8,
#endif
#ifdef sun
	10,
#endif
	{
		"-Dunix",
		"-DEM_WSIZE=4",
		"-DEM_PSIZE=4",
		"-DEM_SSIZE=2",
		"-DEM_LSIZE=4",
		"-DEM_FSIZE=4",
		"-DEM_DSIZE=8",
#ifdef vax
		"-Dvax"
#endif
#ifdef sun
		"-Dmc68000",
		"-Dsun",
		"-Dmc68020"
#endif
	}
};

struct arglist COMP_FLAGS;

#ifdef FCC
struct arglist LD_HEAD = {
#ifdef ACK_BIN
	2,
	{
		"~head_em",
		"~head_cc"
	}
#else ACK_BIN
#ifdef sun
#ifdef SunOs4
	7,
#else
	5,
#endif
	{
		"-e",
		"start",
		"-X",
#ifdef SunOs4
		"-L/usr/lib/fsoft",
		"-Bstatic",
#endif
		"/lib/crt0.o",
		"/lib/Fcrt1.o"
	}
#endif
#ifdef vax
	2,
	{
		"-X",
		"/lib/crt0.o"
	}
#endif
#endif ACK_BIN
};

struct arglist LD_TAIL = {
#ifdef ACK_BIN
#ifdef sun
	6,
	{
		"~tail_cc.1s",
		"~tail_cc.2g",
		"*m68020/tail_fp",
		"*m68020/tail_em",
		"~tail_mon",
		"*m68020/end_em"
	}
#endif sun
#ifdef vax
	4,
	{
		"~tail_cc.1s",
		"~tail_cc.2g",
		"~tail_em",
		"~tail_mon"
	}
#endif vax
#else ACK_BIN
#ifdef sun
	2,
	{
		"~tail_ext",
		"-lc"
	}
#endif
#ifdef vax
	2,
	{
		"~tail_em",
		"-lc"
	}
#endif
#endif ACK_BIN
};
#endif FCC

#ifdef FPC
struct arglist LD_HEAD = {
#ifdef ACK_BIN
	2,
	{
		"~head_em",
#ifdef vax
		"~head_pc"
#endif
#ifdef sun
		"*m68020/head_pc"
#endif
	}
#else ACK_BIN
#ifdef sun
#ifdef SunOs4
	7,
#else
	5,
#endif
	{
		"-e",
		"begtext",
		"-X",
#ifdef SunOs4
		"-L/usr/lib/fsoft",
		"-Bstatic",
#endif
		"~head_em",
		"~head_pc"
	}
#endif sun
#ifdef vax
	3,
	{
		"-X",
		"~head_em",
		"~head_pc"
	}
#endif vax
#endif ACK_BIN
};
struct arglist LD_TAIL = {
#ifndef ACK_BIN
#ifdef sun
	3,
	{
		"~tail_pc",
		"~tail_ext",
		"-lc"
	}
#endif sun
#ifdef vax
	3,
	{
		"~tail_pc",
		"~tail_em",
		"-lc"
	}
#endif vax
#else ACK_BIN
#ifdef sun
	5,
	{
		"*m68020/tail_pc",
		"*m68020/tail_fp",
		"*m68020/tail_em",
		"~tail_mon",
		"*m68020/end_em"
	}
#endif sun
#ifdef vax
	3,
	{
		"~tail_pc",
		"~tail_em",
		"~tail_mon"
	}
#endif vax
#endif ACK_BIN
};
#endif FPC

#ifdef FM2
struct arglist LD_HEAD = {
#ifdef ACK_BIN
	2,
	{
		"~head_em",
		"~head_m2"
	}
#else ACK_BIN
#ifdef sun
#ifdef SunOs4
	7,
#else
	5,
#endif
	{
		"-e",
		"begtext",
		"-X",
#ifdef SunOs4
		"-L/usr/lib/fsoft",
		"-Bstatic",
#endif
		"~head_em",
		"~head_m2"
	}
#endif sun
#ifdef vax
	3,
	{
		"-X",
		"~head_em",
		"~head_m2"
	}
#endif vax
#endif ACK_BIN
};
struct arglist LD_TAIL = {
#ifndef ACK_BIN
#ifdef sun
	3,
	{
		"~tail_m2",
		"~tail_ext",
		"-lc"
	}
#endif sun
#ifdef vax
	3,
	{
		"~tail_m2",
		"~tail_em",
		"-lc"
	}
#endif vax
#else ACK_BIN
#ifdef sun
	5,
	{
		"~tail_m2",
		"*m68020/tail_fp",
		"*m68020/tail_em",
		"~tail_mon",
		"*m68020/end_em"
	}
#endif sun
#ifdef vax
	3,
	{
		"~tail_m2",
		"~tail_em",
		"~tail_mon"
	}
#endif vax
#endif ACK_BIN
};
#endif FM2

#if ACK_BIN && !vax
#ifdef sun
struct arglist def_align = {
	5,
	{
		"-a0:4",
		"-a1:4",
		"-a2:0x20000",
		"-a3:4",
		"-b0:0x2020"
	}
};

struct arglist n_align = {
	5,
	{
		"-a0:4",
		"-a1:4",
		"-a2:0x20000",
		"-a3:4",
		"-b0:0x2000"
	}
};

struct arglist N_align = {
	5,
	{
		"-a0:4",
		"-a1:4",
		"-a2:4",
		"-a3:4",
		"-b0:0x2000"
	}
};
#endif sun

struct arglist *sect_align = &def_align;

#endif ACK_BIN && !vax

char *o_FILE = "a.out"; /* default name for executable file */

#define remove(str)	((noexec || unlink(str)), (str)[0] = '\0')
#define cleanup(str)		(str && str[0] && remove(str))
#define init(al)		((al)->al_argc = 1)

char ProgCall[128];

struct arglist SRCFILES;
struct arglist LDFILES;
struct arglist GEN_LDFILES;

int RET_CODE = 0;

struct arglist LD_FLAGS;

struct arglist CALL_VEC;

int o_flag = 0;
int c_flag = 0;
int v_flag = 0;
int O_flag = 0;

char *mkstr();
char *malloc();
char *alloc();
char *extension();

USTRING ofile;
USTRING BASE;
USTRING tmp_file;

int noexec = 0;

extern char *strcat(), *strcpy(), *mktemp(), *strchr();

trapcc(sig)
	int sig;
{
	signal(sig, SIG_IGN);
	if (kids != -1) kill(kids, sig);
	cleanup(ofile);
	cleanup(tmp_file);
	exit(1);
}

#ifdef FCC
#define lang_suffix()	"c"
#ifndef ACK_BIN
#define comp_name()	"*cemcom_ce"
#else
#ifdef vax
#define comp_name()	"~cemcom_ce"
#endif
#ifdef sun
#define comp_name()	"*m68020/cemcom_ce"
#endif
#endif ACK_BIN
#endif FCC

#ifdef FM2
#define lang_suffix()	"mod"
#ifndef ACK_BIN
#define comp_name()	"*m2_ce"
#else
#ifdef vax
#define comp_name()	"~m2_ce"
#endif
#ifdef sun
#define comp_name()	"*m68020/m2_ce"
#endif
#endif ACK_BIN
#endif FM2

#ifdef FPC
#define lang_suffix()	"p"
#ifndef ACK_BIN
#define comp_name()	"*pc_ce"
#else
#ifdef vax
#define comp_name()	"~pc_ce"
#endif
#ifdef sun
#define comp_name()	"*m68020/pc_ce"
#endif
#endif ACK_BIN

#endif FPC


#ifdef FCC
int
lang_opt(str)
	char *str;
{
	switch(str[1]) {
	case '-':	/* debug options */
	case 'w':	/* disable warnings */
	case 'R':	/* strict K&R C.  */
		append(&COMP_FLAGS, str);
		return 1;
	}
	return 0;
}
#endif FCC

#ifdef FM2
int
lang_opt(str)
	char *str;
{
	switch(str[1]) {
	case '-':	/* debug options */
	case 'w':	/* disable warnings */
	case 'R':	/* no runtime checks */
	case 'W':	/* add warnings */
	case 'L':	/* no line numbers */
	case 'A':	/* extra array bound checks */
	case '3':	/* only accept 3rd edition Modula-2 */
		append(&COMP_FLAGS, str);
		return 1;
	case 'I':
		append(&COMP_FLAGS, str);
		break;	/* !!! */
	case 'U':	/* underscores in identifiers allowed */
		if (str[2] == '\0') {
			append(&COMP_FLAGS, str);
			return 1;
		}
		break;
	case 'e':	/* local extension for Modula-2 compiler:
			   procedure constants
			*/
		str[1] = 'l';
		append(&COMP_FLAGS, str);
		return 1;
	}
	return 0;
}
#endif FM2

#ifdef FPC
int
lang_opt(str)
	char *str;
{
	switch(str[1]) {
	case '-':	/* debug options */
	case 'a':	/* enable assertions */
	case 'd':	/* allow doubles (longs) */
	case 'i':	/* set size of integer sets */
	case 't':	/* tracing */
	case 'w':	/* disable warnings */
	case 'A':	/* extra array bound checks */
	case 'C':	/* distinguish between lower case and upper case */
	case 'L':	/* no FIL and LIN instructions */
	case 'R':	/* no runtime checks */
		append(&COMP_FLAGS, str);
		return 1;
	case 'u':
	case 'U':
		/* underscores in identifiers */
	case 's':
		/* only compile standard pascal */
	case 'c':
		/* C type strings */
		if (str[2] == '+' && str[3] == '\0') {
			str[2] = 0;
			append(&COMP_FLAGS, str);
			return 1;
		}
	}
	return 0;
}
#endif FPC

main(argc, argv)
	char *argv[];
{
	char *str;
	char **argvec;
	int count;
	char *ext;
	register struct arglist *call = &CALL_VEC;
	char *file;
	char *ldfile = 0;
	USTRING COMP;
	int compile_cnt = 0;

	setbuf(stdout, (char *) 0);
	basename(*argv++,ProgCall);
	strcat(INCLUDE, ROOT_DIR);
#ifdef FM2
	strcat(INCLUDE, M2DEF);
#endif FM2
#ifdef FCC
#ifdef ACK_BIN
	strcat(INCLUDE, CCINCL);
#else
	INCLUDE[0] = '\0';
#endif ACK_BIN
#endif FCC
#ifdef FPC
	INCLUDE[0] = '\0';
#endif FPC
	strcpy(COMP,comp_name());

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
		case 'o':	/* target file */
			if (argc-- >= 0) {
				o_flag = 1;
				o_FILE = *argv++;
				ext = extension(o_FILE);
				if (ext != o_FILE && ! strcmp(ext, lang_suffix())
#ifndef ACK_BIN
				     || !strcmp(ext, "s")
#endif
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
			if (! O_flag) strcat(COMP, "opt");
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
		case 'M':	/* use other compiler (for testing) */
			strcpy(COMP, str+2);
			break;
#if ACK_BIN && !vax
		case 'n':	/* text portion sharable, read-only. No
				   demand paging
				*/
			sect_align = &n_align;
			break;
		case 'N':	/* text portion not sharable, not read-only */
			sect_align = &N_align;
			break;
		case 'd':	/* force definition of common storage */
			append(&LD_FLAGS, "-c");
			break;
#endif ACK_BIN && !vax
		default:
#ifdef sun
			if (! strcmp(str, "-sun3")) {
				/* SunOs4.0 make passes this flag to
				   $(CC). Ignore it silently.
				*/
				break;
			}
#endif
			if (strchr(TO_LD, str[1]) == 0)
				warning("%s flag ignored", str);
			else {
				append(&LD_FLAGS, str);
			}
			break;
		}
	}

	if (ecount) exit(1);

#if FCC && !ACK_BIN
	append(&COMP_FLAGS, "-L"); /* disable line number and filename admin */
#endif FCC && !ACK_BIN
	count = SRCFILES.al_argc;
	argvec = &(SRCFILES.al_argv[0]);
	while (count-- > 0) {
		ext = extension(*argvec);
		if (*argvec[0] != '-' && 
		    ext != *argvec++ && (! strcmp(ext, lang_suffix())
#ifndef ACK_BIN
		     || !strcmp(ext, "s")
#endif
		)) {
			compile_cnt++;
		}
	}

	if (compile_cnt > 1 && c_flag && o_flag) {
		warning("-o flag ignored");
		o_flag = 0;
	}

	count = SRCFILES.al_argc;
	argvec = &(SRCFILES.al_argv[0]);
	while (count-- > 0) {
		register char *f;
		basename(file = *argvec++, BASE);

		ext = extension(file);

		if (file[0] != '-' &&
		    ext != file && (!strcmp(ext, lang_suffix())
#ifndef ACK_BIN
		    || !strcmp(ext, "s")
#endif
		)) {
#ifndef ACK_BIN
			if (compile_cnt > 1)
				printf("%s:\n", file);
#else
			printf("%s\n", file);
#endif

			ldfile = c_flag ? ofile : alloc((unsigned)strlen(BASE)+3);
			if (
#ifdef FCC
			    !strcmp(ext, "s") &&
#endif
			    needsprep(file)) {
				strcpy(tmp_file, TMP_DIR);
				strcat(tmp_file, "/F_XXXXXX");
				mktemp(tmp_file);
				init(call);
				append(call, CPP);
				concat(call, &CPP_FLAGS);
				append(call, INCLUDE);
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
			else	f = mkstr(ldfile, BASE, ".o", (char *)0);
#ifndef ACK_BIN
			if (strcmp(ext, "s")) {
#endif
				append(call, COMP);
#ifdef FCC
				concat(call, &CPP_FLAGS);
#endif
				concat(call, &COMP_FLAGS);
#if FM2 || FCC
				append(call, INCLUDE);
#endif
				append(call, file);
				append(call, f);
#ifndef ACK_BIN
			}
			else {
				append(call, AS);
				append(call, "-o");
				append(call, f);
#ifdef sun
				append(call, "-mc68020");
#endif
				append(call, file);
			}
#endif
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
		if (ldfile) {
			append(&GEN_LDFILES, ldfile);
			ldfile = 0;
		}
	}

	/* *.s to a.out */
	if (RET_CODE == 0 && LDFILES.al_argc > 0) {
#ifndef ACK_BIN
		if (compile_cnt > 1) {
			printf("Linking:\n");
		}
#endif
		init(call);
		append(call, LD);
		concat(call, &LD_HEAD);
		concat(call, &LD_FLAGS);
		append(call, "-o");
#if ACK_BIN && !vax
		strcpy(tmp_file, TMP_DIR);
		strcat(tmp_file, "/LF_XXXXXX");
		mktemp(tmp_file);
		append(call, tmp_file);
		concat(call, sect_align);
#else ACK_BIN && !vax
		append(call, o_FILE);
#endif ACK_BIN && !vax
		concat(call, &LDFILES);
		concat(call, &LD_TAIL);
#if ACK_BIN && !vax
		if (! runvec(call, (char *) 0)) {
			cleanup(tmp_file);
			exit(RET_CODE);
		}
		init(call);
		append(call, CV);
		append(call, tmp_file);
		append(call, o_FILE);
#endif ACK_BIN && !vax
		if (runvec(call, (char *) 0) && GEN_LDFILES.al_argc == 1)
#ifdef ACK_BIN
			;
#else
			remove(GEN_LDFILES.al_argv[0]);
#endif ACK_BIN
#if ACK_BIN && !vax
		cleanup(tmp_file);
#endif ACK_BIN && !vax
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

append(al, arg)
	struct arglist *al;
	char *arg;
{
	if (!arg || !*arg) return;
	if (al->al_argc >= MAXARGC)
		panic("argument list overflow");
	if (*arg == '~' || *arg == '*') {
		char *p;
		char *lb = *arg == '~' ? LIBDIR : BINDIR;
		p = alloc((unsigned)strlen(ROOT_DIR)+strlen(lb)+strlen(arg+1)+1);
		strcpy(p, ROOT_DIR);
		strcat(p, lb);
		strcat(p, arg+1);
		arg = p;
	}
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
	while (i-- > 0) {
		if (**q == '~' || **q == '*') {
			char *lb = **q == '~' ? LIBDIR : BINDIR;
			*p = alloc((unsigned)strlen(ROOT_DIR)+strlen(lb)+strlen(*q+1)+2);
			strcpy(*p, ROOT_DIR);
			strcat(*p, lb);
			strcat(*p++, *q+1);
			q++;
		}
		else	*p++ = *q++;
	}
}

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
	fputs(*ap, stderr);
	while (*++ap) {
		putc(' ', stderr);
		fputs(*ap, stderr);
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
