        # $Header$
#include "em_abs.h"
.globl  .lar

.lar:
	cmpl    r0,$4
	bneq    Lillins
	jmp     .lar4

.globl  .sar

.sar:
	cmpl    r0,$4
	bneq    Lillins
	jmp     .sar4

.globl  .aar

.aar:
	cmpl    r0,$4
	bneq    Lillins
	jmp     .aar4

Lillins:
	pushl	$EILLINS
	jmp     .fat
