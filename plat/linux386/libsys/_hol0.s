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

! 
! This data block is used to store information about the current line number
! and file.

.define hol0
.comm hol0, 8
