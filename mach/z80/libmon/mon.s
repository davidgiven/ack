.define .mon

! Monitor call
! Expects on stack:	monitor call number
!			parameters
! Implemented are the following monitor calls:
! number  1:	exit
! number  3:	read
! number  4:	write
! number  5:	open
! number  6:	close
! number 54:	ioctl
! If called with a number of a call that is not implemented,
! a trap is generated.

.mon:
	pop ix 	! returnaddress

	pop hl		! monitor call number
	ld a,l
	cp 1
	jp z,monexit	! is it an exit?
	cp 3
	jp z,monread	! is it a read?
	cp 4
	jp z,monwrite	! is it a write?
	cp 5
	jp z,monopen	! is it an open?
	cp 6
	jp z,monclose	! is it a close?
	cp 54
	jp z,monioctl
	jp ebadmon	! trap

monexit:
	jp 0x38

monread:
	pop hl		! file-descriptor, not used
	pop hl		! hl = pointer to output buffer
	pop de		! de = number of bytes to be read
	ld bc,0		! bc will contain the number of bytes actually read
1:	ld a,d
	or e
	jr z,2f
	call getchar
	push af
	call putchar	! echo character
	pop af
	ld (hl),a
	inc hl
	inc bc
	dec de
	cp 0x0A	! is it a newline?
	jp nz,1b
2:	push bc
	ld hl,0
	push hl
	jp (ix)

monwrite:
	pop hl		! file-descriptor, not used
	pop hl		! hl = pointer to output buffer
	pop de		! de = number of bytes
	push de
1:	ld a,e
	or d
	jr z,2f
	ld a,(hl)
	call putchar
	inc hl
dec de
	jp 1b

2:	push de		! no error
	jp (ix)


monopen:
	pop hl		! pointer to string
	pop hl		! flag
	ld hl,-1
	push hl		! push file descriptor
	push hl		! push error code twice
	push hl
	jp (ix)

monclose:
	ex (sp),hl		! pop file descriptor and push error code
	pop hl		! file descriptor
	ld hl,-1
	push hl		! push error code twice
	push hl
	jp (ix)

monioctl:
	pop hl		! file descriptor
	pop hl		! request
	ld hl,0
	ex (sp),hl		! remove argp and push error code
	jp (ix)

