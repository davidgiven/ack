/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * dit programma leest een a.out file
 * voor een kleine uP (adres space = 64K)
 * en levert aan de standaard output een
 * download formaat voor de MCCPM computer.
 *
 */

#define MAXBYTE	24
#include <stdio.h>
#include <out.h>
char hex[] =  "0123456789ABCDEF";
char **s;
int bytes, bytcnt, checksum;
unsigned pc;
unsigned offset;
unsigned htou();


main (argc,argv)
int argc;
char *argv[];
	{
	if (argc > 3)
		fatal ("usage: %s filename [start-adres]\n",argv[0]);
	offset = 0;
	if (argc == 3)
		if (!(offset = htou(argv[2])))
			fatal ("adres error %s\n", argv[2]);
	if (! rd_open(*++argv)) fatal ("can't open %s\n",*argv);
	else 	{
		s = argv;
		convert ();
		}
	}

convert ()
	{
	struct outhead head;
	struct outsect sect[MAXSECT];
	int i;

	rd_ohead(&head);
	if (head.oh_flags & HF_LINK) {
		fatal("%s contains unresolved references\n",s);
	}
	rd_sect(sect, head.oh_nsect);
	for (i = 0; i < head.oh_nsect; i++) {
		rd_outsect(i);
		pc = sect[i].os_base - offset;
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
			while (sz >= MAXBYTE) {
				data(MAXBYTE, (int) pc, buf);
				checksum = 0;
				sz -= MAXBYTE;
				buf += MAXBYTE;
				pc += MAXBYTE;
			}
			if (sz > 0) {
				data(sz, (int) pc, buf);
				checksum = 0;
			}
			free(pbuf);
		}
	}
	printf (":00000001FF\n");
	}


data (sz, pc, buf)
	register char *buf;
{
	printf (":");
	outbyte (sz);
	bytcnt += 4;
	outbyte (pc >> 8);
	outbyte (pc);
	outbyte (0);
	while (sz != 0) 
		{
		outbyte (*buf++);
		sz--;
		}
	outbyte (-checksum);
	putchar ('\n');
	putchar (0);
	putchar (0);
	}

outbyte (b)
int b;
	{
	checksum = (checksum + b) & 0xFF;
	putchar (hex[(b>>4) & 0xF]);
	putchar  (hex[b & 0xF]);
	}

fatal (s,a)
	char *s, *a;
	{
	fprintf (stderr,s,a);
	exit (-1);
	}

rd_fatal()
{
	fatal("Read error\n");
}
/* convert a string of hex digits to an unsigned 16 bit number */

unsigned htou(t)
char *t;
{
unsigned n = 0;
char c;
while(c = *t++){
	if(c >= '0' && c <= '9')
		c -= '0';
	else if(c >= 'a' && c <= 'f')
		c -= 'a' - 10;
	else if(c >= 'A' && c <= 'F')
		c -= 'A' - 10;
	else
		return(0);
	n = n * 16 + c;
	}
return(n);
}
