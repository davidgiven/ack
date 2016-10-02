/*
 * This emulates brk() and sbrk() using mmap() and mprotect().
 *
 * We reserve exactly SEGMENTSZ bytes of address space by calling
 * mmap() with PROT_NONE.  Then we allocate pages in our segment by
 * calling mprotect() with PROT_READ|PROT_WRITE.
 *
 * This emulation can't resize its segment.  If SEGMENTSZ is too big,
 * then programs might run out of address space for other mappings.
 */
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>

/*
 * PAGESZ must be correct for this system!
 * SEGMENTSZ must be a multiple of PAGESZ.
 */
#define PAGESZ 0x1000		/* page size for i386, powerpc */
#define SEGMENTSZ 0x20000000

static char *segment;
static char *cbreak;		/* current break */

static void brk_init(void)
{
	/*
	 * Try exactly once to reserve our segment.  If we fail, then
	 * segment == MAP_FAILED and we never try again.
	 */
	if (segment == NULL) {
		segment = mmap(NULL, SEGMENTSZ, PROT_NONE,
		    MAP_PRIVATE|MAP_ANON, -1, 0);
		cbreak = segment;
	}
}

static int brk1(char *nbreak)
{
	size_t sz;
	char *new, *old;

	sz = (segment == MAP_FAILED) ? 0 : SEGMENTSZ;
	if (nbreak < segment || nbreak > segment + sz) {
		errno = ENOMEM;
		return -1;
	}

	/* Round up to page size. */
	old = (char *)(((size_t)cbreak + (PAGESZ-1)) & ~(PAGESZ-1));
	new = (char *)(((size_t)nbreak + (PAGESZ-1)) & ~(PAGESZ-1));

	if (new > old) {
		/* Allocate pages by unprotecting them. */
		if (mprotect(old, new - old, PROT_READ|PROT_WRITE) < 0) {
			errno = ENOMEM;
			return -1;
		}
	} else if (new < old) {
		/*
		 * Free pages by using MAP_FIXED to replace the
		 * mapping.  Ignore errors.
		 */
		mmap(new, old - new, PROT_NONE,
		    MAP_PRIVATE|MAP_ANON|MAP_FIXED, -1, 0);
	}
	cbreak = nbreak;
	return 0;
}

int brk(void *addr)
{
	brk_init();
	return brk1(addr);
}

void *sbrk(int incr)
{
	char *base, *nbreak;

	brk_init();
	base = cbreak;
	nbreak = base + incr;

	/* Did base + incr overflow? */
	if ((incr < 0 && nbreak > base) ||
	    (incr > 0 && nbreak < base)) {
		errno = ENOMEM;
		return (void*)-1;
	}

	if (brk1(nbreak) < 0)
		return (void*)-1;
	return base;
}
