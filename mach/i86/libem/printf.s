.define	printf

printf:
        ! $Header$
	pop	bx		! return address
	xchg	ax,di
	mov	si,sp
1:
	movb	al,(di)
	inc	di
	testb	al,al
	jz	6f
	cmpb	al,'%'
	je	3f
2:
	call	printc
	jmp	1b
3:
	movb	al,(di)
	inc	di
	cmpb	al,'c'
	jne	4f
	lods
	jmp	2b
4:
	cmpb	al,'d'
	jne	5f
	lods
	call	printd
	jmp	1b
5:
	cmpb	al,'s'
	jne	2b
	lods
	call	prints
	jmp	1b
6:
	jmp	bx
