.define getchar, putchar

! These getchar and putchar routines can be used for HERMAC computer

! Read a character from HERMAC-monitor
! Character is returned in a-reg

getchar:
	in 0xF1
	ani 1
	jz getchar
	in 0xF0
	cpi 0x0D
	jnz 1f
	mvi a,0x0A
1:	ret

! Write character on HERMAC monitor
! Assumes character in a-reg

putchar:
	cpi 0x0A
	jnz 1f
	mvi a,0x1F
1:	push psw
2:	in 0xF1
	ani 4
	jz 2b
	pop psw
	out 0xF0
	ret
