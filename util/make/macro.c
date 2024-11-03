/*
 *  Macro control for make
 *
 *  $Header$
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "h.h"


struct macro *      macrohead;


static struct macro *getmp(char *name)
{
    struct macro * rp;

    for (rp = macrohead; rp; rp = rp->m_next)
        if (strcmp(name, rp->m_name) == 0)
            return rp;
    return (struct macro *)0;
}


char * getmacro(char* name)
{
    struct macro *      mp;

    if ((mp = getmp(name)))
        return mp->m_val;
    else
        return "";
}


struct macro * setmacro(char* name, char* val, int prio)
{
    struct macro * rp;
    char *     cp;


            /*  Replace macro definition if it exists  */
    for (rp = macrohead; rp; rp = rp->m_next)
        if (strcmp(name, rp->m_name) == 0)
        {
            if (prio < rp->m_prio)
                return rp;
            free(rp->m_val);    /*  Free space from old  */
            break;
        }

    if (!rp)        /*  If not defined, allocate space for new  */
    {
        if ((rp = (struct macro *)malloc(sizeof (struct macro)))
                     == (struct macro *)0)
            fatal("No memory for macro",NULL);

        rp->m_next = macrohead;
        macrohead = rp;
        rp->m_flag = FALSE;

        if ((cp = malloc((unsigned)(strlen(name)+1))) == (char *)0)
            fatal("No memory for macro",NULL);
        strcpy(cp, name);
        rp->m_name = cp;
    }

    if ((cp = malloc((unsigned)(strlen(val)+1))) == (char *)0)
        fatal("No memory for macro",NULL);
    strcpy(cp, val);        /*  Copy in new value  */
    rp->m_val = cp;
    rp->m_prio = (uchar)prio;

    return rp;
}

#define MBUFSIZ 128

/*
 *  Do the dirty work for expand
 */
static void doexp(char **to, char* from, int* len, char* buf)
{
    char *     rp;
    char *     p;
    char *     q;
    struct macro * mp;


    rp = from;
    p = *to;
    while (*rp)
    {
        if (*rp != '$')
        {
            *p++ = *rp++;
            (*len)--;
        }
        else
        {
            q = buf;
            if (*++rp == '{')
                while (*++rp && *rp != '}') {
                    if (q < &buf[MBUFSIZ-1]) *q++ = *rp;
                }
            else if (*rp == '(')
                while (*++rp && *rp != ')') {
                    if (q < &buf[MBUFSIZ-1]) *q++ = *rp;
                }
            else if (!*rp)
            {
                *p++ = '$';
                break;
            }
            else
                *q++ = *rp;
            *q = '\0';
            if (*rp)
                rp++;
            if (!(mp = getmp(buf)))
                mp = setmacro(buf, "", 2);
            if (mp->m_flag)
                fatal("Infinitely recursive macro %s", mp->m_name);
            mp->m_flag = TRUE;
            *to = p;
            doexp(to, mp->m_val, len, buf);
            p = *to;
            mp->m_flag = FALSE;
        }
        if (*len <= 0)
            error("Expanded line too line", NULL);
    }
    *p = '\0';
    *to = p;
}


/*
 *  Expand any macros in str.
 */
void expand(char *str)
{
    char            *a;
    static char     b[MBUFSIZ]; /* temp storage for macroname */
    char *          p = str;
    int         len = LZ-1;

    a = malloc((unsigned)(strlen(str)+1));
    if (!a) fatal("No memory for expand",NULL);
    strcpy(a, str);
    doexp(&p, a, &len, b);
    free(a);
}
