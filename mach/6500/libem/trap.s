.define Trap
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs the trap instruction.

Trap:
	txa
	cmp #64
	bcc 1f
    2:	jmp Dotrap
    1:	bmi 2b
	pha
	lda IGNMASK	! get bitmask (lowbyte)
	sta ARTH
	lda IGNMASK+1	! get bitmask (highbyte)
    2:	lsr a
	ror ARTH	! shiftright bitmask n times
	dex
	bne 2b
	lda #1
	and ARTH
	bne 3f
	pla		! clear hardware_stack
	jmp Dotrap
    3:	pla		! clear hardware_stack
	rts

Dotrap:
	sta TRAPVAL
	lda #0
	cmp ERRPROC+1
	bne 1f		! ERRPROC <> 0 (highbyte)
	cmp ERRPROC
	bne 1f		! ERRPROC <> 0 (lowbyte)
	jmp Mtrap
    1:	lda #0
	ldx #RETURN
	jsr Ldi		! save return area
	lda hol0+5
	ldx hol0+4
	jsr Push	! save filename pointer
	lda hol0+1
	ldx hol0
	jsr Push	! save linenumber
	lda ERRPROC
	sta ADDR	! address of errorhandler (lowbyte)
	lda ERRPROC+1
	sta ADDR+1	! address of errorhandler (highbyte)
    	lda #0
	sta ERRPROC	! reset ERRPROC (lowbyte)
	sta ERRPROC+1	! reset ERRPROC (highbyte)
	ldx TRAPVAL
	jsr Push
	jmp (ADDR)	! proceed with errorhandler

Mtrap:
	cpx #0
	bne 1f
	lda #[EARRAY].h
	ldx #[EARRAY].l
	jsr Mprint
	jmp errorend
    1:	cpx #1
	bne 1f
	lda #[ERANGE].h
	ldx #[ERANGE].l
	jsr Mprint
	jmp errorend
    1:	cpx #2
	bne 1f
	lda #[ESET].h
	ldx #[ESET].l
	jsr Mprint
	jmp errorend
    1:	cpx #3
	bne 1f
	lda #[EIOVFL].h
	ldx #[EIOVFL].l
	jsr Mprint
	jmp errorend
    1:	cpx #10
	bne 1f
	lda #[ECONV].h
	ldx #[ECONV].l
	jsr Mprint
	jmp errorend
    1:	cpx #16
	bne 1f
	lda #[ESTACK].h
	ldx #[ESTACK].l
	jsr Mprint
	jmp errorend
    1:	cpx #17
	bne 1f
	lda #[EHEAP].h
	ldx #[EHEAP].l
	jsr Mprint
	jmp errorend
    1:	cpx #19
	bne 1f
	lda #[EODDZ].h
	ldx #[EODDZ].l
	jsr Mprint
	jmp errorend
    1:	cpx #20
	bne 1f
	lda #[ECASE].h
	ldx #[ECASE].l
	jsr Mprint
	jmp errorend
    1:	cpx #25
	bne 1f
	lda #[EBADMON].h
	ldx #[EBADMON].l
	jsr Mprint
	jmp errorend
    1:	cpx #26
	bne 1f
	lda #[EBADLIN].h
	ldx #[EBADLIN].l
	jsr Mprint
	jmp errorend
    1:	cpx #27
	bne errorend
	lda #[EBADGTO].h
	ldx #[EBADGTO].l
	jsr Mprint
errorend:
	ldx TRAPVAL
	jmp EXIT
