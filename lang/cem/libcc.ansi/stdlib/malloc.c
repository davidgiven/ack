/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>

#define	CLICK_SIZE	4096

#if	EM_WSIZE == EM_PSIZE
typedef unsigned int vir_bytes;
#else
typedef long vir_bytes;
#endif


#define ALIGN(x, a)	(((x) + (a - 1)) & ~(a - 1))
#define BUSY		1
#define NEXT(p)		(* (void **) (p))

#ifdef pdp
#define BUGFIX	64	/* cannot set break in top 64 bytes */
#else
#define BUGFIX	0
#endif

extern void bcopy(void *, void *, size_t);
extern void *sbrk(unsigned int);
extern void *brk(unsigned int);
static void *bottom, *top;

static int
grow(size_t len)
{
	register void *p;
	register int click = CLICK_SIZE;

	p = sbrk(0);
	len += (void *) ALIGN((vir_bytes) p, sizeof(void *)) - p;
	while (click >= 4) {
		size_t len1 = ALIGN((vir_bytes)p+len+sizeof(char *),click)-(vir_bytes)p;
		void *p1 = p;

		if (p + len1 + BUGFIX < p || (p1 = sbrk(len1)) == (void *) -1) {
			click >>= 1;
			continue;
		}
		p = p1;
		if (top + sizeof(void *) != p) {
			/* someone else has done an sbrk */
			NEXT(top) = (void *) ((vir_bytes) p | BUSY);
		} else {
			for (p = bottom; NEXT(p) != NULL; p = (void *) (* (vir_bytes *) p & ~BUSY))
				;
		}
		top = p + len1 - sizeof(void *);
		NEXT(p) = top;
		NEXT(top) = NULL;
		return 1;
	}
	return 0;
}

void *
malloc(size_t size)
{
	register void *p, *next, *new;
	register size_t len = ALIGN(size, sizeof(void *)) + sizeof(void *);

	if ((p = bottom) == NULL) {
		p = sbrk(sizeof(void *));
		sbrk((void *) ALIGN((vir_bytes) p, sizeof(void *)) - p);
		p = (void *) ALIGN((vir_bytes) p, sizeof(void *));
		top = bottom = p;
		NEXT(p) = NULL;
	}
	while ((next = NEXT(p)) != NULL)
		if ((vir_bytes) next & BUSY)		/* already in use */
			p = (void *) ((vir_bytes) next & ~BUSY);
		else {
			while ((new = NEXT(next)) != NULL
				&& !((vir_bytes) new & BUSY))
			    next = new;
			if (next - p >= len) {			/* fits */
				if ((new = p + len) < next)	/* too big */
					NEXT(new) = next;
				NEXT(p) = (void *) ((vir_bytes) new | BUSY);
				return(p + sizeof(void *));
			}
			p = next;
		}
	return grow(len) ? malloc(size) : (void *)NULL;
}

void *
realloc(void *old, size_t size)
{
	register void *p = old - sizeof(void *), *next, *new;
	register size_t len = ALIGN(size, sizeof(void *)) + sizeof(void *), n;

	next = (void *) (* (vir_bytes *) p & ~BUSY);
	n = next - old;					/* old size */
	while ((new = NEXT(next)) != NULL && !((vir_bytes) new & BUSY))
		next = new;
	if (next - p >= len) {				/* does it still fit */
		if ((new = p + len) < next) {		/* even too big */
			NEXT(new) = next;
			NEXT(p) = (void *) ((vir_bytes) new | BUSY);
		}
		else
			NEXT(p) = (void *) ((vir_bytes) next | BUSY);
		return(old);
	}
	if ((new = malloc(size)) == NULL)		/* it didn't fit */
		return((void *)NULL);
	bcopy(old, new, n);				/* n < size */
	* (vir_bytes *) p &= ~BUSY;
	return(new);
}

void
free(void *p)
{
	if (p != (void *)NULL)
		*(vir_bytes *) (p - sizeof(void *)) &= ~BUSY;
}
