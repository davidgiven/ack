/* M A I N   P R O G R A M */

#include	"debug.h"

#include	<em.h>
#include	<em_mes.h>
#include	<system.h>

#include	"LLlex.h"
#include	"Lpars.h"
#include	"const.h"
#include	"def.h"
#include	"f_info.h"
#include	"idf.h"
#include	"input.h"
#include	"main.h"
#include	"node.h"
#include	"required.h"
#include	"tokenname.h"
#include	"type.h"

char		options[128];
char		*ProgName;
char		*input = "input";
char		*output = "output";

label		data_label;
label		text_label;

struct def	*program;
extern int	fp_used;	/* set if floating point used */


main(argc, argv)
	register char **argv;
{
	register int Nargc = 1;
	register char **Nargv = &argv[0];

	ProgName = *argv++;

	while( --argc > 0 )	{
		if( **argv == '-' )
			DoOption((*argv++) + 1);
		else
			Nargv[Nargc++] = *argv++;
	}
	Nargv[Nargc] = 0;	/* terminate the arg vector	*/
	if( Nargc < 2 )	{
		fprint(STDERR, "%s: Use a file argument\n", ProgName);
		exit(1);
	}
	exit(!Compile(Nargv[1], Nargv[2]));
}

Compile(src, dst)
	char *src, *dst;
{
	extern struct tokenname tkidf[];
	extern struct tokenname tkstandard[];

	if( !InsertFile(src, (char **) 0, &src) )	{
		fprint(STDERR, "%s: cannot open %s\n", ProgName, src);
		return 0;
	}
	LineNumber = 1;
	FileName = src;
	init_idf();
	InitCst();
	reserve(tkidf);
	reserve(tkstandard);
	InitScope();
	InitTypes();
	AddRequired();
#ifdef DEBUG
	if( options['l'] )	{
		LexScan();
		return 1;
	}
#endif DEBUG
	C_init(word_size, pointer_size);
	if( !C_open(dst) )
		fatal("couldn't open output file");
	C_magic();
	C_ms_emx(word_size, pointer_size);
	C_df_dlb(++data_label);
	C_rom_scon(FileName, strlen(FileName) + 1);
	LLparse();
	C_ms_src((arith) (LineNumber - 1), FileName);
	if( fp_used ) C_ms_flt();
	C_close();
#ifdef DEBUG
	if( options['I'] ) Info();
#endif DEBUG
	return !err_occurred;
}

#ifdef DEBUG
LexScan()
{
	register struct token *tkp = &dot;
	extern char *symbol2str();

	while( LLlex() > 0 )	{
		print(">>> %s ", symbol2str(tkp->tk_symb));
		switch( tkp->tk_symb )	{
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
				print("'%s'\n", tkp->TOK_STR);
				break;

			default:
				print("\n");
		}
	}
}
#endif

AddRequired()
{
	register struct def *df;
	extern struct def *Enter();
	static struct node maxintnode = { 0, 0, Value, 0, { INTEGER, 0 } };

	/* PROCEDURES */

	/* File handling procedures, Read(ln) & Write(ln) are handled
	 * in the grammar
	 */

	(void) Enter("rewrite", D_PROCEDURE, std_type, R_REWRITE);
	(void) Enter("put", D_PROCEDURE, std_type, R_PUT);
	(void) Enter("reset", D_PROCEDURE, std_type, R_RESET);
	(void) Enter("get", D_PROCEDURE, std_type, R_GET);
	(void) Enter("page", D_PROCEDURE, std_type, R_PAGE);

	/* DYNAMIC ALLOCATION PROCEDURES */
	(void) Enter("new", D_PROCEDURE, std_type, R_NEW);
	(void) Enter("dispose", D_PROCEDURE, std_type, R_DISPOSE);

	/* TRANSFER PROCEDURES */
	(void) Enter("pack", D_PROCEDURE, std_type, R_PACK);
	(void) Enter("unpack", D_PROCEDURE, std_type, R_UNPACK);

	/* FUNCTIONS */

	/* ARITHMETIC FUNCTIONS */
	(void) Enter("abs", D_FUNCTION, std_type, R_ABS);
	(void) Enter("sqr", D_FUNCTION, std_type, R_SQR);
	(void) Enter("sin", D_FUNCTION, std_type, R_SIN);
	(void) Enter("cos", D_FUNCTION, std_type, R_COS);
	(void) Enter("exp", D_FUNCTION, std_type, R_EXP);
	(void) Enter("ln", D_FUNCTION, std_type, R_LN);
	(void) Enter("sqrt", D_FUNCTION, std_type, R_SQRT);
	(void) Enter("arctan", D_FUNCTION, std_type, R_ARCTAN);

	/* TRANSFER FUNCTIONS */
	(void) Enter("trunc", D_FUNCTION, std_type, R_TRUNC);
	(void) Enter("round", D_FUNCTION, std_type, R_ROUND);

	/* ORDINAL FUNCTIONS */
	(void) Enter("ord", D_FUNCTION, std_type, R_ORD);
	(void) Enter("chr", D_FUNCTION, std_type, R_CHR);
	(void) Enter("succ", D_FUNCTION, std_type, R_SUCC);
	(void) Enter("pred", D_FUNCTION, std_type, R_PRED);

	/* BOOLEAN FUNCTIONS */
	(void) Enter("odd", D_FUNCTION, std_type, R_ODD);
	(void) Enter("eof", D_FUNCTION, std_type, R_EOF);
	(void) Enter("eoln", D_FUNCTION, std_type, R_EOLN);

	/* TYPES */
	(void) Enter("char", D_TYPE, char_type, 0);
	(void) Enter("integer", D_TYPE, int_type, 0);
	(void) Enter("real", D_TYPE, real_type, 0);
	(void) Enter("boolean", D_TYPE, bool_type, 0);
	(void) Enter("text", D_TYPE, text_type, 0);

	/* DIRECTIVES */
	(void) Enter("forward", D_FORWARD, NULLTYPE, 0);
	(void) Enter("extern", D_EXTERN, NULLTYPE, 0);

	/* CONSTANTS */
	/* nil is TOKEN and thus part of the grammar */

	df = Enter("maxint", D_CONST, int_type, 0);
	df->con_const = &maxintnode;
	maxintnode.nd_type = int_type;
	maxintnode.nd_INT = max_int;		/* defined in cstoper.c */
	df = Enter("true", D_ENUM, bool_type, 0);
	df->enm_val = 1;
	df->enm_next = Enter("false", D_ENUM, bool_type, 0);
	df = df->enm_next;
	df->enm_val = 0;
	df->enm_next = NULLDEF;
}

#ifdef DEBUG
	int cntlines;

Info()
{
	extern int cnt_def, cnt_node, cnt_paramlist, cnt_type, cnt_scope,
			cnt_scopelist, cnt_tmpvar, cnt_withdesig,
			cnt_case_hdr, cnt_case_entry;

	print("\
%6d def\n%6d node\n%6d paramlist\n%6d type\n%6d scope\n%6d scopelist\n\
%6d lab\n%6d tmpvar\n%6d withdesig\n%6d casehdr\n%6d caseentry\n",
cnt_def, cnt_node, cnt_paramlist, cnt_type, cnt_scope, cnt_scopelist, cnt_lab, cnt_tmpvar, cnt_withdesig, cnt_case_hdr, cnt_case_entry);
print("\nNumber of lines read: %d\n", cntlines);
}
#endif
