/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*
 * cvmach.c - convert ack.out to Mach-o
 *
 * Mostly pinched from aelflod (util/amisc/aelflod.c), which pinched
 * from the ARM cv (mach/arm/cv/cv.c), which pinched from the m68k2 cv
 * (mach/m68k2/cv/cv.c).  The code to read ack.out format using
 * libobject is pinched from the Xenix i386 cv (mach/i386/cv/cv.c).
 */

#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <out.h>
#include <object.h>

/* Header and section table of ack.out */
struct outhead	outhead;
struct outsect	outsect[S_MAX];
uint32_t ack_off_char;		/* Offset of string table in ack.out */

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
#define LC_SYMTAB			2
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
#define SZ_SECTION_HEADER		68
#define SZ_SYMTAB_COMMAND		24
#define SZ_THREAD_COMMAND_BF_STATE	16
#define SZ_NLIST			12

/* the page size for x86 and PowerPC */
#define CV_PGSZ				4096
/* u modulo page size */
#define pg_mod(u) ((u) & (CV_PGSZ - 1))
/* u rounded down to whole pages */
#define pg_trunc(u) ((u) & ~(CV_PGSZ - 1))
/* u rounded up to whole pages */
#define pg_round(u) pg_trunc((u) + (CV_PGSZ - 1))

const char zero_pg[CV_PGSZ] = { 0 };

/*
 * machseg[0]: __PAGEZERO with address 0, size CV_PGSZ
 * machseg[1]: __TEXT for ack TEXT, ROM
 * machseg[2]: __DATA for ack DATA, BSS
 */
struct {
	const char	*ms_name;
	uint32_t	 ms_vmaddr;
	uint32_t	 ms_vmsize;
	uint32_t	 ms_fileoff;
	uint32_t	 ms_filesize;
	uint32_t	 ms_prot;
	uint32_t	 ms_nsects;
} machseg[3] = {
	"__PAGEZERO", 0, CV_PGSZ, 0, 0, VM_PROT_NONE, 0,
	"__TEXT", 0, 0, 0, 0, VM_PROT_READ | VM_PROT_EXECUTE, 2,
	"__DATA", 0, 0, 0, 0, VM_PROT_READ | VM_PROT_WRITE, 2,
};


static void
usage(void)
{
	fprintf(stderr, "Usage: %s -m<num> <inputfile> <outputfile>\n",
	    program);
	exit(1);
}

/* Produce an error message and exit. */
static void
fatal(const char* s, ...)
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

void
rd_fatal(void)
{
	fatal("read error");
}

/* Returns n such that 2**n == a. */
static uint32_t
log2u(uint32_t a)
{
	uint32_t n = 0;
	while (a) {
		a >>= 1;
		n++;
	}
	return n - 1;
}

/* Writes a byte. */
static void
emit8(uint8_t value)
{
	writef(&value, 1, 1);
}

/* Writes out a 16-bit value in the appropriate endianness. */
static void
emit16(uint16_t value)
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

/* Writes out a 32-bit value in the appropriate endianness. */
static void
emit32(uint32_t value)
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
static void
emit_section(int section_nr)
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
	while (n > 0)
	{
		blocksize = (n > sizeof(zero_pg)) ? sizeof(zero_pg) : n;
		writef(zero_pg, 1, blocksize);
		n -= blocksize;
	}
}

static void
emit_lc_segment(int i)
{
	uint32_t sz;
	int flags, maxprot;
	char namebuf[16];

	if (i == 0) {
		/* special values for __PAGEZERO */
		maxprot = VM_PROT_NONE;
		flags = 4; /* SG_NORELOC */
	} else {
		maxprot = VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE;
		flags = 0;
	}

	/*
	 * The size of this command includes the size of its section
	 * headers, see emit_section_header().
	 */
	sz = SZ_SEGMENT_COMMAND + machseg[i].ms_nsects * SZ_SECTION_HEADER;

	/* Use strncpy() to pad namebuf with '\0' bytes. */
	strncpy(namebuf, machseg[i].ms_name, sizeof(namebuf));

	emit32(LC_SEGMENT);		/* command */
	emit32(sz);			/* size of command */
	writef(namebuf, 1, sizeof(namebuf));
	emit32(machseg[i].ms_vmaddr);	/* vm address */
	emit32(machseg[i].ms_vmsize);	/* vm size */
	emit32(machseg[i].ms_fileoff);	/* file offset */
	emit32(machseg[i].ms_filesize);	/* file size */
	emit32(maxprot);		/* max protection */
	emit32(machseg[i].ms_prot);	/* initial protection */
	emit32(machseg[i].ms_nsects);	/* number of Mach sections */
	emit32(flags);			/* flags */
}

static void
emit_section_header(int ms, const char *name, int os)
{
	uint32_t fileoff, flags;
	char namebuf[16];

	switch (os) {
	case TEXT:
		/* S_ATTR_PURE_INSTRUCTIONS | S_ATTR_SOME_INSTRUCTIONS */
		flags = 0x80000400;
		break;
	case BSS:
		flags = 0x1; /* S_ZEROFILL */
		break;
	default:
		flags = 0x0; /* S_REGULAR */
		break;
	}

	if (os == BSS)
		fileoff = 0;
	else
		fileoff = machseg[ms].ms_fileoff +
		    (outsect[os].os_base - machseg[ms].ms_vmaddr);

	/* name of Mach section */
	strncpy(namebuf, name, sizeof(namebuf));
	writef(namebuf, 1, sizeof(namebuf));
	/* name of Mach segment */
	strncpy(namebuf, machseg[ms].ms_name, sizeof(namebuf));
	writef(namebuf, 1, sizeof(namebuf));
	emit32(outsect[os].os_base);	/* vm address */
	emit32(outsect[os].os_size);	/* vm size */
	emit32(fileoff);		/* file offset */
	emit32(log2u(outsect[os].os_lign)); /* alignment */
	emit32(0);			/* offset of relocations */
	emit32(0);			/* number of relocations */
	emit32(flags);			/* flags */
	emit32(0);			/* reserved */
	emit32(0);			/* reserved */
}

static void
emit_lc_symtab(void)
{
	uint32_t off1, off2;

	/* Symbol table will be at next page after machseg[2]. */
	off1 = pg_round(machseg[2].ms_fileoff + machseg[2].ms_filesize);
	/* String table will be after symbol table. */
	off2 = off1 + 12 * outhead.oh_nname;

	emit32(LC_SYMTAB);		/* command */
	emit32(SZ_SYMTAB_COMMAND);	/* size of command */
	emit32(off1);			/* offset of symbol table */
	emit32(outhead.oh_nname);	/* number of symbols */
	emit32(off2);			/* offset of string table */
	emit32(1 + outhead.oh_nchar);	/* size of string table */
}

static void
emit_lc_unixthread(void)
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

static void
emit_symbol(struct outname *np)
{
	uint32_t soff;
	uint8_t type;
	uint8_t sect;
	uint16_t desc;

	if (np->on_type & S_STB) {
		/* stab for debugger */
		type = np->on_type >> 8;
		desc = np->on_desc;
	} else {
		desc = 0;

		switch (np->on_type & S_TYP) {
		case S_UND:
			type = 0x0; /* N_UNDF */
			break;
		case S_ABS:
			type = 0x2; /* N_ABS */
			break;
		default:
			type = 0xe; /* N_SECT */
			break;
		}

		if (np->on_type & S_EXT)
			type |= 0x1; /* N_EXT */
	}

	switch (np->on_type & S_TYP) {
	case S_MIN + TEXT:
		sect = 1;
		break;
	case S_MIN + ROM:
		sect = 2;
		break;
	case S_MIN + DATA:
		sect = 3;
		break;
	case S_MIN + BSS:
	case S_MIN + NUM_SEGMENTS:
		sect = 4;
		break;
	default:
		sect = 0; /* NO_SECT */
		break;
	}

	/*
	 * To find the symbol's name, ack.out uses an offset from the
	 * beginning of the file, but Mach-o uses an offset into the
	 * string table.  Both formats use offset 0 for a symbol with
	 * no name.  We will prepend a '\0' at offset 0, so every
	 * named symbol needs + 1.
	 */
	if (np->on_foff)
		soff = np->on_foff - ack_off_char + 1;
	else
		soff = 0;

	emit32(soff);
	emit8(type);
	emit8(sect);
	emit16(desc);
	emit32(np->on_valu);
}

static void
emit_symtab(void)
{
	struct outname *names, *np;
	int i;
	char *chars;

	/* Using calloc(a, b) to check if a * b would overflow. */
	names = calloc(outhead.oh_nname, sizeof(struct outname));
	if (!names)
		fatal("out of memory");
	chars = malloc(outhead.oh_nchar);
	if (!names || !chars)
		fatal("out of memory");
	rd_name(names, outhead.oh_nname);
	rd_string(chars, outhead.oh_nchar);

	ack_off_char = OFF_CHAR(outhead);

	/* Emit each symbol entry. */
	for (i = 0, np = names; i < outhead.oh_nname; i++, np++)
		emit_symbol(np);

	/*
	 * Emit the string table.  The first character of a Mach-o
	 * string table must be '\0', so we prepend a '\0'.
	 */
	emit8(0);
	writef(chars, 1, outhead.oh_nchar);
}


int
main(int argc, char *argv[])
{
	uint32_t end, pad[3], sz, sz_load_cmds;
	int cpu_subtype, fd, mflag = 0;

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
		/* Use mode 0777 to allow executing the output file. */
		fd = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0777);
		if (fd < 0)
			fatal("unable to open output file.");
		output = fdopen(fd, "w");
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
	 * machseg[1] will start at a page boundary and include the
	 * Mach header and load commands before ack TEXT and ROM.
	 *
	 * Find our entry point (immediately after the load commands)
	 * and check that TEXT begins there.
	 */
	machseg[1].ms_vmaddr = pg_trunc(outsect[TEXT].os_base);
	sz_load_cmds = 3 * SZ_SEGMENT_COMMAND + 4 * SZ_SECTION_HEADER +
	    SZ_SYMTAB_COMMAND + sz_thread_command;
	entry = machseg[1].ms_vmaddr + SZ_MACH_HEADER + sz_load_cmds;
	if (entry != outsect[TEXT].os_base) {
		fatal("text segment must have base 0x%lx, not 0x%lx"
		    "\n\t(suggest em_led -b0:0x%lx)",
		    (unsigned long)entry,
		    (unsigned long)outsect[TEXT].os_base,
		    (unsigned long)entry);
	}

	/* Pad for alignment between TEXT and ROM. */
	sz = outsect[ROM].os_base - outsect[TEXT].os_base;
	pad[0] = sz - outsect[TEXT].os_size;
	if (sz < outsect[TEXT].os_size || pad[0] >= outsect[ROM].os_lign)
		fatal("the rom segment must follow the text segment.");

	/*
	 * Pad between ROM and DATA such that we can map machseg[2] at
	 * a page boundary with DATA at its correct base address.
	 *
	 * For example, if ROM ends at 0x2bed and DATA begins at
	 * 0x3000, then we pad to the page boundary.  If ROM ends at
	 * 0x2bed and DATA begins at 0x3bf0, then pad = 3 and we map
	 * the page twice, at both 0x2000 and 0x3000.
	 */
	end = outsect[ROM].os_base + outsect[ROM].os_size;
	pad[1] = pg_mod(outsect[DATA].os_base - end);

	sz = end - machseg[1].ms_vmaddr;
	machseg[1].ms_vmsize = machseg[1].ms_filesize = sz;
	machseg[2].ms_vmaddr = pg_trunc(outsect[DATA].os_base);
	machseg[2].ms_fileoff = pg_trunc(sz + pad[1]);
	if (machseg[2].ms_vmaddr < end &&
	    machseg[2].ms_vmaddr >= machseg[1].ms_vmaddr)
		fatal("the data and rom segments are too close."
		    "\n\t(suggest em_led -a2:%d)", (int)CV_PGSZ);

	if (outsect[BSS].os_flen != 0)
		fatal("the bss space contains initialized data.");
	sz = outsect[BSS].os_base - outsect[DATA].os_base;
	if (sz < outsect[DATA].os_size ||
	    sz - outsect[DATA].os_size >= outsect[BSS].os_lign)
		fatal("the bss segment must follow the data segment.");

	end = outsect[DATA].os_base + outsect[DATA].os_size;
	machseg[2].ms_filesize = end - machseg[2].ms_vmaddr;
	end = outsect[BSS].os_base + outsect[BSS].os_size;
	machseg[2].ms_vmsize = end - machseg[2].ms_vmaddr;

	if (outhead.oh_nsect == NUM_SEGMENTS + 1) {
		if (outsect[NUM_SEGMENTS].os_base !=
		    outsect[BSS].os_base + outsect[BSS].os_size)
			fatal("end segment must follow bss");
		if (outsect[NUM_SEGMENTS].os_size != 0)
			fatal("end segment must be empty");
	}

	/*
	 * Pad to page boundary between BSS and symbol table.
	 *
	 * Also, some versions of Mac OS X refuse to load any
	 * executable smaller than 4096 bytes (1 page).
	 */
	pad[2] = pg_mod(-(uint32_t)machseg[2].ms_filesize);

	/* Emit the Mach header. */
	emit32(MH_MAGIC);	/* magic */
	emit32(cpu_type);	/* cpu type */
	emit32(cpu_subtype);	/* cpu subtype */
	emit32(MH_EXECUTE);	/* file type */
	emit32(5);		/* number of load commands */
	emit32(sz_load_cmds);	/* size of load commands */
	emit32(0);		/* flags */

	emit_lc_segment(0);
	emit_lc_segment(1);
	emit_section_header(1, "__text", TEXT);
	emit_section_header(1, "__rom", ROM);
	emit_lc_segment(2);
	emit_section_header(2, "__data", DATA);
	emit_section_header(2, "__bss", BSS);
	emit_lc_symtab();
	emit_lc_unixthread();

	/* Emit non-empty sections. */
	emit_section(TEXT);
	writef(zero_pg, 1, pad[0]);
	emit_section(ROM);
	writef(zero_pg, 1, pad[1]);
	emit_section(DATA);

	writef(zero_pg, 1, pad[2]);
	emit_symtab();

	if (ferror(output))
		fatal("write error");

	return 0;
}
