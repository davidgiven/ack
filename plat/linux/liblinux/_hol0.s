#
! $Source: /cvsroot/tack/Ack/plat/linux386/libsys/_hol0.s,v $
! $State: Exp $
! $Revision: 1.1 $

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

! 
! This data block is used to store information about the current line number
! and file.

.define hol0
.comm hol0, 8
