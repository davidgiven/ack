#include <lib.h>
/* mktemp - make a name for a temporary file */
#include <sys/types.h>
#define mktemp _mktemp
#define getpid _getpid
#define access _access
#include <unistd.h>

PUBLIC char *mktemp(char *template)
{
  register int pid, k;
  register char *p;

  pid = getpid();		/* get process id as semi-unique number */
  p = template;
  while (*p) p++;		/* find end of string */

  /* Replace XXXXXX at end of template with pid. */
  while (*--p == 'X') {
	*p = '0' + (pid % 10);
	pid /= 10;
  }
  p++;
  for (k = 'a'; k <= 'z'; k++) {
	*p = k;
	if (access(template, 0) < 0) {
		return template;
	}
  }
  return("/");
}
