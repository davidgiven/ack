/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * This program converts ack.out format to PC/IX a.out format.
 * It uses ~em/modules/lib/libobject.a.
 */

#include <stdio.h>

/* unsigned char: not portable */

struct exec {
	char   a_magic[2];
	char   a_flags;
	char   a_cpu;
	char   a_hdrlen;
	char   a_unused;
	unsigned short  a_version;
	long            a_text;
	long            a_data;
	long            a_bss;
	long            a_entry;
	long            a_misc;
	long            a_syms;
};

struct  nlist
{       char            n_name[8];
	long            n_value; 
	char   n_sclass;
	char   n_numaux;
	unsigned short  n_type;
};

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

char *program ;
char *chars;
char *malloc();

char flag ;

/* Output file definitions and such */

#define TEXTSG	0
#define ROMSG	1
#define DATASG	2
#define BSSSG	3
#define LSECT	BSSSG+1
#define NSECT	LSECT+1

long align(a,b)
        long a,b;
{
        a += b - 1;
        return a - a % b;
}

int
follows(pa, pb)
        register struct outsect *pa, *pb;
{
        /* return 1 if pa follows pb */

        return pa->os_base == align(pb->os_base+pb->os_size, pa->os_lign);
}

main(argc, argv)
	int	argc;
	char	*argv[];
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
	case 3:	if ((output = fopen(argv[2], "w")) == 0) {
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
	/* A few checks */
	if ( outsect[TEXTSG].os_base != ENTRY)
		fatal("text must start at %d not at 0x%lx\n", ENTRY,
			outsect[TEXTSG].os_base) ;
	if ( outsect[BSSSG].os_flen != 0 )
		fatal("bss space contains initialized data\n") ;
	if (! follows(&outsect[BSSSG], &outsect[DATASG]))
		fatal("bss segment must follow data segment\n") ;
	if (! follows(&outsect[DATASG], &outsect[ROMSG]))
		fatal("data segment must follow rom\n") ;

	outsect[ROMSG].os_size = outsect[DATASG].os_base - outsect[ROMSG].os_base;
	outsect[DATASG].os_size = outsect[BSSSG].os_base - outsect[DATASG].os_base;
	e->a_magic[0] = 01;
	e->a_magic[1] = 03;
	e->a_cpu = 04;
	e->a_hdrlen = 32;
	e->a_data = outsect[BSSSG].os_base - outsect[ROMSG].os_base;
	e->a_bss = outsect[BSSSG].os_size;
	e->a_entry = outsect[TEXTSG].os_base;
	e->a_syms = (long) outhead.oh_nname * sizeof (struct nlist);
	e->a_misc = 0x10000;
	if ( outsect[ROMSG].os_base == 0x0 ) {
		/* Separate I/D */
		e->a_flags = 0x20;
		outsect[TEXTSG].os_size = e->a_text =
			align(outsect[TEXTSG].os_size,16L);
	} else {
		e->a_flags = 0x10;
		outsect[TEXTSG].os_size = e->a_text =
			outsect[ROMSG].os_base - outsect[TEXTSG].os_base;
		if (! follows(&outsect[ROMSG], &outsect[TEXTSG]))
			fatal("rom segment must follow text\n") ;
	}
	if ( outhead.oh_nsect==NSECT ) {
		if (! follows(&outsect[LSECT], &outsect[BSSSG]))
			fatal("end segment must follow bss\n") ;
		if ( outsect[LSECT].os_size != 0 )
			fatal("end segment must be empty\n") ;
	}

	if (((unsigned) outhead.oh_nchar != outhead.oh_nchar) ||
	     (outhead.oh_nchar != 0 &&
	      (chars = malloc((unsigned)outhead.oh_nchar)) == 0)) {
		fprintf(stderr, "%s: (warning) No name list generated\n", program);
		e->a_syms = 0;
	}

	/* Action at last */
	fwrite((char *) e, 1, 6, output);
	wr_int2((int) e->a_version);
	wr_long(e->a_text);
	wr_long(e->a_data);
	wr_long(e->a_bss);
	wr_long(e->a_entry);
	wr_long(e->a_misc);
	wr_long(e->a_syms);
	emits(&outsect[TEXTSG]) ;
	emits(&outsect[ROMSG]) ;
	emits(&outsect[DATASG]) ;
	if (e->a_syms) emit_symtab();
	fclose(output);
	if ( outputfile_created ) chmod(argv[2],0755);
	exit(0);
}

wr_int2(n)
	int n;
{
	char buf[2];

	buf[0] = n;
	buf[1] = (n >> 8);
	fwrite(buf, 1, 2, output);
}

wr_long(l)
	long l;
{
	char buf[4];

	buf[0] = l;
	buf[1] = (l >> 8);
	buf[2] = (l >> 16);
	buf[3] = (l >> 24);
	fwrite(buf, 1, 4, output);
}

/*
 * Transfer the emitted byted from one file to another.
 */
emits(section) struct outsect *section ; {
	register long	n ;
	register int	blk;
	char		buffer[BUFSIZ];

	n= section->os_flen ;
	rd_outsect(section - outsect);
	while (n > 0) {
		blk = n > BUFSIZ ? BUFSIZ : n;
		rd_emit(buffer, (long) blk);
		fwrite(buffer, 1, blk, output);
		n -= blk;
	}
	if ((n = section->os_size - section->os_flen) > 0) {
		for (blk = BUFSIZ - 1; blk >= 0; blk--) {
			buffer[blk] = 0;
		}
		while (n > 0) {
			blk = n > BUFSIZ ? BUFSIZ : n;
			fwrite(buffer, 1, blk, output);
			n -= blk;
		}
	}
}

emit_symtab()
{
	struct outname ACK_name;  /* symbol table entry in ACK format */
	struct nlist IX_name;	  /* symbol table entry in PC/IX format */
	register unsigned short i;

	long l;
	long off = OFF_CHAR(outhead);
	int j;
	char *p;

	rd_string(chars,outhead.oh_nchar);
	for (i = 0; i < outhead.oh_nname; i++) {
		rd_name(&ACK_name, 1);
		switch(ACK_name.on_type & S_TYP) {
			case S_UND:
				IX_name.n_sclass = 0;
				break;
			case S_ABS:
				IX_name.n_sclass = 01;
				break;
			case S_MIN + TEXTSG:
				IX_name.n_sclass = 02; 
				break;
			case S_MIN + ROMSG:
			case S_MIN + DATASG:
				IX_name.n_sclass = 03;
				break;
			case S_MIN + BSSSG:
			case S_MIN + LSECT:
				IX_name.n_sclass = 04;
				break;
			default:
				fprintf(stderr,"warning: unknown s_type: %d\n",
					ACK_name.on_type & S_TYP);
		}
		if (ACK_name.on_type & S_EXT) IX_name.n_sclass |= 020;
		IX_name.n_value = ACK_name.on_valu;
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
			IX_name.n_name[j] = *p++;
			if (*p == '\0') break;
		}
		for (j++; j < 8; j++) {
			IX_name.n_name[j] = 0;
		}
		fwrite((char *) &IX_name, 1, 8, output);
		wr_long(IX_name.n_value);
		fwrite(&(IX_name.n_sclass), 1, 2, output);
		wr_int2((int) IX_name.n_type);
	}
}

/* VARARGS1 */
fatal(s, a1, a2)
	char	*s;
{
	fprintf(stderr,"%s: ",program) ;
	fprintf(stderr, s, a1, a2);
	if (outputfile_created) {
		fclose(output);
		unlink(output_file);
	}
	exit(1);
}

rd_fatal()
{
	fatal("read error\n");
}
