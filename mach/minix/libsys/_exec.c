#include <lib.h>
#define execl	_execl
#define execle	_execle
#define execv	_execv
#define execve	_execve

extern char **environ;		/* environment pointer */

#define	PTRSIZE	(sizeof(char *))
_PROTOTYPE( char *_sbrk, (int _incr)					);

#if _ANSI
#include	<stdarg.h>

PUBLIC int execl(char *name, ...)
{
	int retval;
	va_list ap;

	va_start(ap, name);
	retval = execve(name, (char **)ap, environ);
	va_end(ap);
	return retval;
}
#else
PUBLIC int execl(name, arg0)
char *name;
char *arg0;
{
  return(execve(name, &arg0, environ));
}
#endif

#if _ANSI
PUBLIC int execle(char *name, ...)
{
	int retval;
	va_list ap;
	char *p;

	va_start(ap, name);
	do {
	    p = va_arg(ap, char *);
	} while(p);
	p = (char *)va_arg(ap, char **);
	va_end(ap);
	va_start(ap, name);
	retval = execve(name, (char **)ap, (char **) p);
	va_end(ap);
	return retval;
}
#else
PUBLIC int execle(name, argv)
char *name, *argv;
{
  char **p;
  p = (char **) &argv;
  while (*p++)			/* null statement */
	;
  return(execve(name, &argv, (char **) *p));
}
#endif

PUBLIC int execv(name, argv)
char *name, *argv[];
{

  return(execve(name, argv, environ));
}

PUBLIC int execve(path, argv, envp)
char *path;			/* pointer to name of file to be executed */
char *argv[];			/* pointer to argument array */
char *envp[];			/* pointer to environment */
{
  register char **argtop;
  register char **envtop;

	/* Count the argument pointers and environment pointers. */
  for (argtop = argv; *argtop != (char *) NULL; ) argtop++;
  for (envtop = envp; *envtop != (char *) NULL; ) envtop++;
  return(__execve(path, argv, envp, (int)(argtop - argv), (int)(envtop - envp)));
}

PUBLIC int __execve(path, argv, envp, nargs, nenvps)
char *path;			/* pointer to name of file to be executed */
char *argv[];			/* pointer to argument array */
char *envp[];			/* pointer to environment */
int nargs;			/* number of args */
int nenvps;			/* number of environment strings */
{
/* This is split off from execve to be called from execvp, so execvp does not
 * have to allocate up to ARG_MAX bytes just to prepend "sh" to the arg array.
 */

  char *hp, **ap, *p;
  int i, stackbytes, npointers, overflow, temp;
  char *stack;
  /* Decide how big a stack is needed. Be paranoid about overflow. */
#if ARG_MAX > INT_MAX
#error /* overflow checks and sbrk depend on sizes being ints */
#endif
  overflow = FALSE;
  npointers = 1 + nargs + 1 + nenvps + 1;	/* 1's for argc and NULLs */
  stackbytes = 0;	/* changed because _len is used now */
  if (nargs < 0 || nenvps < 0 || nargs+nenvps < 0 || npointers < 0)
	overflow = TRUE;
  for (i = PTRSIZE; i != 0; i--) {
	temp = stackbytes + npointers;
	if (temp < stackbytes) overflow = TRUE;
	stackbytes = temp;
  }
  for (i = 0, ap = argv; i < nargs; i++) {
	temp = stackbytes + _len(*ap++);
	if (temp < stackbytes) overflow = TRUE;
	stackbytes = temp;
  }
  for (i = 0, ap = envp; i < nenvps; i++) {
	temp = stackbytes + _len(*ap++);
	if (temp < stackbytes) overflow = TRUE;
	stackbytes = temp;
  }
  temp = stackbytes + PTRSIZE - 1;
  if (temp < stackbytes) overflow = TRUE;
  stackbytes = (temp / PTRSIZE) * PTRSIZE;

  /* Check for overflow before committing sbrk. */
  if (overflow || stackbytes > ARG_MAX) {
	errno = E2BIG;
	return(-1);
  }

  /* Allocate the stack. */
  stack = _sbrk(stackbytes);
  if (stack == (char *) -1) {
	errno = E2BIG;
	return(-1);
  }

  /* Prepare the stack vector and argc. */
  ap = (char **) stack;
  hp = &stack[npointers * PTRSIZE];
  *ap++ = (char *) nargs;

  /* Prepare the argument pointers and strings. */
  for (i = 0; i < nargs; i++) {
	*ap++ = (char *) (hp - stack);
	p = *argv++;
	while ((*hp++ = *p++) != 0)
		;
  }
  *ap++ = (char *) NULL;

  /* Prepare the environment pointers and strings. */
  for (i = 0; i < nenvps; i++) {
	*ap++ = (char *) (hp - stack);
	p = *envp++;
	while ((*hp++ = *p++) != 0)
		;
  }
  *ap++ = (char *) NULL;

  /* Do the real work. */
  temp = _callm1(MM, EXEC, _len(path), stackbytes, 0, path, stack, NIL_PTR);
  _sbrk(-stackbytes);
  return(temp);
}
