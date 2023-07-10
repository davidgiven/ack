#
#include "asm.h"

.sect .text

! Immediately after a subtraction, adjusts the C flag to reflect the result of
! a signed comparison.

.define .signedcmp
.signedcmp:
	ret z
	scf
	jp m, s_set			! jump if S
s_clear:
	ret pe              ! set C if P and !S
clear_carry:
	ccf					! set !C if !P and !S
	ret

s_set:
	ret po            	! set C if P and !S
	ccf
	ret					! set !C if !P and !S


