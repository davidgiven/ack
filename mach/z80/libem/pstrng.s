.define	pstrng
! print a string of characters to the console
! entry: DE points to string
!	 string terminator is 0x00
! exit:	 DE points to string terminator
pstrng:	push	af
1:	ld	a,(de)
	or	a
	jr	z,2f
	call	putchr
	inc	de
	jr	1b
2:	pop	af
	ret
