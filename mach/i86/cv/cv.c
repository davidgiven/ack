/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/*
 * This program converts ack.out format to PC/IX a.out format.
 * It uses ~em/modules/lib/libobject.a.
 */

#include <stdio.h>

struct exec {
	unsigned char   a_magic[2];
	unsigned char   a_flags;
	unsigned char   a_cpu;
	unsigned char   a_hdrlen;
	unsigned char   a_unused;
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
	unsigned char   n_sclass;
	unsigned char   n_numaux;
	unsigned short  n_type;
};

#include <out.h>

#ifndef NORCSID
static char rcs_id[] = "$Header$" ;
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
int	output;

char *program ;

char flag ;

/* Output file definitions and such */

#define TEXTSG	0
#define ROMSG	1
#define DATASG	2
#define BSSSG	3
#define LSECT	BSSSG+1
#define NSECT	LSECT+1

main(argc, argv)
	int	argc;
	char	*argv[];
{
	register int		nsect;
	long			magic ;
	long			textsize ;
	long			datasize ;
	long			bsssize;
	long			symstart;
	register struct exec *e = &exec;

	output = 1;
	program= argv[0] ;
	if ( argc>1 && argv[1][0]=='-' ) {
		flag=argv[1][1] ;
		argc-- ; argv++ ;
	}
	switch (argc) {
	case 1: rd_fdopen(0);
		break;
	case 3:	if ((output = creat(argv[2], 0644)) < 0) {
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
	while (outsect[TEXTSG].os_size % outsect[TEXTSG].os_lign) 
		outsect[TEXTSG].os_size++;
	while (outsect[ROMSG].os_size % outsect[ROMSG].os_lign) 
		outsect[ROMSG].os_size++;
	while (outsect[DATASG].os_size % outsect[DATASG].os_lign) 
		outsect[DATASG].os_size++;
	/* A few checks */
	if ( outsect[TEXTSG].os_base != ENTRY)
		fatal("text must start at %d not at 0x%lx\n", ENTRY,
			outsect[TEXTSG].os_base) ;
	if ( outsect[BSSSG].os_flen != 0 )
		fatal("bss space contains initialized data\n") ;
	if ( outsect[BSSSG].os_base != outsect[DATASG].os_base+
					outsect[DATASG].os_size )
		fatal("bss segment must follow data segment\n") ;

	e->a_magic[0] = 01;
	e->a_magic[1] = 03;
	e->a_cpu = 04;
	e->a_hdrlen = 32;
	e->a_text = outsect[TEXTSG].os_size;
	e->a_data = outsect[ROMSG].os_size + outsect[DATASG].os_size;
	e->a_bss = outsect[BSSSG].os_size;
	e->a_entry = outsect[TEXTSG].os_base;
	e->a_syms = (long) outhead.oh_nname * sizeof (struct nlist);
	e->a_misc = 0x10000;
	if ( outsect[ROMSG].os_base == 0x0 ) {
		/* Separate I/D */
		e->a_flags = 0x20;
		if ( outsect[DATASG].os_base != outsect[ROMSG].os_base+
						outsect[ROMSG].os_size )
			fatal("data segment must follow rom\n") ;
	} else {
		e->a_flags = 0x10;
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
	write(output, (char *) e, 32);
	emits(&outsect[TEXTSG]) ;
	emits(&outsect[ROMSG]) ;
	emits(&outsect[DATASG]) ;
	emit_symtab();
	if ( outputfile_created ) chmod(argv[2],0755);
	return 0;
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
		write(output, buffer, blk);
		n -= blk;
	}
	if ((n = section->os_size - section->os_flen) > 0) {
		for (blk = BUFSIZ - 1; blk >= 0; blk--) {
			buffer[blk] = 0;
		}
		while (n > 0) {
			blk = n > BUFSIZ ? BUFSIZ : n;
			write(output, buffer, blk);
			n -= blk;
		}
	}
}

emit_symtab()
{
	struct outname ACK_name;  /* symbol table entry in ACK format */
	struct nlist IX_name;	  /* symbol table entry in PC/IX format */
	register unsigned short i;

	char x;
	long y;
	extern char *malloc();
	char *chars;
	long l;
	long off = OFF_CHAR(outhead);
	int j;
	char *p;

	chars = malloc(outhead.oh_nchar);
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
		write(output, (char *) &IX_name, sizeof(struct nlist));
	}
}

/* VARARGS1 */
fatal(s, a1, a2)
	char	*s;
{
	fprintf(stderr,"%s: ",program) ;
	fprintf(stderr, s, a1, a2);
	if (outputfile_created)
		unlink(output_file);
	exit(1);
}

rd_fatal()
{
	fatal("read error\n");
}
