.define getchar, putchar
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! These getchar and putchar routines can be used for NASCOM computer.

! Read character from NASCOM-monitor
! Character is returned in a-reg

getchar:
	call 0x69
	jnc getchar
	cpi 0x1F
	jz CR
	cpi 0x1D
	jz BS
	ret

CR:	mvi a,0x0A
	ret

BS:	mvi a,0x08
	ret

! Write charcacter on NASCOM-monitor
! Assumes character in a-reg

putchar:
	push h
	push b
	lxi h,tab
	mvi b,5
1:	cmp m
	jz fetch
	inx h
	inx h
	dcr b
	jnz 1b
2:	call 0x013B
	pop b
	pop h
	ret
fetch:	inx h
	mov a,m
	jmp 2b
! conversion table for NASCOM characters
tab:	.data1 0x0D,0x00
	.data1 0x1B,0x1E
	.data1 0x08,0x1D
	.data1 0x0A,0x1F
	.data1 0x7F,0x00

