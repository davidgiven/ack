.sect .text; .sect .rom; .sect .data; .sect .bss
.define _fork, _par_uid
.sect .bss
.comm	_par_uid, 4
.sect .text
_fork:
	mov	ax,2
	callf	0x7:0x0
	jb	1f
	and	dx,dx
	jne	2f
	ret
1:
	jmp	cerror
2:
	mov	(_par_uid),ax
	xor	ax,ax
	ret
