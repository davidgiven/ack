#
! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

#define EINVAL 22

! Perform a Linux system call.

.define __syscall
__syscall:
	lw r2, 0(sp)        ! syscall number
	addiu r2, r2, 4000  ! MIPS uses non-standard numbers
	lw r4, 4(sp)
	lw r5, 8(sp)
	lw r6, 12(sp)
	syscall 0

	beq r7, zero, exit  ! branch on success
	nop

	/* The result (in r2) is the errno. */
	lui at, ha16[_errno]
	sw r2, lo16[_errno] (at)
	li r2, -1
exit:
	jr ra
	nop

