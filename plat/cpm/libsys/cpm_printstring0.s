#
#include "asm.h"

! Prints a \0-terminated string.

.define _cpm_printstring0
_cpm_printstring0:
	pop h		   ! pop return address
	pop d          ! pop parameter (possibly junk)
	push d
	push h
    
.1:
    ldax d         ! fetch byte
    ora a
    rz             ! exit if zero
    inx d

    push b         ! save FP as the BDOS will corrupt it
    push d
    mov e, a
    mvi c, 2       ! conout
    call 0x0005
    pop d
    pop b

    jmp .1
