/* STARTUP PROCEDURE FOR UNIX FORTRAN PROGRAMS */

#include "stdio.h"
#include "signal.h"

#ifndef SIGIOT
#define SIGIOT SIGABRT
#endif

#ifdef NO__STDC
#define ONEXIT onexit
extern void f_exit();
#else
#ifdef __STDC__
#include "stdlib.h"
extern void f_exit(void);
#ifndef NO_ONEXIT
#define ONEXIT atexit
extern int atexit(void (*)(void));
#endif
#else
#ifndef NO_ONEXIT
#define ONEXIT onexit
extern void f_exit();
#endif
#endif
#endif

extern void sig_die();

static void sigfdie(n)
{
sig_die("Floating Exception", 1);
}


static void sigidie(n)
{
sig_die("IOT Trap", 1);
}

#ifdef SIGQUIT
static void sigqdie(n)
{
sig_die("Quit signal", 1);
}
#endif


static void sigindie(n)
{
sig_die("Interrupt", 0);
}



static void sigtdie(n)
{
sig_die("Killed", 0);
}


int xargc;
char **xargv;

main(argc, argv)
int argc;
char **argv;
{
xargc = argc;
xargv = argv;
signal(SIGFPE, sigfdie);	/* ignore underflow, enable overflow */
signal(SIGIOT, sigidie);
#ifdef SIGQUIT
if(signal(SIGQUIT,sigqdie) == SIG_IGN)
	signal(SIGQUIT, SIG_IGN);
#endif
if(signal(SIGINT, sigindie) == SIG_IGN)
	signal(SIGINT, SIG_IGN);
signal(SIGTERM,sigtdie);

#ifdef pdp11
	ldfps(01200); /* detect overflow as an exception */
#endif

f_init();
#ifndef NO_ONEXIT
ONEXIT(f_exit);
#endif
MAIN__();
#ifdef NO_ONEXIT
f_exit();
#endif
exit(0);	/* exit(0) rather than return(0) to bypass Cray bug */
}
