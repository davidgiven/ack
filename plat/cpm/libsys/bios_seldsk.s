#
#include "asm.h"

! Calls the BIOS SELDSK routine, which returns its result in HL. 

.define _cpm_bios_seldsk
_cpm_bios_seldsk:
	pop h		   ! pop return address
	pop d          ! pop parameter 
	push d
	push h
	push b         ! save FP as we'll corrupt it

    mov b, d       ! put parameter in BC
    mov c, e

    mvi a, 0x1b-3  ! offset from WBOOT to SELDSK
    call call_bios_raw
    
    pop b
	xchg           ! DE = HL
    ret
