.define	.diagnos
.sect .text
.sect .rom
.sect .data
.sect .bss

space	= 040
del	= 0177

.sect .data
.range:	.data1 space, del-1
.align	2

.sect .text
.diagnos:
	movem.l	d0/d1/d2/a0, -(sp)
	move.l	(.lino), -(sp)
	move.l	(.filn), d2
	beq	1f
	move.l	d2, a0
	move.l	#40, d0
3:
	move.b	(a0)+, d1
	beq	2f
	cmp2.b	(.range), d1
	bcs	1f
	dbf	d0, 3b
2:
	move.l	d2, -(sp)
	pea	(fmt)
	jsr	(_printf)
	lea	(8, sp), sp
	move.l	(28, sp),(sp)
	move.l	(24, sp),-(sp)
	jsr	(_printf)
	lea	(8, sp), sp
	movem.l	(sp)+, d0/d1/d2/a0
	move.l	(sp), (4, sp)
	lea	(4, sp), sp
	rts

1:
	move.l	#unknwn, d2
	bra	2b

.sect .data
fmt:		.asciz "%s, line %d: "
unknwn:		.asciz "unknown file"
.align 2
