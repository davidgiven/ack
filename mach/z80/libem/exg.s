.define .exg
.exg:	
	pop ix
	pop de
	ld hl,0
	add hl,sp
	ld b,h
	ld c,l
	add hl,de
1:
	ld a,(bc)
	ex af,af2
	ld a,(hl)
	ld (bc),a
	ex af,af2
	ld (hl),a
	inc bc
	inc hl
	dec de
	ld a,d
	or e
	jr nz,1b
	jp (ix)


