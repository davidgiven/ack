/*
 * dit programma leest een a.out file
 * voor een kleine uP (adres space = 64K)
 * en levert aan de standaard output een
 * download formaat voor de MCCPM computer.
 *
 */

#define MAXBYTE	24
#include <stdio.h>
char hex[] =  "0123456789ABCDEF";
FILE *fp, *fopen();
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
		pc -= offset;
		bytes = getword ();
		bytes = getword ();
		while (bytes != 0) 
			{
			bytcnt = (bytes < MAXBYTE) ? bytes : MAXBYTE;
			bytes -= bytcnt;
			checksum = 0;
			Irecord ();
			}
		c = getc (fp);
		ungetc (c, fp);
		}
	while (c != EOF);
	printf (":00000001FF\n");
	}


Irecord ()
	{
	printf (":");
	outbyte (bytcnt);
	bytcnt += 4;
	outbyte (pc >> 8);
	outbyte (pc);
	outbyte (0);
	record ();
	}


record ()
	{
	while (bytcnt != 0) 
		{
		outbyte (getbyte ());
		pc ++;
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
