/ $Header$
	.globl RT,RTr2,RTr2r4

RTr2r4:
	mov (sp)+,r4
RTr2:
	mov (sp)+,r2
RT:
	mov r5,sp
	mov (sp)+,r5
	rts pc
