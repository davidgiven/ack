.define .los
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text



.los:
	pop ix		! save return address
	pop de		! number of bytes to transfer
	pop hl		! address of lowest byte
	add hl,de
	dec hl		! address of highest byte
	srl d		! divide de by 2
	rr e
	jr nc,1f	! see if de was odd
	ld a,e		! yes, then it must be 1
	or d
	jr nz,.trp.z	! no, error
	ld e,(hl)	! pack 1 byte into integer
	push de
	jp (ix)		! return
1:
	ld b,(hl)	! get 2 bytes
	dec hl
	ld c,(hl)
	dec hl
	push bc		! put them on stack, most
			! significant byte first
	dec de
	ld a,d
	or e
	jr nz,1b	! done ?
	jp (ix)		! yes, return
