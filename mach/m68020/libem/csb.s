.define	.csb
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

.csb:
	move.l	(sp)+, a0	! case descriptor
	move.l	(sp)+, d0	! index
	move.l	(a0)+, a1	! default jump address
	move.l	(a0)+, d1	! # entries
	beq	3f
	sub.l	#1, d1
1:
	cmp.l	(a0,d1.w*8), d0
	dbeq	d1, 1b
	bne	3f
	move.l	(4,a0,d1.w*8), a1 ! jump address
3:
	move.l	a1, d0
	beq	4f
	jmp	(a1)
4:
	move.l	#ECASE, -(sp)
	jmp	(.fatal)
.align 2
