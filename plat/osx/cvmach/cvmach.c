/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*
 * cvmach.c - convert ack.out to mach-o
 *
 * Mostly pinched from aelflod (util/amisc/aelflod.c), which pinched
 * from the ARM cv (mach/arm/cv/cv.c), which pinched from the m68k2 cv
 * (mach/m68k2/cv/cv.c).  The code to read ack.out format using
 * liboject is pinched from the Xenix i386 cv (mach/i386/cv/cv.c).
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <out.h>
/* Can't find #include <object.h>*/

/* Header and section table of ack.out */
struct outhead	outhead;
struct outsect	outsect[S_MAX];

int bigendian;			/* Emit big-endian Mach-o? */
int cpu_type;
uint32_t entry;			/* Virtual address of entry point */
uint32_t sz_thread_command;

char *outputfile = NULL;	/* Name of output file, or NULL */
char *program;			/* Name of current program: argv[0] */
FILE *output;			/* Output stream */
#define writef(a, b, c)	fwrite((a), (b), (c), output)

/* Segment numbers in ack.out */
enum {
	TEXT = 0,
	ROM,
	DATA,
	BSS,
	NUM_SEGMENTS
};

/* Constants from Mach headers */
#define MH_MAGIC			0xfeedface
#define MH_EXECUTE			2
#define LC_SEGMENT			1
#define LC_UNIXTHREAD			5

#define CPU_TYPE_X86			7
#define CPU_SUBTYPE_X86_ALL		3
#define x86_THREAD_STATE32		1
#define x86_THREAD_STATE32_COUNT	16

#define CPU_TYPE_POWERPC		18
#define CPU_SUBTYPE_POWERPC_ALL		0
#define PPC_THREAD_STATE		1
#define PPC_THREAD_STATE_COUNT		40

#define VM_PROT_NONE			0x0
#define VM_PROT_READ			0x1
#define VM_PROT_WRITE			0x2
#define VM_PROT_EXECUTE			0x4

/* sizes of Mach structs */
#define SZ_MACH_HEADER			28
#define SZ_SEGMENT_COMMAND		56
#define SZ_THREAD_COMMAND_BF_STATE	16

/* the page size for x86 and PowerPC */
#define CV_PGSZ				4096
/* u modulo page size */
#define pg_mod(u) ((u) & (CV_PGSZ - 1))
/* u rounded down to whole pages */
#define pg_trunc(u) ((u) & ~(CV_PGSZ - 1))


void usage(void)
{
	fprintf(stderr, "Usage: %s -m<num> <inputfile> <outputfile>\n",
	    program);
	exit(1);
}

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

void rd_fatal(void)
{
	fatal("read error");
}

/* Calculate the result of a aligned to b (rounding up if necessary).
 * b must be a power of two. */
uint32_t align(uint32_t a, uint32_t b)
{
	a += b - 1;
	return a & ~(b-1);
}


/* Writes out a 32-bit value in the appropriate endianness. */
void emit32(uint32_t value)
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
void emit_section(int section_nr)
{
	struct outsect *section = &outsect[section_nr];
	size_t blocksize;
	uint32_t n = section->os_flen;
	char buffer[BUFSIZ];

	rd_outsect(section_nr);
	while (n > 0)
	{
		blocksize = (n > BUFSIZ) ? BUFSIZ : n;
		rd_emit(buffer, (long)blocksize);
		writef(buffer, 1, blocksize);
		n -= blocksize;
	}

	/* Zero fill any remaining space. */
	n = section->os_size - section->os_flen;
	if (n > 0)
	{
		memset(buffer, 0, BUFSIZ);
		while (n > 0)
		{
			blocksize = (n > BUFSIZ) ? BUFSIZ : n;
			writef(buffer, 1, blocksize);
			n -= blocksize;
		}
	}
}

void emit_lc_segment(char *name, uint32_t vm_ad, uint32_t vm_sz,
    uint32_t f_off, uint32_t f_sz, int prot)
{
	char namebuf[16];
	int flags, maxprot;

	if (prot == VM_PROT_NONE) {
		/* special values for __PAGEZERO */
		maxprot = VM_PROT_NONE;
		flags = 4; /* NORELOC */
	} else {
		maxprot = VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE;
		flags = 0;
	}

	/* Use strncpy() to pad namebuf with '\0' bytes. */
	strncpy(namebuf, name, sizeof(namebuf));

	emit32(LC_SEGMENT);		/* command */
	emit32(SZ_SEGMENT_COMMAND);	/* size of command */
	writef(namebuf, 1, sizeof(namebuf));
	emit32(vm_ad);			/* vm address */
	emit32(vm_sz);			/* vm size */
	emit32(f_off);			/* file offset */
	emit32(f_sz);			/* file size */
	emit32(maxprot);		/* max protection */
	emit32(prot);			/* initial protection */
	emit32(0);			/* number of Mach sections */
	emit32(flags);			/* flags */
}

void emit_lc_unixthread(void)
{
	int i, ireg, ts, ts_count;

	/*
	 * The thread state has ts_count registers.  The ireg'th
	 * register holds the entry point.  We can set other registers
	 * to zero.  At execution time, the kernel will allocate a
	 * stack and set the stack pointer.
	 */
	switch (cpu_type) {
	case CPU_TYPE_X86:
		ireg = 10;	/* eip */
		ts = x86_THREAD_STATE32;
		ts_count = x86_THREAD_STATE32_COUNT;
		break;
	case CPU_TYPE_POWERPC:
		ireg = 0;	/* srr0 */
		ts = PPC_THREAD_STATE;
		ts_count = PPC_THREAD_STATE_COUNT;
		break;
	}

	emit32(LC_UNIXTHREAD);		/* command */
	emit32(sz_thread_command);	/* size of command */
	emit32(ts);			/* thread state */
	emit32(ts_count);		/* thread state count */
	for (i = 0; i < ts_count; i++) {
		if (i == ireg)
			emit32(entry);
		else
			emit32(0);
	}
}


int main(int argc, char *argv[])
{
	uint32_t len2, len3, mach_base, pad, sz_bf_entry, sz_load_cmds;
	int cpu_subtype, mflag = 0;

	/* General housecleaning and setup. */
	output = stdout;
	program = argv[0];

	/* Read in and process any flags. */
	while ((argc > 1) && (argv[1][0] == '-')) {
		switch (argv[1][1]) {
		case 'm': /* machine cpu type */
			mflag = 1;
			cpu_type = atoi(&argv[1][2]);
			break;
		case 'h': /* help */
		default:
			usage();
		}

		argv++;
		argc--;
	}

	if (!mflag)
		usage();

	/* Check cpu type. */
	switch (cpu_type) {
	case CPU_TYPE_X86:
		bigendian = 0;
		cpu_subtype = CPU_SUBTYPE_X86_ALL;
		sz_thread_command = 4 * x86_THREAD_STATE32_COUNT;
		break;
	case CPU_TYPE_POWERPC:
		bigendian = 1;
		cpu_subtype = CPU_SUBTYPE_POWERPC_ALL;
		sz_thread_command = 4 * PPC_THREAD_STATE_COUNT;
		break;
	default:
		/* Can't emit LC_UNIXTHREAD for unknown cpu. */
		fatal("unknown cpu type -m%d", cpu_type);
	}
	sz_thread_command += SZ_THREAD_COMMAND_BF_STATE;

	/* Process the rest of the arguments. */
	switch (argc) {
	case 1: /* No parameters --- read from stdin, write to stdout. */
		rd_fdopen(0);
		break;

	case 3: /* Both input and output files specified. */
		output = fopen(argv[2], "w");
		if (!output)
			fatal("unable to open output file.");
		outputfile = argv[2];
		/* FALLTHROUGH */

	case 2: /* Input file specified. */
		if (! rd_open(argv[1]))
			fatal("unable to open input file.");
		break;

	default:
		usage();
	}

	rd_ohead(&outhead);
	if (BADMAGIC(outhead))
		fatal("Not an ack object file.");
	if (outhead.oh_flags & HF_LINK)
		fatal("Contains unresolved references.");
	if (outhead.oh_nrelo > 0)
		fprintf(stderr, "Warning: relocation information present.");
	if (outhead.oh_nsect != NUM_SEGMENTS &&
	    outhead.oh_nsect != NUM_SEGMENTS + 1 ) {
		fatal("Input file must have %d sections, not %ld\n",
		    NUM_SEGMENTS, (long)outhead.oh_nsect);
	}

	rd_sect(outsect, outhead.oh_nsect);

	/*
	 * 1st Mach segment: __PAGEZERO
	 * 2nd Mach segment: __TEXT
	 *   Mach headers and load commands
	 *   ack TEXT
	 *   ack ROM
	 * 3rd Mach segment: __DATA
	 *   ack DATA
	 *   ack BSS
	 */

	/* Find entry point and check that TEXT begins there. */
	mach_base = pg_trunc(outsect[TEXT].os_base);
	sz_load_cmds = 3 * SZ_SEGMENT_COMMAND + sz_thread_command;
	sz_bf_entry = SZ_MACH_HEADER + sz_load_cmds;
	entry = mach_base + sz_bf_entry;
	if (entry != outsect[TEXT].os_base) {
		fatal("text segment must have base 0x%lx, not 0x%lx\n",
		    entry, outsect[TEXT].os_base);
	}

	/* Check that ROM can follow TEXT in 2nd Mach segment. */
	outsect[TEXT].os_size =
	    align(outsect[TEXT].os_size, outsect[ROM].os_lign);
	if (outsect[ROM].os_base !=
	    outsect[TEXT].os_base + outsect[TEXT].os_size)
		fatal("the rom segment must follow the text segment.");

	/*
	 * Insert padding between ROM and DATA, such that
	 *   pg_mod(len2) == pg_mod(outsect[DATA].os_base)
	 *
	 * This will allow us to map the 3rd Mach segment at the
	 * beginning of a page, such that DATA is at its base.
	 */
	len2 = sz_bf_entry + outsect[TEXT].os_size + outsect[ROM].os_size;
	pad = pg_mod(outsect[DATA].os_base - len2);
	outsect[ROM].os_size += pad;
	len2 = pg_trunc(len2 + pad);

	/* Check that BSS can follow DATA in 3rd Mach segment. */
	if (outsect[BSS].os_flen != 0)
		fatal("the bss space contains initialized data.");
	if (outsect[BSS].os_base <
	    outsect[DATA].os_base + outsect[DATA].os_size)
		fatal("the bss segment must follow the data segment.");

	len3 = outsect[BSS].os_base - pg_trunc(outsect[DATA].os_base) +
	    outsect[BSS].os_size;

	if (outhead.oh_nsect == NUM_SEGMENTS + 1) {
		if (outsect[NUM_SEGMENTS].os_base !=
		    outsect[BSS].os_base + outsect[BSS].os_size)
			fatal("end segment must follow bss");
		if (outsect[NUM_SEGMENTS].os_size != 0)
			fatal("end segment must be empty");
	}

	/* Emit the Mach header. */
	emit32(MH_MAGIC);	/* magic */
	emit32(cpu_type);	/* cpu type */
	emit32(cpu_subtype);	/* cpu subtype */
	emit32(MH_EXECUTE);	/* file type */
	emit32(4);		/* number of load commands */
	emit32(sz_load_cmds);	/* size of load commands */
	emit32(0);		/* flags */

	/*			vm address:	vm size:
	 * 1st Mach segment:	NULL		CV_PGSZ
	 * 2nd Mach segment:	mach_base	len2
	 * 3rd Mach segment:	mach_base+len2	len3
	 *
	 *			file offset:	file size:
	 * 2nd Mach segment:	0		len2
	 * 3rd Mach segment:	len2		DATA os_size
	 */
	emit_lc_segment("__PAGEZERO", 0 /* NULL */, CV_PGSZ,
	    0, 0, VM_PROT_NONE);
	emit_lc_segment("__TEXT", mach_base, len2,
	    0, len2, VM_PROT_READ | VM_PROT_EXECUTE);
	emit_lc_segment("__DATA", mach_base + len2, len3,
	    len2, outsect[DATA].os_size, VM_PROT_READ | VM_PROT_WRITE);
	emit_lc_unixthread();

	/* Emit non-empty sections. */
	emit_section(TEXT);
	emit_section(ROM);
	emit_section(DATA);

	if (ferror(output))
		fatal("write error");

	return 0;
}
