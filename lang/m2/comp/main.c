/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* M A I N   P R O G R A M */

/* $Header$ */

#include	"debug.h"

#include	<system.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<alloc.h>

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
int		nDEF, mDEF;
struct def 	*Defined;
extern int 	err_occurred;
extern int	fp_used;		/* set if floating point used */

extern		C_inp(), C_exp();
int		(*c_inp)() = C_inp;

main(argc, argv)
	register char **argv;
{
	register int Nargc = 1;
	register char **Nargv = &argv[0];

	ProgName = *argv++;
	warning_classes = W_INITIAL;
	DEFPATH = (char **) Malloc(10 * sizeof(char *));
	mDEF = 10;
	nDEF = 1;

	while (--argc > 0) {
		if (**argv == '-')
			DoOption((*argv++) + 1);
		else
			Nargv[Nargc++] = *argv++;
	}
	Nargv[Nargc] = 0;	/* terminate the arg vector	*/
	if (Nargc < 2) {
		fprint(STDERR, "%s: Use a file argument\n", ProgName);
		exit(1);
	}
	if (options['x']) c_inp = C_exp;
	exit(!Compile(Nargv[1], Nargv[2]));
}

Compile(src, dst)
	char *src, *dst;
{
	extern struct tokenname tkidf[];

	if (! InsertFile(src, (char **) 0, &src)) {
		fprint(STDERR,"%s: cannot open %s\n", ProgName, src);
		return 0;
	}
	LineNumber = 1;
	FileName = src;
	init_idf();
	InitCst();
	reserve(tkidf);
	InitScope();
	InitTypes();
	AddStandards();
#ifdef DEBUG
	if (options['l']) {
		LexScan();
		return 1;
	}
#endif DEBUG
	open_scope(OPENSCOPE);
	GlobalVis = CurrVis;
	close_scope(0);
	C_init(word_size, pointer_size);
	if (! C_open(dst)) fatal("could not open output file");
	C_magic();
	C_ms_emx(word_size, pointer_size);
	CheckForLineDirective();
	CompUnit();
	C_ms_src((int)LineNumber - 1, FileName);
	if (!err_occurred) {
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
	register struct token *tkp = &dot;
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
			print("%s\n", tkp->TOK_REL);
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

AddStandards()
{
	register struct def *df;
	extern struct def *Enter();
	static struct node nilnode = { 0, 0, Value, 0, { INTEGER, 0}};

	Enter("ABS", D_PROCEDURE, std_type, S_ABS);
	Enter("CAP", D_PROCEDURE, std_type, S_CAP);
	Enter("CHR", D_PROCEDURE, std_type, S_CHR);
	Enter("FLOAT", D_PROCEDURE, std_type, S_FLOAT);
	Enter("HIGH", D_PROCEDURE, std_type, S_HIGH);
	Enter("HALT", D_PROCEDURE, std_type, S_HALT);
	Enter("EXCL", D_PROCEDURE, std_type, S_EXCL);
	Enter("DEC", D_PROCEDURE, std_type, S_DEC);
	Enter("INC", D_PROCEDURE, std_type, S_INC);
	Enter("VAL", D_PROCEDURE, std_type, S_VAL);
	Enter("NEW", D_PROCEDURE, std_type, S_NEW);
	Enter("DISPOSE", D_PROCEDURE, std_type, S_DISPOSE);
	Enter("TRUNC", D_PROCEDURE, std_type, S_TRUNC);
	Enter("SIZE", D_PROCEDURE, std_type, S_SIZE);
	Enter("ORD", D_PROCEDURE, std_type, S_ORD);
	Enter("ODD", D_PROCEDURE, std_type, S_ODD);
	Enter("MAX", D_PROCEDURE, std_type, S_MAX);
	Enter("MIN", D_PROCEDURE, std_type, S_MIN);
	Enter("INCL", D_PROCEDURE, std_type, S_INCL);

	Enter("CHAR", D_TYPE, char_type, 0);
	Enter("INTEGER", D_TYPE, int_type, 0);
	Enter("LONGINT", D_TYPE, longint_type, 0);
	Enter("REAL", D_TYPE, real_type, 0);
	Enter("LONGREAL", D_TYPE, longreal_type, 0);
	Enter("BOOLEAN", D_TYPE, bool_type, 0);
	Enter("CARDINAL", D_TYPE, card_type, 0);
	df = Enter("NIL", D_CONST, address_type, 0);
	df->con_const = &nilnode;
	nilnode.nd_INT = 0;
	nilnode.nd_type = address_type;

	Enter("PROC",
		     D_TYPE,
		     construct_type(T_PROCEDURE, NULLTYPE),
		     0);
	df = Enter("BITSET", D_TYPE, bitset_type, 0);
	df = Enter("TRUE", D_ENUM, bool_type, 0);
	df->enm_val = 1;
	df->enm_next = Enter("FALSE", D_ENUM, bool_type, 0);
	df = df->enm_next;
	df->enm_val = 0;
	df->enm_next = 0;
}

do_SYSTEM()
{
	/*	Simulate the reading of the SYSTEM definition module
	*/
	static char systemtext[] = SYSTEMTEXT;

	open_scope(CLOSEDSCOPE);
	Enter("WORD", D_TYPE, word_type, 0);
	Enter("BYTE", D_TYPE, byte_type, 0);
	Enter("ADDRESS", D_TYPE, address_type, 0);
	Enter("ADR", D_PROCEDURE, std_type, S_ADR);
	Enter("TSIZE", D_PROCEDURE, std_type, S_TSIZE);
	if (!InsertText(systemtext, sizeof(systemtext) - 1)) {
		fatal("could not insert text");
	}
	DefModule();
	close_scope(SC_CHKFORW);
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
