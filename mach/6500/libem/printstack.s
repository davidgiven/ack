.define Printstack
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This a special subroutine which prints some things to the
! monitorscreen for debugging.


Printstack:
	ldy #0
    2:	lda (hol0+4),y
	beq 1f
	jsr WRCH	! print filename
	iny
	jmp 2b
    1:	lda #32
	jsr WRCH	! print a space
	lda hol0+1
	jsr Printhex	! print line number (highbyte)
	lda hol0
	jsr Printhex	! print line number (lowbyte)
	lda #32
	jsr WRCH	! print a space
	lda SP+1
	jsr Printhex	! print stackpointer (highbyte)
	lda SP+2
	jsr Printhex	! print stackpointer (lowbyte)
	lda #32
	jsr WRCH	! print a space
	lda LB+1
	jsr Printhex	! print real localbase (highbyte)
	lda LB
	jsr Printhex	! print real localbase (lowbyte)
	lda #32
	jsr WRCH	! print a space
	lda LBl+1
	jsr Printhex	! print second lowerbase (highbyte)
	lda LBl
	jsr Printhex	! print second lowerbase (lowbyte)
	lda #10
	jsr WRCH	! print a newline
	lda #13
	jsr WRCH	! print a carriagereturn
	rts


