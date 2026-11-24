/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef lint
static char rcsid[] = "$Id$";
#endif

/*
 * Memory manager. Memory is divided into NMEMS pieces. There is a struct
 * for each piece telling where it is, how many bytes are used, and how many
 * are left. Each piece is its own malloc() block; if a request for core
 * doesn't fit in the left bytes, the piece is grown with realloc().
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <out.h>
#include "const.h"
#include "debug.h"
#include "memory.h"
#include "object.h"
#include "sym.h"
#include "finish.h"
#include "write.h"
#include "scan.h"

static void free_saved_moduls(void);

struct memory mems[NMEMS];

bool incore = true; /* true while everything can be kept in core. */
ind_t core_position = (ind_t)0; /* Index of current module. */

static size_t modl_initial_size;
static bool frozen = false; /* true after freeze_core(). */

/*
 * Initialize some pieces of core. We hope that this will be our last
 * real allocation, meaning we've made the right choices.
 */
void init_core(void)
{
	struct memory* failed_mem = NULL;
	struct memory* mem;
	bool string_area;

#include "mach.h"
	modl_initial_size = mems[ALLOMODL].mem_left;

	for (mem = mems; mem < &mems[NMEMS]; mem++)
	{
		string_area = mem == &mems[ALLOLCHR] || mem == &mems[ALLOGCHR];
		/* String areas need at least one byte. */
		if (string_area && mem->mem_left == 0)
			mem->mem_left++;
		/* Don't malloc() size zero. */
		if (mem->mem_left > 0)
		{
			mem->mem_base = malloc(mem->mem_left);
			if (mem->mem_base == NULL)
			{
				failed_mem = mem;
				break;
			}
		}
		mem->mem_full = 0;
		if (string_area)
		{
			mem->mem_left--;
			mem->mem_full++;
		}
	}
	if (failed_mem != NULL)
	{
		incore = false; /* In core strategy failed. */
		/* Undo allocations. */
		for (mem = mems; mem != failed_mem; mem++)
			free(mem->mem_base);
		/* Allocate only the string areas. */
		for (mem = mems; mem < &mems[NMEMS]; mem++)
		{
			if (mem == &mems[ALLOLCHR] || mem == &mems[ALLOGCHR])
			{
				mem->mem_base = malloc(1);
				if (mem->mem_base == NULL)
					fatal("no core at all");
				mem->mem_left = 0;
				mem->mem_full = 1;
			}
			else
			{
				mem->mem_base = NULL;
				mem->mem_left = mem->mem_full = 0;
			}
		}
	}
}

/*
 * Grow `piece' by `incr' bytes with realloc(); the other pieces stay where
 * they are. Grow by as much as possible, if "incr" fails.
 */
static ind_t move_up(int piece, ind_t incr)
{
	size_t oldsize = mems[piece].mem_full + mems[piece].mem_left;
	size_t newsize;
	char* newbase;

	if (frozen)
		return 0; /* Can't realloc() frozen core. */

	/* We realloc() this piece without moving the other pieces. */
	while (incr > 0)
	{
		newsize = oldsize + incr;
		if (newsize > oldsize)
		{
			newbase = realloc(mems[piece].mem_base, newsize);
			if (newbase != NULL)
			{
				mems[piece].mem_base = newbase;
				mems[piece].mem_left += incr;
				return incr;
			}
		}
		incr -= INCRSIZE < incr ? INCRSIZE : incr;
	}
	return 0;
}

/*
 * This routine is called if `piece' needs `incr' bytes and the system won't
 * give them. We shrink all other pieces to what they use, then try to grow
 * `piece' again, and return whether we have enough bytes.
 */
static bool compact(int piece, ind_t incr, int flag)
#define NORMAL 0
#define FREEZE 1
#define FORCED 2
{
	struct memory* mem;
	size_t newsize, oldsize;
	char* newbase;

	if (frozen)
		return incr == 0; /* Can't realloc() frozen core. */
	/*
	 * We realloc() to shrink most pieces.
	 * We can't control how realloc() moves the pieces.
	 */
	for (mem = mems; mem < &mems[NMEMS]; mem++)
	{
		if (mem == &mems[piece])
			continue;
		if (flag == FREEZE && mem == &mems[ALLOMODL])
			continue;
		if (mem->mem_full == 0)
		{
			/* Don't try to realloc() to size zero. */
			free(mem->mem_base);
			mem->mem_base = NULL;
			mem->mem_left = 0;
		}
		else
		{
			newbase = realloc(mem->mem_base, mem->mem_full);
			if (newbase != NULL)
			{
				mem->mem_base = newbase;
				mem->mem_left = 0;
			}
		}
	}
	/*
	 * After FREEZE, we must be able to grow ALLOMODL without
	 * moving it.  The old allocator allowed ALLOMODL to grow in
	 * the brk area, but we can't call realloc() later, so we must
	 * leave some extra space in ALLOMODL now.
	 */
	if (flag == FREEZE)
	{
		mem = &mems[ALLOMODL];
		oldsize = mem->mem_full + mem->mem_left;
		newsize = mem->mem_full + modl_initial_size / 2;
		/* Don't shrink ALLOMODL. */
		while (newsize > oldsize)
		{
			newbase = realloc(mem->mem_base, newsize);
			if (newbase != NULL)
			{
				mem->mem_base = newbase;
				mem->mem_left = newsize - mem->mem_full;
				break;
			}
			newsize -= INCRSIZE < newsize ? INCRSIZE : newsize;
		}
		frozen = true; /* Prevent later realloc(). */
	}
	/* Now grow our piece. */
	if (incr == 0)
		return true;
	mem = &mems[piece];
	oldsize = mem->mem_full + mem->mem_left;
	newsize = oldsize + incr;
	if (newsize < mem->mem_full)
		return false; /* The size overflowed. */
	newbase = realloc(mem->mem_base, newsize);
	if (newbase == NULL)
		return false;
	mem->mem_base = newbase;
	mem->mem_left += incr;
	return true;

}


static int alloctype = NORMAL;

/*
 * Add `size' bytes to the bytes already allocated for `piece'. If it has no
 * free bytes left, ask them from memory or, if that fails, from the free
 * bytes of other pieces. The offset of the new area is returned. No matter
 * how many times the area is moved, because of another allocate, this offset
 * remains valid.
 */
ind_t alloc(int piece, size_t size)
{
	ind_t incr = 0;
	ind_t left = mems[piece].mem_left;
	ind_t full = mems[piece].mem_full;

	assert(passnumber == FIRST || (!incore && piece == ALLOMODL));
	if (size == 0)
		return full;
	if (size != (ind_t)size)
		return BADOFF;
	switch (piece)
	{
		case ALLOMODL:
		case ALLORANL:
			size = int_align(size);
			if (size == 0)
				return BADOFF;
	}

	if (size > left)
	{
		incr = ((size - left + (INCRSIZE - 1)) / INCRSIZE) * INCRSIZE;
		if (incr == 0)
			return BADOFF;
	}

	if (incr == 0 || (incr < left + full && move_up(piece, left + full) >= incr)
	    || move_up(piece, incr) == incr || compact(piece, size, alloctype))
	{
		mems[piece].mem_full += size;
		mems[piece].mem_left -= size;
		return full;
	}
	else
	{
		incore = false;
		return BADOFF;
	}
}

/*
 * Same as alloc() but for a piece which really needs it. If the first
 * attempt fails, release the space occupied by other pieces and try again.
 */
ind_t hard_alloc(int piece, size_t size)
{
	ind_t ret;
	int i;

	if (size != (ind_t)size)
		return BADOFF;
	if ((ret = alloc(piece, size)) != BADOFF)
	{
		return ret;
	}

	/*
	 * Deallocate what we don't need.
	 */
	for (i = 0; i < NMEMS; i++)
	{
		switch (i)
		{
			case ALLOGLOB:
			case ALLOGCHR:
			case ALLOSYMB:
			case ALLOARCH:
			case ALLOMODL:
			case ALLORANL:
				break; /* Do not try to deallocate this. */
			default:
				dealloc(i);
				break;
		}
	}
	free_saved_moduls();

	if ((ret = alloc(piece, size)) != BADOFF)
	{
		return ret;
	}

	alloctype = FORCED;
	ret = alloc(piece, size);
	alloctype = NORMAL;
	return ret;
}

/*
 * We don't need the previous modules, so we put the current module
 * at the start of the piece allocated for module contents, thereby
 * overwriting the saved modules, and release its space.
 */
static void free_saved_moduls(void)
{
	ind_t size;
	char *old, *new;
	struct memory* mem = &mems[ALLOMODL];

	size = mem->mem_full - core_position;
	new = mem->mem_base;
	old = new + core_position;
	while (size--)
		*new ++ = *old++;
	mem->mem_full -= core_position;
	mem->mem_left += core_position;
	core_position = (ind_t)0;
}

/*
 * The piece of memory with index `piece' is no longer needed.
 * We take care that it can be used by compact() later, if needed.
 */
void dealloc(int piece)
{
	/*
	 * Some pieces need their memory throughout the program.
	 */
	assert(piece != ALLOGLOB);
	assert(piece != ALLOGCHR);
	assert(piece != ALLOSYMB);
	assert(piece != ALLOARCH);
	mems[piece].mem_left += mems[piece].mem_full;
	mems[piece].mem_full = (ind_t)0;
}

char* core_alloc(int piece, size_t size)
{
	ind_t off;

	if ((off = alloc(piece, size)) == BADOFF)
		return (char*)0;
	return address(piece, off);
}

void core_free(int piece, char* p)
{
	char* q = address(piece, mems[piece].mem_full);

	assert(p < q);
	mems[piece].mem_full -= (ind_t)q - (ind_t)p;
	mems[piece].mem_left += (ind_t)q - (ind_t)p;
}

/*
 * Reset index into piece of memory for modules and
 * take care that the allocated pieces will not be moved.
 */
void freeze_core(void)
{
	int i;

	core_position = (ind_t)0;

	if (incore)
		return;

	for (i = 0; i < NMEMS; i++)
	{
		switch (i)
		{
			case ALLOGLOB:
			case ALLOGCHR:
			case ALLOSYMB:
			case ALLOARCH:
				break; /* Do not try to deallocate this. */
			default:
				dealloc(i);
				break;
		}
	}
	compact(NMEMS - 1, (ind_t)0, FREEZE);
}

/* ------------------------------------------------------------------------- */

/*
 * To transform the various pieces of the output in core to the file format,
 * we must order the bytes in the unsigned shorts and longs as ACK prescribes.
 */
void write_bytes(void)
{
	unsigned short nsect;
	long offchar;
	struct memory* mem;
	extern long NLChars, NGChars;
	extern int flagword;
	extern struct outhead outhead;
	extern struct outsect outsect[];
	extern char* outputname;
	int sectionno = 0;

	nsect = outhead.oh_nsect;
	offchar = OFF_CHAR(outhead);

	/*
	 * We allocated two areas: one for local and one for global names.
	 * Also, we used another kind of on_foff than on file.
	 * At the end of the global area we have put the section names.
	 */
	if (!(flagword & SFLAG))
	{
		do_crs((struct outname*)mems[ALLOLOCL].mem_base, NLocals);
		namecpy((struct outname*)mems[ALLOLOCL].mem_base, NLocals, offchar);
		namecpy((struct outname*)mems[ALLOGLOB].mem_base, NGlobals + nsect, offchar + NLChars);
	}
	/*
	 * These pieces must always be written.
	 */
	wr_ohead(&outhead);
	wr_sect(outsect, nsect);
	for (mem = &mems[ALLOEMIT]; mem < &mems[ALLORELO]; mem++)
		wrt_emit(mem->mem_base, sectionno++, mem->mem_full);
	/*
	 * The rest depends on the flags.
	 */
	if (flagword & (RFLAG | CFLAG))
		wr_relo((struct outrelo*)mems[ALLORELO].mem_base, outhead.oh_nrelo);
	if (!(flagword & SFLAG))
	{
		wr_name((struct outname*)mems[ALLOLOCL].mem_base, NLocals);
		wr_name((struct outname*)mems[ALLOGLOB].mem_base, NGlobals + nsect);
		wr_string(mems[ALLOLCHR].mem_base + 1, (long)NLChars);
		wr_string(mems[ALLOGCHR].mem_base + 1, (long)NGChars);
#ifdef SYMDBUG
		wr_dbug(mems[ALLODBUG].mem_base, mems[ALLODBUG].mem_full);
#endif /* SYMDBUG */
	}
}

void namecpy(struct outname* name, unsigned nname, long offchar)
{
	while (nname--)
	{
		if (name->on_foff)
			name->on_foff += offchar - 1;
		name++;
	}
}
