#
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

bdos: ! BDOS entry point
	out 0xff
	ora a
	ret

COLDSTART: ! system startup entry point --- this needs to be four bytes after FBASE.
	jmp boot	    ! 0: Cold start routine
bios:
	jmp wboot	! 1: Warm boot - reload command processor
	jmp const	! 2: Console status
	jmp conin	! 3: Console input
	jmp conout	! 4: Console output
	jmp list	    ! 5: Printer output
	jmp punch	! 6: Paper tape punch output
    jmp reader	! 7: Paper tape reader input
	jmp home	    ! 8: Move disc head to track 0
	jmp seldsk	! 9: Select disc drive
	jmp settrk	!10: Set track number
	jmp setsec	!11: Set sector number
	jmp setdma	!12: Set DMA address
	jmp read     !13: Read a sector
	jmp write	!14: Write a sector

boot:
	xra a
	sta 3 ! iobyte
	sta 4 ! drive
	! falls through
wboot:
	mvi a, 0xc3 ! jmp
	sta 0
	sta 5

	lxi h, bios
	shld 1

	lxi h, bdos
	shld 6

	lda 4 ! get the current drive/user
	mov c, a
	out 1

const:
	out 2
	ora a
	ret

conin:
	out 3
	ora a
	ret

conout:
	out 4
	ora a
	ret

list:
	out 5
	ora a
	ret

punch:
	out 6
	ora a
	ret

reader:
	out 7
	ora a
	ret

home:
	out 8
	ora a
	ret

seldsk:
	out 9
	ora a
	ret

settrk:
	out 10
	ora a
	ret

setsec:
	out 11
	ora a
	ret

setdma:
	out 12
	ora a
	ret

read:
	out 13
	ora a
	ret

write:
	out 14
	ora a
	ret

