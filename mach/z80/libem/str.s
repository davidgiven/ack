.define .strhp
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

.strhp:
	pop ix
	pop hl
	push hl
	or a
	sbc hl,sp
	jp m,1f
	pop hl
	push hl
	ld a,l
	rra
	jp c,1f
	pop hl
	ld (.reghp),hl
	jp (ix)
1:
	pop hl
	ld hl,EHEAP
	call .trp.z
	jp (ix)
