/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

char *long2str();

static int
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

int
_format(buf, fmt, argp)
	char *buf, *fmt;
	char *argp;
{
	register char *pf = fmt, *pa = argp;
	register char *pb = buf;

	while (*pf) {
		if (*pf == '%') {
			register width, base, pad, npad;
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
				cbuf[0] = * (int *) pa;
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
