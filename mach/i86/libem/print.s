.define	printc,printd,prints

SIO_D		= 0xD8
SIO_S		= 0xDA
TXRDY		= 0x01

        ! $Header$
	! argument in ax
	! uses bx
prints:
	xchg	ax,bx
1:
	movb	al,(bx)
	inc	bx
	testb	al,al
	jz	2f
	call	printc
	jmp	1b
2:
	ret

	! argument in ax
	! uses cx and dx
printd:
	xor	dx,dx
	mov	cx,10
	div	cx
	test	ax,ax
	jz	1f
	push	dx
	call	printd
	pop	dx
1:
	xchg	ax,dx
	addb	al,'0'
	.errnz	printc - .

	! argument in ax
printc:
	push	ax
1:
	inb	SIO_S
	andb	al,TXRDY
	jz	1b
	pop	ax
	outb	SIO_D
	ret
