#
#include "asm.h"

! Calls the BIOS SECTRAN routine, which returns its result in HL. 

.define _cpm_bios_sectran
_cpm_bios_sectran:
	pop d		   ! pop return address
	pop h          ! pop translation table 
    shld trans_table
    pop h          ! pop sector number
    shld sec_num
    push h
    push h
    push d
	push b         ! save FP as we'll corrupt it

trans_table = . + 1
    lxi d, 0
sec_num = . + 1
    lxi b, 0

    lda 0x30-3     ! BIOS offset from WBOOT to SECTRAN
    call call_bios_raw

    pop b
	xchg           ! DE = HL
    ret



