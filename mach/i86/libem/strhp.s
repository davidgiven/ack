! $Source$
! $State$
! $Revision$

.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	.strhp
.extern .reghp, .limhp, EHEAP, .trp

! Updates the heap pointer:
!
! int .strhp(void* newpointer)
!
! .reghp is the current heap pointer;
! .limhp is the current top of memory.
!
! If the desired new heap pointer is above the top of memory, then BRK is
! called to extend the memory.

.strhp:
	pop	bx
	pop	ax
	mov	(.reghp),ax
	cmp	ax,(.limhp)
	jb	1f
	add	ax,02000
	and	ax,~0777
	push	bx
	push	ax
	call	BRK
	pop	cx
	pop	bx
	cmp	ax,-1
	je	2f
1:
	mov	(.limhp),cx
	jmp	bx
2:
	mov	ax,EHEAP
	push	bx
	jmp	.trp
