.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define ngf~
.extern setfloat~
! $Header$

ngf~:
	jsr	pc,setfloat~
	negf	2(sp)
	setd
	rts	pc
