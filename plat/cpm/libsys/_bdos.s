#
#include "asm.h"

! Calls a BDOS routine and returns the result.
!   a = BDOS opcode

.define call_bdos
call_bdos:
	pop h		   ! pop return address
	pop d          ! pop parameter (possibly junk)
	push d
	push h

	push b         ! save FP as the BDOS will corrupt it
	mov c, a       ! move opcode to C
	call 0x0005
	pop b          ! restore FP
	
	xchg           ! DE = HL
	ret