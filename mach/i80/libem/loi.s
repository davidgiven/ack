.define .loi
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Load indirect
! Expects in de-registers: number of bytes to be loaded
!			   (this number should be 1 or even )
! Expects on stack:	   base address
! Yields on stack:	   result

.loi:	pop h
	shld .retadr

	pop h		! hl = base address
	dad d		! hl = load pointer
	xra a		! clear carry bit
	mov a,d		! divide d by 2
	rar
	mov d,a
	mov a,e
	rar
	mov e,a
	jnc 1f

! if 1 byte has to be loaded only:
	mov a,d
	ora e
	cnz eoddz	! trap if number is odd and <> 1
	dcx h
	mov c,m
	mvi b,0
	push b
	jmp 2f

1:	dcx h
	mov b,m
	dcx h
	mov c,m
	push b
	dcx d		! is count exhausted?
	mov a,d
	ora e
	jnz 1b

2:	
	lhld .retadr
	pchl
