.define .gto
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern .gto

.gto:
	! nonlocal goto
	! the argument on the stack is a pointer to a GTO-descriptor containing:
	!  - the new local base
	!  - the new stackpointer
	!  - the new program counter
	!
	! The main task of the GTO instruction is to restore the registers
	! used for local variables. It uses a word stored in each stackframe,
	! indicating how many data -and address registers the procedure of
	! that stackframe has.

	move.l	(sp)+,a0
	add.l	#8,a0		! a0 now points to new local base entry
				! of the descriptor
	cmp.l	(a0),a6		! GTO within same procedure?
	beq  	noregs
	move.l	d0,savd0	! gto may not destroy the return area
	move.l	d1,savd1
1:
	tst.l	(a6)
	beq  	err
	unlk	a6
	move.w	(sp)+,d0	! word indicating which regs. were saved
	jsr	restr
	cmp.l	(a0),a6
	bne	1b
	move.l	savd0,d0
	move.l	savd1,d1
noregs:
	move.l	-4(a0),sp
	move.l	-8(a0),a0	! new program counter
	jmp	(a0)
err:
	move.w	#EBADGTO,-(sp)
	jmp	.fat

restr:
	! restore the registers. Note that scratch register a0 may
	! not be changed here. d0 contains (8*#addr.regs + #data regs.)
	! note that registers are assigned in the order d7,d6 .. and
	! a5,a4...

	move.l	(sp)+,d2	! return address; can't use a0 here
	move.w	d0,d1
	and.l	#7,d0		! #data registers
	asl.l	#1,d0		! * 2
	lea	etabd,a1
	sub.l	d0,a1
	jmp	(a1)
	move.l	(sp)+,d3
	move.l	(sp)+,d4
	move.l	(sp)+,d5
	move.l	(sp)+,d6
	move.l	(sp)+,d7
etabd:
	and.l	#070,d1
	asr.l	#2,d1		! # address registers
	lea	etaba,a1
	sub.l	d1,a1
	jmp	(a1)
	move.l	(sp)+,a2
	move.l	(sp)+,a3
	move.l	(sp)+,a4
	move.l	(sp)+,a5
etaba:
	move.l	d2,a1
	jmp	(a1)		! return
.sect .data
savd0:	.data4 0
savd1:	.data4 0
.sect .text
