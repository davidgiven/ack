.define Mon
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs some monitor calls.
! The exit call just resets the hardware_stackpointer so
! this routine will return to the operating system.
! The close call just returns a zero.
! The ioctl call just pops its arguments and returns a zero.
! The write routine is a real one.


Mon:
	cpx #1
	bne 1f		! exit
	ldx STACK	! load stackpointer
	dex
	dex		! adjust stackpointer
	txs		! set stackpointer
	rts
    1:	cpx #4
	bne 1f
	jmp Mwrite
    1:	cpx #6		! facked
	bne 1f		! close
	lda #0
	tax		! return zero
	rts
    1:	cpx #54
	jsr Pop		! pop first argument (fildes)
	jsr Pop		! pop second argument (request)
	jsr Pop		! pop third argument (argp)
	lda #0
	tax		! return zero
	rts


