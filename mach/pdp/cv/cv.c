/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * This program converts ack.out format to PDP 11 V7 a.out format.
 * It uses ~em/modules/lib/libobject.a.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>

struct exec {
	short a_magic;
	short a_text;
	short a_data;
	short a_bss;
	short a_syms;
	short a_entry;
	short a_unused;
	short a_flag;
};

struct  nlist
{       char            n_name[8];
	short           n_type;
	short           n_value; 
};

#include <object.h>
#include <out.h>

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

#define ENTRY 0x0	/* entry point */

/*
 * Header and section table of new format object file.
 */
struct outhead	outhead;
struct outsect	outsect[S_MAX];

struct exec exec;

char	*output_file;
int	outputfile_created;
FILE	*output;
int	rom_in_text;

static void cv_int2(int n);
static void fatal(const char* s, ...);
static void emits(struct outsect*);
static void emit_symtab(void);

char *program ;

char flag ;

/* Output file definitions and such */

#define TEXTSG	0
#define ROMSG	1
#define DATASG	2
#define BSSSG	3
#define LSECT	BSSSG+1
#define NSECT	LSECT+1

int main(int argc, char* argv[])
{
	register struct exec *e = &exec;

	output = stdout;
	program= argv[0] ;
	if ( argc>1 && argv[1][0]=='-' ) {
		flag=argv[1][1] ;
		argc-- ; argv++ ;
	}
	switch (argc) {
	case 1: rd_fdopen(0);
		break;
	case 3:	if ((output = fopen(argv[2], "wb")) == NULL) {
			fatal("Can't write %s.\n", argv[2]);
		}
		output_file = argv[2];
		outputfile_created = 1;
		/* FALLTHROUGH */
	case 2:
		if (! rd_open(argv[1]))
			fatal("Can't read %s.\n", argv[1]);
		break;
	default:fatal("Usage: %s <as object> <dl object>.\n", argv[0]);
	}
	rd_ohead(&outhead);
	if (BADMAGIC(outhead))
		fatal("Not an ack object file.\n");
	if (outhead.oh_flags & HF_LINK)
		fatal("Contains unresolved references.\n");
	if (outhead.oh_nrelo > 0)
		fprintf(stderr, "Warning: relocation information present.\n");
	if ( outhead.oh_nsect!=LSECT && outhead.oh_nsect!=NSECT )
		fatal("Input file must have %d sections, not %ld\n",
			NSECT,outhead.oh_nsect) ;
	rd_sect(outsect, outhead.oh_nsect);
	if (outsect[TEXTSG].os_size & 1)
		outsect[TEXTSG].os_size++;
	if (outsect[ROMSG].os_size & 1)
		outsect[ROMSG].os_size++;
	if (outsect[DATASG].os_size & 1)
		outsect[DATASG].os_size++;
	if (outsect[BSSSG].os_size & 1)
		outsect[BSSSG].os_size++;
	/* A few checks */
	if ( outsect[TEXTSG].os_base != ENTRY)
		fatal("text must start at %d not at 0x%lx\n", ENTRY,
			outsect[TEXTSG].os_base) ;
	if ( outsect[BSSSG].os_flen != 0 )
		fatal("bss space contains initialized data\n") ;
	if ( outsect[BSSSG].os_base != outsect[DATASG].os_base+
					outsect[DATASG].os_size )
		fatal("bss segment must follow data segment\n") ;

	e->a_magic = 0407;
	e->a_text = outsect[TEXTSG].os_size;
	e->a_data = outsect[ROMSG].os_size + outsect[DATASG].os_size;
	e->a_bss = outsect[BSSSG].os_size;
	e->a_entry = outsect[TEXTSG].os_base;
	e->a_syms = outhead.oh_nname * sizeof (struct nlist);
	e->a_flag = 1;
	if ( outsect[ROMSG].os_base == 0x0 ) {
		/* Separate I/D */
		e->a_magic = 0411;
		if ( outsect[DATASG].os_base != outsect[ROMSG].os_base+
						outsect[ROMSG].os_size )
			fatal("data segment must follow rom\n") ;
	} else  if ( outsect[ROMSG].os_lign == 0x2000 ) {
		/* -n, rom in data */
		e->a_magic = 0410;
		if ( outsect[DATASG].os_base != outsect[ROMSG].os_base+
						outsect[ROMSG].os_size )
			fatal("data segment must follow rom\n") ;
	} else  if ( outsect[DATASG].os_lign == 0x2000 ) {
		/* -n, rom in text */
		rom_in_text = 1;
		e->a_magic = 0410;
		e->a_text += outsect[ROMSG].os_size;
		e->a_data -= outsect[ROMSG].os_size;
		if ( outsect[ROMSG].os_base != outsect[TEXTSG].os_base+
						outsect[TEXTSG].os_size )
			fatal("rom segment must follow text\n") ;
	}
	else {
		if ( outsect[ROMSG].os_base != outsect[TEXTSG].os_base+
						outsect[TEXTSG].os_size )
			fatal("rom segment must follow text\n") ;
		if ( outsect[DATASG].os_base != outsect[ROMSG].os_base+
						outsect[ROMSG].os_size )
			fatal("data segment must follow rom\n") ;
	}
	if ( outhead.oh_nsect==NSECT ) {
		if ( outsect[LSECT].os_base != outsect[BSSSG].os_base+
						outsect[BSSSG].os_size )
			fatal("end segment must follow bss\n") ;
		if ( outsect[LSECT].os_size != 0 )
			fatal("end segment must be empty\n") ;
	}

	/* Action at last */
	cv_int2(e->a_magic);
	cv_int2(e->a_text);
	cv_int2(e->a_data);
	cv_int2(e->a_bss);
	cv_int2(e->a_syms);
	cv_int2(e->a_entry);
	cv_int2(e->a_unused);
	cv_int2(e->a_flag);
	emits(&outsect[TEXTSG]) ;
	emits(&outsect[ROMSG]) ;
	emits(&outsect[DATASG]) ;
	emit_symtab();
	if ( outputfile_created ) chmod(argv[2],0755);
	return 0;
}

void cv_int2(int n)
{
	putc(n, output);
	putc((n>>8), output);
}

/*
cv_long(l)
	long l;
{
	putc((int)(l >> 16), output);
	putc((int)(l >> 24), output);
	putc((int) l, output);
	putc(((int)l >> 8), output);
}
*/

/*
 * Transfer the emitted byted from one file to another.
 */
void emits(struct outsect* section) {
	register long	n ;
	register int	blk;
	char		buffer[BUFSIZ];

	n= section->os_flen ;
	rd_outsect(section - outsect);
	while (n > 0) {
		blk = n > BUFSIZ ? BUFSIZ : n;
		rd_emit(buffer, (long) blk);
		fwrite(buffer, sizeof(char), blk, output);
		n -= blk;
	}
	if ((n = section->os_size - section->os_flen) > 0) {
		for (blk = BUFSIZ - 1; blk >= 0; blk--) {
			buffer[blk] = 0;
		}
		while (n > 0) {
			blk = n > BUFSIZ ? BUFSIZ : n;
			fwrite(buffer, sizeof(char), blk, output);
			n -= blk;
		}
	}
}

void emit_symtab(void)
{
	struct outname ACK_name;  /* symbol table entry in ACK format */
	struct nlist PDP_name;	  /* symbol table entry in PDP V7 format */
	register unsigned short i;

	char *chars;
	long l;
	long off = OFF_CHAR(outhead);
	int j;
	char *p;

	chars = malloc((unsigned) (outhead.oh_nchar));
	rd_string(chars,outhead.oh_nchar);
	for (i = 0; i < outhead.oh_nname; i++) {
		rd_name(&ACK_name, 1);
		switch(ACK_name.on_type & S_TYP) {
			case S_UND:
				PDP_name.n_type = 0;	
				break;
			case S_ABS:
				PDP_name.n_type = 01;
				break;
			case S_MIN + TEXTSG:
				PDP_name.n_type = 02; 
				break;
			case S_MIN + ROMSG:
				if (rom_in_text) {
					PDP_name.n_type = 02;
					break;
				}
				/* Fall through */
			case S_MIN + DATASG:
				PDP_name.n_type = 03;
				break;
			case S_MIN + BSSSG:
			case S_MIN + LSECT:
				PDP_name.n_type = 04;
				break;
			default:
				fprintf(stderr,"warning: unknown s_type: %d\n",
					ACK_name.on_type & S_TYP);
		}
		if (ACK_name.on_type & S_EXT) PDP_name.n_type |= 040; 
		PDP_name.n_value = ACK_name.on_valu;
		if (ACK_name.on_foff == 0) {
			p = "\0\0";
		}
		else {
			l = ACK_name.on_foff - off;
			if (l < 0 || l >= outhead.oh_nchar) {
				fatal("bad on_off: %ld\n",l);
			}
			p = &chars[l];
		}
		for (j = 0; j < 8; j++) {
			PDP_name.n_name[j] = *p++;
			if (*p == '\0') break;
		}
		for (j++; j < 8; j++) {
			PDP_name.n_name[j] = 0;
		}
		fwrite((char *) &PDP_name, sizeof(char), 8, output);
		cv_int2(PDP_name.n_type);
		cv_int2(PDP_name.n_value);
	}
}

/* VARARGS1 */
void fatal(const char* s, ...)
{
	va_list ap;

	fprintf(stderr,"%s: ",program) ;
	va_start(ap, s);
	vfprintf(stderr, s, ap);
	va_end(ap);

	if (outputfile_created)
		unlink(output_file);
	exit(1);
}

void rd_fatal(void)
{
	fatal("read error\n");
}
