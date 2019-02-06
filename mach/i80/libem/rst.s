#
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

.define .rst_init
.rst_init:
    mvi a, 0xc3     ! jmp <a16>
    sta 0x08
    lxi h, rst1
    shld 0x09
    ret

    ! de = [bc+const2] (remember bc is the frame pointer)
rst1:
    pop h
    mov e, m
    inx h
    mov d, m
    inx h
    push h
    xchg        ! hl = offset
    dad b
    mov e, m
    inx h
    mov d, m
    ret
