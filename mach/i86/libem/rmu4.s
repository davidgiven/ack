.define .rmu4

yl=2
yh=4
xl=6
xh=8

.rmu4:
        ! $Header$
	mov     si,sp           ! copy of sp
	mov     bx,yl(si)
	mov     ax,yh(si)
	or      ax,ax
	jne     7f
1:
	xor     dx,dx
	mov     cx,xl(si)
	mov     ax,xh(si)
2:
	div     bx
	xchg    ax,cx
	div     bx
	xor     bx,bx
9:
			! bx is high order result
			! dx is low order result
	ret     8

7:
	mov     di,ax
	xor     bx,bx
	mov     ax,xl(si)
	mov     dx,xh(si)
	mov     cx,16
1:
	shl     ax,1
	rcl     dx,1
	rcl     bx,1
	cmp     di,bx
	ja      3f
	jb      2f
	cmp     yl(si),dx
	jbe     2f
3:
	loop    1b
		! dx=result(low), bx=result(high)
	jmp     9b
2:
	sub     dx,yl(si)
	sbb     bx,di
	inc     ax
	loop    1b
1:
		! dx=result(low), bx=result(high)
	jmp     9b
