/* M A I N   P R O G R A M */

#include    <stdlib.h>
#include    <string.h>
#include    "parameters.h"
#include	"debug.h"

#include	<em.h>
#include	<em_mes.h>
#include	<system.h>
#include	<stb.h>

#include	"LLlex.h"
#include	"Lpars.h"
#include	"class.h"
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
#include	"scope.h"

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
		sys_stop(S_EXIT);
	}
	if(!Compile(Nargv[1], Nargv[2])) sys_stop(S_EXIT);
	sys_stop(S_END);
}

Compile(src, dst)
	char *src, *dst;
{
	extern struct tokenname tkidf[];
	extern struct tokenname tkstandard[];
	int tk;

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

	CheckForLineDirective();
	tk = LLlex();			/* Read the first token and put */
	aside = dot;			/* it aside. In this way, options */
	asidetype = toktype;		/* inside comments will be seen */
	dot.tk_symb = tk;		/* before the program starts. */
	tokenseen = 1;

	InitScope();
	InitTypes();

	if( options['c'] ) tkclass['"'] = STSTR;
	if( options['u'] || options['U'] ) {
		class('_') = STIDF;
		inidf['_'] = 1;
	}
	if( tk == '"' || tk == '_' ) {
		PushBack();
		ASIDE = 0;
	}

#ifdef DEBUG
	if( options['l'] )	{
		LexScan();
		return 0;	/* running the optimizer is not very useful */
	}
#endif /* DEBUG */
	C_init(word_size, pointer_size);
	if( !C_open(dst) )
		fatal("couldn't open output file");
	C_magic();
	C_ms_emx(word_size, pointer_size);
#ifdef DBSYMTAB
	if (options['g']) {
		C_ms_std(FileName, N_SO, 0);
	}
#endif /* DBSYMTAB */
	AddRequired();
	C_df_dlb(++data_label);
	C_rom_scon(FileName,(arith) strlen(FileName) + 1);
	LLparse();
	C_ms_src((int)LineNumber - 1, FileName);
	if( fp_used ) C_ms_flt();
	C_close();
#ifdef DEBUG
	if( options['I'] ) Info();
#endif /* DEBUG */
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

	df = Enter("false", D_ENUM, bool_type, 0);
	df->enm_val = 0;
	df->df_flags |= D_SET;
	bool_type->enm_enums = df;
	df->enm_next = Enter("true", D_ENUM, bool_type, 0);
	df->enm_next->enm_val = 1;
	df->df_flags |= D_SET;
	df->enm_next->enm_next = NULLDEF;

	(void) Enter("rewrite", D_PROCEDURE, std_type, R_REWRITE);
	(void) Enter("put", D_PROCEDURE, std_type, R_PUT);
	(void) Enter("reset", D_PROCEDURE, std_type, R_RESET);
	(void) Enter("get", D_PROCEDURE, std_type, R_GET);
	(void) Enter("page", D_PROCEDURE, std_type, R_PAGE);

	/* DYNAMIC ALLOCATION PROCEDURES */
	(void) Enter("new", D_PROCEDURE, std_type, R_NEW);
	(void) Enter("dispose", D_PROCEDURE, std_type, R_DISPOSE);
	if( !options['s'] ) {
		(void) Enter("mark", D_PROCEDURE, std_type, R_MARK);
		(void) Enter("release", D_PROCEDURE, std_type, R_RELEASE);
	}

	/* MISCELLANEOUS PROCEDURE(S) */
	if( !options['s'] )
		(void) Enter("halt", D_PROCEDURE, std_type, R_HALT);

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
	(void) Enter("(void)", D_TYPE, void_type, 0);

	if( options['d'] )
		(void) Enter("long", D_TYPE, long_type, 0);
	if( options['c'] )
		(void) Enter("string", D_TYPE, string_type, 0);

	/* DIRECTIVES */
	(void) Enter("forward", D_FORWARD, error_type, 0);
	(void) Enter("extern", D_EXTERN, error_type, 0);

	/* CONSTANTS */
	/* nil is TOKEN and thus part of the grammar */

	maxintnode.nd_type = int_type;
	maxintnode.nd_INT = max_int;		/* defined in cstoper.c */
	df = define(str2idf("maxint", 0), CurrentScope, D_CONST);
	df->df_type = int_type;
	df->con_const = &maxintnode;
	df->df_flags |= D_SET;
#ifdef DBSYMTAB
	if (options['g']) stb_string(df, D_CONST);
#endif /* DBSYMTAB */
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
