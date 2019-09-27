.define	.csb8
.sect .text
.sect .rom
.sect .data
.sect .bss
ECASE=20

.sect .text

.csb8:
		! case descriptor in a0
		! index in (sp)

	move.l	(a0)+, a1	! default jump address
	move.l	(sp)+, d2
	move.l	(sp), a2	! index in d2:a2
	move.l	d7, (sp)
	add.l	#4, a0
	move.l	(a0)+, d1	! # entries (assuming <= 65536)
	beq	3f
	sub.l	#1, d1
1:
	move.l	(a0)+, d0
	move.l	(a0)+, d7	! descriptor's index in d0:d7
	add.l	#4, a0
	sub.l	a2, d7
	subx.l	d2, d0		! descriptor's index - our index
	dbeq	d1, 1b
	bne	3f
	move.l	(-4,a0), a1	! jump address
3:
	move.l	(sp)+, d7	! caller's d7
	move.l	a1, d0
	beq	4f
	jmp	(a1)
4:
	move.l	#ECASE, -(sp)
	jmp	(.fatal)
.align 2
