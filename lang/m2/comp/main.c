/* M A I N   P R O G R A M */

static char *RcsId = "$Header$";

#include	<system.h>
#include	<em_arith.h>
#include	"input.h"
#include	"f_info.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"main.h"
#include	"debug.h"

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

#ifdef DEBUG
	printf("%s\n", src);
#endif DEBUG
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
