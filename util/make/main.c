/*
 *  make [-f makefile] [-ins] [target(s) ...]
 *
 *  (Better than EON mk but not quite as good as UNIX make)
 *
 *  -f makefile name
 *  -i ignore exit status
 *  -n Pretend to make
 *  -p Print all macros & targets
 *  -q Question up-to-dateness of target.  Return exit status 1 if not
 *  -r Don't not use inbuilt rules
 *  -s Make silently
 *  -t Touch files instead of making them
 *  -m Change memory requirements (EON only)
 *  -k For the time being: accept but ignore
 *
 *  $Header$
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "h.h"

char *          myname;
char *          makefile;   /*  The make file  */
#ifdef eon
unsigned        memspace = MEMSPACE;
#endif

FILE *          ifd;        /*  Input file desciptor  */
bool            domake = TRUE;  /*  Go through the motions option  */
bool            ignore = FALSE; /*  Ignore exit status option  */
bool            silent = FALSE; /*  Silent option  */
bool            print = FALSE;  /*  Print debuging information  */
bool            rules = TRUE;   /*  Use inbuilt rules  */
bool            dotouch = FALSE;/*  Touch files instead of making  */
bool            quest = FALSE;  /*  Question up-to-dateness of file  */

static void usage(void);


int main(int argc, char** argv)
{
    char *     p;      /*  For argument processing  */
    int         estat = 0;  /*  For question  */
    struct name *  np;
    int         nargc = 0;
    char            **nargv;
    int         fflag = 0;


    myname = (argc-- < 1) ? "make" : *argv++;
    nargv = argv;

    while (argc > 0)
    {
        argc--;     /*  One less to process  */
        p = *argv++;    /*  Now processing this one  */

        if (*p == '-') while (*++p != '\0')
        {
            switch(*p)
            {
            case 'f':   /*  Alternate file name  */
                fflag = 1;
                break;
            case 'n':   /*  Pretend mode  */
                domake = FALSE;
                break;
            case 'i':   /*  Ignore fault mode  */
                ignore = TRUE;
                break;
            case 's':   /*  Silent about commands  */
                silent = TRUE;
                break;
            case 'p':
                print = TRUE;
                break;
            case 'r':
                rules = FALSE;
                break;
            case 't':
                dotouch = TRUE;
                break;
            case 'q':
                quest = TRUE;
                break;
            case 'k':
                break;
            default:    /*  Wrong option  */
                usage();
            }
        }
        else {
            if (fflag) {
                if (argc <= 0) usage();
                makefile = p;
                fflag = 0;
            }
            else {
                nargc++;
                *nargv++ = p;
            }
        }
    end_of_args:;
    }
    argv = nargv - nargc;
    argc = nargc;


    if (makefile && strcmp(makefile, "-") == 0) /*  Can use stdin as makefile  */
        ifd = stdin;
    else
        if (!makefile)      /*  If no file, then use default */
        {
            if ((ifd = fopen(DEFN1, "r")) == NULL)
            if ((ifd == NULL)
                  && ((ifd = fopen(DEFN2, "r")) == NULL))
                fatal("Can't open %s", DEFN2);
        }
        else
            if ((ifd = fopen(makefile, "r")) == NULL)
                fatal("Can't open %s", makefile);

    makerules();

    setmacro("$", "$", 4);

    while (argc && (p = strchr(*argv, '=')))
    {
        char        c;

        c = *p;
        *p = '\0';
        setmacro(*argv, p+1, 3);
        *p = c;

        argv++;
        argc--;
    }

    input(ifd); /*  Input all the gunga  */
    fclose(ifd);    /*  Finished with makefile  */
    lineno = 0; /*  Any calls to error now print no line number */

    if (print)
        prt();  /*  Print out structures  */

    np = newname(".SILENT");
    if (np->n_flag & N_TARG)
        silent = TRUE;

    np = newname(".IGNORE");
    if (np->n_flag & N_TARG)
        ignore = TRUE;

    precious();

    if (!firstname)
        fatal("No targets defined",NULL);

    circh();    /*  Check circles in target definitions  */

    if (!argc)
        estat = make(firstname, 0);
    else while (argc--)
    {
        if (!print && !silent && strcmp(*argv, "love") == 0)
            printf("Not war!\n");
        estat |= make(newname(*argv++), 0);
    }

    if (quest)
        exit(estat);
    else
        exit(0);
    return 0;
}


static void usage(void)
{
    fprintf(stderr, "Usage: %s [-f makefile] [-inpqrst] [macro=val ...] [target(s) ...]\n", myname);
    exit(1);
}


/*VARARGS1*/
void fatal(char *msg, char* value)
{
    if (value != NULL)
    {
        fprintf(stderr, "%s: ", myname);
        fprintf(stderr, msg, value);
    } else
    {
        fprintf(stderr, "%s: ", myname);
        fprintf(stderr, "%s",msg);
    }
    fputc('\n', stderr);
    exit(1);
}

