.define prstring

! print a string of characters to the console
! entry: de-reg points to the string
! 	 string terminator is 0x00
! exit:	 de-reg points to string terminator

prstring:
	push psw
1:	ldax d
	ora a
	jz 2f
	call putchar
	inx d
	jmp 1b
2:	pop psw
	ret

