#
! $Source$
! $State$
! $Revision$

#include "powerpc.h"
	
.sect .text

! Multiplies two floats, and returns the fraction and integer.

.define .fif8
.fif8:
	lfd f0, 8(sp)
	lfd f1, 0(sp)
	fmul f0, f0, f1
	fabs f1, f0              ! f0 = result
	
	! The following chunk does f1 = floor(f1). See page 158 of the book.
	
	mtfsfi cr7, 3              ! set rounding mode to -inf.
	mtfsb0 23
	fctid f2, f1
	fcfid f2, f2
	mcrfs cr7, cr5
	bc IFFALSE, 31, toobig
	fmr f1, f2
toobig:

	fabs f2, f1              ! f2 = fabs(f1)
	fsub f2, f2, f1
	stfd f2, 8(sp)
	
	fneg f2, f1
	fsel f2, f0, f1, f2
	stfd f2, 0(sp)
	
	bclr ALWAYS, 0, 0
