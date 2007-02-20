#
! $Source$
! $State$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

! This file contains the code necessary to extend the ACK heap. This is called
! by a i86/libem helper function called .strhp, which takes care of updating
! some magic global variables --- defined here.

! Pointer to the current top of the heap.

.sect .data
.define .reghp
.reghp:
	.data2 endbss
	
! Pointer to the current top of memory.

.sect .data
.define .limhp
.limhp:
	.data2 endbss
	
! Claims more memory from the system, but does not actually change those
! global variables (.strhp does that). This does not use the C calling
! convention!
!
! Stack: ( desired_limhp : actual_limhp )
! Also returns: ax = -1 on failure

.sect .text
.define BRK
BRK:
	pop bx ! holds return address
	pop ax ! holds desired limhp

	cmp ax, sp               ! compare sp with si
	jae fail                 ! si too big? (Overlaps stack?)
	cmp ax, endbss           ! compare with bottom of heap
	jb fail                  ! si too small? (Overlaps bss?)
	
return:
	push ax                  ! success
	jmp bx
	
fail:
	mov ax, -1
	jmp return
