/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* T E M P O R A R Y   V A R I A B L E S */

/* $Header$ */

/*	Code for the allocation and de-allocation of temporary variables,
	allowing re-use.
	The routines use "ProcScope" instead of "CurrentScope", because
	"CurrentScope" also reflects WITH statements, and these scopes do not
	have local variabes.
*/

#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<em_reg.h>
#include	<alloc.h>
#include	<assert.h>

#include	"def.h"
#include	"type.h"
#include	"scope.h"
#include	"main.h"

struct tmpvar {
	struct tmpvar	*next;
	arith		t_offset;	/* offset from LocalBase */
};

/* STATICALLOCDEF "tmpvar" 10 */

static struct tmpvar	*TmpInts,	/* for integer temporaries */
			*TmpPtrs;	/* for pointer temporaries */
static struct scope	*ProcScope;	/* scope of procedure in which the
					   temporaries are allocated
					*/

TmpOpen(sc) struct scope *sc;
{
	/*	Initialize for temporaries in scope "sc".
	*/
	ProcScope = sc;
}

arith
TmpSpace(sz, al)
	arith sz;
{
	register struct scope *sc = ProcScope;

	sc->sc_off = - WA(align(sz - sc->sc_off, al));
	return sc->sc_off;
}

arith
NewInt()
{
	register arith offset;
	register struct tmpvar *tmp;

	if (!TmpInts) {
		offset = TmpSpace(int_size, int_align);
		if (! options['n']) C_ms_reg(offset, int_size, reg_any, 0);
	}
	else {
		tmp = TmpInts;
		offset = tmp->t_offset;
		TmpInts = tmp->next;
		free_tmpvar(tmp);
	}
	return offset;
}

arith
NewPtr()
{
	register arith offset;
	register struct tmpvar *tmp;

	if (!TmpPtrs) {
		offset = TmpSpace(pointer_size, pointer_align);
		if (! options['n']) C_ms_reg(offset, pointer_size, reg_pointer, 0);
	}
	else {
		tmp = TmpPtrs;
		offset = tmp->t_offset;
		TmpPtrs = tmp->next;
		free_tmpvar(tmp);
	}
	return offset;
}

FreeInt(off)
	arith off;
{
	register struct tmpvar *tmp;

	tmp = new_tmpvar();
	tmp->next = TmpInts;
	tmp->t_offset = off;
	TmpInts = tmp;
}

FreePtr(off)
	arith off;
{
	register struct tmpvar *tmp;

	tmp = new_tmpvar();
	tmp->next = TmpPtrs;
	tmp->t_offset = off;
	TmpPtrs = tmp;
}

TmpClose()
{
	register struct tmpvar *tmp, *tmp1;

	tmp = TmpInts;
	while (tmp) {
		tmp1 = tmp;
		tmp = tmp->next;
		free_tmpvar(tmp1);
	}
	tmp = TmpPtrs;
	while (tmp) {
		tmp1 = tmp;
		tmp = tmp->next;
		free_tmpvar(tmp1);
	}
	TmpInts = TmpPtrs = 0;
}
