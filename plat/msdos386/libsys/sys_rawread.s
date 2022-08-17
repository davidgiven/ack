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

.extern pmode_ds
.extern pmode_cs
.extern rmode
.extern transfer_buffer_ptr
.extern interrupt_ptr

! Read bytes from a file descriptor.  These routines do not do any
! translation between CRLF and LF line endings.
!
! Note that, for MS-DOS, a raw "write" request of zero bytes will truncate
! (or extend) the file to the current file position.

.define __sys_rawread
__sys_rawread:
	int 3

