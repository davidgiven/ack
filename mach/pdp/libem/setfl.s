.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define setfloat~,setint~
.extern unknown~
! $Id$

setfloat~:
	cmp	r0,$8
	bne	1f
	rts	pc
1:	cmp	r0,$04
	bne	3f
	setf
2:	rts	pc
3:	jmp	unknown~
setint~:
	cmp	r0,$04
	bne	4f
	setl
	rts	pc
4:	cmp	r0,$02
	bne	3b
5:	rts	pc
