/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* M A I N   P R O G R A M */

/* $Id$ */

#include	"debug.h"

#include	<system.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include	<alloc.h>
#include	<assert.h>
#include	<stb.h>

#include	"strict3rd.h"
#include	"dbsymtab.h"
#include	"input.h"
#include	"f_info.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"type.h"
#include	"def.h"
#include	"scope.h"
#include	"standards.h"
#include	"tokenname.h"
#include	"node.h"
#include	"warning.h"
#include	"SYSTEM.h"

int		state;			/* either IMPLEMENTATION or PROGRAM */
char		options[128];
int		DefinitionModule; 
char		*ProgName;
char		**DEFPATH;
int		nDEF = 2, mDEF = 10;
int		pass_1 = 1;
t_def	 	*Defined;
extern int 	err_occurred;
extern int	fp_used;		/* set if floating point used */
static t_node	_emptystat = { Stat, 0, NULLTYPE, { ';' }};
t_node		*EmptyStatement = &_emptystat;

main(argc, argv)
	register char **argv;
{
	register int Nargc = 1;
	register char **Nargv = &argv[0];

	ProgName = *argv++;
	DEFPATH = (char **) Malloc((unsigned)mDEF * sizeof(char *));
	DEFPATH[1] = 0;

	while (--argc > 0) {
		if (**argv == '-')
			DoOption((*argv++) + 1);
		else
			Nargv[Nargc++] = *argv++;
	}
	Nargv[Nargc] = 0;	/* terminate the arg vector	*/
	if (Nargc < 2) {
		fprint(STDERR, "%s: Use a file argument\n", ProgName);
		sys_stop(S_EXIT);
	}
	sys_stop(Compile(Nargv[1], Nargv[2]) ? S_END : S_EXIT);
	/*NOTREACHED*/
}

Compile(src, dst)
	char *src, *dst;
{
	extern struct tokenname tkidf[];
	extern char *getwdir();

	if (! InsertFile(src, (char **) 0, &src)) {
		fprint(STDERR,"%s: cannot open %s\n", ProgName, src);
		return 0;
	}
	LineNumber = 1;
	FileName = src;
	WorkingDir = getwdir(src);
	C_init(word_size, pointer_size);
	if (! C_open(dst)) fatal("could not open output file");
	C_magic();
	C_ms_emx(word_size, pointer_size);
#ifdef DBSYMTAB
	if (options['g']) {
		C_ms_std(FileName, N_SO, 0);
	}
#endif /* DBSYMTAB */
	init_idf();
	InitCst();
	reserve(tkidf);
	InitScope();
	InitTypes();
	AddStandards();
#ifdef DEBUG
	if (options['t']) {
		LexScan();
		return 1;
	}
#endif /* DEBUG */
	open_scope(OPENSCOPE);
	GlobalVis = CurrVis;
	close_scope(0);
	CheckForLineDirective();
	CompUnit();
	C_ms_src((int)LineNumber - 1, FileName);
	if (!err_occurred) {
		pass_1 = 0;
		C_exp(Defined->mod_vis->sc_scope->sc_name);
		WalkModule(Defined);
		if (fp_used) C_ms_flt();
	}
	C_close();
#ifdef DEBUG
	if (options['i']) Info();
#endif
	return ! err_occurred;
}

#ifdef DEBUG
LexScan()
{
	register t_token *tkp = &dot;
	extern char *symbol2str();

	while (LLlex() > 0) {
		print(">>> %s ", symbol2str(tkp->tk_symb));
		switch(tkp->tk_symb) {

		case IDENT:
			print("%s\n", tkp->TOK_IDF->id_text);
			break;
		
		case INTEGER:
			print("%ld\n", tkp->TOK_INT);
			break;
		
		case REAL:
			print("%s\n", tkp->TOK_RSTR);
			break;

		case STRING:
			print("\"%s\"\n", tkp->TOK_STR);
			break;

		default:
			print("\n");
		}
	}
}
#endif

static struct stdproc {
	char *st_nam;
	int  st_con;
} stdprocs[] = {
	{ "ABS",	S_ABS },
	{ "CAP",	S_CAP },
	{ "CHR",	S_CHR },
	{ "FLOAT",	S_FLOAT },
	{ "HIGH",	S_HIGH },
	{ "HALT",	S_HALT },
	{ "EXCL",	S_EXCL },
	{ "DEC",	S_DEC },
	{ "INC",	S_INC },
	{ "VAL",	S_VAL },
#ifndef STRICT_3RD_ED
	{ "NEW",	S_NEW },
	{ "DISPOSE",	S_DISPOSE },
#endif
	{ "TRUNC",	S_TRUNC },
	{ "SIZE",	S_SIZE },
	{ "ORD",	S_ORD },
	{ "ODD",	S_ODD },
	{ "MAX",	S_MAX },
	{ "MIN",	S_MIN },
	{ "INCL",	S_INCL },
	{ "LONG",	S_LONG },
	{ "SHORT",	S_SHORT },
	{ "TRUNCD",	S_TRUNCD },
	{ "FLOATD",	S_FLOATD },
	{ 0,		0 }
};

static struct stdproc sysprocs[] = {
	{ "TSIZE",	S_TSIZE },
	{ "ADR",	S_ADR },
	{ 0,		0 }
};

extern t_def *Enter(), *EnterType();

AddProcs(p)
	register struct stdproc *p;
{
	for (; p->st_nam != 0; p++) {
		if (! Enter(p->st_nam, D_PROCEDURE, std_type, p->st_con)) {
			assert(0);
		}
	}
}

AddStandards()
{
	register t_def *df;
	static t_token nilconst = { INTEGER, 0};

	AddProcs(stdprocs);
	EnterType("CHAR", char_type);
	EnterType("INTEGER", int_type);
	EnterType("LONGINT", longint_type);
	EnterType("REAL", real_type);
	EnterType("LONGREAL", longreal_type);
	EnterType("CARDINAL", card_type);
	if (options['l']) {
		/* local extension: LONGCARD. */
		EnterType("LONGCARD", longcard_type);
	}
	EnterType("(void)", void_type);
	df = Enter("NIL", D_CONST, address_type, 0);
	df->con_const = nilconst;

	EnterType("PROC", construct_type(T_PROCEDURE, NULLTYPE));
	EnterType("BITSET", bitset_type);
	df = Enter("FALSE", D_ENUM, bool_type, 0);
	bool_type->enm_enums = df;
	df->enm_next = Enter("TRUE", D_ENUM, bool_type, 0);
	df->enm_next->enm_val = 1;
	assert(df->enm_val == 0 && df->enm_next->enm_next == 0);
	EnterType("BOOLEAN", bool_type);
}

do_SYSTEM()
{
	/*	Simulate the reading of the SYSTEM definition module
	*/
	static char systemtext[] = SYSTEMTEXT;

	EnterType("WORD", word_type);
	EnterType("BYTE", byte_type);
	EnterType("ADDRESS",address_type);
	AddProcs(sysprocs);
	if (!InsertText(systemtext, sizeof(systemtext) - 1)) {
		fatal("could not insert text");
	}
	DefModule();
}

#ifdef DEBUG

int	cntlines;

Info()
{
	extern int cnt_def, cnt_node, cnt_paramlist, cnt_type,
		   cnt_switch_hdr, cnt_case_entry, 
		   cnt_scope, cnt_scopelist, cnt_tmpvar;

	print("\
%6d def\n%6d node\n%6d paramlist\n%6d type\n%6d switch_hdr\n\
%6d case_entry\n%6d scope\n%6d scopelist\n%6d tmpvar\n",
cnt_def, cnt_node, cnt_paramlist, cnt_type,
cnt_switch_hdr, cnt_case_entry, 
cnt_scope, cnt_scopelist, cnt_tmpvar);
print("\nNumber of lines read: %d\n", cntlines);
}
#endif

void
No_Mem()
{
	fatal("out of memory");
}

void
C_failed()
{
	fatal("write failed");
}
