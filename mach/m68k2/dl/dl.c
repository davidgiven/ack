#define MAXBYTE	24
#include <stdio.h>
char hex[] =  "0123456789ABCDEF";
FILE *fp, *fopen();
char **s;
int bytes, bytcnt, checksum;
long pc;


main (argc,argv)
int argc;
char *argv[];
	{
	if (argc != 2)   fatal ("usage: %s filename\n",argv[0]);
	if ((fp = fopen (*++argv,"r")) == NULL)
		fatal ("can't open %s\n",*argv);
	else 	{
		s = argv;
		convert ();
		fclose (fp);
		}
	}

convert ()
	{
	int c;
	do
		{
		pc = getword ();
		pc = (pc << 16) | getword ();
		bytes = getword ();
		while (bytes != 0) 
			{
			bytcnt = (bytes < MAXBYTE) ? bytes : MAXBYTE;
			bytes -= bytcnt;
			checksum = 0;
			if (pc > 0xffffL) S2record (); else S1record ();
			}
		c = getc (fp);
		ungetc (c, fp);
		}
	while (c != EOF);
	printf ("S9030000FC\n");
	}


S2record ()
	{
	printf ("S2");
	bytcnt += 4;
	outbyte (bytcnt);
	outbyte (pc);
	record ();
	}

S1record ()
	{
	printf ("S1");
	bytcnt += 3;
	outbyte (bytcnt);
	record ();
	}

record ()
	{
	outbyte (pc << 8);
	outbyte (pc << 16);
	while (bytcnt != 0) 
		{
		outbyte (getbyte ());
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

getword ()
	{
	int c;
	c = getbyte ();
	return ((getbyte () << 8) | c );
	}

getbyte ()
	{
	int c;
	if ((c = getc (fp)) == EOF) fatal ("end of %s\n",*s);
	return (c);
	}
fatal (s,a)
	{
	printf (s,a);
	exit (-1);
	}
