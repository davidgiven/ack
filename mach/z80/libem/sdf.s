.define .sdf

! store double offsetted

.sdf:
	pop bc
	push bc		!test
	pop ix		! return address
	pop hl		! address
	add hl,de
	pop bc
	ld (hl),c
	inc hl
	ld (hl),b
	inc hl
	pop bc
	ld (hl),c
	inc hl
	ld (hl),b
	jp (ix)		! return
