.define .cmu4

! 4 byte cmu and cmi routine
! parameters:
!   a:   0 for cmu, 1 for cmi
!  stack: operands
!  de:   result (out)



.cmu4:
	pop ix
	ld de,4
	ld b,d
	ld c,e
	ld hl,0
	add hl,sp
	add hl,bc
	dec hl
	ld d,h
	ld e,l
	add hl,bc
	ld (savesp),hl		! save new sp-1
	or a
	jr z,1f
	ld a,(de)
	cp (hl)
	dec hl
	dec de
	dec bc
	jr z,1f
	jp p,4f
	jr 6f
1:
	ld a,(de)
	cp (hl)
	dec de
	dec hl
	dec bc
	jr nz,2f
	ld a,b
	or c
	jr nz,1b
	ld d,a
	ld e,a
	jr 3f
2:
	jr nc,4f
6:
	ld de,1
	jr 3f
4:
	ld de,-1
3:
	ld hl,(savesp)
	inc hl
	ld sp,hl
	jp (ix)
.data
savesp: .word 0
