.define .rck
.rck:
	pop bc
	pop ix
3:	pop hl
	push hl
	ld e,(ix)
	ld d,(ix+1)
	ld a,h
	xor d		! check sign bit to catch overflow with subtract
	jp m,1f
	sbc hl,de
	jr 2f
1:	xor d		! now a equals (original) h again
2:	call m,e.rck
	pop de
	push de
	ld l,(ix+2)
	ld h,(ix+3)
	ld a,h
	xor d		! check sign bit to catch overflow with subtract
	jp m,1f
	sbc hl,de
	jr 2f
1:	xor d		! now a equals (original) h again
2:	call m,e.rck
	push bc
	pop ix
	jp (ix)


