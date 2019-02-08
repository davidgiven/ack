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
    lxi h, rst1
    shld 0x09
    lxi h, rst2
    shld 0x11
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

