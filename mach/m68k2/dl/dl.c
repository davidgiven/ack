/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
static char rcsid[] = "$Id$";
#define MAXBYTE	24
#include <stdio.h>
#include <out.h>
char hex[] =  "0123456789ABCDEF";
FILE *fp, *fopen();
char **s;
int bytes, bytcnt, checksum;
long pc;
struct outhead ohead;
struct outsect sect[MAXSECT];


main (argc,argv)
int argc;
char *argv[];
	{
	if (argc != 2)   fatal ("usage: %s filename\n",argv[0]);
	if (! rd_open (*++argv))
		fatal ("can't open %s\n",*argv);
	else 	{
		s = argv;
		convert ();
		}
	}

convert ()
	{
	int i;

	rd_ohead(&ohead);
	if (ohead.oh_flags & HF_LINK) {
		fatal("%s contains unresolved references\n",s);
	}
	rd_sect(sect, ohead.oh_nsect);
	for (i = 0; i < ohead.oh_nsect; i++) {
		rd_outsect(i);
		pc = sect[i].os_base;
		while (sect[i].os_size) {
			unsigned int sz = 8096, fl;
			extern char *calloc();
			register char *buf;
			char *pbuf;

			if (sz > sect[i].os_size) sz = sect[i].os_size;
			sect[i].os_size -= sz;
			pbuf = buf = calloc(sz, 1);
			if (fl = sect[i].os_flen) {
				if (fl > sz) fl = sz;
				sect[i].os_flen -= fl;

				rd_emit(buf, (long) fl);
			}
			while (sz > 0) {
				int p = bytcnt = sz < MAXBYTE ? sz : MAXBYTE;
				checksum = 0;
				sz -= p;
				if (pc > 0xffffL) 
					S2record (buf);
				else S1record (buf);
				buf += p;
			}
			free(pbuf);
		}
	}
	printf ("S9030000FC\n");
	}


S2record (buf)
	char *buf;
	{
	printf ("S2");
	bytcnt += 4;
	outbyte (bytcnt);
	outbyte ((int) (pc >> 16));
	outbyte ((int) (pc >> 8));
	outbyte ((int) pc);
	record (buf);
	}

S1record (buf)
	char *buf;
	{
	printf ("S1");
	bytcnt += 3;
	outbyte (bytcnt);
	outbyte ((int) (pc >> 8));
	outbyte((int) pc);
	record (buf);
	}

record (buf)
	register char *buf;
	{
	while (bytcnt != 0) 
		{
		outbyte (*buf++);
		pc ++;
		}
	outbyte (~checksum);
	putchar ('\n');
	putchar (0);
	putchar (0);
	}

outbyte (b)
int b;
	{
	checksum = (checksum + b) & 0377;
	putchar (hex[(b>>4) & 017]);
	putchar  (hex[b & 017]);
	-- bytcnt;
	}

rd_fatal() { fatal("Read error\n"); }

fatal (s,a)
	{
	printf (s,a);
	exit (-1);
	}
