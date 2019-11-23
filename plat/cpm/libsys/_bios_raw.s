#
#include "asm.h"

! Low level BIOS call routine. The entrypoint offset from WBOOT is in A.
! HL is corrupted on entry.
.define call_bios_raw
call_bios_raw:
	lhld 1         ! get BIOS entrypoint + 3
    add l          ! add on offset (the -3 is supplied by the caller)
    mov l, a
    jnc .1
    inr h
.1:
    pchl           ! call the routine
