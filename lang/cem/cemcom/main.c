/* $Header$ */
/* MAIN PROGRAM */

#include	<system.h>
#include	"nopp.h"
#include	"target_sizes.h"
#include	"debug.h"
#include	"myalloc.h"
#include	"use_tmp.h"
#include	"maxincl.h"
#include	"inputtype.h"

#include	"input.h"
#include	"level.h"
#include	"idf.h"
#include	"arith.h"
#include	"type.h"
#include	"declar.h"
#include	"tokenname.h"
#include	"Lpars.h"
#include	"LLlex.h"
#include	"alloc.h"
#include	"specials.h"

extern struct tokenname tkidf[], tkother[];
extern char *symbol2str();
char options[128];			/* one for every char	*/

#ifndef NOPP
int inc_pos = 1;			/* place where next -I goes */
char *inctable[MAXINCL] = {		/* list for includes	*/
	".",
	"/usr/include",
	0
};

char **WorkingDir = &inctable[0];
#endif NOPP

struct sp_id special_ids[] =	{
	{"setjmp", SP_SETJMP},	/* non-local goto's are registered	*/
	{0, 0}
};

arith
	short_size = SZ_SHORT,
	word_size = SZ_WORD,
	dword_size = (2 * SZ_WORD),
	int_size = SZ_INT,
	long_size = SZ_LONG,
#ifndef NOFLOAT
	float_size = SZ_FLOAT,
	double_size = SZ_DOUBLE,
#endif NOFLOAT
	pointer_size = SZ_POINTER;

int
	short_align = AL_SHORT,
	word_align = AL_WORD,
	int_align = AL_INT,
	long_align = AL_LONG,
#ifndef NOFLOAT
	float_align = AL_FLOAT,
	double_align = AL_DOUBLE,
#endif NOFLOAT
	pointer_align = AL_POINTER,
	struct_align = AL_STRUCT,
	union_align = AL_UNION;

#ifndef NOPP
arith ifval;	/* ifval will contain the result of the #if expression	*/
#endif NOPP

char *prog_name;

main(argc, argv)
	char *argv[];
{
	/* parse and interpret the command line options	*/
	prog_name = argv[0];

#ifdef	OWNALLOC
	init_mem();
#endif	OWNALLOC

	init_hmask();
#ifndef NOPP
	init_pp();	/* initialise the preprocessor macros	*/
#endif NOPP

	/*	Note: source file "-" indicates that the source is supplied
		as standard input.  This is only allowed if READ_IN_ONE is
		not defined!
	*/
#ifdef READ_IN_ONE
	while (argc > 1 && *argv[1] == '-')
#else READ_IN_ONE
	while (argc > 1 && *argv[1] == '-' && argv[1][1] != '\0')
#endif READ_IN_ONE
	{
		char *par = &argv[1][1];

		if (*par == '-')
			par++;
		do_option(par);
		argc--, argv++;
	}
	compile(argc - 1, &argv[1]);

#ifdef	OWNALLOC
#ifdef	DEBUG
	mem_stat();
#endif	DEBUG
#endif	OWNALLOC

#ifdef	DEBUG
	hash_stat();
#endif	DEBUG

	return err_occurred;
}

char *source = 0;
char *destination = 0;

char *nmlist = 0;

#ifdef USE_TMP
extern char *mktemp();		/* library routine	*/
char *tmpfdir = "/tmp";		/* where to keep the temporary file */
static char *tmpfname = "/Cem.XXXXXX";
char *tmpfile = 0;
#endif USE_TMP

compile(argc, argv)
	char *argv[];
{
#ifdef USE_TMP
	char tmpf[256];
#endif

#ifndef NOPP
	int pp_only = options['E'] || options['P'];
#endif NOPP

	switch (argc) {
	case 1:
#ifndef NOPP
		if (!pp_only)
#endif NOPP
			fatal("%s: destination file not specified", prog_name);
		break;
	case 2:
		destination = argv[1];
		break;
	case 3:
		nmlist = argv[2];
		destination = argv[1];
		break;
	default:
		fatal("use: %s source destination [namelist]", prog_name);
		break;
	}
	source = strcmp(argv[0], "-") ? argv[0] : 0;

#ifdef USE_TMP
	strcpy(tmpf, tmpfdir);
	strcat(tmpf, tmpfname);
	tmpfile = mktemp(tmpf);
#endif USE_TMP

	if (strcmp(destination, "-") == 0)
		destination = 0;
	if (!InsertFile(source, (char **) 0)) /* read the source file	*/
		fatal("%s: no source file %s\n", prog_name, 
			source ? source : "stdin");
	init();
	/* FileName = source; /* needed ???	*/
	PushLex();

#ifndef NOPP
	if (pp_only) /* run the preprocessor as if it is stand-alone	*/
		preprocess();
	else	{
#endif NOPP

#ifdef	USE_TMP
		init_code(tmpfile);
#else	USE_TMP
		init_code(destination);
#endif	USE_TMP

		/* compile the source text			*/
		C_program();
		end_code();

#ifdef USE_TMP
		prepend_scopes(destination);
		AppendFile(tmpfile, destination);
		sys_remove(tmpfile);
#endif USE_TMP

#ifdef	DEBUG
		if (options['u'])	{
			unstack_level();	/* unstack L_GLOBAL */
		}
		if (options['f'] || options['t'])
			dumpidftab("end of main", options['f'] ? 0 : 0);
#endif	DEBUG
#ifndef NOPP
	}
#endif	NOPP
	PopLex();
}

init()
{
	init_cst();	/* initialize variables of "cstoper.c"		*/
	reserve(tkidf);		/* mark the C reserved words as such	*/
	init_specials(special_ids);	/* mark special ids as such	*/

	if (options['R'])
		reserve(tkother);

	char_type = standard_type(CHAR, 0, 1, (arith)1);
	uchar_type = standard_type(CHAR, UNSIGNED, 1, (arith)1);

	short_type = standard_type(SHORT, 0, short_align, short_size);
	ushort_type = standard_type(SHORT, UNSIGNED, short_align, short_size);

	/*	Treat type `word' as `int', having its own size and
		alignment requirements.
		This type is transparent to the user.
	*/
	word_type = standard_type(INT, 0, word_align, word_size);
	uword_type = standard_type(INT, UNSIGNED, word_align, word_size);

	int_type = standard_type(INT, 0, int_align, int_size);
	uint_type = standard_type(INT, UNSIGNED, int_align, int_size);

	long_type = standard_type(LONG, 0, long_align, long_size);
	ulong_type = standard_type(LONG, UNSIGNED, long_align, long_size);

#ifndef NOFLOAT
	float_type = standard_type(FLOAT, 0, float_align, float_size);
	double_type = standard_type(DOUBLE, 0, double_align, double_size);
#endif NOFLOAT
	void_type = standard_type(VOID, 0, 0, (arith)0);
	label_type = standard_type(LABEL, 0, 0, (arith)0);
	error_type = standard_type(ERRONEOUS, 0, 1, (arith)1);

	/*	Pointer Arithmetic type: all arithmetics concerning
		pointers is supposed to be performed in the
		pointer arithmetic type which is equal to either
		int_type or long_type, depending on the pointer_size
	*/
	if (pointer_size == word_size)
		pa_type = word_type;
	else
	if (pointer_size == short_size)
		pa_type = short_type;
	else
	if (pointer_size == int_size)
		pa_type = int_type;
	else
	if (pointer_size == long_size)
		pa_type = long_type;
	else
		fatal("pointer size incompatible with any integral size");

	if (int_size != word_size)
		fatal("int_size and word_size are not equal");
	if (short_size > int_size || int_size > long_size)
		fatal("sizes of short/int/long decreasing");

	/* Build a type for function returning int, RM 13 */
	funint_type = construct_type(FUNCTION, int_type, (arith)0);
	string_type = construct_type(POINTER, char_type, (arith)0);

	/* Define the standard type identifiers. */
	add_def(str2idf("char"), TYPEDEF, char_type, L_UNIVERSAL);
	add_def(str2idf("int"), TYPEDEF, int_type, L_UNIVERSAL);
#ifndef NOFLOAT
	add_def(str2idf("float"), TYPEDEF, float_type, L_UNIVERSAL);
	add_def(str2idf("double"), TYPEDEF, double_type, L_UNIVERSAL);
#endif NOFLOAT
	add_def(str2idf("void"), TYPEDEF, void_type, L_UNIVERSAL);
	stack_level();
}

init_specials(si)
	struct sp_id *si;
{
	while (si->si_identifier)	{
		struct idf *idf = str2idf(si->si_identifier);
		
		if (idf->id_special)
			fatal("maximum identifier length insufficient");
		idf->id_special = si->si_flag;
		si++;
	}
}

#ifndef NOPP
preprocess()
{
	/*	preprocess() is the "stand-alone" preprocessor which
		consecutively calls the lexical analyzer LLlex() to get
		the tokens and prints them in a suitable way.
	*/
	static unsigned int lastlineno = 0;
	static char *lastfilenm = "";

	while (LLlex() !=  EOI)	{
		if (lastlineno != dot.tk_line)	{
			if (strcmp(lastfilenm, dot.tk_file) == 0)	{
				if (dot.tk_line - lastlineno <= 1)	{
					lastlineno++;
					print("\n");
				}
				else	{
					lastlineno = dot.tk_line;
					if (!options['P'])
						print("\n#line %ld \"%s\"\n",
							lastlineno,
							lastfilenm
						);
				}
			}
			else	{
				lastfilenm = dot.tk_file;
				lastlineno = dot.tk_line;
				if (!options['P'])
					print("\n#line %ld \"%s\"\n",
						lastlineno, lastfilenm);
			}
		}
		else
		if (strcmp(lastfilenm, dot.tk_file) != 0)	{
			lastfilenm = dot.tk_file;
			if (!options['P'])
				print("\n#line %ld \"%s\"\n",
					lastlineno, lastfilenm);
		}
		switch (DOT)	{
		case IDENTIFIER:
		case TYPE_IDENTIFIER:
			print("%s ", dot.tk_idf->id_text);
			break;
		case STRING:
		{
			char sbuf[1024];	/* a transient buffer */
			char *bts2str();

			print("\"%s\" ", bts2str(dot.tk_bts, dot.tk_len, sbuf));
			break;
		}
		case INTEGER:
			print("%ld ", dot.tk_ival);
			break;
#ifndef NOFLOAT
		case FLOATING:
			print("%s ", dot.tk_fval);
			break;
#endif NOFLOAT
		case EOI:
		case EOF:
			return;
		default:	/* very expensive...	*/
			print("%s ", symbol2str(DOT));
		}
	}
}
#endif NOPP

#ifdef USE_TMP
AppendFile(src, dst)
	char *src, *dst;
{
	File *fp_src, *fp_dst;
	char buf[BUFSIZ];
	int n;

	if (sys_open(src, OP_READ, &fp_src) == 0)
		fatal("cannot read %s", src);
	if (dst) {
		if (sys_open(dst, OP_APPEND, &fp_dst) == 0)
			fatal("cannot write to %s", src);
	}
	else
		fp_dst = STDOUT;
	while (sys_read(fp_src, buf, BUFSIZ, &n) != 0 && n > 0)
		if (sys_write(fp_dst, buf, n) == 0)
			fatal("(AppendFile) write error");
	if (n != 0)
		fatal("(AppendFile) read error");
	sys_close(fp_src);
	if (fp_dst != STDOUT)
		sys_close(fp_dst);
}
#endif USE_TMP
