.define .sti

! Store indirect
! Expects on stack:	number of bytes to be stored
!			bytes to be stored
	
.sti:	pop h
	shld .retadr
	mov l,c
	mov h,b
	shld .bcreg	! save bc

	pop h
	xra a
	mov a,d
	rar
	mov d,a
	mov a,e
	rar
	mov e,a		! de = word count
	jnc 1f

! if 1 byte array element only:
	mov a,d		! trap if de odd and <>1
	ora e
	cnz eoddz
	pop b
	mov m,c
	jmp 2f
	
1:	pop b
	mov m,c
	inx h
	mov m,b
	inx h
	dcx d
	mov a,d
	ora e
	jnz 1b

2:	lhld .bcreg
	mov c,l
	mov b,h
	lhld .retadr
	pchl

