.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Does a tristate signed comparison of hl <> de.
! Returns m flag if hl < de.
! Returns p flag if hl >= de.
! This doesn't set z coherently.

.define .cmps_mag
.cmps_mag:
    mov a, d
    xra h
    jp .cmpu_mag ! signs are the same, so an unsigned comparison will do
    xra h        ! set A=H and set the sign flag
    ral          ! move sign flag into carry
    ret
