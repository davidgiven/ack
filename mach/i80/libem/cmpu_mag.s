.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Does a tristate unsigned comparison of hl <> de.
! Returns m flag if hl < de.
! Returns p flag if hl >= de.
! This doesn't set z coherently.

.define .cmpu_mag
.cmpu_mag:
    mov a, e
    sub l
    mov a, d
    sbb h
    ret
