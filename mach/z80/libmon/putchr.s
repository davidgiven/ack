.define putchr
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

putchr:
	push hl
	push de
	push bc
	cp 0x0A
	jr nz,1f
	ld a,0x1F
1:
	ld c,a
2:
	in a,0xF1
	and 4
	jr z,2b
	ld a,c
	out 0xF0,a
	pop bc
	pop de
	pop hl
	ret
