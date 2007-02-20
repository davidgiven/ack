#
! $Source$
! $State$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .bss

.define __sys_params_in
.comm __sys_params_in, 6

.define __sys_params_out
.comm __sys_params_out, 2
	
.comm opcode, 2
.comm returnto, 2
	
.sect .text


! Called on system call. This does *not* use the C calling convention:
!   ax: syscall number
!   stack: ( param3 param2 param1 - result ) 

.define .mon
.mon:
	mov (opcode), ax
	pop (returnto)
	
	cmp ax, 1
	je exit
	cmp ax, 3
	je read
	cmp ax, 4
	je write
	cmp ax, 5
	je open
	cmp ax, 6
	je nop_1 ! close
	cmp ax, 20
	je nop_0 ! getpid
	cmp ax, 35
	je nop_1 ! time
	cmp ax, 48
	je sigtrp
	cmp ax, 54
	je ioctl
	
	! Syscall not supported --- write out an error message and halt.	

unsupported:
	mov si, msg
1:
	lodsb
	andb al, al
	jz 2f
	movb ah, 0xE   ! service
	mov bx, 0x0007 ! page 0, white
	int 0x10
	jmp 1b
2:

	! Write out the syscall number.

	mov dx, (opcode)	
	mov cx, 4       ! 4 hex digits
1:
	rol	dx, 1       ! rotate so that highest 4 bits are at the bottom
	rol dx, 1
	rol dx, 1
	rol dx, 1
	mov	ax, 0xE0F	! ah = request, al = mask for nybble
	andb al, dl
	addb al, 0x90	! convert al to ascii hex (four instructions)
	daa
	adcb al, 0x40
	daa
	int	0x10
	loop 1b

	! Exit.
	
	jmp EXIT
	
.sect .rom
msg:
	.asciz 'NOSYS'
	
.sect .text

exit:
	jmp EXIT
	
read:
	mov ax, __sys_read
	jmp in_3_out_1
	
write:
	mov ax, __sys_write
	jmp in_3_out_1

open:
	add sp, 2*2
	jmp unimplemented
	
ioctl:
	mov ax, __sys_ioctl
	jmp in_3_out_0

sigtrp:
	add sp, 4
	jmp unimplemented

in_3_out_0:
	pop (__sys_params_in+0)
	pop (__sys_params_in+2)
	pop (__sys_params_in+4)
	call ax
	jmp out_0
	
in_3_out_1:
	pop (__sys_params_in+0)
	pop (__sys_params_in+2)
	pop (__sys_params_in+4)
	call ax
	jmp out_1
	
out_0:
	or ax, ax
	jnz failed
	push ax
	jmp return
	
out_1:
	or ax, ax
	jnz failed
	push (__sys_params_out)
	push ax
	jmp return

unimplemented:	
	mov ax, EBADMON
failed:
	push ax
	push ax
	jmp return

nop_1:
	add sp, 1*2
	jmp nop_0
nop_3:
	add sp, 3*2
nop_0:
	mov ax, 0
	push ax
	jmp return
	
return:
	jmp (returnto)
	