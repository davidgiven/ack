#include <stdio.h>

extern int err, yylineno;
extern char *curr_file;

report(fmt, arg1, arg2, arg3) char *fmt;
{
	fprintf(stderr, "%s (%d) F: ", curr_file, yylineno);
	fprintf(stderr, fmt, arg1, arg2, arg3);
	putc('\n', stderr);
	err=1;
}

warning(fmt, arg1, arg2, arg3) char *fmt, *arg1;
{
	fprintf(stderr, "%s (%d) E: ", curr_file, yylineno);
	fprintf(stderr, fmt, arg1, arg2, arg3);
	putc('\n', stderr);
}
