.extern _exit
.sect .text
.sect .rom
.sect .data
.sect .bss
! Fake system call for 68000 running MACSBUG

.sect .text

_exit:
	jmp 0x0200f6
