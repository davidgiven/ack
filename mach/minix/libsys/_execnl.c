/*
 * This file contains two functions that can be used to perform
 * an EXEC call without the need for a "big" stack of MAX_ARG
 * bytes.  It is primarily used by the INIT module of the system.
 */
#include <lib.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXSTK	256		/* maximum EXEC stack size */
#define	PTRSIZE	sizeof(char *)

_PROTOTYPE(int _execn,(char *name));
_PROTOTYPE(int _execnl,(char *name, char *arg0));
PRIVATE _PROTOTYPE(int _nexec,(char *name, char *argv[]));

PUBLIC int _execn(name)
char *name;			/* pointer to file to be exec'd */
{
/* This funcion uses no arguments at all. */
  PRIVATE char stack[3 * PTRSIZE];

  return(_callm1(MM, EXEC, _len(name), sizeof(stack), 0, name, stack, NIL_PTR));
}


PUBLIC int _execnl(name, arg0)
char *name;
char *arg0;
{
  /* This function resembles execl(2). */

  return(_nexec(name, &arg0));
}

PRIVATE int _nexec(name, argv)
char *name;			/* pointer to name of file to be executed */
char *argv[];			/* pointer to argument array */
{
  char stack[MAXSTK];
  char **argorg, *hp, **ap, *p;
  int i, nargs, stackbytes, offset;

  /* Count the argument pointers. */
  nargs = 0;
  argorg = argv;
  while (*argorg++ != NIL_PTR) nargs++;

  /* Prepare to set up the initial stack. */
  hp = &stack[(nargs + 3) * PTRSIZE];
  if (hp + nargs >= &stack[MAXSTK]) {
	errno = E2BIG;
	return(-1);
  }
  ap = (char **) stack;
  *ap++ = (char *) nargs;

  /* Prepare the argument pointers and strings. */
  for (i = 0; i < nargs; i++) {
	offset = hp - stack;
	*ap++ = (char *) offset;
	p = *argv++;
	while (*p) {
		*hp++ = *p++;
		if (hp >= &stack[MAXSTK]) {
			errno = E2BIG;
			return(-1);
		}
	}
	*hp++ = (char) 0;
  }
  *ap++ = NIL_PTR;

  stackbytes = (((int) (hp - stack) + PTRSIZE - 1) / PTRSIZE) * PTRSIZE;
  return(_callm1(MM, EXEC, _len(name), stackbytes, 0, name, stack, NIL_PTR));
}
