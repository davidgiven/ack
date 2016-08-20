/*
 * Simple tool to produce a memory dump for an absolute
 * ack.out file. Suitable for noddy operating systems
 * like DOS, CP/M, Arthur, etc. Also useful for RAM
 * images (but *not* ROM images, note) and, more
 * importantly, general test purposes.
 * 
 * Mostly pinched from the ARM cv (and then rewritten in
 * ANSI C). Which, according to the comment, was pinched
 * from m68k2; therefore I am merely continuing a time-
 * honoured tradition.
 * 
 * (I was 10 when the original for this was checked into
 * CVS...)
 * 
 * dtrg, 2006-10-17
 * 
 * $Source$
 * $State$
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/stat.h>
#include "out.h"

#define ASSERT(x) switch (2) { case 0: case (x): ; }

/*
 * Header and section table of ack object file.
 */
 
struct outhead outhead;
struct outsect outsect[S_MAX];
char* stringarea;

char* outputfile = NULL;                /* Name of output file, or NULL */
char* program;                          /* Name of current program: argv[0] */

FILE* input;                            /* Input stream */
FILE* output;                           /* Output stream */

#define readf(a, b, c)	fread((a), (b), (int)(c), input)
#define writef(a, b, c)	fwrite((a), (b), (int)(c), output)

/* Output file definitions and such */

#define HDR_LENGTH	32

char hdr[HDR_LENGTH] ;

/* Segment numbers understood by aslod. */

enum {
	TEXT = 0,
	ROM,
	DATA,
	BSS,
	NUM_SEGMENTS
};

#define N_EXT	040
#define N_UNDEF	00
#define N_ABS	01
#define N_TEXT	02
#define N_DATA	03
#define N_BSS	04
#define N_FN	037

/* Produce an error message and exit. */

void fatal(const char* s, ...)
{
	va_list ap;
	
	fprintf(stderr, "%s: ",program) ;
	
	va_start(ap, s);
	vfprintf(stderr, s, ap);
	va_end(ap);
	
	fprintf(stderr, "\n");
	
	if (outputfile)
		unlink(outputfile);
	exit(1);
}

/* Calculate the result of a aligned to b (rounding up if necessary). */

long align(long a, long b)
{
	a += b - 1;
	return a - a % b;
}
 
int follows(struct outsect* pa, struct outsect* pb)
{
	/* return 1 if pa follows pb */
 
	return (pa->os_base == align(pb->os_base+pb->os_size, pa->os_lign));
}

/* Copies the contents of a section from the input stream
 * to the output stream, zero filling any uninitialised
 * space. */
 
void emits(struct outsect* section)
{
	char buffer[BUFSIZ];

	/* Copy the actual data. */
	
	{
		long n = section->os_flen;
		while (n > 0)
		{
			int blocksize = (n > BUFSIZ) ? BUFSIZ : n;
			readf(buffer, 1, blocksize);
			writef(buffer, 1, blocksize);
			n -= blocksize;
		}
	}
	
	/* Zero fill any remaining space. */
	
	if (section->os_flen != section->os_size)
	{
		long n = section->os_size - section->os_flen;
		memset(buffer, 0, BUFSIZ);

		while (n > 0)
		{
			int blocksize = (n > BUFSIZ) ? BUFSIZ : n;
			writef(buffer, 1, blocksize);
			n -= blocksize;
		}
	}
}


/* Macros from modules/src/object/obj.h */
#define Xchar(ch)	((ch) & 0377)
#define uget2(c)	(Xchar((c)[0]) | ((unsigned) Xchar((c)[1]) << 8))
#define get4(c)		(uget2(c) | ((long) uget2((c)+2) << 16))

/* Read the ack.out file header. */

int rhead(FILE* f, struct outhead* head)
{
	char buf[SZ_HEAD], *c;
	
	if (fread(buf, sizeof(buf), 1, f) != 1)
		return 0;

	c = buf;
	head->oh_magic = uget2(c); c += 2;
	head->oh_stamp = uget2(c); c += 2;
	head->oh_flags = uget2(c); c += 2;
	head->oh_nsect = uget2(c); c += 2;
	head->oh_nrelo = uget2(c); c += 2;
	head->oh_nname = uget2(c); c += 2;
	head->oh_nemit = get4(c); c += 4;
	head->oh_nchar = get4(c);
	return 1;
}

/* Read an ack.out section header. */
 
int rsect(FILE* f, struct outsect* sect)
{
	char buf[SZ_SECT], *c;
	
	if (fread(buf, sizeof(buf), 1, f) != 1)
		return 0;

	c = buf;
	sect->os_base = get4(c); c += 4;
	sect->os_size = get4(c); c += 4;
	sect->os_foff = get4(c); c += 4;
	sect->os_flen = get4(c); c += 4;
	sect->os_lign = get4(c);
	return 1 ;
}

int main(int argc, char* argv[])
{
	/* General housecleaning and setup. */
	
	input = stdin;
	output = stdout;
	program = argv[0];
	
	/* Read in and process any flags. */
	
	while ((argc > 1) && (argv[1][0] == '-'))
	{
		switch (argv[1][1])
		{
			case 'h':
				fprintf(stderr, "%s: Syntax: aslod [-h] <inputfile> <outputfile>\n",
					program);
				exit(0);
				
			default:
			syntaxerror:
				fatal("syntax error --- try -h for help");
		}
		
		argv++;
		argc--;
	}

	/* Process the rest of the arguments. */
	
	switch (argc)
	{
		case 1: /* No parameters --- read from stdin, write to stdout. */
			break;
			
		case 3: /* Both input and output files specified. */
			output = fopen(argv[2], "w");
			if (!output)
				fatal("unable to open output file.");
			outputfile = argv[2];
			/* fall through */
			
		case 2: /* Input file specified. */
			input = fopen(argv[1], "r");
			if (!input)
				fatal("unable to open input file.");
			break;
			
		default:
			goto syntaxerror;
	}

	/* Read and check the ack.out file header. */
				
	if (!rhead(input,&outhead))
		fatal("failed to read file header.");
	if (BADMAGIC(outhead))
		fatal("this isn't an ack object file.");
	if (outhead.oh_nrelo > 0)
		fprintf(stderr, "Warning: relocation information present.");
	if (!((outhead.oh_nsect == NUM_SEGMENTS) ||
	      (outhead.oh_nsect == (NUM_SEGMENTS+1))))
		fatal("the input file must have %d sections, not %ld.",
			NUM_SEGMENTS, outhead.oh_nsect);
			
	/* Read in the section headers. */
	
	{
		int i;
		for (i=0; i<outhead.oh_nsect; i++)
		{
			if (!rsect(input, &outsect[i]))
				fatal("failed to read section header %d.", i);
		}
	}

	/* A few checks */

	if (outsect[BSS].os_flen != 0)
		fatal("the bss space contains initialized data.");

	if (!follows(&outsect[BSS], &outsect[DATA]))
		fatal("the bss segment must follow the data segment.");

	if (!follows(& outsect[ROM], &outsect[TEXT]))
		fatal("the rom segment must follow the text segment.");

	if (!follows(&outsect[DATA], &outsect[ROM]))
		fatal("the data segment must follow the rom segment.") ;

	/* Check for an optional end segment (which is otherwise
	 * ignored). */
	 
	if (outhead.oh_nsect == (NUM_SEGMENTS+1))
	{
		if (!follows(&outsect[NUM_SEGMENTS], &outsect[BSS]))
			fatal("end segment must follow bss");
		if ( outsect[NUM_SEGMENTS].os_size != 0 )
			fatal("end segment must be empty");
	}

	/* And go! */
	
	emits(&outsect[TEXT]);
	emits(&outsect[ROM]);
	emits(&outsect[DATA]);

	if (ferror(output))
		fatal("output write error");
	if (outputfile)
		chmod(outputfile, 0755);

	/* Summarise what we've done. */
	
	{
		long ss = 0;
		printf(" base : %08"PRIx32"\n", outsect[TEXT].os_base) ;
		printf(" text = %08"PRIx32"\n", outsect[TEXT].os_size);
		printf(" rom  = %08"PRIx32"\n", outsect[ROM].os_size);
		printf(" data = %08"PRIx32"\n", outsect[DATA].os_size);
		printf(" bss  = %08"PRIx32"\n", outsect[BSS].os_size);
		ss += outsect[TEXT].os_size;
		ss += outsect[ROM].os_size;
		ss += outsect[DATA].os_size;
		ss += outsect[BSS].os_size;
		printf("TOTAL = %08lX\n", ss);
	}
	
	return 0;
}

