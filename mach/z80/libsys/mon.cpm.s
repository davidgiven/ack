.define .mon
.define uxfinish

! monitor instruction
! a small collection of UNIX system calls implemented under CP/M

!	ux_indir=e.mon
!	ux_fork=e.mon
!	ux_wait=e.mon
!	ux_link=e.mon
!	ux_exec=e.mon
!	ux_chdir=e.mon
!	ux_mknod=e.mon
!	ux_chmod=e.mon
!	ux_chown=e.mon
!	ux_break=e.mon
!	ux_stat=e.mon
!	ux_seek=e.mon
!	ux_mount=e.mon
!	ux_umount=e.mon
!	ux_setuid=e.mon
!	ux_getuid=e.mon
!	ux_stime=e.mon
!	ux_ptrace=e.mon
!	ux_alarm=e.mon
!	ux_fstat=e.mon
!	ux_pause=e.mon
!	ux_utime=e.mon
!	ux_stty=e.mon
!	ux_gtty=e.mon
!	ux_access=e.mon
!	ux_nice=e.mon
!	ux_sync=e.mon
!	ux_kill=e.mon
!	ux_dup=e.mon
!	ux_pipe=e.mon
!	ux_times=e.mon
!	ux_prof=e.mon
!	ux_unused=e.mon
!	ux_setgid=e.mon
!	ux_getgid=e.mon
!	ux_sig=e.mon
!	ux_umask=e.mon
!	ux_chroot=e.mon

	EPERM	= 1
	ENOENT	= 2
	ESRCH	= 3
	EINTR	= 4
	EIO	= 5
	ENXIO	= 6
	E2BIG	= 7
	ENOEXEC	= 8
	EBADF	= 9
	ECHILD	= 10
	EAGAIN	= 11
	ENOMEM	= 12
	EACCES	= 13
	EFAULT	= 14
	ENOTBLK	= 15
	EBUSY	= 16
	EEXIST	= 17
	EXDEV	= 18
	ENODEV	= 19
	ENOTDIR	= 20
	EISDIR	= 21
	EINVAL	= 22
	ENFILE	= 23
	EMFILE	= 24
	ENOTTY	= 25
	ETXTBSY	= 26
	EFBIG	= 27
	ENOSPC	= 28
	ESPIPE	= 29
	EROFS	= 30
	EMLINK	= 31
	EPIPE	= 32
	EDOM	= 33
! Structure of filearea maintained by this implementation
! First iobuffer of 128 bytes
! Then the fcb area of 36 bytes
! The number of bytes left in the buffer, 1 byte
! The iopointer into the buffer, 2 bytes
! The openflag 0 unused, 1 reading, 2 writing, 1 byte
! The filedescriptor starting at 3, 1 byte
! The number of CTRL-Zs that have been absorbed, 1 byte
! The byte read after a sequence of CTRL-Zs, 1 byte

	maxfiles=8
	filesize=128+36+1+2+1+1+1+1

	filefcb=0	! pointers point to fcb
	position=33
	nleft=36
	iopointer=37
	openflag=39
	fildes=40
	zcount=41
	zsave=42

	.errnz filefcb

0:	.space maxfiles*filesize
	filearea = 0b+128
sibuf:
	.word 0
	.space 82
siptr:	.space 2
saveargs:
	.space 128
argc:	.space 2
ttymode:.byte 9,9,8,21;.short 06310+RAW*040	! raw = 040

return:
	.word 0,0
uxinit:
	xor a
	ld c,maxfiles
	ld hl,0b
1:	ld b,filesize
2:	ld (hl),a
	inc hl
	djnz 2b
	dec c
	jr nz,1b
	ret

uxfinish:
	ld a,maxfiles-1
1:	push af
	call closefil
	pop af
	dec a
	cp 0377
	jr nz,1b
	ret

.mon:
	pop ix
	ld (return),ix	! return adres
	pop de		! system call number
	xor a
	or d
	jr nz,unimpld	! too big
	ld a,e
	and 0300	! only 64 system calls
	jr nz,unimpld
	sla e
	ld hl,systab
	add hl,de
	ld e,(hl)
	inc hl
	ld d,(hl)
	ex de,hl
	jp (hl)

systab:	
	.word e.mon	! ux_indir
	.word ux_exit
	.word e.mon	! ux_fork
	.word ux_read
	.word ux_write
	.word ux_open
	.word ux_close
	.word e.mon	! ux_wait
	.word ux_creat
	.word e.mon	! ux_link
	.word ux_unlink
	.word e.mon	! ux_exec
	.word e.mon	! ux_chdir
	.word ux_time
	.word e.mon	! ux_mknod
	.word e.mon	! ux_chmod
	.word e.mon	! ux_chown
	.word e.mon	! ux_break
	.word e.mon	! ux_stat
	.word e.mon	! ux_seek
	.word ux_getpid
	.word e.mon	! ux_mount
	.word e.mon	! ux_umount
	.word e.mon	! ux_setuid
	.word e.mon	! ux_getuid
	.word e.mon	! ux_stime
	.word e.mon	! ux_ptrace
	.word e.mon	! ux_alarm
	.word e.mon	! ux_fstat
	.word e.mon	! ux_pause
	.word e.mon	! ux_utime
	.word e.mon	! ux_stty
	.word e.mon	! ux_gtty
	.word e.mon	! ux_access
	.word e.mon	! ux_nice
	.word ux_ftime
	.word e.mon	! ux_sync
	.word e.mon	! ux_kill
	.word unimpld
	.word unimpld
	.word unimpld
	.word e.mon	! ux_dup
	.word e.mon	! ux_pipe
	.word e.mon	! ux_times
	.word e.mon	! ux_prof
	.word e.mon	! ux_unused
	.word e.mon	! ux_setgid
	.word e.mon	! ux_getgid
	.word e.mon	! ux_sig
	.word unimpld
	.word unimpld
	.word unimpld
	.word unimpld
	.word unimpld
	.word ux_ioctl
	.word unimpld
	.word unimpld
	.word unimpld
	.word unimpld
	.word unimpld	! ux_exece
	.word e.mon	! ux_umask
	.word e.mon	! ux_chroot
	.word unimpld
	.word unimpld

emptyfile:
	! searches for a free filestructure
	! returns pointer in iy, 0 if not found
	ld ix,filearea
	ld l,maxfiles
1:
	xor a
	or (ix+openflag)
	jr nz,3f
	ld a,maxfiles+3
	sub l
	ld (ix+fildes),a
! #ifdef	CPM1
	push iy
	push ix
	ld de,-128
	add ix,de
	push ix
	pop de
	ld c,setdma
	call bdos
	pop ix
	pop iy
	or a		! to clear C
! #endif
	ret
3:
	ld de,filesize
	add ix,de
	dec l
	jr nz,1b
	scf
	ret

findfile:
	ld ix,filearea
	ld de,filesize
0:
	dec a
	ret m
	add ix,de
	jr 0b

getchar:
	push iy
	push de
	push hl
	dec (ix+nleft)
	jp p,0f
	push ix
	pop hl
	ld de,-128
	add hl,de
	ld (ix+iopointer),l
	ld (ix+iopointer+1),h
	ex de,hl
	push ix
	ld c,setdma
	call bdos
! #ifdef	CPM1
	ld c,seqread
! #else
! 	ld c,randomread
! #endif
	pop de
	call bdos
	or a
	jr z,1f
	ld (ix+zcount),0
	pop hl
	pop de
	pop iy
	scf
	ret
1:
	inc (ix+position)
	jr nz,2f
	inc (ix+position+1)
2:
	ld a,127
	ld (ix+nleft),a
0:
	ld h,(ix+iopointer+1)
	ld l,(ix+iopointer)
	ld a,(hl)
	inc hl
	ld (ix+iopointer),l
	ld (ix+iopointer+1),h
	pop hl
	pop de
	pop iy
	ret
	or a

putchar:
	push hl
	ld h,(ix+iopointer+1)
	ld l,(ix+iopointer)
	ld (hl),a
	dec (ix+nleft)
	jr z,0f
	inc hl
	ld (ix+iopointer+1),h
	ld (ix+iopointer),l
	pop hl
	ret
0:
	pop hl
flsbuf:
	push hl
	push de
	push iy
	push ix
	pop hl
	ld de,-128
	add hl,de
	ld (ix+iopointer+1),h
	ld (ix+iopointer),l
	ex de,hl
	push ix
	ld c,setdma
	call bdos
	pop de
! #ifdef	CPM1
	ld c,seqwrite
! #else
! 	ld c,randomwrite
! #endif
	call bdos
	or a
	jr z,1f
	pop iy
	pop de
	pop hl
	scf
	ret
1:
	inc (ix+position)
	jr nz,2f
	inc (ix+position+1)
2:
	ld a,128
	ld (ix+nleft),a
	ld b,a
	push ix
	pop hl
	ld de,-128
	add hl,de
	ld a,26			! ctrl z
1:	ld (hl),a
	inc hl
	djnz 1b
	pop iy
	pop de
	pop hl
	or a
	ret

parsename:
	! parses file name pointed to by hl and fills in fcb
	! of the file pointed to by ix.
	! recognizes filenames as complicated as 'b:file.zot'
	! and as simple as 'x'

	push iy
	push ix
	pop de
	xor a
	push de
	ld b,36		! sizeof fcb
0:	ld (de),a
	inc de
	djnz 0b
	pop de
	inc hl
	ld a,(hl)
	dec hl
	cp ':'		! drive specified ?
	jr nz,1f
	ld a,(hl)
	inc hl
	inc hl
	dec a
	and 15
	inc a		! now 1<= a <= 16
	ld (de),a
1:	inc de
	ld b,8		! filename maximum of 8 characters
1:	ld a,(hl)
	or a
	jr nz,8f
	dec hl
	ld a,'.'
8:
	inc hl
	cp '.'
	jr z,2f
	and 0177	! no parity
	bit 6,a
	jr z,9f
	and 0337	! UPPER case
9:
	ld (de),a
	inc de
	djnz 1b
	ld a,(hl)
	inc hl
	cp '.'
	jr z,3f
	ld a,' '
	ld (de),a
	inc de
	ld (de),a
	inc de
	ld (de),a
	pop iy
	ret		! filenames longer than 8 are truncated
2:	ld a,' '	! fill with spaces
0:	ld (de),a
	inc de
	djnz 0b
3:	ld b,3		! length of extension
1:	ld a,(hl)
	inc hl
	or a
	jr z,4f
	cp 0100
	jp m,2f
	and 0137
2:	ld (de),a
	inc de
	djnz 1b
	pop iy
	ret
4:	ld a,' '
0:	ld (de),a
	inc de
	djnz 0b
	pop iy
	ret

! various routines
ux_close:
	pop hl
	ld a,l
	sub 3
	jp m,1f
	cp maxfiles
	call m,closefil
1:	ld hl,0
	push hl ; jr rtn

closefil:
	call findfile
	ld a,(ix+openflag)
	or a
	jr z,3f
	ld (ix+openflag),0
	cp 1
	jr z,2f
	ld a,(ix+nleft)
	cp 128
	jr z,2f
	call flsbuf
2:
	push iy
	push ix
	pop de
	ld c,close
	call bdos
	pop iy
3:	ret

ux_ioctl:
	pop hl
	ld a,l
	sub 3
	jp p,1f
	pop hl
	ld a,h
	cp 't'
	jr nz,e.mon
	ld a,l
	cp 8
	jr z,tiocgetp
	cp 9
	jr z,tiocsetp
	jr e.mon
1:	pop hl
	pop hl
	ld hl,-1
	push hl ; jr rtn
tiocgetp:
	pop de
	ld hl,ttymode
2:	push bc
	ld bc,6
	ldir
	ld h,b
	ld l,c
	pop bc
	push hl ; jr rtn
tiocsetp:
	pop hl
	ld de,ttymode
	jr 2b

ux_time:
	call time4
rtn:	ld ix,(return) ; jp (ix)

ux_ftime:
	pop hl
	ld (retarea+6),hl
	call time4
	ld hl,(retarea+6)
	pop de
	ld (hl),e
	inc hl
	ld (hl),d
	inc hl
	pop de
	ld (hl),e
	inc hl
	ld (hl),d
	inc hl
	xor a
	ld (hl),a
	inc hl
	ld (hl),a
	inc hl
	ld (hl),a
	inc hl
	ld (hl),a
	inc hl
	ld (hl),a
	inc hl
	ld (hl),a
	ld ix,(return) ; jp (ix)

time4:
	pop hl
	ld (retarea),iy
	ld (retarea+2),bc
	ld (retarea+4),hl
	ld hl,(timebuf+2)
	push hl
	ld hl,(timebuf)
	push hl
	ld hl,0
	push hl
	ld hl,50
	push hl
	call .dvu4
	ld iy,(retarea)
	ld bc,(retarea+2)
	ld hl,(retarea+4)
	jp (hl)
ux_exit:
	call uxfinish
	ld c,reset
	call bdos
	! no return

ux_creat:
	call emptyfile
	jr c,openfailed
	pop hl
	call parsename
	pop hl			! file mode, not used under CP/M
	push iy
	push ix
	push ix
	pop de
	ld c,delete
	call bdos
	pop de
	ld c,makefile
	call bdos
	pop iy
	ld l,1
	jr afteropen
ux_open:
	call emptyfile
	jr nc,1f
openfailed:
	pop hl
	pop hl		! remove params
	ld hl,EMFILE
	push hl
	push hl ; jr rtn
1:
	pop hl		! filename
	call parsename
	push iy
	ld c,open
	push ix
	pop de
	call bdos
	pop iy
	pop hl
afteropen:
	inc a
	jr nz,1f
	ld hl,ENOENT
	push hl
	push hl ; jr rtn
1:
	inc l
	ld (ix+openflag),l
	xor a
	ld (ix+nleft),a
	ld (ix+zcount),a
	ld (ix+zsave),26
	bit 1,l
	jr z,2f
	ld (ix+nleft),128
2:
	ld (ix+position),a
	ld (ix+position+1),a
	push ix
	pop hl
	push bc
	ld b,128
3:	dec hl
	ld (hl),26
	djnz 3b
	pop bc
	ld (ix+iopointer+1),h
	ld (ix+iopointer),l
	ld h,a
	ld l,(ix+fildes)
	push hl
	ld l,a
	push hl ; jr rtn

ux_read:
	pop hl
	ld a,l
	sub 3
	jp p,readfile
	ld a,(ttymode+4)
	bit 5,a
	jr z,1f			! not raw
	push iy
! #ifdef	CPM1
!raw echo interface
	ld c,consolein
	call bdos
! #else
! !no echo interface
! 4:
! 	ld c,diconio
! 	ld e,0xff
! 	call bdos
! 	or a
! 	jr z,4b
!end of no echo interface
! #endif
	pop iy
	pop hl
	ld (hl),a
	pop hl
	ld hl,1
	push hl
	ld hl,0
	push hl ; jr rtn
1:
	ld hl,sibuf+1		! read from console assumed
	dec (hl)
	jp p,2f
	dec hl			! go read console line
	ld (hl),80		! max line length
	push iy
	push hl
	ld c,readconsole
	ex de,hl
	call bdos
	ld c,writeconsole
	ld e,'\n'
	call bdos
	pop hl
	pop iy
	inc hl
	inc (hl)
	ld (siptr),hl		! ready for transfer
	push hl
	ld e,(hl)
	ld d,0
	add hl,de
	ld (hl),'\r'
	inc hl
	ld (hl),'\n'
	pop hl
2:
	push bc
	pop ix
	ld b,(hl)
	inc b			! bytes remaining
	pop hl			! copy to
	pop de			! bytes wanted (probably 512)
	push ix
	ld ix,(siptr)		! copy from
	xor a			! find out minimum of ramaining and wanted
	or d
	jr nz,3f		! more than 255 wanted (forget that)
	ld a,b
	cp e
	jp m,3f			! not enough remaining
	ld b,e
3:
	ld c,b			! keep copy
0:
	inc ix
	ld a,(ix)
	ld (hl),a
	inc hl
	djnz 0b
	ld a,(sibuf+1)
	sub c
	inc a
	ld (sibuf+1),a
	ld (siptr),ix
	pop hl
	push bc
	ld c,b
	push bc			! load 0
	ld b,h
	ld c,l
	ld ix,(return) ; jp (ix)
readfile:
	call findfile
	pop de
	pop hl			! count
	push bc
	ld bc,0
0:
	xor a
	or l
	jr z,1f
	dec l
3:
! warning: this may not work if zcount overflows
	ld a,(ix+zcount)
	or a
	jr nz,5f
	ld a,(ix+zsave)
	cp 26
	jr z,4f
	ld (ix+zsave),26
	jr 8f
4:
	call getchar
	jr c,2f
	ld (de),a
	sub 26		! CTRL-Z
	jr z,7f
	ld a,(ix+zcount)
	or a
	jr z,6f
	ld a,(de)
	ld (ix+zsave),a
5:
	ld a,26
	dec (ix+zcount)
8:
	ld (de),a
6:
	inc de
	inc bc
	jr 0b
1:
	dec l
	dec h
	jp p,3b
2:
	pop hl
	push bc
	ld b,h
	ld c,l
	ld hl,0
	push hl ; jr rtn
7:
	inc (ix+zcount)
	jr 4b

ux_write:
	pop hl
	ld a,l
	sub 3
	jp p,writefile
	pop hl			! buffer address
	pop de			! count
	push de
	ld ix,0
	push ix
	push bc
	ld b,e			! count now in 'db'
0:
	ld a,b
	or a
	jr nz,1f
	ld a,d
	or a
	jr nz,2f
	pop bc
	ld ix,(return) ; jp (ix)
2:
	dec d
1:
	dec b
	ld e,(hl)
	inc hl
	push bc
	push de
	push hl
	ld c,writeconsole
	call bdos
	pop hl
	pop de
	pop bc
	jr 0b
writefile:
	call findfile
	pop de
	pop hl			! count
	push bc
	ld bc,0
0:
	xor a
	or l
	jr z,1f
	dec l
3:
	ld a,(de)
	inc de
	call putchar
	jr c,4f
	inc bc
	jr 0b
1:
	dec l
	dec h
	jp p,3b
	ld ix,0
2:
	pop hl
	push bc
	ld b,h
	ld c,l
	push ix
	ld ix,(return) ; jp (ix)
4:
	ld ix,ENOSPC
	jr 2b

ux_unlink:
	pop hl
	ld ix,fcb
	call parsename
	push bc
	ld c,delete
	ld de,fcb
	call bdos
	pop bc
	inc a
	jr nz,1f
	ld hl,ENOENT
	push hl ; jr rtn
1:
	ld hl,0
	push hl ; jr rtn

ux_getpid:
	ld hl,12345		! nice number
	push hl ; jr rtn






retarea: .word 0	! base of buffer for result values (max 8 bytes)
	 .word 0
	 .word 0
	 .word 0

trapproc:
	.word 0

nextp:	.byte 0

header:
ntext:	.word 0
ndata:	.word 0
nproc:	.word 0
entry:	.word 0
nline:	.word 0

hp:	.word 0
pb:	.word 0
pd:	.word 0
