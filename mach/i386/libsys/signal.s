.sect .text; .sect .rom; .sect .data; .sect .bss
.define _signal
.sect .text
_signal:
	call	sig1
	mov	ax,48
	mov	dx,ctch
	callf	0x7:0x0
	jb	4f
	ret
sig1:
	mov	ax,8(sp)
	cmp	ax,20
	jae	1f
	mov	dx,table(ax*4)
	mov	(prev),dx
	mov	dx,12(sp)
	mov	table(ax*4),dx
	ret
1:
	pop	ax
	mov	ax,0x16
4:
	mov	(_errno),ax
	mov	ax,-1
	cdq
	ret

ctch:
	add	sp,4
	callf	0xf:0x0

.sect .bss
table:
	.space 40
prev:
	.space 4

