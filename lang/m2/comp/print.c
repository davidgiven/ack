/* P R I N T    R O U T I N E S */

#include	<system.h>
#include	<em_arith.h>

#define SSIZE	1024	/* string-buffer size for print routines	*/

char *long2str();

doprnt(fp, fmt, argp)
	File *fp;
	char *fmt;
	int argp[];
{
	char buf[SSIZE];

	sys_write(fp, buf, format(buf, fmt, (char *)argp));
}

/*VARARGS1*/
printf(fmt, args)
	char *fmt;
	char args;
{
	char buf[SSIZE];

	sys_write(STDOUT, buf, format(buf, fmt, &args));
}

/*VARARGS1*/
fprintf(fp, fmt, args)
	File *fp;
	char *fmt;
	char args;
{
	char buf[SSIZE];

	sys_write(fp, buf, format(buf, fmt, &args));
}

/*VARARGS1*/
char *
sprintf(buf, fmt, args)
	char *buf, *fmt;
	char args;
{
	buf[format(buf, fmt, &args)] = '\0';
	return buf;
}

int
format(buf, fmt, argp)
	char *buf, *fmt;
	char *argp;
{
	register char *pf = fmt, *pa = argp;
	register char *pb = buf;

	while (*pf) {
		if (*pf == '%') {
			register int width, base, pad, npad;
			char *arg;
			char cbuf[2];
			char *badformat = "<bad format>";
			
			/* get padder */
			if (*++pf == '0') {
				pad = '0';
				++pf;
			}
			else
				pad = ' ';
			
			/* get width */
			width = 0;
			while (*pf >= '0' && *pf <= '9')
				width = 10 * width + *pf++ - '0';
			
			/* get text and move pa */
			if (*pf == 's') {
				arg = *(char **)pa;
				pa += sizeof(char *);
			}
			else
			if (*pf == 'c') {
				cbuf[0] = * (char *) pa;
				cbuf[1] = '\0';
				pa += sizeof(int);
				arg = &cbuf[0];
			}
			else
			if (*pf == 'l') {
				/* alignment ??? */
				if (base = integral(*++pf)) {
					arg = long2str(*(long *)pa, base);
					pa += sizeof(long);
				}
				else {
					pf--;
					arg = badformat;
				}
			}
			else
			if (base = integral(*pf)) {
				arg = long2str((long)*(int *)pa, base);
				pa += sizeof(int);
			}
			else
			if (*pf == '%')
				arg = "%";
			else
				arg = badformat;

			npad = width - strlen(arg);

			while (npad-- > 0)
				*pb++ = pad;
			
			while (*pb++ = *arg++);
			pb--;
			pf++;
		}
		else
			*pb++ = *pf++;
	}
	return pb - buf;
}

integral(c)
{
	switch (c) {
	case 'b':
		return -2;
	case 'd':
		return 10;
	case 'o':
		return -8;
	case 'u':
		return -10;
	case 'x':
		return -16;
	}
	return 0;
}
