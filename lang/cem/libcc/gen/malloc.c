/* replace undef by define */
#undef	 DEBUG		/* check assertions */
#undef	 SLOWDEBUG	/* some extra test loops (requires DEBUG) */

#ifdef DEBUG
#define	ASSERT(b)	if (!(b)) assert_failed();
#else
#define	ASSERT(b)	/* empty */
#endif

#if _EM_WSIZE == _EM_PSIZE
#define	ptrint		int
#else
#define	ptrint		long
#endif

#if _EM_PSIZE == 2
#define BRKSIZE		1024
#else
#define BRKSIZE		4096
#endif
#define	PTRSIZE		sizeof(char *)
#define Align(x,a)	(((x) + (a - 1)) & ~(ptrint)(a - 1))
#define NextSlot(p)	(* (char **) ((p) - PTRSIZE))
#define NextFree(p)	(* (char **) (p))

/*
 * A short explanation of the data structure and algorithms.
 * An area returned by malloc() is called a slot. Each slot
 * contains the number of bytes requested, but preceeded by
 * an extra pointer to the next the slot in memory.
 * '_bottom' and '_top' point to the first/last slot.
 * More memory is asked for using brk() and appended to top.
 * The list of free slots is maintained to keep malloc() fast.
 * '_empty' points the the first free slot. Free slots are
 * linked together by a pointer at the start of the
 * user visable part, so just after the next-slot pointer.
 * Free slots are merged together by free().
 */

extern char *sbrk(), *brk();
static char *_bottom, *_top, *_empty;

static grow(len)
unsigned len;
{
  register char *p;

  ASSERT(NextSlot(_top) == 0);
  p = (char *) Align((ptrint)_top + len, BRKSIZE);
  if (p < _top || brk(p) != 0)
	return(0);
  NextSlot(_top) = p;
  NextSlot(p) = 0;
  free(_top);
  _top = p;
  return(1);
}

char *malloc(size)
unsigned size;
{
  register char *prev, *p, *next, *new;
  register unsigned len, ntries;

  if (size == 0)
	size = PTRSIZE;		/* avoid slots less that 2*PTRSIZE */
  for (ntries = 0; ntries < 2; ntries++) {
	len = Align(size, PTRSIZE) + PTRSIZE;
	if (_bottom == 0) {
		p = sbrk(2 * PTRSIZE);
		p = (char *) Align((ptrint)p, PTRSIZE);
		p += PTRSIZE;
		_top = _bottom = p;
		NextSlot(p) = 0;
	}
#ifdef SLOWDEBUG
	for (p = _bottom; (next = NextSlot(p)) != 0; p = next)
		ASSERT(next > p);
	ASSERT(p == _top);
#endif
	for (prev = 0, p = _empty; p != 0; prev = p, p = NextFree(p)) {
		next = NextSlot(p);
		new = p + len;
		if (new > next)
			continue;		/* too small */
		if (new + PTRSIZE < next) {	/* too big, so split */
			/* + PTRSIZE avoids tiny slots on free list */
			NextSlot(new) = next;
			NextSlot(p) = new;
			NextFree(new) = NextFree(p);
			NextFree(p) = new;
		}
		if (prev)
			NextFree(prev) = NextFree(p);
		else
			_empty = NextFree(p);
		return(p);
	}
	if (grow(len) == 0)
		break;
  }
  ASSERT(ntries != 2);
  return(0);
}

char *realloc(old, size)
char *old;
unsigned size;
{
  register char *prev, *p, *next, *new;
  register unsigned len, n;

  len = Align(size, PTRSIZE) + PTRSIZE;
  next = NextSlot(old);
  n = (int)(next - old);			/* old length */
  /*
   * extend old if there is any free space just behind it
   */
  for (prev = 0, p = _empty; p != 0; prev = p, p = NextFree(p)) {
	if (p > next)
		break;
	if (p == next) {	/* 'next' is a free slot: merge */
		NextSlot(old) = NextSlot(p);
		if (prev)
			NextFree(prev) = NextFree(p);
		else
			_empty = NextFree(p);
		next = NextSlot(old);
		break;
	}
  }
  new = old + len;
  /*
   * Can we use the old, possibly extended slot?
   */
  if (new <= next) {				/* it does fit */
	if (new + PTRSIZE < next) {		/* too big, so split */
		/* + PTRSIZE avoids tiny slots on free list */
		NextSlot(new) = next;
		NextSlot(old) = new;
		free(new);
	}
	return(old);
  }
  if ((new = malloc(size)) == 0)		/* it didn't fit */
	return(0);
  bcopy(old, new, n);				/* n < size */
  free(old);
  return(new);
}

free(p)
char *p;
{
  register char *prev, *next;

  ASSERT(NextSlot(p) > p);
  for (prev = 0, next = _empty; next != 0; prev = next, next = NextFree(next))
	if (p < next)
		break;
  NextFree(p) = next;
  if (prev)
	NextFree(prev) = p;
  else
	_empty = p;
  if (next) {
	ASSERT(NextSlot(p) <= next);
	if (NextSlot(p) == next) {		/* merge p and next */
		NextSlot(p) = NextSlot(next);
		NextFree(p) = NextFree(next);
	}
  }
  if (prev) {
	ASSERT(NextSlot(prev) <= p);
	if (NextSlot(prev) == p) {		/* merge prev and p */
		NextSlot(prev) = NextSlot(p);
		NextFree(prev) = NextFree(p);
	}
  }
}

#ifdef DEBUG
static assert_failed()
{
	write(2, "assert failed in lib/malloc.c\n", 30);
	abort();
}
#endif
