.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define sigtrp~
.define trp~,save~,retu~
! $Header$

indir	= 0
signal	= 060

sig1:	mov	sig.trp+0,-(sp)
	br	1f
sig2:	mov	sig.trp+2,-(sp)
	br	1f
sig3:	mov	sig.trp+4,-(sp)
	br	1f
sig4:	mov	sig.trp+6,-(sp)
	br	1f
sig5:	mov	sig.trp+010,-(sp)
	br	1f
sig6:	mov	sig.trp+012,-(sp)
	br	1f
sig7:	mov	sig.trp+014,-(sp)
	br	1f
sig10:	mov	sig.trp+022,-(sp)
	br	1f
sig11:	mov	sig.trp+024,-(sp)
	br	1f
sig12:	mov	sig.trp+026,-(sp)
	br	1f
sig13:	mov	sig.trp+030,-(sp)
	br	1f
sig14:	mov	sig.trp+032,-(sp)
	br	1f
sig15:	mov	sig.trp+034,-(sp)
	br	1f
sig16:	mov	sig.trp+036,-(sp)
	br	1f
1:
	jsr	pc,trp~
	rti

sigtrp~:
	jsr	pc,save~
	tst	(sp)+
	mov	(sp)+,r1
	mov	(sp)+,r0
	ble	sig.bad
	cmp	r0,$020
	bhi	sig.bad
	mov	r0,call+02
	asl	r0
	mov	sig.trp-2(r0),r3
	cmp	r1,$0400
	bhis	1f
	mov	sig.adr-2(r0),r2
	bne	2f
sig.bad:
	mov	$-1,r0
sigbad:
	mov	r0,-(sp)
	mov	r0,-(sp)
	jmp	retu~
1:	cmp	r1,$-3
	blo	sig.bad
	mov	r1,r2
	inc	r2
	inc	r2
2:	mov	r1,sig.trp-2(r0)
	mov	r2,call+04
	sys	indir ; .data2 call
	bcs	sigbad
	asr	r0
	bcc	1f
	mov	$-3,-(sp)
	clr	-(sp)
	jmp	retu~
1:	mov	r3,-(sp)
	clr	-(sp)
	jmp	retu~

.sect .data
call:	sys	signal
	.data2	0, 0
sig.trp:
	.data2 -2,	-2,	-2,	-2
	.data2 -2,	-2,	-2,	-2
	.data2	-2,	-2,	-2,	-2
	.data2	-2,	-2,	-2,	-2
sig.adr:
	.data2	sig1,	sig2,	sig3,	sig	
	.data2	sig5,	sig6,	sig7,	0
	.data2	0,	sig10,	sig11,	sig12
	.data2	sig13,	sig14,	sig15,	sig16
