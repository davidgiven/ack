/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Convert ACK a.out file to ST-Minix object format.
 */

#include <stdlib.h>
#include <stdio.h>
#include <out.h>
#include <assert.h>
#include <stdlib.h>

struct outhead	outhead;
struct outsect	outsect[S_MAX];

#define TEXTSG	0
#define ROMSG	1
#define DATASG	2
#define BSSSG	3
#define LSECT	BSSSG+1
#define NSECT	LSECT+1

char	*output_file;
int	outputfile_created;

char *program ;

/* Output file definitions and such */

int		output;

int unresolved;
long	textsize ; 
long	datasize ;
long	bsssize;

char *chmemstr;

minixhead()
{
	long		mh[8];
	long		stack;
	long		chmem();
	int		i;

	mh[0] = 0x04100301L;
	mh[1] = 0x00000020L;
	mh[2] = textsize;
	mh[3] = datasize;
	mh[4] = bsssize;
	mh[5] = 0;
	stack = 0x00010000L - (mh[3] + mh[4]);
	if ((mh[0] & 0x00200000L) == 0)		/* not SEPARATE */
		stack -= mh[2];
	while (stack < 0)
		stack += 0x00010000L;
	if (chmemstr)
		stack = chmem(chmemstr, stack);
	printf("%ld bytes assigned to stack+malloc area\n", stack);
	mh[6] = stack + (mh[3] + mh[4]);
	if ((mh[0] & 0x00200000L) == 0)		/* not SEPARATE */
		mh[6] += mh[2];
	mh[7] = 0;
	for (i = 0; i < 8; i++) {
		cvlong(&mh[i]);
	}

	if (write(output, (char *) mh, sizeof(mh)) != sizeof(mh))
		fatal("write error\n");
}

long align(a,b)
	long a,b;
{
	if (b == 0) return a;
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

	program= argv[0] ;
	if (argc > 1) {
		switch (argv[1][0]) { 
		case '-':
		case '+':
		case '=':
			chmemstr = argv[1];
			argc--;
			argv++;
		}
	}
	switch (argc) {
	case 3:	if ((output = creat(argv[2], 0644)) < 0)
			fatal("Can't write %s.\n", argv[2]);
		output_file = argv[2];
		outputfile_created = 1;
		if (! rd_open(argv[1]))
			fatal("Can't read %s.\n", argv[1]);
		break;
	default:fatal("Usage: %s [+-= amount] <ACK object> <ST-MINIX object>.\n", argv[0]);
	}
	rd_ohead(&outhead);
	if (BADMAGIC(outhead))
		fatal("Not an ack object file.\n");
	if (outhead.oh_flags & HF_LINK) {
		unresolved++;
		fatal("Contains unresolved references.\n");
	}
	if ( outhead.oh_nsect!=LSECT && outhead.oh_nsect!=NSECT )
		fatal("Input file must have %d sections, not %ld\n",
			NSECT,outhead.oh_nsect) ;
	rd_sect(outsect, outhead.oh_nsect);
	/* A few checks */
	if ( outsect[BSSSG].os_flen != 0 )
		fatal("bss space contains initialized data\n") ;
	if (! follows(&outsect[BSSSG], &outsect[DATASG]))
		fatal("bss segment must follow data segment\n") ;
	textsize= (outsect[DATASG].os_base - outsect[TEXTSG].os_base);
	if (! follows(&outsect[ROMSG],&outsect[TEXTSG]))
		fatal("rom segment must follow text\n") ;
	if (! follows(&outsect[DATASG],&outsect[ROMSG]))
		fatal("data segment must follow rom\n") ;
	outsect[TEXTSG].os_size = outsect[ROMSG].os_base - outsect[TEXTSG].os_base;
	outsect[ROMSG].os_size = outsect[DATASG].os_base - outsect[ROMSG].os_base;
	outsect[DATASG].os_size = outsect[BSSSG].os_base - outsect[DATASG].os_base;
	datasize= outsect[DATASG].os_size ;
	bsssize = outsect[BSSSG].os_size;
	if ( outhead.oh_nsect==NSECT ) {
		if (! follows(&outsect[LSECT],&outsect[BSSSG]))
			fatal("end segment must follow bss\n") ;
		if ( outsect[LSECT].os_size != 0 )
			fatal("end segment must be empty\n") ;
	}

	minixhead();
	emits(&outsect[TEXTSG]) ;
	emits(&outsect[ROMSG]) ;
	emits(&outsect[DATASG]) ;
	emit_relo();
	if ( outputfile_created) chmod(argv[2],0755);
	return 0;
}

/*
 * Transfer the emitted byted from one file to another.
 */
emits(section) struct outsect *section ; {
	char		*p;
	long sz = section->os_flen;

	rd_outsect(section - outsect);
	while (sz) {
		unsigned int i = (sz >= 0x4000 ? 0x4000 : sz);
		if (!(p = malloc(0x4000))) {
			fatal("No memory.\n");
		}
		rd_emit(p, (long) i);
		if (write(output, p, (int)i) < i) {
			fatal("write error.\n");
		}
		free(p);
		sz -= i;
	}

	sz = section->os_size - section->os_flen;
	if (sz) {
		if (!(p = calloc(0x4000, 1))) {
			fatal("No memory.\n");
		}
		while (sz) {
			unsigned int i = (sz >= 0x4000 ? 0x4000 : sz);
			if (write(output, p, (int)i) < i) {
				fatal("write error.\n");
			}
			sz -= i;
		}
		free(p);
	}
}

int
compare(a,b)
	register struct outrelo *a, *b;
{
	if (a->or_sect < b->or_sect) return -1;
	if (a->or_sect > b->or_sect) return 1;
	if (a->or_addr < b->or_addr) return -1;
	if (a->or_addr > b->or_addr) return 1;
	return 0;
}

emit_relo()
{
	struct outrelo *ACKrelo;
	register struct outrelo *ap;
	unsigned int cnt = outhead.oh_nrelo;
	long last, curr, base;
	int sect;
	char *bp;
	register char *b;

	ACKrelo = ap = (struct outrelo *) calloc(cnt, sizeof(struct outrelo));
	bp = b = malloc(4 + cnt);
	if (!ap || !bp) {
		fatal("No memory.\n");
	}
	rd_relo(ap, cnt);
	qsort((char *) ap, (int) cnt, sizeof(struct outrelo), compare);
	/*
         * read relocation, modify to GEMDOS format, and write.
         * Only longs can be relocated.
         *
         * The GEMDOS format starts with a long L: the offset to the
         * beginning of text for the first long to be relocated.
         * If L==0 then no relocations have to be made.
         *
         * The long is followed by zero or more bytes. Each byte B is
         * processed separately, in one of the following ways:
         *
         * B==0:
         *      end of relocation
         * B==1:
         *      no relocation, but add 254 to the current offset
         * B==0bWWWWWWW0:
         *      B is added to the current offset and the long addressed
         *      is relocated. Note that 00000010 means 1 word distance.
         * B==0bXXXXXXX1:
         *      illegal
         */

	last = 0;
	curr = 0;
	for (sect = S_MIN; sect <= S_MIN+2; sect++) {
		base = outsect[sect-S_MIN].os_base;
		for (;cnt > 0 && ap->or_sect == sect; ap++, cnt--) {
			if (ap->or_type & RELPC ||
			    ap->or_nami == outhead.oh_nname) {
				continue;
			}
			assert(ap->or_type & RELO4);
			curr = base + ap->or_addr;
			if (last == 0) {
				last = curr;
				cvlong(&curr);
				*((long *) b) = curr;
				b += 4;
			}
			else {
				while (curr - last > 255) {
					*b++ = 1;
					last += 254;
				}
				*b++ = curr - last;
				last = curr;
			}
		}
		assert(cnt == 0 || ap->or_sect > sect);
	}
	assert(cnt == 0);
	if (cnt = (b - bp)) {
		*b++ = '\0';
		write(output, bp, (int) cnt+1);
	}
	else write(output, "\0\0\0", 4);
	free((char *) ACKrelo);
	free(bp);
}

long
chmem(str, old)
char *str;
long old;
{
        register long num, new;
        long atol();

        num = atol(str+1);
        if (num == 0)
                fatal("bad chmem amount %s\n", str+1);
        switch (str[0]) {
        case '-':
                new = old - num; break;
        case '+':
                new = old + num; break;
        case '=':
                new = num; break;
        }
        return(new);
}

cvlong(l)
	long *l;
{
	long x = *l;
	char *p = (char *) l;

	*p++ = x >> 24;
	*p++ = x >> 16;
	*p++ = x >> 8;
	*p = x;
}

/* VARARGS1 */
fatal(s, a1, a2)
	char	*s;
{
	fprintf(stderr,"%s: ",program) ;
	fprintf(stderr, s, a1, a2);
	if (outputfile_created)
		unlink(output_file);
	exit(-1);
}

rd_fatal() { fatal("read error.\n"); }
