/* mod2 -- compiler , althans: een aanzet daartoe */

#include <stdio.h>
#undef BUFSIZ			/* Really neccesary??? */
#include <system.h>
#include "input.h"
#include "f_info.h"
#include "idf.h"
#include "LLlex.h"
#include "Lpars.h"

static char *RcsId = "$Header$";

char options[128];
char *ProgName;
extern int err_occurred;

main(argc, argv)
	char *argv[];
{
	register Nargc = 1;
	register char **Nargv = &argv[0];

	ProgName = *argv++;

# ifdef DEBUG
	setbuf(stdout, (char *) 0);
# endif
	while (--argc > 0) {
		if (**argv == '-')
			Option(*argv++);
		else
			Nargv[Nargc++] = *argv++;
	}
	Nargv[Nargc] = 0;	/* terminate the arg vector	*/
	if (Nargc != 2) {
		fprintf(stderr, "%s: Use one file argument\n", ProgName);
		return 1;
	}
#ifdef DEBUG
	printf("Mod2 compiler -- Debug version\n");
	debug("-D: Debugging on");
#endif DEBUG
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
		fprintf(stderr,"%s: cannot open %s\n", ProgName, src);
		return 0;
	}
	LineNumber = 1;
	FileName = src;
	init_idf();
	reserve(tkidf);
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

	while ((symb = LLlex()) != EOF) {
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
	while (LLlex() != EOF) /* nothing */;
}
#endif

Option(str)
	char *str;
{
#ifdef DEBUG
	debug("option %c", str[1]);
#endif DEBUG
	options[str[1]]++;	/* switch option on	*/
}
