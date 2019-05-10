/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* T E M P O R A R Y   V A R I A B L E S */

/* $Id$ */

/*	Code for the allocation and de-allocation of temporary variables,
	allowing re-use.
	The routines use "ProcScope" instead of "CurrentScope", because
	"CurrentScope" also reflects WITH statements, and these scopes do not
	have local variabes.
*/

#include 	"parameters.h"
#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include	<em_reg.h>
#include	<alloc.h>
#include	<assert.h>

#include	"LLlex.h"
#include	"def.h"
#include	"type.h"
#include	"scope.h"
#include	"tmpvar.h"
#include	"main.h"

struct tmpvar {
	struct tmpvar	*t_next;
	arith		t_offset;	/* offset from LocalBase */
};

/* STATICALLOCDEF "tmpvar" 10 */

static struct tmpvar	*TmpInts,	/* for integer temporaries */
			*TmpPtrs;	/* for pointer temporaries */
static struct scope	*ProcScope;	/* scope of procedure in which the
					   temporaries are allocated
					*/

void TmpOpen(struct scope *sc)
{
	/*	Initialize for temporaries in scope "sc".
	*/
	ProcScope = sc;
}

arith TmpSpace(arith sz, int al)
{
	register struct scope *sc = ProcScope;

	sc->sc_off = - WA(align(sz - sc->sc_off, al));
	return sc->sc_off;
}

static arith NewTmp(struct tmpvar **plist, arith sz, int al, int regtype)
{
	register arith offset;
	register struct tmpvar *tmp;

	if (!*plist) {
		offset = TmpSpace(sz, al);
		if (! options['n']) C_ms_reg(offset, sz, regtype, 0);
	}
	else {
		tmp = *plist;
		offset = tmp->t_offset;
		*plist = tmp->t_next;
		free_tmpvar(tmp);
	}
	return offset;
}

arith NewInt(void)
{
	return NewTmp(&TmpInts, int_size, int_align, reg_any);
}

arith NewPtr(void)
{
	return NewTmp(&TmpPtrs, pointer_size, pointer_align, reg_pointer);
}


static void FreeTmp(struct tmpvar **plist, arith off)
{
	register struct tmpvar *tmp = new_tmpvar();

	tmp->t_next = *plist;
	tmp->t_offset = off;
	*plist = tmp;
}

void FreeInt(arith off)
{
	FreeTmp(&TmpInts, off);
}

void FreePtr(arith off)
{
	FreeTmp(&TmpPtrs, off);
}

void TmpClose(void)
{
	register struct tmpvar *tmp, *tmp1;

	tmp = TmpInts;
	while (tmp) {
		tmp1 = tmp;
		tmp = tmp->t_next;
		free_tmpvar(tmp1);
	}
	tmp = TmpPtrs;
	while (tmp) {
		tmp1 = tmp;
		tmp = tmp->t_next;
		free_tmpvar(tmp1);
	}
	TmpInts = TmpPtrs = 0;
}
