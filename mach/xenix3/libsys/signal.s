.sect .text; .sect .rom; .sect .data; .sect .bss
.define _signal
.sect .text
_signal:
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	cx,6(bp)
	mov	bx,4(bp)
	cmp	bx,20
	jae	1f
	mov	ax,bx
	add	bx,bx
	push	table(bx)
	mov	table(bx),cx
	and	cx,cx
	je	2f
	cmp	cx,1
	je	2f
	add	bx,bx
	lea	cx,tbl(bx)
2:
	mov	bx,ax
	mov	ax,48
	call	syscal
	pop	bx
	jb	4f
	and	ax,ax
	je	5f
	cmp	ax,1
	jne	6f
5:
	jmp	7f
6:
	xchg	ax,bx
	jmp	7f
1:
	mov	ax,22
4:
	mov	(_errno),ax
	mov	ax,-1
7:
	pop	di
	pop	si
	pop	bp
	ret

ctch:
	push	bp
	mov	bp,sp
	push	ax
	push	bx
	push	cx
	push	dx
	push	si
	push	di
	mov	ax,2(bp)
	sub	ax,3
	sub	ax,tbl
	shr	ax,1
	mov	bx,ax
	shr	ax,1
	push	ax
	call	table(bx)
	pop	di
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	pop	bp
	add	sp,2
	pop	(retadr)
	pop	(dummy)
	popf
	jmp	(retadr)
.sect .bss
retadr: .space 2
dummy: .space 2
.sect .text
.align 2
tbl:
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
	call	ctch
.align 2
.sect .bss
table:
	.space 40
