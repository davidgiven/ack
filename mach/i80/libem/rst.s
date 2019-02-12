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
    ret

    ! de = [fp+const1]
rst1:
    lhld .fp
    xchg

    pop h
    mov a, m
    inx h
    push h

	mov l, a
	ral
	sbb a
	mov h, a

	dad d
    mov e, m
    inx h
    mov d, m
    ret

    ! [fp+const1] = bc
rst2:
    lhld .fp
    xchg
    
    pop h
    mov a, m
    inx h
    push h

	mov l, a
	ral
	sbb a
	mov h, a

	dad d
    mov m, c
    inx h
    mov m, b
    ret

    ! hl = fp+const1
rst3:
    lhld .fp
    xchg

    pop h
    mov a, m
    inx h
    push h

    mov l, a
    ral
    sbb a
    mov h, a

    dad d
    ret

