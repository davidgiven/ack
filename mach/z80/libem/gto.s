.define .gto
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

.gto:
	ld e,(hl)
	inc hl
	ld d,(hl)
	push de
	pop ix		! new pc
	inc hl	
	ld e,(hl)
	inc hl
	ld d,(hl)	! new sp
	inc hl
	ld c,(hl)
	inc hl	
	ld b,(hl)	! new lb
	push bc
	pop iy
	push de	
	pop hl
	ld sp,hl
	jp (ix)
