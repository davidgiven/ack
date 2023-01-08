#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

.sect .bss

! This data block is used to store information about the current line number
! and file.

.define hol0
.comm hol0, 8
