.define _read,_write,_ioctl,_getpid,_open,_close,_exit,_errno
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_read:
	ld (savebc),bc
	push af
	pop bc
	ld (saveaf),bc	! save all registers in savereg
	ld (savede),de
	ld (savehl),hl
	ld (saveix),ix
	ex	(sp),hl	! return address in hl
	pop	bc	! skip return address
	pop	bc	! get fd
	ld	a,b	! check fd = 0
	or	c
	jr	nz,errrd
	pop	de	! get buffer
	pop	bc	! get count
	ld	ix,0	! reset counter
	push	bc
	push	de
	push	ix
	push	hl	! return address
	ex	de,hl	! buffer to hl
1:	ld	a,b
	or	c
	jr	z,done	! done if count = 0
	call	getchr
	ld	(hl),a
	inc	hl	! increment pointer
	inc	ix	! increment char counter
	dec	bc	! decrement count
	cp	0xA
	jr	nz,1b	! done if char = CR
done:
	ld bc,(saveaf)
	push bc
	pop af
	ld bc,(savebc)
	ld de,(savede)
	ld hl,(savehl)
	ld ix,(saveix)
	ret
errrd:	
	push	bc
	push	hl	! return address
	ld bc,(saveaf)
	push bc
	pop af
	ld bc,(savebc)
	ld de,(savede)
	ld hl,(savehl)
	ld ix,(saveix)
	ld	ix,-1
	ret

_write:
	ld (savebc),bc
	push af
	pop bc
	ld (saveaf),bc	! save all registers in savereg
	ld (savede),de
	ld (savehl),hl
	ld (saveix),ix
	ex	(sp),hl	! return address in hl
	pop	de	! skip return address
	pop	de	! get fd
	ld	a,e	! check for fd = 1
	cp	1
	jr	nz,errwr
	ld	a,d
	or	a
	jr	nz,errwr
	pop	de      ! buffer in de
	pop	bc	! count in bc
	push	bc
	push	de
	push	de
	push	hl
	ex	de,hl	! buffer in hl
	ld	e,c
	ld	d,b	! count also in de
1:	ld	a,b
	or	c
	jr	z,exit
	ld	a,(hl)
	call	putchr
	inc	hl
	dec	bc
	jr	1b
errwr:
	push	de
	push	hl
	ld bc,(saveaf)
	push bc
	pop af
	ld bc,(savebc)
	ld de,(savede)
	ld hl,(savehl)
	ld ix,(saveix)
	ld	ix,-1	! error in fd
	ret
exit:
	push	de	! count on stack
	ld bc,(saveaf)
	push bc
	pop af
	ld bc,(savebc)
	ld de,(savede)
	ld hl,(savehl)
	ld ix,(saveix)
	pop	ix	! return count to caller
	ret

_ioctl:
	ret
_getpid:
	ret

! open return a file descriptor (0,1,2)
! depending on 'mode'
! mode 2 doesn't work!!
_open:
	ld (savebc),bc
	push af
	pop bc
	ld (saveaf),bc	! save all registers in savereg
	ld (savede),de
	ld (savehl),hl
	ld (saveix),ix
	pop	bc	! return address
	pop	de	! name pointer
	pop	ix	! mode (0 for read,
			! 1 for write)
	push	ix
	push	de
	push	bc
	ld bc,(saveaf)
	push bc
	pop af
	ld bc,(savebc)
	ld de,(savede)
	ld hl,(savehl)
	ld ix,(saveix)
	ret		! return fd = 0 for read
			! fd = 1 for write

_close:
	ld	ix,0	! return succes
	ret
_exit:
jp 0x38
.sect .data
_errno:
	.data2	0
! output routine in monitor
CRT	= 0x013B
! output a charcter
! entry: ascii character in a
.sect .text
!putchr:
!	push	hl
!	push	bc
!	ld	hl,tab
!	ld	b,5
!1:	cp	(hl)
!	jr	z,fetch
!	inc	hl
!	inc	hl
!	djnz	1b
!2:	call	CRT
!	pop	bc
!	pop	hl
!	ret
!fetch:	inc	hl
!	ld	a,(hl)
!	jr	2b
!! conversion table for nascom characters
!tab:	.data1	0x0D,0x00
!	.data1	0x1B,0x1E
!	.data1	0x08,0x1D
!	.data1	0x0A,0x1F
!	.data1	0x7F,0x00

KBD	= 0x69
! get character from keyboard
getchr:
	call	KBD
	jr	nc,getchr
	cp	0x1F
	jr	z,CR
	cp	0x1D
	jr	z,BS
	ret
CR:	ld	a,0xA
	ret
BS:	ld	a,0x8
	ret
