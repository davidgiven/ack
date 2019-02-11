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
    sta 0x10
    sta 0x18
    lxi h, rst1
    shld 0x09
    lxi h, rst2
    shld 0x11
    lxi h, rst3
    shld 0x19

    lhld rst4
    shld 0x20
    lhld rst4+2
    shld 0x22
    ret

    ! de = [bc+const1] (remember bc is the frame pointer)
rst1:
    pop h
    mov a, m
    inx h
    push h

	mov l, a
	ral
	sbb a
	mov h, a

	dad b
    mov e, m
    inx h
    mov d, m
    ret

    ! [bc+const1] = de (remember bc is the frame pointer)
rst2:
    pop h
    mov a, m
    inx h
    push h

	mov l, a
	ral
	sbb a
	mov h, a

	dad b
    mov m, e
    inx h
    mov m, d
    ret

    ! hl = bc+const1
rst3:
    pop h
    mov a, m
    inx h
    push h

    mov l, a
    ral
    sbb a
    mov h, a

    dad b
    ret
    
    ! Adjust flags for signed comparison
rst4:
    rpo
    xri 0x80
    ret
