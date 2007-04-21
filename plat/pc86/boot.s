#
! $Source$
! $State$
! $Revision$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

! Some definitions.

BOOT_SEGMENT = 0x07C0        ! Where we've been loaded

#define PRINT(N) push ax; push bx; movb ah, 0x0E; movb al, N; mov bx, 0x0007; int 0x10; pop bx; pop ax

begtext:
	! This code makes up the PC boot sector, and is the first thing on the
	! floppy disk. The PC will load this sector to 0x07C0:0000 and jump to it
	! with dl set to our drive, but it won't necessarily do it in a sane way
	! (some BIOSes jump to 0x0000:7C00 instead). So, we need to fix that.

	jmpf BOOT_SEGMENT : start2
start2:
	! Set up the segment descriptors. We're running in tiny mode, so it's just
	! a matter of copying the contents of cs (already been set up by the jmpf)
	! into the other registers.

	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	
	! Initialise the stack, which will start at the top of our segment and work
	! down.
	
	mov sp, 0 ! the first push will wrap round to 0xFFFF
	
	! Some more startup housekeeping.
	
	sti
	cld

	! We're now set up for actual code. Write out our banner. Remember that
	! at this point dl contains our drive number, which we want to keep.

	mov si, banner_msg
	call write_string

	! Probe the drive to figure out its geometry.
	
	push dx	
	mov ax, 0x0800           ! service number
	int 0x13
	pop ax
	jc cant_boot
	
	! At this point:
	!   al: current drive
	!   cl: maximum sector number (bottom six bits)
	!   dh: maximum head number
	! We don't care about the rest.
	andb cl, 0x3F
	
	! We now need to go through a loop loading each sector in turn.
	! During this loop, the registers will be set up as follows:
	!   al: current cylinder
	!   ah: maximum head
	!   bx: address
	!   cl: current sector (one based)
	!   ch: maximum sector (one based)
	!   dl: current drive
	!   dh: current head
	! Why, yes, they are painstakingly shoehorned in to get all the data
	! into registers.
	
	movb dl, al
	movb ch, cl
	movb ah, dh
	movb al, 0                ! start on cylinder 0
	mov bx, 0x0200           ! don't overwrite boot sector
	movb cl, 2                ! start on sector 2 (skip boot sector)
	movb dh, 0                ! start on head 0
	
1:
	call read_sector
	
	! Next memory area.
	
	add bx, 0x0200
	cmp bx, enddata
	ja finished
	
	! Next sector.
	
	incb cl
	cmpb cl, ch
	jle 1b
	movb cl, 1               ! back to sector 1 again
	
	! Next head.
	
	incb dh
	cmpb dh, ah
	jle 1b
	movb dh, 0               ! back to head 1 again
	
	! Next cylinder.
	
	incb al
	jmp 1b
	
cant_boot:
	mov si, bootfail_msg
	call write_string
	jmp EXIT

	! Reads a sector into memory. The parameters are:
	!   al: cylinder
	!   bx: address
	!   cl: sector
	!   dl: drive
	!   dh: head
	! If an error occurs, it'll automatically try again. And again.
	! And again...
	
read_sector:
	push ax
	push bx
	push cx
	push dx
	
#if 0
	push dx
	xorb dh, dh
	movb dl, cl
	call write_hex4
	pop dx
	PRINT(0x20)
	push dx
	movb dl, dh
	xorb dh, dh
	call write_hex4
	pop dx
	PRINT(0x20)
	push dx
	movb dl, al
	xorb dh, dh
	call write_hex4
	pop dx
#endif
	
1:
	movb ch, al
	mov ax, 0x0201           ! service 2, read one sector
	int 0x13
	jc 2f
	
	mov ax, 0x0E2E           ! write out a .
	mov bx, 0x0007           ! page 0, white
	int 0x10
	
	pop dx
	pop cx
	pop bx
	pop ax
	ret

	! If a read fail occurs, the spec (such as it is) states that we need
	! to reset the fd controller and try again.
2:
	push ax
	push bx
	
	mov ax, 0x0E21           ! write out a !
	mov bx, 0x0007           ! page 0, white
	int 0x10
	
	mov ax, 0x0000
	int 0x13
	
	pop bx
	pop ax
	jmp 1b

	! Waits for a keystroke (and then discards it).
	
pause:
	push ax
	xorb ah, ah
	int 0x16
	pop ax
	ret
	
	! This utility writes the string pointed to by ds:si out to the console.

write_string:
	push ax
	push bx
1:
	lodsb
	andb al, al
	jz 2f
	movb ah, 0xE   ! service
	mov bx, 0x0007 ! page 0, white
	int 0x10
	jmp 1b
2:
	pop bx
	pop ax
	ret
	
	! Writes out the contents of dx as hex.

write_hex4:
	push ax
	push cx
	mov cx, 4      ! 4 hex digits
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
	pop cx
	pop ax
	ret

	! Everything loaded successfully!
	!
	! We now need to do some setup and start the program itself.
		
finished:
	mov si, running_msg
	call write_string
	
	! Wipe the bss. (I'm a little suprised that __m_a_i_n doesn't do this.)
	
	mov di, begbss
	mov cx, endbss
	sub cx, di
	mov ax, 0
	rep stosb
	
	! Push standard parameters onto the stack and go.
	
	xor ax, ax
	push ax                  ! argc
	push ax                  ! argv
	push ax                  ! envp
	call __m_a_i_n
	! fall through into the exit routine.
	
	! Halts, waits for a keypress, and reboots. This also becomes the
	! application termination routine.
	
.define __exit
.extern __exit
.define EXIT
.extern EXIT
__exit:
EXIT:
	mov si, halted_msg
	call write_string
	
1:
	jmp 1b
	
	xor ax, ax
	int 0x16 ! get key
	int 0x19 ! reboot

	! Some text messages.
	
banner_msg: .asciz 'ACKBOOT\n\r'
nl_msg = banner_msg + 7 ! cheap trick

bootfail_msg: .asciz 'Unable to boot!\n\r'
loading_msg: .asciz '\n\rLoading...\n\r'
halted_msg: .asciz '\n\rHalted.\n\r'
running_msg: .asciz '\n\rRunning.\n\r'
 
	! ...and we need this to fool the PC into booting our boot sector.
	
	.align 510
	.data2 0xAA55


.define begtext,begdata,begbss
.define ERANGE,ESET,EHEAP,ECASE,EILLINS,EIDIVZ,EODDZ
.extern _end

! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

! Some magic data. All EM systems need these.

.define .trppc, .ignmask
.comm .trppc, 4
.comm .ignmask, 4
