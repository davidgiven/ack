.text
.globl aar~

/r0 : description address
/r1 : element number
/base address is on stack
aar~:
	sub	(r0),r1
	mov	04(r0),r0
	mul	r0,r1
	add	r1,02(sp)
	rts	pc
