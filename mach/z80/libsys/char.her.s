.define getchar, putchar
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! These getchar and putchar routines can be used for HERMAC computer

! Read a character from HERMAC-monitor
! Character is returned in a-reg

getchar:
	in a,0xF1
	and 1
	jp z,getchar
	in a,0xF0
	cp 0x0D
	jp nz,1f
	ld a,0x0A
1:	ret

! Write character on HERMAC monitor
! Assumes character in a-reg

putchar:
	cp 0x0A
	jp nz,1f
	ld a,0x1F
1:	push af
2:	in a,0xF1
	and 4
	jp z,2b
	pop af
	out 0xF0,a
	ret
