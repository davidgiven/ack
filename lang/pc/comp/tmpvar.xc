/* T E M P O R A R Y   V A R I A B L E S */

/*	Code for the allocation and de-allocation of temporary variables,
	allowing re-use.
	The routines use "ProcScope" instead of "CurrentScope", because
	"CurrentScope" also reflects WITH statements, and these scopes do not
	have local variables.
*/

#include	"debug.h"

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<em_reg.h>

#include	"def.h"
#include	"main.h"
#include	"scope.h"
#include	"type.h"

struct tmpvar	{
	struct tmpvar	*next;
	arith		t_offset;	/* offset from LocalBase */
};

/* ALLOCDEF "tmpvar" 10 */

static struct tmpvar	*TmpInts,	/* for integer temporaries */
			*TmpPtrs;	/* for pointer temporaries */
static struct scope	*ProcScope;	/* scope of procedure in which the
					   temporaries are allocated
					*/

TmpOpen(sc)
	struct scope *sc;
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

STATIC arith
NewTmp(plist, sz, al, regtype, priority)
	struct tmpvar **plist;
	arith sz;
{
	register arith offset;
	register struct tmpvar *tmp;

	if( !*plist )	{
		offset = TmpSpace(sz, al);
		if( !options['n'] ) C_ms_reg(offset, sz, regtype, priority);
	}
	else	{
		tmp = *plist;
		offset = tmp->t_offset;
		*plist = tmp->next;
		free_tmpvar(tmp);
	}
	return offset;
}

arith
NewInt(reg_prior)
{
	return NewTmp(&TmpInts, int_size, int_align, reg_any, reg_prior);
}

arith
NewPtr(reg_prior)
{
   return NewTmp(&TmpPtrs, pointer_size, pointer_align, reg_pointer, reg_prior);
}

STATIC
FreeTmp(plist, off)
	struct tmpvar **plist;
	arith off;
{
	register struct tmpvar *tmp = new_tmpvar();

	tmp->next = *plist;
	tmp->t_offset = off;
	*plist = tmp;
}

FreeInt(off)
	arith off;
{
	FreeTmp(&TmpInts, off);
}

FreePtr(off)
	arith off;
{
	FreeTmp(&TmpPtrs, off);
}

TmpClose()
{
	register struct tmpvar *tmp, *tmp1;

	tmp = TmpInts;
	while( tmp )	{
		tmp1 = tmp;
		tmp = tmp->next;
		free_tmpvar(tmp1);
	}
	tmp = TmpPtrs;
	while( tmp )	{
		tmp1 = tmp;
		tmp = tmp->next;
		free_tmpvar(tmp1);
	}
	TmpInts = TmpPtrs = 0;
}
