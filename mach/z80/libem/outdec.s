.define outdec
! output contents of HL as a sequence
! of decimal digits
outdec:
	push	hl
	push	de
	push	bc
	push	af
	ld	de,table
	ld	b,4
1:	call	convert
	or	0x30
	ld (iy+0),a
	inc iy
	djnz	1b
	ld	a,l
	or	0x30
	ld (iy+0),a
	pop	af
	pop	bc
	pop	de
	pop	hl
	ret
! convert returns in a a count
! hl is decremented count times by (de)
! as a usefull side effect de is incremented
! by 2
convert:
	push	bc
	ld	b,h
	ld	c,l
	ex	de,hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	push	hl	! save pointer to new value
	ld	h,b
	ld	l,c
	xor	a
1:	inc	a
	sbc	hl,de
	jr	nc,1b
	add	hl,de
	dec	a
	pop	de
	pop	bc
	ret
table:
	.short	10000
	.short	1000
	.short	100
	.short	10
