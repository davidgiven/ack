.define .nop
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text



! NOP
! changed into output routine to print linenumber
! in octal (6 digits)

.nop:
	push iy
	ld iy,1f+5
	ld hl,(hol0)
	call outdec
	ld iy,1f+18
	ld hl,0
	add hl,sp
	call octnr
	ld de,20
	push de
	ld de,1f
	push de
	ld de,1
	push de
	call __write
	pop de
	pop de
	pop de
	pop iy
	ret
1:	.asciz 'test xxxxx 0xxxxxx\r\n'

octnr:
	ld b,6
1:	ld a,7
	and l
	add a,'0'
	dec iy
	ld (iy+0),a
	srl h
	rr l
	srl h
	rr l
	srl h
	rr l
	djnz 1b
	ret


