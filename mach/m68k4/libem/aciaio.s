.define	.outch1,.outch2,.inch1,.inch2
.sect .text
.sect .rom
.sect .data
.sect .bss

	! a0,d0 used
	! character in d1

	.sect .text
.inch1:
	move.l	#acia1,a0
	bra	.inch
.inch2:
	move.l	#acia2,a0
.inch:
	move.b	(a0),d1
	asr	#1,d1
	bcc	.inch
	move.b	2(a0),d1
	and	#0x7f,d1
	rts

.outch1:
	move.l	#acia1,a0
	bra	.outch
.outch2:
	move.l	#acia2,a0
.outch:
	move.b	(a0),d0
	asr	#2,d0
	bcc	.outch
	move.b	d1,2(a0)
	rts

.align 2
