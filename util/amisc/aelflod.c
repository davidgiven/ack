/*
 * $Source$
 * $State$
 *
 * Simple tool to produce an utterly basic ELF executable
 * from an absolute ack.out file. Suitable for operating
 * systems like Linux.
 * 
 * This tool produces an executable with a program header
 * only and no section header.
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
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include "out.h"

#define ASSERT(x) switch (2) { case 0: case (x): ; }

/* Global settings. */

int bigendian = 0;
int elfabi = 3;                         /* abi = Linux */
int elfmachine = 3;                     /* machine = EM_386 */

/* Header and section table of an ack object file. */
 
struct outhead outhead;
struct outsect outsect[S_MAX];
char* stringarea;

char* outputfile = NULL;                /* Name of output file, or NULL */
char* program;                          /* Name of current program: argv[0] */

FILE* input;                            /* Input stream */
FILE* output;                           /* Output stream */

#define readf(a, b, c)	fread((a), (b), (int)(c), input)
#define writef(a, b, c)	fwrite((a), (b), (int)(c), output)

/* Header and program header table of an ELF object file. */

#define ELF_HEADER_SIZE 0x34
#define PROGRAM_HEADER_SIZE 0x20
#define PROGRAM_HEADER_COUNT 1
unsigned long codeoffset;

const char elf_le_ident_string[] = {
	0x7F, 'E', 'L', 'F'
};

/* Output file definitions and such */

#define HDR_LENGTH	32

char hdr[HDR_LENGTH] ;

/* Segment numbers understood by aelflod. */

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

/* Calculate the result of a aligned to b (rounding up if necessary).
 * b must be a power of two. */

long align(long a, long b)
{
	a += b - 1;
	return a & ~(b-1);
}
 
int follows(struct outsect* pa, struct outsect* pb)
{
	/* return 1 if pa follows pb */
 
	return (pa->os_base >= align(pb->os_base+pb->os_size, pa->os_lign));
}

/* Writes a byte. */

void emit8(unsigned char value)
{
	writef(&value, 1, 1);
}

/* Writes out 16 and 32 bit words in the appropriate endianness. */

void emit16(unsigned short value)
{
	unsigned char buffer[2];
	
	if (bigendian)
	{
		buffer[0] = (value >> 8) & 0xFF;
		buffer[1] = (value >> 0) & 0xFF;
	}
	else
	{
		buffer[1] = (value >> 8) & 0xFF;
		buffer[0] = (value >> 0) & 0xFF;
	}
	
	writef(buffer, 1, sizeof(buffer));
}

void emit32(unsigned long value)
{
	unsigned char buffer[4];
	
	if (bigendian)
	{
		buffer[0] = (value >> 24) & 0xFF;
		buffer[1] = (value >> 16) & 0xFF;
		buffer[2] = (value >>  8) & 0xFF;
		buffer[3] = (value >>  0) & 0xFF;
	}
	else
	{
		buffer[3] = (value >> 24) & 0xFF;
		buffer[2] = (value >> 16) & 0xFF;
		buffer[1] = (value >>  8) & 0xFF;
		buffer[0] = (value >>  0) & 0xFF;
	}
	
	writef(buffer, 1, sizeof(buffer));
}
		
/* Copies the contents of a section from the input stream
 * to the output stream. */
 
void emits(struct outsect* section)
{
	char buffer[BUFSIZ];
	long n = section->os_flen;
	while (n > 0)
	{
		int blocksize = (n > BUFSIZ) ? BUFSIZ : n;
		readf(buffer, 1, blocksize);
		writef(buffer, 1, blocksize);
		n -= blocksize;
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

/* Writes out an ELF program header. */

void emitphdr(unsigned long address, unsigned long filesize,
	unsigned int memsize, unsigned int alignment, int flags)
{
	static unsigned long fileoffset = 0;
	
	emit32(1);                         /* type = PT_LOAD */
	emit32(fileoffset);                /* file offset */
	emit32(address);                   /* virtual address */
	emit32(0);                         /* physical address */
	emit32(filesize);                  /* file size */
	emit32(memsize);                   /* memory size */
	emit32(flags);                     /* executable, readable, writable */
	emit32(alignment);                 /* alignment */
	
	fileoffset += filesize;
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
			case 'a':
				elfabi = atoi(&argv[1][2]);
				break;

			case 'b':
				bigendian = 1;
				break;

			case 'h':
				fprintf(stderr, "%s: Syntax: aelflod [-a<number>] [-b] [-h] [-l]\n\t[-m<number>] <inputfile> <outputfile>\n",
					program);
				exit(0);

			case 'l':
				bigendian = 0;
				break;

			case 'm':
				elfmachine = atoi(&argv[1][2]);
				break;

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

	/* Ensure the base address doesn't overlap the file header. */

	codeoffset = outsect[TEXT].os_base & 0x1FFF;
	if (codeoffset < (ELF_HEADER_SIZE + PROGRAM_HEADER_SIZE*PROGRAM_HEADER_COUNT))
		fatal("base address too small --- overlaps ELF header");

	/* Rationalise the memory sizes. */
	
	outsect[TEXT].os_size = outsect[ROM ].os_base - outsect[TEXT].os_base;
	outsect[ROM ].os_size = outsect[DATA].os_base - outsect[ROM ].os_base;
	outsect[DATA].os_size = outsect[BSS ].os_base - outsect[DATA].os_base;
	outsect[BSS ].os_size = align(outsect[BSS].os_size, outsect[BSS].os_lign);

	/* Write out the ELF file header. */
	
	writef(elf_le_ident_string, 4, 1);
	emit8(1);                          /* class = ELFCLASS32 */
	emit8(bigendian ? 2 : 1);          /* endianness */
	emit8(1);                          /* ELF version */
	emit8(elfabi);                     /* ABI */
	emit8(0);                          /* ABI version */
	emit8(0); emit16(0);               /* padding... */
	emit32(0);                         /* ...to offset 0x10 */
	
	emit16(2);                         /* type = ET_EXEC */
	emit16(elfmachine);                /* machine */
	emit32(1);                         /* ELF version again */
	emit32(outsect[TEXT].os_base);     /* entry point */
	emit32(ELF_HEADER_SIZE);           /* program header offset */
	emit32(0);                         /* section header offset */
	emit32(0);                         /* flags */
	emit16(ELF_HEADER_SIZE);           /* elf header size */
	emit16(PROGRAM_HEADER_SIZE);       /* program header entry size */
	emit16(1);                         /* number of program header entries */
	emit16(0x28);                      /* section header entry size */
	emit16(0);                         /* number of section header entries */
	emit16(0);                         /* section header string table index = SHN_UNDEF */
	
	/* Write out a single rwx section for the entire program. */
	
	{
		unsigned long filelength = codeoffset +
			outsect[TEXT].os_size +
			outsect[ROM].os_size +
			outsect[DATA].os_size;
			
		unsigned long memlength = filelength +
			outsect[BSS].os_size;
		
		emitphdr(outsect[TEXT].os_base & ~0x1FFF, filelength, memlength,
			0, 4|2|1);
	}

	/* Write padding until the code start. */
	
	fseek(output, codeoffset, SEEK_SET);

	/* Write out the actual data. */
	
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
		printf("        address  length\n");
		printf(" ehdr : %08"PRIx32" %08"PRIx32"\n", outsect[TEXT].os_base & ~0x1FFF, codeoffset);
		printf(" text : %08"PRIx32" %08"PRIx32"\n", outsect[TEXT].os_base, outsect[TEXT].os_size);
		printf(" rom  : %08"PRIx32" %08"PRIx32"\n", outsect[ROM].os_base, outsect[ROM].os_size);
		printf(" data : %08"PRIx32" %08"PRIx32"\n", outsect[DATA].os_base, outsect[DATA].os_size);
		printf(" bss  : %08"PRIx32" %08"PRIx32"\n", outsect[BSS].os_base, outsect[BSS].os_size);
		ss += outsect[TEXT].os_size;
		ss += outsect[ROM].os_size;
		ss += outsect[DATA].os_size;
		ss += outsect[BSS].os_size;
		printf("TOTAL :          %08lX\n", ss);
	}
	
	return 0;
}

