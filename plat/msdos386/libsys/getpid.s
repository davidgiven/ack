#
! $Source$
! $State$
! $Revision$

! Copyright (c) 2021--2022 TK Chia
!
! The authors hereby grant permission to use, copy, modify, distribute,
! and license this software and its documentation for any purpose, provided
! that existing copyright notices are retained in all copies and that this
! notice is included verbatim in any distributions. No written agreement,
! license, or royalty fee is required for any of the authorized uses.
! Modifications to this software may be copyrighted by their authors
! and need not follow the licensing terms described here, provided that
! the new terms are clearly indicated on the first page of each file where
! they apply.

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

! Get the current process identifier.  For MS-DOS, use the Program Segment
! Prefix (PSP) segment as the PID, unless the system supports an actual
! `getpid' syscall, e.g. European MS-DOS 4.0.
!
! (Note that pid_t is a 32-bit type.  This is to allow for PSP segments and
! MS-DOS PIDs above 0x7FFF.)

.define _getpid
_getpid:
	movb ah, 0x87
	stc
	int 0x21
	jnc .eur_dos
	movb ah, 0x51
	int 0x21
	xchg bx, ax
.eur_dos:
	xor dx, dx
	ret
