.define _printf
.define _putchar
.define _getal
.define _char
.bss
_getal:
	.space	12
_char:
	.space	1
.data
sav:
	.long	0
.text
_printf:
	movem.l	d0/d1/d3/a0/a1/a2/a3/a4/a5/a6,.savreg
	move.l	(sp)+,sav	!return address
	move.l	sp,a6		!a6 <- address of arguments
	move.l	(a6)+,a5	!a5 <- address of format
	clr.l	d0		!d0 <- char to be printed
next:	move.b	(a5)+,d0
	beq	out
	cmp.b	#'%',d0
	beq	procnt
put:	move.l	d0,-(sp)
	jsr	_putchar	!argument is long en op de stack
	tst.l	(sp)+
	jmp	next

procnt:	move.b	(a5)+,d0
	cmp.b	#'d',d0		!NOTE: %d means unsigned.
	beq	digit
	cmp.b	#'s',d0
	beq	string
	cmp.b	#'%',d0		!second % has to be printed.
	beq	put
	tst.b	-(a5)		!normal char should be printed
	jmp	next

string:	move.l	(a6)+,a2	!a2 <- address of string
sloop:	move.b	(a2)+,d0
	beq	next
	move.l	d0,-(sp)
	jsr	_putchar	!argument is long en op de stack
	tst.l	(sp)+
	jmp	sloop

digit:	move.l	(a6)+,d1	!d1 <- integer
	move.l	#_getal+12,a3	!a3 <- ptr to last part of buf
	move.b	#0,-(a3)	!stringterminator
dloop:	move.l	d1,-(sp)
	move.l	#10,-(sp)
	jsr	.dvu		!d1 <- quotient d3 <- remainder
	add.l	#'0',d3
	move.b	d3,-(a3)
	tst.l	d1		!if quotient = 0 then ready
	bne	dloop
	move.l	a3,a2
	jmp	sloop		!print digitstring.

out:
	move.l	sav,-(sp)
	movem.l	.savreg,d0/d1/d3/a0/a1/a2/a3/a4/a5/a6
	rts


_putchar:
	movem.l	d0,.savreg
	move.l	4(sp),d0
	move.b	d0,_char
	move.l	#1,-(sp)
	move.l	#_char,-(sp)
	move.l	#1,-(sp)
	jsr	_write
	add.l	#12,sp
	movem.l	.savreg,d0
	rts
.align 2
