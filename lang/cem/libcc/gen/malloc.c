/* $Header$ */

#define CLICK_SIZE	4096
#if EM_WSIZE == EM_PSIZE
typedef unsigned int vir_bytes;
#else
typedef long vir_bytes;
#endif
extern bcopy();

#define ALIGN(x, a)	(((x) + (a - 1)) & ~(a - 1))
#define BUSY		1
#define NEXT(p)		(* (char **) (p))

#ifdef pdp
#define BUGFIX	64	/* cannot set break in top 64 bytes */
#else
#define BUGFIX	0
#endif

extern char *sbrk(), *brk();
static char *bottom, *top;

static grow(len)
unsigned len;
{
  register char *p;
  register int click = CLICK_SIZE;

  p = sbrk(0);
  len += (char *) ALIGN((vir_bytes) p, sizeof(char *)) - p;
  while (click >= 4) {
  	unsigned len1 = ALIGN((vir_bytes) p + len + sizeof(char *), click) - (vir_bytes) p;
	char *p1 = p;
  	if (p + len1 + BUGFIX < p || (p1 = sbrk(len1)) == (char *) -1) {
		click >>= 1;
		continue;
	}
	p = p1;
  	if (top + sizeof(char *) != p) {
		/* someone else has done an sbrk */
		NEXT(top) = (char *) ((vir_bytes) p | BUSY);
  	} else {
		for (p = bottom; NEXT(p) != 0; p = (char *) (* (vir_bytes *) p & ~BUSY))
			;
	}
	top = p + len1 - sizeof(char *);
	NEXT(p) = top;
	NEXT(top) = 0;
	return 1;
  }
  return 0;
}

char *malloc(size)
unsigned size;
{
  register char *p, *next, *new;
  register unsigned len = ALIGN(size, sizeof(char *)) + sizeof(char *);

  if ((p = bottom) == 0) {
	p = sbrk(sizeof(char *));
	sbrk((char *) ALIGN((vir_bytes) p, sizeof(char *)) - p);
	p = (char *) ALIGN((vir_bytes) p, sizeof(char *));
	top = bottom = p;
	NEXT(p) = 0;
  }
  while ((next = NEXT(p)) != 0)
	if ((vir_bytes) next & BUSY)			/* already in use */
		p = (char *) ((vir_bytes) next & ~BUSY);
	else {
		while ((new = NEXT(next)) != 0 && !((vir_bytes) new & BUSY))
			next = new;
		if (next - p >= len) {			/* fits */
			if ((new = p + len) < next)	/* too big */
				NEXT(new) = next;
			NEXT(p) = (char *) ((vir_bytes) new | BUSY);
			return(p + sizeof(char *));
		}
		p = next;
	}
  return grow(len) ? malloc(size) : 0;
}

char *realloc(old, size)
char *old;
unsigned size;
{
  register char *p = old - sizeof(char *), *next, *new;
  register unsigned len = ALIGN(size, sizeof(char *)) + sizeof(char *), n;

  next = (char *) (* (vir_bytes *) p & ~BUSY);
  n = next - old;					/* old size */
  while ((new = NEXT(next)) != 0 && !((vir_bytes) new & BUSY))
	next = new;
  if (next - p >= len) {				/* does it still fit */
	if ((new = p + len) < next) {			/* even too big */
		NEXT(new) = next;
		NEXT(p) = (char *) ((vir_bytes) new | BUSY);
	}
	else
		NEXT(p) = (char *) ((vir_bytes) next | BUSY);
	return(old);
  }
  if ((new = malloc(size)) == 0)			/* it didn't fit */
	return(0);
  bcopy(old, new, n);					/* n < size */
  * (vir_bytes *) p &= ~BUSY;
  return(new);
}

free(p)
char *p;
{
  * (vir_bytes *) (p - sizeof(char *)) &= ~BUSY;
}
