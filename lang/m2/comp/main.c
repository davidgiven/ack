/* M A I N   P R O G R A M */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

#include	"debug.h"
#include	"ndir.h"

#include	<system.h>
#include	<em_arith.h>
#include	<em_label.h>

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

char	options[128];
int	DefinitionModule; 
int	SYSTEMModule = 0;
char	*ProgName;
extern int err_occurred;
char	*DEFPATH[NDIRS+1];
struct def *Defined;

main(argc, argv)
	char *argv[];
{
	register int Nargc = 1;
	register char **Nargv = &argv[0];

	ProgName = *argv++;

	while (--argc > 0) {
		if (**argv == '-')
			DoOption((*argv++) + 1);
		else
			Nargv[Nargc++] = *argv++;
	}
	Nargv[Nargc] = 0;	/* terminate the arg vector	*/
	if (Nargc < 2) {
		fprint(STDERR, "%s: Use a file argument\n", ProgName);
		return 1;
	}
#ifdef DEBUG
	DO_DEBUG(1, debug("Debugging level: %d", options['D']));
#endif DEBUG
	return !Compile(Nargv[1], Nargv[2]);
}

Compile(src, dst)
	char *src, *dst;
{
	extern struct tokenname tkidf[];

	DO_DEBUG(1, debug("Filename : %s", src));
	DO_DEBUG(1, (!dst || debug("Targetfile: %s", dst)));
	if (! InsertFile(src, (char **) 0, &src)) {
		fprint(STDERR,"%s: cannot open %s\n", ProgName, src);
		return 0;
	}
	LineNumber = 1;
	FileName = src;
	DEFPATH[0] = "";
	DEFPATH[NDIRS] = 0;
	init_idf();
	InitCst();
	reserve(tkidf);
	init_scope();
	init_types();
	InitDef();
	AddStandards();
#ifdef DEBUG
	if (options['l']) {
		LexScan();
		return 1;
	}
#endif DEBUG
	open_scope(CLOSEDSCOPE);
	GlobalScope = CurrentScope;
	C_init(word_size, pointer_size);
	if (! C_open(dst)) {
		fatal("Could not open output file");
	}
	C_magic();
	C_ms_emx(word_size, pointer_size);
	CompUnit();
	if (err_occurred) {
		C_close();
		return 0;
	}
	WalkModule(Defined);
	C_close();
	if (err_occurred) return 0;
	return 1;
}

#ifdef DEBUG
LexScan()
{
	register int symb;
	char *symbol2str();

	while ((symb = LLlex()) > 0) {
		print(">>> %s ", symbol2str(symb));
		switch(symb) {

		case IDENT:
			print("%s\n", dot.TOK_IDF->id_text);
			break;
		
		case INTEGER:
			print("%ld\n", dot.TOK_INT);
			break;
		
		case REAL:
			print("%s\n", dot.TOK_REL);
			break;
		
		case STRING:
			print("\"%s\"\n", dot.TOK_STR);
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
	struct def *Enter();
	static struct node nilnode = { 0, 0, Value, 0, { INTEGER, 0}};

	(void) Enter("ABS", D_PROCEDURE, std_type, S_ABS);
	(void) Enter("CAP", D_PROCEDURE, std_type, S_CAP);
	(void) Enter("CHR", D_PROCEDURE, std_type, S_CHR);
	(void) Enter("FLOAT", D_PROCEDURE, std_type, S_FLOAT);
	(void) Enter("HIGH", D_PROCEDURE, std_type, S_HIGH);
	(void) Enter("HALT", D_PROCEDURE, std_type, S_HALT);
	(void) Enter("EXCL", D_PROCEDURE, std_type, S_EXCL);
	(void) Enter("DEC", D_PROCEDURE, std_type, S_DEC);
	(void) Enter("INC", D_PROCEDURE, std_type, S_INC);
	(void) Enter("VAL", D_PROCEDURE, std_type, S_VAL);
	(void) Enter("TRUNC", D_PROCEDURE, std_type, S_TRUNC);
	(void) Enter("SIZE", D_PROCEDURE, std_type, S_SIZE);
	(void) Enter("ORD", D_PROCEDURE, std_type, S_ORD);
	(void) Enter("ODD", D_PROCEDURE, std_type, S_ODD);
	(void) Enter("MAX", D_PROCEDURE, std_type, S_MAX);
	(void) Enter("MIN", D_PROCEDURE, std_type, S_MIN);
	(void) Enter("INCL", D_PROCEDURE, std_type, S_INCL);

	(void) Enter("CHAR", D_TYPE, char_type, 0);
	(void) Enter("INTEGER", D_TYPE, int_type, 0);
	(void) Enter("LONGINT", D_TYPE, longint_type, 0);
	(void) Enter("REAL", D_TYPE, real_type, 0);
	(void) Enter("LONGREAL", D_TYPE, longreal_type, 0);
	(void) Enter("BOOLEAN", D_TYPE, bool_type, 0);
	(void) Enter("CARDINAL", D_TYPE, card_type, 0);
	df = Enter("NIL", D_CONST, address_type, 0);
	df->con_const = &nilnode;
	nilnode.nd_INT = 0;
	nilnode.nd_type = address_type;

	(void) Enter("PROC",
		     D_TYPE,
		     construct_type(T_PROCEDURE, NULLTYPE),
		     0);
	df = Enter("BITSET", D_TYPE, bitset_type, 0);
	df = Enter("FALSE", D_ENUM, bool_type, 0);
	df->enm_val = 0;
	df->enm_next = Enter("TRUE", D_ENUM, bool_type, 0);
	df = df->enm_next;
	df->enm_val = 1;
	df->enm_next = 0;
}

do_SYSTEM()
{
	/*	Simulate the reading of the SYSTEM definition module
	*/
	char *SYSTEM = "\
DEFINITION MODULE SYSTEM;\n\
PROCEDURE NEWPROCESS(P:PROC; A:ADDRESS; n:CARDINAL; VAR p1:ADDRESS);\n\
PROCEDURE TRANSFER(VAR p1,p2:ADDRESS);\n\
END SYSTEM.\n";

	open_scope(CLOSEDSCOPE);
	(void) Enter("WORD", D_TYPE, word_type, 0);
	(void) Enter("ADDRESS", D_TYPE, address_type, 0);
	(void) Enter("ADR", D_PROCEDURE, std_type, S_ADR);
	(void) Enter("TSIZE", D_PROCEDURE, std_type, S_TSIZE);
	if (!InsertText(SYSTEM, strlen(SYSTEM))) {
		fatal("Could not insert text");
	}
	SYSTEMModule = 1;
	DefModule();
	close_scope(0);
	SYSTEMModule = 0;
}

AtEoIT()
{
	/*	Make the end of the text noticable
	*/
	return 1;
}
