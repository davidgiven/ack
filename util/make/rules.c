/*
 *  Control of the implicit suffix rules
 *
 *  $Header$
 */

#include <string.h>
#include <stdio.h>
#include "h.h"


/*
 *  Return a pointer to the suffix of a name
 */
char *suffix(char *name)
{
    return strrchr(name, '.');
}


/*
 *  Dynamic dependency.  This routine applies the suffis rules
 *  to try and find a source and a set of rules for a missing
 *  target.  If found, np is made into a target with the implicit
 *  source name, and rules.  Returns TRUE if np was made into
 *  a target.
 */
bool dyndep(struct name *np)
{
    char *     p;
    char *     q;
    char *     suff;       /*  Old suffix  */
    char *     basename;   /*  Name without suffix  */
    struct name *       op;     /*  New dependent  */
    struct name *       sp;     /*  Suffix  */
    struct line *       lp;
    struct depend *     dp;
    char *          newsuff;


    p = str1;
    q = np->n_name;
    if (!(suff = suffix(q)))
        return FALSE;       /* No suffix */
    while (q < suff)
        *p++ = *q++;
    *p = '\0';
    basename = setmacro("*", str1, 4)->m_val;

    if (!((sp = newname(".SUFFIXES"))->n_flag & N_TARG))
        return FALSE;

    for (lp = sp->n_line; lp; lp = lp->l_next)
        for (dp = lp->l_dep; dp; dp = dp->d_next)
        {
            newsuff = dp->d_name->n_name;
            if (strlen(suff)+strlen(newsuff)+1 >= LZ)
                fatal("Suffix rule too long",NULL);
            p = str1;
            q = newsuff;
            while ((*p++ = *q++))
                ;
            p--;
            q = suff;
            while ((*p++ = *q++))
                ;
            sp = newname(str1);
            if (sp->n_flag & N_TARG)
            {
                p = str1;
                q = basename;
                if (strlen(basename) + strlen(newsuff)+1 >= LZ)
                    fatal("Implicit name too long",NULL);
                while ((*p++ = *q++))
                    ;
                p--;
                q = newsuff;
                while ((*p++ = *q++))
                    ;
                op = newname(str1);
                if (!op->n_time)
                    modtime(op);
                if (op->n_time || (op->n_flag & N_TARG))
                {
                    dp = newdep(op, (struct depend *)0);
                    newline(np, dp, sp->n_line->l_cmd, 0);
                    setmacro("<", op->n_name, 4);
                    return TRUE;
                }
            }
        }
    return FALSE;
}


/*
 *  Make the default rules
 */
void makerules(void)
{
    struct cmd *        cp;
    struct name *       np;
    struct depend *     dp;


#ifdef eon
    setmacro("BDSCC", "asm", 0);
    /*  setmacro("BDSCFLAGS", "", 0);   */
    cp = newcmd("$(BDSCC) $(BDSCFLAGS) -n $<", (struct cmd *)0);
    np = newname(".c.o");
    newline(np, (struct depend *)0, cp, 0);

    setmacro("CC", "c", 0);
    setmacro("CFLAGS", "-O", 0);
    cp = newcmd("$(CC) $(CFLAGS) -c $<", (struct cmd *)0);
    np = newname(".c.obj");
    newline(np, (struct depend *)0, cp, 0);

    setmacro("M80", "asm -n", 0);
    /*  setmacro("M80FLAGS", "", 0);    */
    cp = newcmd("$(M80) $(M80FLAGS) $<", (struct cmd *)0);
    np = newname(".mac.o");
    newline(np, (struct depend *)0, cp, 0);

    setmacro("AS", "zas", 0);
    /*  setmacro("ASFLAGS", "", 0); */
    cp = newcmd("$(ZAS) $(ASFLAGS) -o $@ $<", (struct cmd *)0);
    np = newname(".as.obj");
    newline(np, (struct depend *)0, cp, 0);

    np = newname(".as");
    dp = newdep(np, (struct depend *)0);
    np = newname(".obj");
    dp = newdep(np, dp);
    np = newname(".c");
    dp = newdep(np, dp);
    np = newname(".o");
    dp = newdep(np, dp);
    np = newname(".mac");
    dp = newdep(np, dp);
    np = newname(".SUFFIXES");
    newline(np, dp, (struct cmd *)0, 0);
#endif

/*
 *  Some of the UNIX implicit rules
 */
#ifdef unix
    setmacro("CC", "cc", 0);
    setmacro("CFLAGS", "-O", 0);
#ifdef MINIX
    cp = newcmd("$(CC) $(CFLAGS) -S $<", (struct cmd *)0);
    np = newname(".c.s");
#else
    cp = newcmd("$(CC) $(CFLAGS) -c $<", (struct cmd *)0);
    np = newname(".c.o");
#endif /* MINIX */
    newline(np, (struct depend *)0, cp, 0);

    setmacro("AS", "as", 0);
    cp = newcmd("$(AS) -o $@ $<", (struct cmd *)0);
    np = newname(".s.o");
    newline(np, (struct depend *)0, cp, 0);

    setmacro("YACC", "yacc", 0);
    /*  setmacro("YFLAGS", "", 0);  */
    cp = newcmd("$(YACC) $(YFLAGS) $<", (struct cmd *)0);
    cp = newcmd("mv y.tab.c $@", cp);
    np = newname(".y.c");
    newline(np, (struct depend *)0, cp, 0);

    cp = newcmd("$(YACC) $(YFLAGS) $<", (struct cmd *)0);
    cp = newcmd("$(CC) $(CFLAGS) -c y.tab.c", cp);
    cp = newcmd("rm y.tab.c", cp);
    cp = newcmd("mv y.tab.o $@", cp);
    np = newname(".y.o");
    newline(np, (struct depend *)0, cp, 0);

    setmacro("LEX", "lex", 0);
    /*  setmacro("LFLAGS", "", 0);  */
    cp = newcmd("$(LEX) $(LFLAGS) $<", (struct cmd *)0);
    cp = newcmd("mv lex.yy.c $@", cp);
    np = newname(".l.c");
    newline(np, (struct depend *)0, cp, 0);

    cp = newcmd("$(LEX) $(LFLAGS) $<", (struct cmd *)0);
    cp = newcmd("$(CC) $(CFLAGS) -c lex.yy.c", cp);
    cp = newcmd("rm lex.yy.c", cp);
    cp = newcmd("mv lex.yy.o $@", cp);
    np = newname(".l.o");
    newline(np, (struct depend *)0, cp, 0);

    np = newname(".s");
    dp = newdep(np, (struct depend *)0);
    np = newname(".o");
    dp = newdep(np, dp);
    np = newname(".c");
    dp = newdep(np, dp);
    np = newname(".y");
    dp = newdep(np, dp);
    np = newname(".l");
    dp = newdep(np, dp);
    np = newname(".SUFFIXES");
    newline(np, dp, (struct cmd *)0, 0);
#endif
#ifdef os9
/*
 *  Fairlight use an enhanced version of the C sub-system.
 *  They have a specialised macro pre-processor.
 */
    setmacro("CC", "cc", 0);
    setmacro("CFLAGS", "-z", 0);
    cp = newcmd("$(CC) $(CFLAGS) -r $<", (struct cmd *)0);

    np = newname(".c.r");
    newline(np, (struct depend *)0, cp, 0);
    np = newname(".ca.r");
    newline(np, (struct depend *)0, cp, 0);
    np = newname(".a.r");
    newline(np, (struct depend *)0, cp, 0);
    np = newname(".o.r");
    newline(np, (struct depend *)0, cp, 0);
    np = newname(".mc.r");
    newline(np, (struct depend *)0, cp, 0);
    np = newname(".mca.r");
    newline(np, (struct depend *)0, cp, 0);
    np = newname(".ma.r");
    newline(np, (struct depend *)0, cp, 0);
    np = newname(".mo.r");
    newline(np, (struct depend *)0, cp, 0);

    np = newname(".r");
    dp = newdep(np, (struct depend *)0);
    np = newname(".mc");
    dp = newdep(np, dp);
    np = newname(".mca");
    dp = newdep(np, dp);
    np = newname(".c");
    dp = newdep(np, dp);
    np = newname(".ca");
    dp = newdep(np, dp);
    np = newname(".ma");
    dp = newdep(np, dp);
    np = newname(".mo");
    dp = newdep(np, dp);
    np = newname(".o");
    dp = newdep(np, dp);
    np = newname(".a");
    dp = newdep(np, dp);
    np = newname(".SUFFIXES");
    newline(np, dp, (struct cmd *)0, 0);
#endif
}
