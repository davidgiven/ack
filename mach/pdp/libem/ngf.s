/ $Header$
.text
.globl ngf~
.globl setfloat~

ngf~:
	jsr	pc,setfloat~
	negf	2(sp)
	setd
	rts	pc
