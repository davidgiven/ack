/*
 * Simple tool to produce an MS-DOS MZ executable for an
 * absolute ack.out file.  This is currently only useful
 * for the MS-DOS target.
 * 
 * Derived from aslod.c and aelflod.c.
 * 
 * TK Chia, June 2021
 */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/stat.h>
#include "out.h"

/*
 * Header and section table of ack object file.
 */
 
struct outhead outhead;
struct outsect outsect[S_MAX];
char* stringarea;

char* outputfile = NULL;                /* Name of output file, or NULL */
char* program;                          /* Name of current program: argv[0] */

FILE* input;                            /* Input stream */
FILE* output = NULL;                    /* Output stream */

bool verbose = false;

/* Segment numbers understood by amzlod. */

enum {
	TEXT = 0,
	ROM,
	DATA,
	BSS,
	NUM_SEGMENTS
};

/* Produce an error message and exit. */

void fatal(const char* s, ...)
{
	va_list ap;
	
	fprintf(stderr, "%s: ",program) ;
	
	va_start(ap, s);
	vfprintf(stderr, s, ap);
	va_end(ap);
	
	fprintf(stderr, "\n");
	
	if (output)
		fclose(output);
	if (outputfile)
		unlink(outputfile);
	exit(1);
}

/* Read bytes from the input file. */

void readf(void *a, size_t b, size_t c)
{
	int err;
	errno = 0;
	if (fread(a, b, c, input) != c)
	{
		err = errno;
		fatal("read error: %s.", err ? strerror(err) : "end of file?");
	}
}

/* Write bytes to the output file. */

void writef(const void *a, size_t b, size_t c)
{
	int err;
	errno = 0;
	if (fwrite(a, b, c, output) != c)
	{
		err = errno;
		fatal("write error: %s.", err ? strerror(err) : "disk full?");
	}
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

/* Writes out a 16-bit word (in little endian form). */

void emit16(unsigned short value)
{
	unsigned char buffer[2];

	buffer[1] = (value >> 8) & 0xFF;
	buffer[0] = (value >> 0) & 0xFF;

	writef(buffer, 1, sizeof(buffer));
}

/* Number of bytes in a paragraph, and in a "page", in the MZ format
 * definition.
 */

#define MZ_PARA_SZ	0x0010U
#define MZ_PG_SZ	0x0200U

/* Various pre-set parameters for the output MZ file.
 *
 * MZ_HDR_PARAS gives the MZ header size in (16-byte) paragraphs.  (NOTE:
 * MS-DOS 1.x will round this size up to a multiple of the page size ---
 * i.e. 0x20 paragraphs.  So if we want the output program to not crash on
 * DOS 1.x, we will need to pad the header to the page size.  Urrgh.  If
 * compatibility with DOS 1.x is not needed, we can squish the header to only
 * 2 paragraphs.)
 *
 * MZ_INI_SP is the initial stack pointer value within the program's data
 * segment.  MZ_STK_SZ is the minimum size of the stack we want to ensure
 * for the output program.
 */

#define MZ_HDR_PARAS	0x0020U
#define MZ_INI_SP	0x0000U
#define MZ_STK_SZ	0x0100U

/* Writes out an appropriate MS-DOS MZ header.  (The MZ header format is
 * documented at OSDev (https://wiki.osdev.org/MZ), in Ralf Brown's
 * Interrupt List (www.cs.cmu.edu/afs/cs.cmu.edu/user/ralf/pub/WWW/files.html),
 * and possibly elsewhere.)
 */

void emith(void)
{
	uint32_t text_start, text_size, total_size;
	uint16_t data_start, data_end, stack_end, min_extra_paras;
	size_t n;

	/* Write the magic number. */
	emit16(0x5A4D);
	/* Write the size of the executable code and data. */
	text_size = align(outsect[TEXT].os_size, MZ_PARA_SZ);
	total_size  = MZ_HDR_PARAS * MZ_PARA_SZ;
	total_size += text_size;
	total_size += align(outsect[ROM].os_base + outsect[ROM].os_size,
			    outsect[DATA].os_lign) - outsect[ROM].os_base;
	total_size += align(outsect[DATA].os_base + outsect[DATA].os_size,
			    outsect[BSS].os_lign) - outsect[DATA].os_base;
	emit16(total_size % MZ_PG_SZ);
	emit16((total_size + MZ_PG_SZ - 1) / MZ_PG_SZ);
	/* Write the relocation entry count. */
	emit16(0);
	/* Write the header size. */
	emit16(MZ_HDR_PARAS);
	/* Write the minimum and maximum extra paragraph count. */
	data_end = outsect[DATA].os_base + outsect[DATA].os_size;
	data_end = (uint16_t)align(data_end, MZ_PARA_SZ);
	stack_end = (uint16_t)align(MZ_INI_SP, MZ_PARA_SZ);
	emit16((uint16_t)(stack_end - data_end) / MZ_PARA_SZ);
	emit16(0xFFFFU);
	/* Write the initial ss (relative) and sp. */
	data_start = outsect[ROM].os_base;
	emit16((uint16_t)(text_size / MZ_PARA_SZ - data_start / MZ_PARA_SZ));
	emit16(MZ_INI_SP);
	/* Write a dummy checksum. */
	emit16(0);
	/*
	 * Write the initial ip and (relative) cs.  Assume that the entry
	 * point is right at the start of the text section.
	 */
	text_start = outsect[TEXT].os_base;
	emit16(text_start);
	emit16(-(text_start / MZ_PARA_SZ));
	/* Write the relocation table offset. */
	emit16(0x001CU);
	/* Write the overlay number --- 0, since this is a main program. */
	emit16(0);
	/* Pad to the pre-set header size. */
	n = (MZ_PARA_SZ / 2) * MZ_HDR_PARAS - (0x1CU / 2);
	while (n-- != 0)
		emit16(0);
}

/* Copies the contents of a section from the input stream
 * to the output stream, zero filling any uninitialised
 * space. */
 
void emits(struct outsect* section, struct outsect* nextsect)
{
	char buffer[BUFSIZ];
	uint32_t real_size;

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

	/* Calculate the actual size of the section in the final memory
	 * layout.  Take into account the next section's alignment, if any.
	 * If there is no next section, align to a paragraph boundary. */
	real_size = section->os_size;
	if (nextsect)
		real_size = align(section->os_base + real_size, nextsect->os_lign)
			    - section->os_base;
	else
		real_size = align(section->os_base + real_size, MZ_PARA_SZ)
			    - section->os_base;

	/* Zero fill any remaining space. */
	
	if (section->os_flen != real_size)
	{
		uint32_t n = real_size - section->os_flen;
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
	uint16_t bss_max;

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
				fprintf(stderr, "%s: Syntax: amzlod [-h] [-v] <inputfile> <outputfile>\n",
					program);
				exit(0);
				
			case 'v':
				verbose = true;
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
			output = fopen(argv[2], "wb");
			if (!output)
				fatal("unable to open output file.");
			outputfile = argv[2];
			/* fall through */
			
		case 2: /* Input file specified. */
			input = fopen(argv[1], "rb");
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

	if (outsect[TEXT].os_base % MZ_PARA_SZ != 0)
		fatal("the text segment is not paragraph aligned.");

	if (outsect[ROM].os_base % MZ_PARA_SZ != 0)
		fatal("the rom segment is not paragraph aligned.");

	if (outsect[BSS].os_flen != 0)
		fatal("the bss space contains initialized data.");

	if (!follows(&outsect[BSS], &outsect[DATA]))
		fatal("the bss segment must follow the data segment.");

	if (!follows(&outsect[DATA], &outsect[ROM]))
		fatal("the data segment must follow the rom segment.") ;

	if (outsect[TEXT].os_base > 0xFFFFUL ||
	    outsect[TEXT].os_size > 0xFFFFUL - outsect[TEXT].os_base)
		fatal("text segment addresses do not fit into 16 bits.");

	bss_max = (uint16_t)MZ_INI_SP - (uint16_t)MZ_STK_SZ;

	if (outsect[ROM].os_base > 0xFFFFUL ||
	    outsect[ROM].os_size > 0xFFFFUL - outsect[ROM].os_base)
		fatal("rom segment addresses do not fit into 16 bits.");

	if (outsect[DATA].os_base > 0xFFFFUL ||
	    outsect[DATA].os_size > 0xFFFFUL - outsect[DATA].os_base)
		fatal("data segment addresses do not fit into 16 bits.");

	if (outsect[BSS].os_base > 0xFFFFUL ||
	    outsect[BSS].os_size > 0xFFFFUL - outsect[BSS].os_base)
		fatal("bss segment addresses do not fit into 16 bits.");

	if (outsect[BSS].os_base > bss_max ||
	    outsect[BSS].os_size > bss_max - outsect[BSS].os_base)
		fatal("not enough space left for stack.");

	/* Check for an optional end segment (which is otherwise
	 * ignored). */
	 
	if (outhead.oh_nsect == (NUM_SEGMENTS+1))
	{
		if (!follows(&outsect[NUM_SEGMENTS], &outsect[BSS]))
			fatal("end segment must follow bss");
		if ( outsect[NUM_SEGMENTS].os_size != 0 )
			fatal("end segment must be empty");
	}

	/* Write out the MZ header. */

	emith();

	/* And go! */
	
	emits(&outsect[TEXT], NULL);
	emits(&outsect[ROM], &outsect[DATA]);
	emits(&outsect[DATA], &outsect[BSS]);

	if (ferror(output))
		fatal("output write error");
	if (outputfile)
	{
		/* mode = 0755 in standard UNIX */
		chmod(outputfile, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	}
	/* Summarise what we've done. */
	
	if (verbose)
	{
		uint32_t ss = 0;
		printf(" base : %08"PRIx32"\n", outsect[TEXT].os_base) ;
		printf(" text = %08"PRIx32"\n", outsect[TEXT].os_size);
		printf(" rom  = %08"PRIx32"\n", outsect[ROM].os_size);
		printf(" data = %08"PRIx32"\n", outsect[DATA].os_size);
		printf(" bss  = %08"PRIx32"\n", outsect[BSS].os_size);
		ss += outsect[TEXT].os_size;
		ss += outsect[ROM].os_size;
		ss += outsect[DATA].os_size;
		ss += outsect[BSS].os_size;
		printf("TOTAL = %08"PRIx32"\n", ss);
	}
	
	return 0;
}

