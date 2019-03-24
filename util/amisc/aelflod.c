/*
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
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include "out.h"

#define ASSERT(x) switch (2) { case 0: case (x): ; }

/* Global settings. */

int bigendian = 0;
int elfabi = 3;                         /* abi = Linux */
int elfmachine = 3;                     /* machine = EM_386 */
uint32_t elfflags = 0;                  /* elf processor flags */

/* Header and section table of an ack object file. */

struct outhead outhead;
struct outsect outsect[S_MAX];
struct outname* outname = NULL;
char* stringarea;
uint32_t ack_off_char;
int nstab = 0;                          /* S_STB symbol count */
int nsym = 0;                           /* other symbol count */
int nlocal = 0;                         /* local symbols */

char* outputfile = NULL;                /* Name of output file, or NULL */
char* program;                          /* Name of current program: argv[0] */

FILE* input;                            /* Input stream */
FILE* output;                           /* Output stream */

#define readf(a, b, c)	fread((a), (b), (int)(c), input)
#define writef(a, b, c)	fwrite((a), (b), (int)(c), output)

/* Contents of an ELF object file. */

#define ELF_HEADER_SIZE 0x34
#define PROGRAM_HEADER_SIZE 0x20
#define PROGRAM_HEADER_COUNT 1
#define SECTION_HEADER_SIZE 0x28
#define STAB_SYMBOL_SIZE 12
#define ELF_SYMBOL_SIZE 16

uint32_t code_offset;       /* ELF segment */
uint32_t stab_offset;       /* Debugger symbol table */
uint32_t symtab_offset;     /* ELF symbol table */
uint32_t strtab_offset;     /* String table */
uint32_t shstrtab_offset;   /* Section header string table */
uint32_t sh_offset;         /* ELF section headers */

int sh_count = 0;           /* Number of ELF sections */
int shstrtab_nr = 0;        /* Section number of .shstrtab */
int shstrtab_size;

const char elf_le_ident_string[] = {
	0x7F, 'E', 'L', 'F'
};

bool verbose = false;

/* Segment numbers understood by aelflod. */

enum {
	TEXT = 0,
	ROM,
	DATA,
	BSS,
	NUM_SEGMENTS
};

/*
 * ELF section numbers count up in the order that we write the ELF
 * section headers.  If we have no debugger symbols, we will skip
 * .stab and .stabstr, then subtract 2 from all later numbers.
 */
enum {
	N_UNDEF = 0,
	N_TEXT,
	N_RODATA,
	N_DATA,
	N_BSS,
	N_STAB,
	N_STABSTR,
	N_SYMTAB,
	N_STRTAB,
	N_SHSTRTAB,
	NUM_ELF_SECTIONS,
};
const char shstrtab[] =
	"\0.text\0.rodata\0.data\0.bss\0.stab\0.stabstr\0"
	".symtab\0.strtab\0.shstrtab";
	/* Compiler appends one more "\0". */
const int sh_name[] = {
	/* Index of each name in shstrtab: */
	0, 1, 7, 15, 21, 26, 32,
	41, 49, 57,
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

/* Convert a symbol's name index from ack.out to ELF. */

uint32_t cvname(struct outname* n)
{
	if (n->on_foff) {
		/* ack.out: offset from beginning of file
		 * ELF: index in string table
		 * the + 1 because we prepend a '\0' */
		return n->on_foff - ack_off_char + 1;
	} else
		return 0;  /* no name */
}

/* Convert a symbol's type and binding from ack.out to ELF. */

int cvinfo(struct outname* n)
{
	int bind, type;

	switch (n->on_type & S_ETC) {
		case S_SCT:
			type = 3;  /* STT_SECTION */
			break;
		case S_FIL:
		case S_MOD:
			type = 4;  /* STT_FILE */
			break;
		default:
			switch (n->on_type & S_TYP) {
				case S_MIN + TEXT:
					type = 2;  /* STT_FUNC */
					break;
				case S_MIN + ROM:
				case S_MIN + DATA:
				case S_MIN + BSS:
				case S_MIN + NUM_SEGMENTS:
					type = 1;  /* STT_OBJECT */
					break;
				default:
					type = 0;  /* STT_NOTYPE */
					break;
			}
			break;
	}

	if (n->on_type & S_EXT)
		bind = 1;  /* STB_GLOBAL */
	else
		bind = 0;  /* STB_LOCAL */

	return (bind << 4) | type;
}

/* Convert a symbol's section index from ack.out to ELF. */

int cvsect(struct outname* n)
{
	switch (n->on_type & S_TYP) {
		case S_ABS:
			return 0xfff1;  /* SHN_ABS */
		case S_MIN + TEXT:
			return N_TEXT;
		case S_MIN + ROM:
			return N_RODATA;
		case S_MIN + DATA:
			return N_DATA;
		case S_MIN + BSS:
		case S_MIN + NUM_SEGMENTS:
			return N_BSS;
		default:
			return N_UNDEF;
	}
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

/* The next few functions write parts of the symbol table. */

void emit_stab(void)
{
	struct outname* n;
	int i;

	for (i = 0; i < outhead.oh_nname; i++) {
		n = &outname[i];
		if (n->on_type & S_STB) {
			emit32(cvname(n));          /* name index */
			emit8(n->on_type >> 8);     /* type */
			emit8(cvsect(n));           /* section */
			emit16(n->on_desc);         /* desc */
			emit32(n->on_valu);         /* value */
		}
	}
}

void emit_symtab(void)
{
	struct outname* n;
	int i, pass;
	bool global;

	/* ELF .symtab must have local symbols before other symbols.
	 * We emit locals in pass 0, globals in pass 1. */
	for (pass = 0; pass < 2; pass++) {
		for (i = 0; i < outhead.oh_nname; i++) {
			n = &outname[i];

			/* Don't emit .stab symbol in .symtab. */
			if (n->on_type & S_STB)
				continue;

			global = (n->on_type & S_EXT);
			if ((pass == 0 && !global) ||
			    (pass == 1 && global)) {
				emit32(cvname(n));    /* name index */
				emit32(n->on_valu);   /* value */
				emit32(0);            /* size = unknown */
				emit8(cvinfo(n));     /* info */
				emit8(0);             /* other */
				emit16(cvsect(n));    /* section */
			}
		}
	}
}

void emit_strtab(void)
{
	/* We prepend a '\0' because ELF uses offset 0 for symbols
	 * without a name. */
	emit8('\0');
	writef(stringarea, outhead.oh_nchar, 1);
}

void emit_shstrtab(void)
{
	if (nstab) {
		writef(shstrtab, sizeof(shstrtab), 1);
	} else {
		/* Skip .stab and .stabstr */
		int i = sh_name[N_SYMTAB];
		writef(shstrtab, sh_name[N_STAB], 1);
		writef(shstrtab + i, sizeof(shstrtab) - i, 1);
	}
}

/* Writes out an ELF section header. */

void emit_sh(int i)
{
	uint32_t name, type, flags, addr, offset, size, link, info,
		 addralign, entsize;

	/* If no debugger symbols, skip .stab and .stabstr */
	if (nstab == 0 && (i == N_STAB || i == N_STABSTR))
		return;

	name = sh_name[i];
	if (nstab == 0 && i >= N_STAB)
		name -= (sh_name[N_SYMTAB] - sh_name[N_STAB]);

	switch (i) {
		case N_TEXT:
		case N_RODATA:
		case N_DATA:
		case N_STAB:
			type = 1;  /* SHT_PROGBITS */
			break;
		case N_BSS:
			type = 8;  /* SHT_NOBITS */
			break;
		case N_SYMTAB:
			type = 2;  /* SHT_SYMTAB */
			break;
		case N_STABSTR:
		case N_STRTAB:
		case N_SHSTRTAB:
			type = 3;  /* SHT_STRTAB */
			break;
		default:
			type = 0;  /* SHT_NULL */
			break;
	}

	switch (i) {
		case N_TEXT:
			flags = 4|2;  /* SHF_EXECINSTR|SHF_ALLOC */
			addr = outsect[TEXT].os_base;
			offset = code_offset;
			size = outsect[TEXT].os_size;
			addralign = outsect[TEXT].os_lign;
			break;
		case N_RODATA:
			flags = 2;    /* SHF_ALLOC */
			addr = outsect[ROM].os_base;
			offset = code_offset + outsect[TEXT].os_size;
			size = outsect[ROM].os_size;
			addralign = outsect[ROM].os_lign;
			break;
		case N_DATA:
			flags = 2|1;  /* SHF_ALLOC|SHF_WRITE */
			addr = outsect[DATA].os_base;
			offset = code_offset + outsect[TEXT].os_size +
				outsect[ROM].os_size;
			size = outsect[DATA].os_size;
			addralign = outsect[DATA].os_lign;
			break;
		case N_BSS:
			flags = 2|1;  /* SHF_ALLOC|SHF_WRITE */
			addr = outsect[BSS].os_base;
			offset = code_offset + outsect[TEXT].os_size +
				outsect[ROM].os_size + outsect[DATA].os_size;
			size = outsect[BSS].os_size;
			addralign = outsect[BSS].os_lign;
			break;
		default:
			flags = addr = offset = size = addralign = 0;
			break;
	}

	entsize = 0;
	switch (i) {
		case N_STAB:
			offset = stab_offset;
			size = STAB_SYMBOL_SIZE * nstab;
			entsize = STAB_SYMBOL_SIZE;
			break;
		case N_SYMTAB:
			offset = symtab_offset;
			size = ELF_SYMBOL_SIZE * nsym;
			entsize = ELF_SYMBOL_SIZE;
			break;
		case N_STABSTR:
		case N_STRTAB:
			/* .stabstr, .strtab share the string area */
			offset = strtab_offset;
			/* the + 1 because we prepend a '\0' */
			size = 1 + outhead.oh_nchar;
			break;
		case N_SHSTRTAB:
			offset = shstrtab_offset;
			size = shstrtab_size;
			break;
	}

	/* Link .stab to .stabstr and .symtab to .strtab */
	switch (i) {
		case N_STAB:
			link = N_STABSTR;
			break;
		case N_SYMTAB:
			link = N_STRTAB;
			if (nstab == 0)
				link -= 2;
			break;
		default:
			link = 0;
			break;
	}

	switch (i) {
		case N_SYMTAB:
			info = nlocal;
			break;
		default:
			info = 0;
			break;
	}

	emit32(name);
	emit32(type);
	emit32(flags);
	emit32(addr);
	emit32(offset);
	emit32(size);
	emit32(link);
	emit32(info);
	emit32(addralign);
	emit32(entsize);
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

/*
 * Read the ack.out symbol table and string area.  Count symbols.
 * Seek back to the current file position.
 */
int rnames(FILE* f)
{
	long told;
	int i;

	/* If no symbols, then do nothing successfully. */
	if (outhead.oh_nname == 0)
		return 1;

	/* Seek to the symbol table. */
	told = ftell(f);
	if (told == -1)
		return 0;
	ack_off_char = OFF_CHAR(outhead);  /* for cvname() */
	if (fseek(f, OFF_NAME(outhead), SEEK_SET))
		return 0;

	/* Using calloc(a, b) to check if a * b would overflow. */
	outname = calloc(outhead.oh_nname, sizeof(outname[0]));
	if (outname == NULL)
		fatal("out of memory.");
	for (i = 0; i < outhead.oh_nname; i++) {
		char buf[SZ_NAME], *c;
		if (fread(buf, SZ_NAME, 1, f) != 1)
			return 0;
		c = buf;
		outname[i].on_foff = get4(c); c += 4;
		outname[i].on_type = uget2(c); c += 2;
		outname[i].on_desc = uget2(c); c += 2;
		outname[i].on_valu = get4(c);
		if (outname[i].on_type & S_STB) {
			nstab++;
		} else {
			nsym++;
			if (!(outname[i].on_type & S_EXT))
				nlocal++;
		}
	}

	stringarea = malloc(outhead.oh_nchar);
	if (stringarea == NULL)
		fatal("out of memory.");
	if (fread(stringarea, outhead.oh_nchar, 1, f) != 1)
		return 0;

	if (fseek(f, told, SEEK_SET))
		return 0;
	return 1;
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
				elfabi = strtoul(&argv[1][2], NULL, 0);
				break;

			case 'b':
				bigendian = 1;
				break;

			case 'h':
				fprintf(stderr, "%s: Syntax: aelflod [-a<number>] [-b] [-h] [-l]\n\t[-m<number>] [-v] <inputfile> <outputfile>\n",
					program);
				exit(0);

			case 'l':
				bigendian = 0;
				break;

			case 'm':
				elfmachine = strtoul(&argv[1][2], NULL, 0);
				break;

			case 'f':
				elfflags = strtoul(&argv[1][2], NULL, 0);
				break;

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

	/* Read the symbol table, then seek back to the section data. */

	if (!rnames(input))
		fatal("failed to read symbol table.");

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

	code_offset = outsect[TEXT].os_base & 0x1FFF;
	if (code_offset < (ELF_HEADER_SIZE + PROGRAM_HEADER_SIZE*PROGRAM_HEADER_COUNT))
		fatal("base address too small --- overlaps ELF header");

	/* Rationalise the memory sizes. */
	
	outsect[TEXT].os_size = outsect[ROM ].os_base - outsect[TEXT].os_base;
	outsect[ROM ].os_size = outsect[DATA].os_base - outsect[ROM ].os_base;
	outsect[DATA].os_size = outsect[BSS ].os_base - outsect[DATA].os_base;
	outsect[BSS ].os_size = align(outsect[BSS].os_size, outsect[BSS].os_lign);

	stab_offset = code_offset + outsect[TEXT].os_size +
	    outsect[ROM].os_size + outsect[DATA].os_size;

	/* If we have symbols, then calculate some offsets. */

	if (outhead.oh_nname) {
		sh_count = NUM_ELF_SECTIONS;
		shstrtab_nr = N_SHSTRTAB;
		shstrtab_size = sizeof(shstrtab);
		if (nstab == 0) {
			/* Skip .stab and .stabstr */
			sh_count -= 2;
			shstrtab_nr -= 2;
			shstrtab_size -=
			    (sh_name[N_SYMTAB] - sh_name[N_STAB]);
		}

		symtab_offset = stab_offset + STAB_SYMBOL_SIZE * nstab;
		strtab_offset = symtab_offset + ELF_SYMBOL_SIZE * nsym;
		/* the + 1 because we prepend a '\0' */
		shstrtab_offset = strtab_offset + 1 + outhead.oh_nchar;
		sh_offset = shstrtab_offset + shstrtab_size;
	}

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
	emit32(sh_offset);                 /* section header offset */
	emit32(elfflags);                  /* flags */
	emit16(ELF_HEADER_SIZE);           /* elf header size */
	emit16(PROGRAM_HEADER_SIZE);       /* program header entry size */
	emit16(1);                         /* number of program header entries */
	emit16(SECTION_HEADER_SIZE);       /* section header entry size */
	emit16(sh_count);                  /* number of section header entries */
	emit16(shstrtab_nr);               /* section header string table index  */
	
	/* Write out a single rwx section for the entire program. */
	
	{
		uint32_t filelength = stab_offset;
		uint32_t memlength = filelength + outsect[BSS].os_size;

		emitphdr(outsect[TEXT].os_base & ~0x1FFF,
			 filelength, memlength, 0, 4|2|1);
	}

	/* Write padding until the code start. */

	if (fseek(output, code_offset, SEEK_SET))
		fatal("output seek error");

	/* Write out the actual data. */
	
	emits(&outsect[TEXT]);
	emits(&outsect[ROM]);
	emits(&outsect[DATA]);

	/* Write out the symbol table and section headers. */

	if (outhead.oh_nname) {
		int i;
		if (nstab)
			emit_stab();
		emit_symtab();
		emit_strtab();
		emit_shstrtab();
		for (i = 0; i < NUM_ELF_SECTIONS; i++)
			emit_sh(i);
	}

	if (ferror(output))
		fatal("output write error");
	if (outputfile)
		chmod(outputfile, 0755);

	/* Summarise what we've done. */
	
	if (verbose)
	{
		uint32_t ss = 0;
		printf("        address  length\n");
		printf(" ehdr : %08"PRIx32" %08"PRIx32"\n", outsect[TEXT].os_base & ~0x1FFF, code_offset);
		printf(" text : %08"PRIx32" %08"PRIx32"\n", outsect[TEXT].os_base, outsect[TEXT].os_size);
		printf(" rom  : %08"PRIx32" %08"PRIx32"\n", outsect[ROM].os_base, outsect[ROM].os_size);
		printf(" data : %08"PRIx32" %08"PRIx32"\n", outsect[DATA].os_base, outsect[DATA].os_size);
		printf(" bss  : %08"PRIx32" %08"PRIx32"\n", outsect[BSS].os_base, outsect[BSS].os_size);
		ss += outsect[TEXT].os_size;
		ss += outsect[ROM].os_size;
		ss += outsect[DATA].os_size;
		ss += outsect[BSS].os_size;
		printf("TOTAL :          %08"PRIx32"\n", ss);
	}
	
	return 0;
}

