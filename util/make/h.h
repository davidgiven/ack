/*
 *  Include header for make
 *
 *  $Header$
 */


#ifndef uchar
#define uchar       unsigned char
#endif

#include <time.h>

#define bool        int
#define TRUE        (1)
#define FALSE       (0)

#define DEFN1       "makefile"      /*  Default names  */
#define DEFN2       "Makefile"

#define LZ      (2048)          /*  Line size  */



/*
 *  A name.  This represents a file, either to be made, or existant
 */

struct name
{
    struct name *       n_next;     /* Next in the list of names */
    char *          n_name;     /* Called */
    struct line *       n_line;     /* Dependencies */
    time_t          n_time;     /* Modify time of this name */
    uchar           n_flag;     /* Info about the name */
};

#define N_MARK      0x01            /* For cycle check */
#define N_DONE      0x02            /* Name looked at */
#define N_TARG      0x04            /* Name is a target */
#define N_PREC      0x08            /* Target is precious */
#define N_DOUBLE    0x10            /* Double colon target */

/*
 *  Definition of a target line.
 */
struct  line
{
    struct line *       l_next;     /* Next line (for ::) */
    struct depend *     l_dep;      /* Dependents for this line */
    struct cmd *        l_cmd;      /* Commands for this line */
};


/*
 *  List of dependents for a line
 */
struct  depend
{
    struct depend *     d_next;     /* Next dependent */
    struct name *       d_name;     /* Name of dependent */
};


/*
 *  Commands for a line
 */
struct  cmd
{
    struct cmd *        c_next;     /* Next command line */
    char *          c_cmd;      /* Command line */
};


/*
 *  Macro storage
 */
struct  macro
{
    struct macro *  m_next;     /* Next variable */
    char *      m_name;     /* Called ... */
    char *      m_val;      /* Its value */
    uchar       m_flag;     /* Infinite loop check */
    uchar       m_prio;     /* 5 levels:
                       - 0 for internal ($(CC), etc)
                       - 1 (reserved for environment)
                       - 2 for makefile
                       - 3 for command line
                       - 4 for special ($*,$<, etc)
                    */
};

extern char *       myname;
extern struct name  namehead;
extern struct macro *   macrohead;
extern struct name *    firstname;
extern bool     silent;
extern bool     ignore;
extern bool     rules;
extern bool     dotouch;
extern bool     quest;
extern bool     domake;
extern char     str1[];
extern char     str2[];
extern int      lineno;

void            circh(void);
char * getmacro(char* name);
struct macro * setmacro(char* name, char* val, int prio);
void input(FILE *fd);
void error(char *msg, char* a1);
void expand(char *str);
void fatal(char* msg, char* value);
bool dyndep(struct name *np);
int make(struct name *np, int level);
void modtime(struct name *np);
struct name *newname(char *name);
struct depend *newdep(struct name *np, struct depend *dp);
struct cmd *newcmd(char *str, struct cmd *cp);
void newline(struct name *np, struct depend *dp, struct cmd *cp, int flag);
void            prt(void);
char *suffix(char *name);
void touch(struct name *np);
void makerules(void);
char *gettok(char **ptr);
void            precious(void);
bool mgetline(char* str, FILE* fd);
