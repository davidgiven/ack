/* M A I N   P R O G R A M */

static char *RcsId = "$Header$";

#include	<system.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	"input.h"
#include	"f_info.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"debug.h"
#include	"type.h"
#include	"def.h"
#include	"scope.h"
#include	"standards.h"
#include	"tokenname.h"

char	options[128];
int	DefinitionModule; 
int	SYSTEMModule = 0;
char	*ProgName;
extern int err_occurred;
char	*DEFPATH[128];
char	*getenv();

main(argc, argv)
	char *argv[];
{
	register Nargc = 1;
	register char **Nargv = &argv[0];

	ProgName = *argv++;

	while (--argc > 0) {
		if (**argv == '-')
			Option(*argv++);
		else
			Nargv[Nargc++] = *argv++;
	}
	Nargv[Nargc] = 0;	/* terminate the arg vector	*/
	if (Nargc != 2) {
		fprint(STDERR, "%s: Use one file argument\n", ProgName);
		return 1;
	}
#ifdef DEBUG
	print("Mod2 compiler -- Debug version\n");
#endif DEBUG
	DO_DEBUG(1, debug("Debugging level: %d", options['D']));
	return !Compile(Nargv[1]);
}

Compile(src)
	char *src;
{
	extern struct tokenname tkidf[];

	DO_DEBUG(1, debug("Filename : %s", src));
	if (! InsertFile(src, (char **) 0, &src)) {
		fprint(STDERR,"%s: cannot open %s\n", ProgName, src);
		return 0;
	}
	LineNumber = 1;
	FileName = src;
	init_DEFPATH();
	init_idf();
	init_cst();
	reserve(tkidf);
	init_scope();
	init_types();
	add_standards();
#ifdef DEBUG
	if (options['L']) LexScan();
	else {
#endif DEBUG
		(void) open_scope(CLOSEDSCOPE, 0);
		GlobalScope = CurrentScope;
		CompUnit();
#ifdef DEBUG
	}
	if (options['h']) hash_stat();
#endif DEBUG
	if (err_occurred) return 0;
	return 1;
}

#ifdef DEBUG
LexScan()
{
	register int symb;

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

Option(str)
	char *str;
{
	options[str[1]]++;	/* switch option on	*/
}

add_standards()
{
	register struct def *df;
	struct def *Enter();

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
	(void) Enter("NIL", D_CONST, address_type, 0);
	(void) Enter("PROC",
		     D_TYPE,
		     construct_type(PROCEDURE, NULLTYPE),
		     0);
	df = Enter("BITSET", D_TYPE, bitset_type, 0);
	df = Enter("FALSE", D_ENUM, bool_type, 0);
	df->enm_val = 0;
	df->enm_next = Enter("TRUE", D_ENUM, bool_type, 0);
	df = df->enm_next;
	df->enm_val = 1;
	df->enm_next = 0;
}

init_DEFPATH()
{
	register char *p = getenv("M2path");
	register int i = 0;

	if (p) {
		while (*p) {
			DEFPATH[i++] = p;
			while (*p && *p != ':') p++;
			if (*p) *p++ = '\0';
		}
	}
	DEFPATH[i] = 0;
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

	open_scope(CLOSEDSCOPE, 0);
	(void) Enter("WORD", D_TYPE, word_type, 0);
	(void) Enter("ADDRESS", D_TYPE, address_type, 0);
	(void) Enter("ADR", D_PROCEDURE, std_type, S_ADR);
	(void) Enter("TSIZE", D_PROCEDURE, std_type, S_TSIZE);
	if (!InsertText(SYSTEM, strlen(SYSTEM))) {
		fatal("Could not insert text");
	}
	SYSTEMModule = 1;
	DefModule();
	close_scope();
	SYSTEMModule = 0;
}

AtEoIT()
{
	/*	Make the end of the text noticable
	*/
	return 1;
}
