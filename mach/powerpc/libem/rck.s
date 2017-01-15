#include "powerpc.h"

.sect .text

! Bounds check. Traps if the value is out of range.
!  Stack: ( descriptor value -- )

.define .rck
.rck:
    lwz r3, 0 (sp)
    lwz r4, 4 (sp)
    addi sp, sp, 8

    lwz r5, 0 (r3)
    cmp cr0, 0, r4, r5
    bc IFTRUE, LT, .trap_erange

    lwz r5, 4 (r3)
    cmp cr0, 0, r4, r5
    bc IFTRUE, GT, .trap_erange

	bclr ALWAYS, 0, 0
