.define prdec
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! print hl-reg as a decimal number.

prdec:	push h
	push d
	push b
	push psw
	lxi d,table
	mvi b,4
1:	call convert
	ori 0x30
	call putchar
	dcr b
	jnz 1b
	mov a,l
	ori 0x30
	call putchar
	pop psw
	pop b
	pop d
	pop h
	ret

convert:
	push b
	mov b,h
	mov c,l
	xchg
	mov e,m
	inx h
	mov d,m
	inx h
	push h		! save pointer to new value
	mov h,b
	mov l,c
	mvi b,255
1:	inr b
	mov a,l
	sub e
	mov l,a
	mov a,h
	sbb d
	mov h,a
	jnc 1b
	dad d
	mov a,b
	pop d
	pop b
	ret

table:
	.data2	10000
	.data2	1000
	.data2  100
	.data2	10

