.extern _exit
! Fake system call for 68000 running MACSBUG

.text

_exit:
	jmp 0x0200f6
