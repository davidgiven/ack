.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
loop = 100
dvi4:
	xor a
	ld (.flag1),a
	ld (.flag2),a
	ld ix,0
	add ix,sp
	ld b,(ix+7)		! dividend
	bit 7,b
	jr z,1f
	ld c,(ix+6)
	ld d,(ix+5)
	ld e,(ix+4)
	call .negbd
	ld (ix+7),d
	ld (ix+6),e
	ld (ix+5),h
	ld (ix+4),l
	ld a,1
	ld (.flag1),a
1:
	ld b,(ix+3)
	bit 7,b
	jr z,2f
	call .negst
	ld a,1
	ld (.flag2),a
2:
	call .dvu4
	ld a,(.flag1)
	jr z,3f
	call .negbd
3:
	ld a,(.flag2)
	ld b,a
	ld a,(.flag1)
	xor b
	jr z,4f
	call .negst
4:
	jr loop
.negbd:
	xor a
	ld h,a
	ld l,a
	sbc hl,de
	ex de,hl
	ld h,a
	ld l,a
	sbc hl,bc
	ret
.negst:
	pop iy
	pop de
	pop bc
	call .negbd
	push hl
	push de
	jp (iy)
.sect .data
	.flag1: .data1 0
	.flag2: .data1 0
