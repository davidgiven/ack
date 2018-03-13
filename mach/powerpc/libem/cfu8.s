.sect .text; .sect .rom; .sect .data; .sect .bss

.sect .text

! Converts a 64-bit double into a 32-bit unsigned integer.
!
! Stack: ( double -- uint )

.define .cfu8
.cfu8:
	lfd f1, 0(sp)                   ! f1 = value to convert
	lis r3, ha16[.fs_80000000]
	lfs f2, lo16[.fs_80000000](r3)  ! f2 = 2**31
	fsub   f1, f1, f2
	fctiwz f1, f1         ! convert value - 2**31
	stfd   f1, 0(sp)
	lwz   r3, 4(sp)
	xoris r3, r3, 0x8000  ! add 2**31
	stw   r3, 4(sp)
	addi  sp, sp, 4
	blr

.sect .rom
.fs_80000000:
	!float 2.147483648e+9 sz 4
	.data1 0117,00,00,00

! Freescale and IBM provide an example using fsel to select value or
! value - 2**31 for fctiwz.  The following code adapts Freescale's
! _Programming Environments Manual for 32-Bit Implementations of the
! PowerPC Architecture_, section C.3.2, pdf page 557.
!
! Given f2 = value clamped from 0 to 2**32 - 1, f4 = 2**31, then
!	fsub	f5, f2, f4
!	fcmpu	cr2, f2, f4
!	fsel	f2, f5, f5, f2
!	fctiwz	f2, f2
!	stfdu	f2, 0(sp)
!	lwz	r3, 4(sp)
!	blt	cr2, 1f
!	xoris	r3, r3, 0x8000
! 1: yields r3 = the converted value.
!
! Debian's clang 3.5.0-10 and gcc 4.9.2-10 don't clamp the value
! before conversion.  They avoid fsel and use the conditional branch
! to pick between 2 fctwiz instructions.
!
! PowerPC 601 lacks fsel (but kernel might trap and emulate fsel).
! PowerPC 603, 604, G3, G4, G5 have fsel.
