/* T E M P O R A R Y   V A R I A B L E S */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

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

struct tmpvar {
	struct tmpvar	*next;
	arith		t_offset;	/* offset from LocalBase */
};

/* STATICALLOCDEF "tmpvar" */

static struct tmpvar	*TmpInts,	/* for integer temporaries */
			*TmpPtrs;	/* for pointer temporaries */
extern struct scope	*ProcScope;	/* scope of procedure in which the
					   temporaries are allocated
					*/

arith
NewInt()
{
	arith offset;
	register struct tmpvar *tmp;

	if (!TmpInts) {
		offset = - WA(align(int_size - ProcScope->sc_off, int_align));
		ProcScope->sc_off = offset;
		C_ms_reg(offset, int_size, reg_any, 0);
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
	arith offset;
	register struct tmpvar *tmp;

	if (!TmpPtrs) {
		offset = - WA(align(pointer_size - ProcScope->sc_off, pointer_align));
		ProcScope->sc_off = offset;
		C_ms_reg(offset, pointer_size, reg_pointer, 0);
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
