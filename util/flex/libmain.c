/* libmain - flex run-time support library "main" function */

/* $Id$ */

extern int yylex();

int main( argc, argv )
int argc;
char *argv[];

    {
    return yylex();
    }
