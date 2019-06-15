#
#include "asm.h"

.define _cpm_overwrite_ccp
_cpm_overwrite_ccp:
	mvi a, 0xaf              ! 0xaf = xor a = clear carry bit
	sta _cpm_exit
    lhld _cpm_ramtop
    lxi d, 0x800
    dad d
    shld _cpm_ramtop
	ret
