/ $Header$
	.globl eret

eret:
	mov r5,sp
	mov (sp)+,r5
	mov (sp)+,r4
	mov (sp)+,r2
	rts pc
