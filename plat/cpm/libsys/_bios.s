#
#include "asm.h"

! Calls a BIOS routine which returns its value in A.
!   a = BIOS offset

.define call_bios_a
call_bios_a:
	pop h		   ! pop return address
	pop d          ! pop parameter (possibly junk)
	push d
	push h
	push b         ! save FP as we'll corrupt it

    mov b, d       ! put parameter in BC
    mov c, e
    call call_bios_raw
    
    pop b
    mov e, a       ! put result in DE
    mvi d, 0
    ret
