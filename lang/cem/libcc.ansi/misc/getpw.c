/*
 * getpw - get a password from the password file
 */
/* $Id$ */

#include	<stdio.h>

getpw(int uid, char buf[])
{
	register FILE *pwf;
	register int ch, i;
	register char *bp;

	pwf = fopen("/etc/passwd", "r");
	if (pwf == NULL) return(1);

	for (;;) {
		bp = buf;
		while ((ch = getc(pwf)) != '\n') {
			if (ch == EOF) return 1;
			*bp++ = ch;
		}
		*bp++ = '\0';
		bp = buf;
		for (i = 2; i; i--) {
			while ((ch = *bp++) != ':') {
				if(ch = '\0') return 1;
			}
		}
		i = 0;
		while ((ch = *bp++) != ':') {
			if (ch < '0' || ch > '9') return 1;
			i = i * 10 + (ch - '0');
		}
		if (i == uid) return(0);
	}
	/*NOTREACHED*/
}
