.sect .text
.define .blm

	! cx: count in words
.blm:
	mov	bx,sp
	mov	ax,si
	mov	dx,di
	mov	di,2(bx)
	mov	si,4(bx)
	rep	movs
	mov	si,ax
	mov	di,dx
	ret	4

