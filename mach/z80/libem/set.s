.define .set
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
! use .unimpld

! any size sets
! parameters:
!   hl:    size
!   stack: bitnumber
!   stack: result (out)



.set:
	pop ix		! return address
	pop de		! bit number
	ld b,h
	ld c,l
	xor a
0:	push af
	inc sp
	dec c
	jr nz,0b
	dec b
	jp p,0b
	ex de,hl
	ld a,l
	sra h
	jp m,.unimpld
	rr l
	srl h
	rr l
	srl h
	rr l
	push hl
	or a
	sbc hl,de
	pop hl
	jp p,.unimpld
	add hl,sp
	ld (hl),1
	and 7
	jr 1f
0:	sla (hl)
	dec a
1:	jr nz,0b
	jp (ix)
