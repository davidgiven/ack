.text
.globl sigtrp~
.globl trp~,save~,retu~

indir	= 0
signal	= 48.

rti	= 2

sig1:	mov	sig.trp+0.,-(sp)
	br	1f
sig2:	mov	sig.trp+2.,-(sp)
	br	1f
sig3:	mov	sig.trp+4.,-(sp)
	br	1f
sig4:	mov	sig.trp+6.,-(sp)
	br	1f
sig5:	mov	sig.trp+8.,-(sp)
	br	1f
sig6:	mov	sig.trp+10.,-(sp)
	br	1f
sig7:	mov	sig.trp+12.,-(sp)
	br	1f
sig10:	mov	sig.trp+18.,-(sp)
	br	1f
sig11:	mov	sig.trp+20.,-(sp)
	br	1f
sig12:	mov	sig.trp+22.,-(sp)
	br	1f
sig13:	mov	sig.trp+24.,-(sp)
	br	1f
sig14:	mov	sig.trp+026.,-(sp)
	br	1f
sig15:	mov	sig.trp+028.,-(sp)
	br	1f
sig16:	mov	sig.trp+030.,-(sp)
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
	cmp	r0,$16.
	bhi	sig.bad
	mov	r0,call+02
	asl	r0
	mov	sig.trp-2(r0),r3
	cmp	r1,$256.
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
	sys	indir ; call
	bcs	sigbad
	asr	r0
	bcc	1f
	mov	$-3,-(sp)
	clr	-(sp)
	jmp	retu~
1:	mov	r3,-(sp)
	clr	-(sp)
	jmp	retu~

.data
call:	sys	signal;	0; 0
sig.trp:
	-2;	-2;	-2;	-2
	-2;	-2;	-2;	-2
	-2;	-2;	-2;	-2
	-2;	-2;	-2;	-2
sig.adr:
	sig1;	sig2;	sig3;	sig4
	sig5;	sig6;	sig7;	0
	0;	sig10;	sig11;	sig12
	sig13;	sig14;	sig15;	sig16
