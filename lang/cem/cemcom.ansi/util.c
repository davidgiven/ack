/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/* M I S C E L L A N E O U S   U T I L I T I E S */

/* $Id$ */

/*	Code for the allocation and de-allocation of temporary variables,
	allowing re-use.
*/

#include	"parameters.h"
#ifndef	LINT
#include	<em.h>
#else
#include	"l_em.h"
#endif	/* LINT */
#include	<em_arith.h>
#include	<em_reg.h>
#include	<alloc.h>
#include	<em_mes.h>

#include	"util.h"
#include	"sizes.h"
#include	"align.h"
#include	"stack.h"
#include	"Lpars.h"
#include	"def.h"

static struct localvar	*FreeTmps;
#ifdef USE_TMP
static int	loc_id;
#endif /* USE_TMP */

#ifdef PEEPHOLE
#undef REGCOUNT
#define REGCOUNT 1
#endif

extern char options[];

LocalInit()
{
#ifdef USE_TMP
	C_insertpart(loc_id = C_getid());
#endif /* USE_TMP */
}

arith
LocalSpace(sz, al)
	arith sz;
{
	register struct stack_level *stl = local_level;

	stl->sl_max_block = - align(sz - stl->sl_max_block, al);
	return stl->sl_max_block;
}

#define TABSIZ 32
static struct localvar *regs[TABSIZ];

arith
NewLocal(sz, al, regtype, sc)
	arith sz;
{
	register struct localvar *tmp = FreeTmps;
	struct localvar *prev = 0;
	int index;

	while (tmp) {
		if (tmp->t_align >= al &&
		    tmp->t_size >= sz &&
		    tmp->t_sc == sc &&
		    tmp->t_regtype == regtype) {
			if (prev) {
				prev->next = tmp->next;
			}
			else	FreeTmps = tmp->next;
			break;
		}
		prev = tmp;
		tmp = tmp->next;
	}
	if (! tmp) {
		tmp = new_localvar();
		tmp->t_offset = LocalSpace(sz, al);
		tmp->t_align = al;
		tmp->t_size = sz;
		tmp->t_sc = sc;
		tmp->t_regtype = regtype;
		tmp->t_count = REG_DEFAULT;
	}
	index = (int) (tmp->t_offset >> 2) & (TABSIZ - 1);
	tmp->next = regs[index];
	regs[index] = tmp;
	return tmp->t_offset;
}

FreeLocal(off)
	arith off;
{
	int index = (int) (off >> 2) & (TABSIZ - 1);
	register struct localvar *tmp = regs[index];
	struct localvar *prev = 0;

	while (tmp && tmp->t_offset != off) {
		prev = tmp;
		tmp = tmp->next;
	}
	if (tmp) {
		if (prev)	prev->next = tmp->next;
		else		regs[index] = tmp->next;
		tmp->next = FreeTmps;
		FreeTmps = tmp;
	}
}

LocalFinish()
{
	register struct localvar *tmp, *tmp1;
	register int i;

#ifdef USE_TMP
	C_beginpart(loc_id);
#endif
	tmp = FreeTmps;
	while (tmp) {
		tmp1 = tmp;
		if (tmp->t_sc == REGISTER) tmp->t_count += REG_BONUS;
		if (! options['n'] && tmp->t_regtype >= 0) {
			C_ms_reg(tmp->t_offset, tmp->t_size, tmp->t_regtype, tmp->t_count);
		}
		tmp = tmp->next;
		free_localvar(tmp1);
	}
	FreeTmps = 0;
	for (i = 0; i < TABSIZ; i++) {
		tmp = regs[i];
		while (tmp) {
			if (tmp->t_sc == REGISTER) tmp->t_count += REG_BONUS;
			tmp1 = tmp;
			if (! options['n'] && tmp->t_regtype >= 0) {
				C_ms_reg(tmp->t_offset,
					 tmp->t_size,
					 tmp->t_regtype,
					 tmp->t_count);
			}
			tmp = tmp->next;
			free_localvar(tmp1);
		}
		regs[i] = 0;
	}
#ifdef PEEPHOLE
	if (! options['n']) {
		C_mes_begin(ms_reg);
		C_mes_end();
	}
#endif
#ifdef USE_TMP
	C_endpart(loc_id);
#endif
}

void
RegisterAccount(offset, size, regtype, sc)
	arith offset, size;
{
	register struct localvar *p;
	int index;

	if (regtype < 0) return;

	p = new_localvar();
	index = (int) (offset >> 2) & (TABSIZ - 1);
	p->t_offset = offset;
	p->t_regtype = regtype;
	p->t_count = REG_DEFAULT;
	p->t_sc = sc;
	p->t_size = size;
	p->next = regs[index];
	regs[index] = p;
}

static struct localvar *
find_reg(off)
	arith off;
{
	register struct localvar *p = regs[(int)(off >> 2) & (TABSIZ - 1)];

	while (p && p->t_offset != off) p = p->next;
	return p;
}

LoadLocal(off, sz)
	arith off, sz;
{
	register struct localvar *p = find_reg(off);

#ifdef USE_TMP
#ifdef REGCOUNT
	if (p) p->t_count++;
#endif
#endif
	if (p && p->t_size != sz) p->t_regtype = -1;
	if (sz == word_size) C_lol(off);
	else if (sz == dword_size) C_ldl(off);
	else {
		if (p) p->t_regtype = -1;
		C_lal(off);
		C_loi(sz);
	}
}

StoreLocal(off, sz)
	arith off, sz;
{
	register struct localvar *p = find_reg(off);

#ifdef USE_TMP
#ifdef REGCOUNT
	if (p) p->t_count++;
#endif
#endif
	if (p && p->t_size != sz) p->t_regtype = -1;
	if (sz == word_size) C_stl(off);
	else if (sz == dword_size) C_sdl(off);
	else {
		if (p) p->t_regtype = -1;
		C_lal(off);
		C_sti(sz);
	}
}

#ifndef	LINT
AddrLocal(off)
	arith off;
{
	register struct localvar *p = find_reg(off);

	if (p) p->t_regtype = -1;
	C_lal(off);
}
#endif	/* LINT */
