.define .mon
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.mon:	
	cmpd 1,4(sp)
	beq 1f
	cmpd 3,4(sp)
	beq 3f
	cmpd 4,4(sp)
	beq 4f
	cmpd 54,4(sp)
	beq 5f

	movd .add1, tos
	jsr @.prstr
	movd 4(sp), tos
	jsr @.print
	movd .add2, tos
	jsr @.prstr
	ret 4

1:	jump @.stop

3:	save [r0,r1,r2,r3]
	movd 3,r0
	movd 28(sp),r1
	movd 32(sp),r2
	movd 0,r3
	svc
	movd r2, 32(sp)
	movd 0,28(sp)
	restore [r0,r1,r2,r3]
	ret 8

4:	save [r0,r1,r2,r3]
	movd 4,r0
	movd 28(sp),r1
	movd 32(sp),r2
	movd 0,r3
	svc
	movd r2, 32(sp)
	movd 0,28(sp)
	restore [r0,r1,r2,r3]
	ret 8

5:	movd 0, 16(sp)
	ret 12

.sect .data
.add1:	.asciz "monitor call "
.add2:	.asciz " not implemented\n"
.align
