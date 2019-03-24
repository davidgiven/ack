/*	fcc/fm2/fpc 
 Driver for fast ACK compilers.

 Derived from the C compiler driver from Minix.

 Compile this file with
 cc -O -I<ACK home directory>/config -DF?? driver.c
 where F?? is either FCC, FPC, or FM2.
 Install the resulting binaries in the EM bin directory.
 Suggested names: afcc, afm2, and afpc.
 */

#if FM2+FPC+FCC > 1
Something wrong here! Only one of FM2, FPC, or FCC must be defined
#endif

#ifdef sun3
#define MACHNAME	"m68020"
#define SYSNAME		"sun3"
#endif

#ifdef vax4
#define MACHNAME	"vax4"
#define SYSNAME		"vax4"
#endif

#ifdef i386
#define MACHNAME	"i386"
#define SYSNAME		"i386"
#endif

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <stddef.h>
#include "em_path.h"
#include "system.h"

#if(CHAR_BIT!=8)
We do not support machines with non 8-bit characters.
#endif

/* Create em based compiler if otherwise not specified.
 * Size of EM machine depends on current machine. */
#ifndef MACHNAME
#if(INT_MAX==32767)
#define	MACHNAME "em22"
#define	SYSNAME  "em22"
#else
#define	MACHNAME "em44"
#define	SYSNAME  "em44"
#endif
#endif

/*
 Version producing ACK .o files in one pass.
 */
#define MAXARGC	256	/* maximum number of arguments allowed in a list */
#define USTR_SIZE	FILENAME_MAX	/* maximum length of string variable */

typedef char USTRING[USTR_SIZE];

struct arglist
{
	int al_argc;
	char *al_argv[MAXARGC];
};

#define CPP_NAME	"$H/lib/ack/em_cpp"
#define LD_NAME		"$H/lib/ack/em_ass"
#define CV_NAME		"$H/lib/ack/$S/cv"

static char *CPP;
static char *COMP;
static char *cc = "cc";

static int kids = -1;
static int ecount = 0;

struct arglist CPP_FLAGS =
{
#ifdef FCC
		7,
#else
		13,
#endif
		{ "-D__unix", "-D_EM_WSIZE=4", "-D_EM_PSIZE=4", "-D_EM_SSIZE=2",
				"-D_EM_LSIZE=4", "-D_EM_FSIZE=4", "-D_EM_DSIZE=8",
#ifndef FCC
		"-DEM_WSIZE=4", "-DEM_PSIZE=4", "-DEM_SSIZE=2", "-DEM_LSIZE=4",
		"-DEM_FSIZE=4", "-DEM_DSIZE=8",
#endif
	} };

struct arglist LD_HEAD =
{ 2,
{ "$H/lib/$S/head_em",
#ifdef FCC
		"$H/lib/$S/head_$A"
#endif
#ifdef FM2
		"$H/lib/$S/head_m2"
#endif
#ifdef FPC
		"$H/lib/$S/head_pc"
#endif
}};

struct arglist LD_TAIL =
		{
#if defined(sun3) || defined(i386)
				5,
#else
				4,
#endif
				{
#ifdef FCC
						"$H/lib/$S/tail_$A",
#endif
#ifdef FM2
						"$H/lib/$S/tail_m2",
#endif
#ifdef FPC
						"$H/lib/$S/tail_pc",
#endif
#if defined(sun3) || defined(i386)
						"$H/lib/$M/tail_fp",
#endif
						"$H/lib/$M/tail_em", "$H/lib/$S/tail_mon",
						"$H/lib/$M/end_em" } };

struct arglist align =
{ 5,
{
#ifdef sun3
		"-a0:4",
		"-a1:4",
		"-a2:0x20000",
		"-a3:4",
		"-b0:0x2020"
#endif
#ifdef vax4
		"-a0:4",
		"-a1:4",
		"-a2:0x400",
		"-a3:4",
		"-b0:0"
#endif
#ifdef i386
		"-a0:4",
		"-a1:4",
		"-a2:4",
		"-a3:4",
		"-b1:0x1880000"
#endif
	} };

struct arglist COMP_FLAGS;

static char *o_FILE = "a.out"; /* default name for executable file */


#define cleanup(str)		(str && str[0] && remove(str))
#define init(al)		((al)->al_argc = 0)

static char ProgCall[FILENAME_MAX];

static struct arglist SRCFILES;
static struct arglist LDFILES;

static int RET_CODE = 0;

/* The environment variable pointing to ACK_HOME */
static char* ackhome = 0;
/* The environment variable pointing to the temporary directory. */
static char* tmpdir = 0;
static struct arglist LD_FLAGS;

static struct arglist CALL_VEC;

static int o_flag = 0;
static int c_flag = 0;
static int g_flag = 0;
static int v_flag = 0;
static int O_flag = 0;
/* Only ANSI C is now supported. */
static int ansi_c = 1;

char *mkstr(char *, ...);
static char *alloc(unsigned int);
static char *extension(char *);
static char *expand_string(char *);
static void error(char *, char *, char *);
static void warning(char *, char *, char *);
static void panic(char *);
static void append(register struct arglist *, char *);
static void expand(register struct arglist *);
static void concat(struct arglist *, struct arglist *);
static int runvec(struct arglist *, char *);
static int needsprep(char *);

static USTRING ofile;
static USTRING BASE;
static char tmp_file[L_tmpnam];

int noexec = 0;

void trapcc(int sig)
{
	cleanup(ofile);
	cleanup(tmp_file);
	if (ackhome != 0)
	{
		free(ackhome);
	}
	if (tmpdir != 0)
	{
		free(tmpdir);
	}
	exit(EXIT_FAILURE);
}

#ifdef FCC
#define lang_suffix()	"c"
#define comp_name()	"$H/lib/ack/em_cemcom.ansi"
#define ansi_c_name()	"$H/lib/ack/em_cemcom.ansi"
#endif /* FCC */

#ifdef FM2
#define lang_suffix()	"mod"
#define comp_name()	"$H/lib/ack/em_m2"
#endif /* FM2 */

#ifdef FPC
#define lang_suffix()	"p"
#define comp_name()	"$H/lib/ack/em_pc"
#endif /* FPC */

#ifdef FCC
int lang_opt(char *str)
{
	switch (str[1])
	{
	case 'R':
		if (!ansi_c)
		{
			append(&COMP_FLAGS, str);
			return 1;
		}
		break;
	case '-': /* debug options */
		append(&COMP_FLAGS, str);
		return 1;
	case 'w': /* disable warnings */
		if (!ansi_c)
		{
			append(&COMP_FLAGS, str);
			return 1;
		}
		if (str[2])
		{
			str[1] = '-';
			append(&COMP_FLAGS, &str[1]);
		}
		else
			append(&COMP_FLAGS, "-a");
		return 1;
	}
	return 0;
}
#endif /* FCC */

#ifdef FM2
int lang_opt(char *str)
{
	switch(str[1])
	{
		case '-': /* debug options */
		case 'w': /* disable warnings */
		case 'R': /* no runtime checks */
		case 'W': /* add warnings */
		case 'L': /* no line numbers */
		case 'A': /* extra array bound checks */
		case '3': /* only accept 3rd edition Modula-2 */
		append(&COMP_FLAGS, str);
		return 1;
		case 'I':
		append(&COMP_FLAGS, str);
		break; /* !!! */
		case 'U': /* underscores in identifiers allowed */
		if (str[2] == '\0')
		{
			append(&COMP_FLAGS, str);
			return 1;
		}
		break;
		case 'e': /* local extension for Modula-2 compiler:
		 procedure constants
		 */
		str[1] = 'l';
		append(&COMP_FLAGS, str);
		return 1;
	}
	return 0;
}
#endif /* FM2 */

#ifdef FPC
int lang_opt(char *str)
{
	switch(str[1])
	{
		case '-': /* debug options */
		case 'a': /* enable assertions */
		case 'd': /* allow doubles (longs) */
		case 'i': /* set size of integer sets */
		case 't': /* tracing */
		case 'w': /* disable warnings */
		case 'A': /* extra array bound checks */
		case 'C': /* distinguish between lower case and upper case */
		case 'L': /* no FIL and LIN instructions */
		case 'R': /* no runtime checks */
		append(&COMP_FLAGS, str);
		return 1;
		case 'u':
		case 'U':
		/* underscores in identifiers */
		case 's':
		/* only compile standard pascal */
		case 'c':
		/* C type strings */
		if (str[2] == '+' && str[3] == '\0')
		{
			str[2] = 0;
			append(&COMP_FLAGS, str);
			return 1;
		}
	}
	return 0;
}
#endif /* FPC */

char* stringdup(const char* s)
{
	if (s == NULL)
		return NULL;
	char *p = alloc(strlen(s) + sizeof(char));
	strcpy(p, s);
	return p;
}

char* getackhome(void)
{
	char *value = getenv("ACK_HOME");
	if (value == NULL)
	{
#ifndef EM_DIR
		panic("ACK_HOME must be set.");
#else
		return EM_DIR;
#endif
	}
	return value;
}




int main(int argc, char *argv[])
{
	char *str;
	char **argvec;
	int count;
	char *ext;
	FILE* fd;
	register struct arglist *call = &CALL_VEC;
	char *file;
	char *ldfile;
	char *INCLUDE = NULL;
	int compile_cnt = 0;

	ackhome = stringdup(getackhome());
	if (ackhome == NULL)
	{
		panic("ACK_HOME Environment variable is not set.");
	}
	tmpdir = stringdup(sys_gettmpdir());
	if (tmpdir == NULL)
	{
		panic("TMPDIR Environment variable is not set.");
	}

	setbuf(stdout, (char *) 0);
	/* get basebame of application. */
	sys_basename(*argv++, ProgCall);

	/* get compiler to use. */
	COMP = expand_string(comp_name());
	/* get c pre-processor to use */
	CPP = expand_string(CPP_NAME);

#ifdef vax4
	append(&CPP_FLAGS, "-D__vax");
#endif
#ifdef sun3
	append(&CPP_FLAGS, "-D__sun");
#endif
#ifdef m68020
	append(&CPP_FLAGS, "-D__mc68020");
	append(&CPP_FLAGS, "-D__mc68000");
#endif

	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, trapcc);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
		signal(SIGTERM, trapcc);
	while (--argc > 0)
	{
		if (*(str = *argv++) != '-')
		{
			append(&SRCFILES, str);
			continue;
		}

		if (lang_opt(str))
		{
		}
		else
			switch (str[1])
			{

			case 'c': /* stop after producing .o files */
				c_flag = 1;
				break;
			case 'D': /* preprocessor #define */
			case 'U': /* preprocessor #undef */
				append(&CPP_FLAGS, str);
				break;
			case 'I': /* include directory */
				append(&CPP_FLAGS, str);
				break;
			case 'g': /* debugger support */
				append(&COMP_FLAGS, str);
				g_flag = 1;
				break;
			case 'o': /* target file */
				if (argc-- >= 0)
				{
					o_flag = 1;
					o_FILE = *argv++;
					ext = extension(o_FILE);
					if (ext != o_FILE && !strcmp(ext, lang_suffix()))
					{
						error("-o would overwrite %s", o_FILE, NULL);
					}
				}
				break;
			case 'u': /* mark identifier as undefined */
				append(&LD_FLAGS, str);
				if (argc-- >= 0)
					append(&LD_FLAGS, *argv++);
				break;
			case 'O': /* use built in peephole optimizer */
				O_flag = 1;
				break;
			case 'v': /* verbose */
				v_flag++;
				if (str[2] == 'n')
					noexec = 1;
				break;
			case 'l': /* library file */
				append(&SRCFILES, str);
				break;
			case 'M': /* use other compiler (for testing) */
				strcpy(COMP, str + 2);
				break;
			case 'P': /* use other cpp (for testing) */
				strcpy(CPP, str + 2);
				break;
			case 's': /* strip */
				if (str[2] == '\0')
				{
					append(&LD_FLAGS, str);
					break;
				}
				/* fall through */
			default:
				warning("%s flag ignored", str, NULL);
				break;
			}
	}

	if (ecount)
		exit(EXIT_FAILURE);

	count = SRCFILES.al_argc;
	argvec = &(SRCFILES.al_argv[0]);
	while (count-- > 0)
	{
		ext = extension(*argvec);
		if (*argvec[0] != '-' && ext != *argvec++
				&& (!strcmp(ext, lang_suffix())))
		{
			compile_cnt++;
		}
	}

	if (compile_cnt > 1 && c_flag && o_flag)
	{
		warning("-o flag ignored", NULL, NULL);
		o_flag = 0;
	}

#ifdef FM2
	INCLUDE = expand_string("-I$H/lib/m2");
#endif /* FM2 */
#ifdef FCC
/*	INCLUDE = expand_string(
			ansi_c ? "-I$H/include/tail_ac" : "-I$H/include/_tail_cc"); */
	INCLUDE = expand_string("-I$H/share/ack/include");
	append(&COMP_FLAGS, "-L");
#endif /* FCC */
	count = SRCFILES.al_argc;
	argvec = &(SRCFILES.al_argv[0]);

	/* For each source file... */
	while (count-- > 0)
	{
		register char *f;
		sys_basename(file = *argvec++, BASE);

		ext = extension(file);

		/* if not standard input and file is equal to the supported language suffix. */
		if (file[0] != '-' && ext != file && (!strcmp(ext, lang_suffix())))
		{
			if (compile_cnt > 1)
				printf("%s\n", file);

			ldfile = c_flag ? ofile : alloc((unsigned) strlen(BASE) + 3);

			/****************************************************
			 * Run C preprocessor on the source files and save
			 * result in temporary output file.
			 ****************************************************/
			if (
#ifdef FCC
/*			(!strcmp(ext, "s")) && */
#endif
					needsprep(file))
			{
				if (sys_tmpnam(tmp_file)==NULL)
				{
					panic("Cannot get temporary filename.");
				}
				fd = fopen(tmp_file,"w+");
				if (fd==NULL)
				{
					fclose(fd);
					panic("Cannot write temporary file.");
				}
				init(call);
				append(call, CPP);
				concat(call, &CPP_FLAGS);
				append(call, INCLUDE);
				append(call, file);
				if (runvec(call, tmp_file)==EXIT_SUCCESS)
				{
					/* The input file is now the preprocessor
					 * output file.
					 */
					file = tmp_file;
				}
				else
				{
					remove(tmp_file);
					tmp_file[0] = '\0';
					continue;
				}
			}

			/****************************************************
			 * Compile the source file.
			 ****************************************************/
			init(call);
			if (o_flag && c_flag)
			{
				f = o_FILE;
			}
			else
			{
				f = mkstr(ldfile, BASE, ".", "o", (char *) 0);
			}
			append(call, COMP);
			concat(call, &COMP_FLAGS);
#ifdef FM2
			append(call, INCLUDE);
#endif
			append(call, file);
			append(call, f);
			if (runvec(call, (char *) 0)==EXIT_SUCCESS)
			{
				file = f;
			}
			else
			{
				remove(f);
				continue;
			}
			cleanup(tmp_file);
			tmp_file[0] = '\0';
		}

		else if (file[0] != '-' && strcmp(ext, "o") && strcmp(ext, "a"))
		{
			warning("file with unknown suffix (%s) passed to the loader", ext,
					NULL);
		}

		if (c_flag)
			continue;

		/* Add the output object file to the list of files to link. */
		append(&LDFILES, file);
	}

	/* *.s to a.out */
	if (RET_CODE == 0 && LDFILES.al_argc > 0)
	{
		init(call);
//		expand(&LD_HEAD);
//		cc = "cc.2g";
//		expand(&LD_TAIL);
		append(call, expand_string(LD_NAME));
//		concat(call, &align);
		append(call, "-o");

		if (sys_tmpnam(tmp_file)==NULL)
		{
			panic("Cannot get temporary filename.");
		}
		append(call, tmp_file);
//		concat(call, &LD_HEAD);
//		concat(call, &LD_FLAGS);
		concat(call, &LDFILES);
/*		if (g_flag)
			append(call, expand_string("$H/lib/$M/tail_db"));
#ifdef FCC
		if (!ansi_c)
			append(call, expand_string("$H/lib/$S/tail_cc.1s"));
#endif
		concat(call, &LD_TAIL);*/
		if (runvec(call, (char *) 0)==EXIT_FAILURE)
		{
			cleanup(tmp_file);
			exit(RET_CODE);
		}
	/*	init(call);
		append(call, expand_string(CV_NAME));
		append(call, tmp_file);
		append(call, o_FILE);
		runvec(call, (char *) 0);*/
		cleanup(tmp_file);
	}
	if ((LDFILES.al_argc == 0) && (SRCFILES.al_argc==0))
	{
		panic("No input source files or input object files specified.");
	}
	exit(RET_CODE);
}

static int needsprep(char *name)
{
/*	FILE *file;
	char fc;

	file = fopen(name, "r");
	if (file == 0)
		return 0;
	if (fread(file, &fc, 1) != 1)
		fc = 0;
	fclose(file);
	return fc == '#';*/
	return 1;
}

static char * alloc(unsigned int u)
{
	char *p = malloc(u);

	if (p == NULL)
		panic("no space");
	return p;
}

static char * expand_string(char *s)
{
	char buf[1024];
	register char *p = s;
	register char *q = &buf[0];
	int expanded = 0;

	if (!p)
		return p;
	while (*p)
	{
		if (*p == '$')
		{
			p++;
			expanded = 1;
			switch (*p++)
			{
			case 'A':
				if (ansi_c)
					strcpy(q, "ac");
				else
					strcpy(q, cc);
				break;
			case 'H':
				strcpy(q, ackhome);
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
			while (*q)
				q++;
		}
		else
			*q++ = *p++;
	}
	if (!expanded)
		return s;
	*q++ = '\0';
	p = alloc((unsigned int) (q - buf));
	return strcpy(p, buf);
}

static void append(register struct arglist *al, char *arg)
{
	if (!arg || !*arg)
		return;
	if (al->al_argc >= MAXARGC)
		panic("argument list overflow");
	al->al_argv[(al->al_argc)++] = arg;
}

static void expand(register struct arglist *al)
{
	register int i = al->al_argc;
	register char **p = &(al->al_argv[0]);

	while (i-- > 0)
	{
		*p = expand_string(*p);
		p++;
	}
}

static void concat(struct arglist *al1, struct arglist *al2)
{
	register int i = al2->al_argc;
	register char **p = &(al1->al_argv[al1->al_argc]);
	register char **q = &(al2->al_argv[0]);

	if ((al1->al_argc += i) >= MAXARGC)
		panic("argument list overflow");
	while (i-- > 0)
	{
		*p++ = *q++;
	}
}

char *mkstr(char *dst, ...)
{
	va_list ap;

	va_start(ap, dst);
	{
		register char *p;
		register char *q;

		q = dst;
		p = va_arg(ap, char *);

		while (p)
		{
			while ((*q++ = *p++) != 0)
				;
			q--;
			p = va_arg(ap, char *);
		}
	}
	va_end(ap);

	return dst;
}

static char * extension(char *fn)
{
	register char *c = fn;

	while (*c++)
		;
	while (*--c != '.' && c >= fn)
	{
	}
	if (c++ < fn || !*c)
		return fn;
	return c;
}

/* Converts an argument structure to a string.
 * The memory allocated should be freed by
 * the caller.
 *  */
char *arg2str(struct arglist *vec)
{
	int i;
	unsigned int length;
	char *p;

	length = 0;
	for (i = 0; i < vec->al_argc; i++)
	{
		length += strlen(vec->al_argv[i]);
	}
	/* Add space character */
	length += vec->al_argc * sizeof(char);
	p = alloc(length + sizeof(char));
	*p = '\0';
	for (i = 0; i < vec->al_argc; i++)
	{
		strcat(p, vec->al_argv[i]);
		strcat(p, " ");
	}
	return p;
}

/* Run the system command, and outputs to "outp" if not
 * NULL.
 */
static int runvec(struct arglist *vec, char *outp)
{
	int status;
	char *p;
	char *redirect;

	redirect = NULL;

	/* Check if a command interpreter is available. */
	if (system(NULL) == 0)
	{
		panic("No command interpreter available!");
	}
	if(outp != NULL)
	{
		redirect = alloc(strlen(outp)+sizeof(char)*2);
		strcpy(redirect,"1>");
		strcat(redirect,outp);
		append(vec,redirect);
	}
	p = arg2str(vec);

	status = system(p);
	free(p);
	if (outp != NULL)
	{
		free(redirect);
	}
	if (status == 0)
	{
		RET_CODE = EXIT_SUCCESS;
	}
	else
	{
		RET_CODE = EXIT_FAILURE;
	}
	return status;
}

/*VARARGS1*/
static void error(char *str, char *s1, char *s2)
{
	fprintf(stderr, "%s: ", ProgCall);
	fprintf(stderr, str, s1, s2);
	putc('\n', stderr);
	ecount++;
}

/*VARARGS1*/
static void warning(char *str, char *s1, char *s2)
{
	fprintf(stderr, "%s: (warning) ", ProgCall);
	fprintf(stderr, str, s1, s2);
	putc('\n', stderr);
}

static void panic(char *str)
{
	error(str, NULL, NULL);
	trapcc(SIGINT);
}

