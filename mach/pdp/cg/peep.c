#include <stdio.h>

/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 * Author: Hans van Staveren
 */

char buf[512];

main() {
	register n,sa;
	register char *p;

	sa=0;
	for (;;) {
		getline(buf);
		if (n=stackadjust()) {
			sa += n;
			continue;
		}
		if (nullinstruction())
			continue;
		if (sa) {
			if (buf[0]=='t' && buf[1]=='s' && buf[2]=='t' && buf[3]==' ') {
				sa -= 2;
				buf[0]='m';
				buf[1]='o';
				buf[2]='v';
				strcat(buf,",(sp)+");
			} else if (buf[0]=='m' && buf[1]=='o' && buf[2]=='v' &&
			    buf[3]==' ' && (p=index(&buf[5],','))!=0 &&
			    p[1]=='-' && p[2]=='(' && p[3]=='s') {
				sa -= 2;
				p[1]=' ';
			}
		}
		switch(sa) {
		case 0:break;
		case 2:puts("tst (sp)+");sa=0;break;
		case 4:puts("cmp (sp)+,(sp)+");sa=0;break;
		case 6:puts("add $6.,sp");sa=0;break;
		}
		puts(buf);
	}
}

getline(buf) register char *buf; {
	register c;

	while ((c=getchar())==' ' || c=='\t')
		;
	if (c==EOF)
		exit(0);
	do *buf++=c;
	while ((c=getchar())!='\n');
	*buf=0;
}

stackadjust() {

	if (buf[0]=='t' &&
	    buf[1]=='s' &&
	    buf[2]=='t' &&
	    buf[3]==' ' &&
	    buf[4]=='(' &&
	    buf[5]=='s' &&
	    buf[6]=='p' &&
	    buf[7]==')' &&
	    buf[8]=='+') return(2);
	if (buf[0]=='c' &&
	    buf[1]=='m' &&
	    buf[2]=='p' &&
	    buf[3]==' ' &&
	    buf[4]=='(' &&
	    buf[5]=='s' &&
	    buf[6]=='p' &&
	    buf[7]==')' &&
	    buf[8]=='+' &&
	    buf[9]==',' &&
	    buf[10]=='(' &&
	    buf[11]=='s' &&
	    buf[12]=='p' &&
	    buf[13]==')' &&
	    buf[14]=='+') return(4);
	if (buf[0]=='a' &&
	    buf[1]=='d' &&
	    buf[2]=='d' &&
	    buf[3]==' ' &&
	    buf[4]=='$' &&
	    buf[5]=='6' &&
	    buf[6]=='.' &&
	    buf[7]==',' &&
	    buf[8]=='s' &&
	    buf[9]=='p' &&
	    buf[10]==0) return(6);
	return(0);
}

nullinstruction() {
	register char *p;

	if (buf[4]=='$' && buf[5]=='0' && buf[6]=='.' && buf[7]==',') {
		p=index(buf,'-');
		if (p!=0 && p[1]=='(')
			return(0);
		p=index(buf,'+');
		if (p!=0 && p[-1]==')')
			return(0);
		if (buf[0]=='b' && buf[1]=='i' && (buf[2]=='s' || buf[2]=='c'))
			return(1);
		if (buf[0]=='a' && buf[1]=='d' && buf[2]=='d')
			return(1);
		if (buf[0]=='s' && buf[1]=='u' && buf[2]=='b')
			return(1);
	}
	return(0);
}
