.sect .text

! Bounds check. Traps if the value is out of range.
!  Stack: ( value descriptor -- value )

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
