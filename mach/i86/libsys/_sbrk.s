.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __sbrk
__sbrk:
push	bp
mov	bp,sp
mov	ax,4(bp)
mov	bx,(.limhp)
add	ax,bx
mov	cx,sp
sub	cx,128
sub	cx,ax
jbe	1f
mov	(.limhp),ax
sub	ax,bx
jbe	2f
call	xbrk
2:
mov	ax,bx
pop	bp
ret
1:
mov	ax,0xc
pop	bp
jmp	cerror
