.sect .text; .sect .rom; .sect .data; .sect .bss
.define .dvu4

yl=2
yh=4
xl=6
xh=8

.sect .text
.dvu4:
	mov     si,sp           ! copy of sp
	mov     bx,yl(si)
	mov     ax,yh(si)
	or      ax,ax
	jne     7f
	xor     dx,dx
	mov     cx,xl(si)
	mov     ax,xh(si)
	div     bx
	xchg    ax,cx
	div     bx
9:
			! cx is high order result
			! ax is low order result
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
	jmp     9b
2:
	sub     dx,yl(si)
	sbb     bx,di
	inc     ax
	loop    1b
	jmp     9b
