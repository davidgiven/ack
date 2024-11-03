/*
 *  Do the actual making for make
 *
 *  $Header$
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
/* UNIX specific */
#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#ifndef unix
#define unix
#endif
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
#ifndef unix
#define unix
#endif
#endif



#ifdef unix
#include <unistd.h>
#include <utime.h>
#endif
#include "h.h"

void docmds(struct name *np);

#ifndef max
#define max(a,b)    ((a)>(b)?(a):(b))
#endif


/*
 *  Exec a shell that returns exit status correctly (/bin/esh).
 *  The standard EON shell returns the process number of the last
 *  async command, used by the debugger (ugg).
 *  [exec on eon is like a fork+exec on unix]
 */
static int dosh(char *string, char *shell)
{
    return system(string);
}


/*
 *  Do commands to make a target
 */
static void docmds1(struct name *np, struct line *lp)
{
    bool            ssilent;
    bool            signore;
    int         estat;
    char *     q;
    char *     p;
    char *          shell;
    struct cmd *   cp;


    if (*(shell = getmacro("SHELL")) == '\0')
#ifdef unix
        shell = "/bin/sh";
#endif
    for (cp = lp->l_cmd; cp; cp = cp->c_next)
    {
        strcpy(str1, cp->c_cmd);
        expand(str1);
        q = str1;
        ssilent = silent;
        signore = ignore;
        while ((*q == '@') || (*q == '-'))
        {
            if (*q == '@')     /*  Specific silent  */
                ssilent = TRUE;
            else           /*  Specific ignore  */
                signore = TRUE;
            q++;           /*  Not part of the command  */
        }

        if (!domake)
            ssilent = 0;

        if (!ssilent)
            fputs("    ", stdout);

        for (p=q; *p; p++)
        {
            if (*p == '\n' && p[1] != '\0')
            {
                *p = ' ';
                if (!ssilent)
                    fputs("\\\n", stdout);
            }
            else if (!ssilent)
                putchar(*p);
        }
        if (!ssilent) {
            putchar('\n');
            fflush(stdout);
        }

        if (domake)
        {           /*  Get the shell to execute it  */
            if ((estat = dosh(q, shell)) != 0)
            {
                if (estat == -1)
                    fatal("Couldn't execute %s", shell);
                else
                {
                    printf("%s: Error code %d", myname, estat);
                    if (signore)
                        fputs(" (Ignored)\n", stdout);
                    else
                    {
                        putchar('\n');
                        if (!(np->n_flag & N_PREC))
                            if (remove(np->n_name) == 0)
                                printf("%s: '%s' removed.\n", myname, np->n_name);
                        exit(1);
                    }
                }
                fflush(stdout);
            }
        }
    }
}


void docmds(struct name *np)
{
    struct line *  lp;


    for (lp = np->n_line; lp; lp = lp->l_next)
        docmds1(np, lp);
}


/*
 *  Get the modification time of a file.  If the first
 *  doesn't exist, it's modtime is set to 0.
 */
void modtime(struct name *np)
{
    struct stat     info;

    if (stat(np->n_name, &info) < 0)
    {
        if (errno != ENOENT)
	    fatal("Can't open %s", np->n_name);

        np->n_time = 0L;
    }
    else
        np->n_time = info.st_mtime;
}


/*
 *  Update the mod time of a file to now.
 */
void touch(struct name *np)
{
#ifdef unix
    if (!domake || !silent)
        printf("    touch(%s)\n", np->n_name);

    if (domake)
    {
        struct utimbuf a;
        time_t timeval;

        a.actime = a.modtime = time(&timeval);
        if (utime(np->n_name, &a) < 0)
            printf("%s: '%s' not touched - non-existant\n",
                    myname, np->n_name);
    }
#endif
}

static void make1(struct name *np, struct line *lp, struct depend *qdp)
{
    struct depend *    dp;
    char *p;

    if (dotouch)
        touch(np);
    else
    {
        strcpy(str1, "");
        for (dp = qdp; dp; dp = qdp)
        {
            if (strlen(str1))
                strcat(str1, " ");
            strcat(str1, dp->d_name->n_name);
            qdp = dp->d_next;
            free((char *)dp);
        }
        setmacro("?", str1, 4);
        setmacro("@", np->n_name, 4);
        p = strrchr(np->n_name, '.');
        if (p) *p = 0;
        setmacro("*", np->n_name, 4);
        if (p) *p = '.';
        if (lp)     /* lp set if doing a :: rule */
            docmds1(np, lp);
        else
            docmds(np);
    }
}


/*
 *  Recursive routine to make a target.
 */
int make(struct name *np, int level)
{
    struct depend *    dp;
    struct line *      lp;
    struct depend *    qdp;
    time_t              dtime = 1;
    bool                didsomething = 0;
    int             dynamic = 0;


    if (np->n_flag & N_DONE)
        return 0;

    if (!np->n_time)
        modtime(np);        /*  Gets modtime of this file  */

    if (rules)
    {
        for (lp = np->n_line; lp; lp = lp->l_next)
            if (lp->l_cmd)
                break;
        if (!lp) {
            dyndep(np);
            dynamic = 1;
        }
    }

    if (!(np->n_flag & N_TARG) && np->n_time == 0L)
        fatal("Don't know how to make %s", np->n_name);

    for (qdp = (struct depend *)0, lp = np->n_line; lp; lp = lp->l_next)
    {
        for (dp = lp->l_dep; dp; dp = dp->d_next)
        {
            char *sv = 0;
            if (dynamic) {
                char *s = getmacro("<");

                if (s) {
                    sv = malloc((unsigned)(strlen(s)+1));
                    if (!sv) {
                        fatal("no space for saved $<",NULL);
                    }
                    strcpy(sv, s);
                }
            }
            make(dp->d_name, level+1);
            if (dynamic && sv) {
                setmacro("<", sv, 4);
                free(sv);
            }
            if (np->n_time < dp->d_name->n_time)
                qdp = newdep(dp->d_name, qdp);
            dtime = max(dtime, dp->d_name->n_time);
        }
        if (!quest && (np->n_flag & N_DOUBLE) && (np->n_time < dtime))
        {
            make1(np, lp, qdp); /* free()'s qdp */
            dtime = 1;
            qdp = (struct depend *)0;
            didsomething++;
        }
    }

    np->n_flag |= N_DONE;

    if (quest)
    {
        long        t;

        t = np->n_time;
        time(&np->n_time);
        return t < dtime;
    }
    else if (np->n_time < dtime && !(np->n_flag & N_DOUBLE))
    {
        make1(np, (struct line *)0, qdp);   /* free()'s qdp */
        time(&np->n_time);
    }
    else if (level == 0 && !didsomething)
        printf("%s: '%s' is up to date\n", myname, np->n_name);
    return 0;
}


