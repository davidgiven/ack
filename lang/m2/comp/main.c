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
#include	"main.h"
#include	"debug.h"
#include	"type.h"
#include	"def.h"
#include	"standards.h"

char options[128];
char *ProgName;
int state;
extern int err_occurred;

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
		fprintf(STDERR, "%s: Use one file argument\n", ProgName);
		return 1;
	}
#ifdef DEBUG
	printf("Mod2 compiler -- Debug version\n");
#endif DEBUG
	DO_DEBUG(debug(1,"Debugging level: %d", options['D']));
	return !Compile(Nargv[1]);
}

Compile(src)
	char *src;
{
	extern struct tokenname tkidf[];

	DO_DEBUG(debug(1,"Filename : %s", src));
	if (! InsertFile(src, (char **) 0)) {
		fprintf(STDERR,"%s: cannot open %s\n", ProgName, src);
		return 0;
	}
	LineNumber = 1;
	FileName = src;
	init_idf();
	reserve(tkidf);
	init_scope();
	init_types();
	add_standards();
#ifdef DEBUG
	if (options['L'])
		LexScan();
	else if (options['T'])
		TimeScan();
	else
#endif DEBUG
		CompUnit();
#ifdef DEBUG
	if (options['h']) hash_stat();
#endif DEBUG
	if (err_occurred) return 0;
	return 1;
}

#ifdef DEBUG
LexScan()
{
	register int symb;

	while ((symb = LLlex()) != EOI) {
		printf(">>> %s ", symbol2str(symb));
		switch(symb) {

		case IDENT:
			printf("%s\n", dot.TOK_IDF->id_text);
			break;
		
		case INTEGER:
			printf("%ld\n", dot.TOK_INT);
			break;
		
		case REAL:
			printf("%s\n", dot.TOK_REL);
			break;
		
		case STRING:
			printf("\"%s\"\n", dot.TOK_STR);
			break;

		default:
			putchar('\n');
		}
	}
}

TimeScan() {
	while (LLlex() != -1) /* nothing */;
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
	register struct type *tp;
	struct def *Enter();

	(void) Enter("ABS", D_STDFUNC, NULLTYPE, S_ABS);
	(void) Enter("CAP", D_STDFUNC, NULLTYPE, S_CAP);
	(void) Enter("CHR", D_STDFUNC, NULLTYPE, S_CHR);
	(void) Enter("FLOAT", D_STDFUNC, NULLTYPE, S_FLOAT);
	(void) Enter("HIGH", D_STDFUNC, NULLTYPE, S_HIGH);
	(void) Enter("HALT", D_STDPROC, NULLTYPE, S_HALT);
	(void) Enter("EXCL", D_STDPROC, NULLTYPE, S_EXCL);
	(void) Enter("DEC", D_STDPROC, NULLTYPE, S_DEC);
	(void) Enter("INC", D_STDPROC, NULLTYPE, S_INC);
	(void) Enter("VAL", D_STDFUNC, NULLTYPE, S_VAL);
	(void) Enter("TRUNC", D_STDFUNC, NULLTYPE, S_TRUNC);
	(void) Enter("SIZE", D_STDFUNC, NULLTYPE, S_SIZE);
	(void) Enter("ORD", D_STDFUNC, NULLTYPE, S_ORD);
	(void) Enter("ODD", D_STDFUNC, NULLTYPE, S_ODD);
	(void) Enter("MAX", D_STDFUNC, NULLTYPE, S_MAX);
	(void) Enter("MIN", D_STDFUNC, NULLTYPE, S_MIN);
	(void) Enter("INCL", D_STDPROC, NULLTYPE, S_INCL);

	(void) Enter("CHAR", D_TYPE, char_type, 0);
	(void) Enter("INTEGER", D_TYPE, int_type, 0);
	(void) Enter("LONGINT", D_TYPE, longint_type, 0);
	(void) Enter("REAL", D_TYPE, real_type, 0);
	(void) Enter("LONGREAL", D_TYPE, longreal_type, 0);
	(void) Enter("BOOLEAN", D_TYPE, bool_type, 0);
	(void) Enter("CARDINAL", D_TYPE, card_type, 0);
	(void) Enter("NIL", D_CONST, nil_type, 0);
	(void) Enter("PROC",
		     D_TYPE,
		     construct_type(PROCEDURE, NULLTYPE),
		     0);
	tp = construct_type(SUBRANGE, int_type);
	tp->tp_value.tp_subrange.su_lb = 0;
	tp->tp_value.tp_subrange.su_ub = wrd_size * 8 - 1;
	df = Enter("BITSET", D_TYPE, construct_type(SET, tp), 0);
	df->df_type->tp_size = wrd_size;
	df = Enter("FALSE", D_ENUM, bool_type, 0);
	df->df_value.df_enum.en_val = 0;
	df->df_value.df_enum.en_next = Enter("TRUE", D_ENUM, bool_type, 0);
	df = df->df_value.df_enum.en_next;
	df->df_value.df_enum.en_val = 1;
	df->df_value.df_enum.en_next = 0;
}
