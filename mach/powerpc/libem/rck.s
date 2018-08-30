.sect .text

! Bounds check. Traps if the value is out of range.
!  Stack: ( value descriptor -- value )
!
! This ".rck" only works with 4-byte integers.  The name is ".rck" and
! not ".rck4" because many back ends only do rck with the word size.

.define .rck
.rck:
    lwz r3, 0 (sp)
    lwz r4, 4 (sp)
    addi sp, sp, 4 ! leave value on stack

    lwz r5, 0 (r3)
    cmpw r4, r5
    blt .trap_erange

    lwz r5, 4 (r3)
    cmpw r4, r5
    bgt .trap_erange

    blr

.trap_erange:
    li r3, 1       ! ERANGE = 1 in h/em_abs.h
    b .trp
