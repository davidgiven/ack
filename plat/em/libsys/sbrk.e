#
#include <em_abs.h>

 mes 2,EM_WSIZE,EM_PSIZE

.1
 bss EM_PSIZE,0,0
.2
 bss EM_WSIZE,0,0

 pro $ctch,0
 lol 0
 loc EHEAP
 beq *1
 lae .1
 loi EM_PSIZE
 sig
 asp EM_PSIZE
 lol 0
 trp
 rtt
1
 loc 1
 ste .2
 rtt
 end


 exp $sbrk
 pro $sbrk,0
 lor 2
 lor 2
 lpi $ctch
 sig
 lae .1
 sti EM_PSIZE
 loc 0
 ste .2
 lol 0
 ads EM_WSIZE	; this is the new heap pointer, but watch out for overflow!
 dup EM_PSIZE
 lor 2
 cmp		; compare with old heap pointer
 zlt *1
 str 2          ; The - possibly - occurring trap is caught
 lae .1
 loi EM_PSIZE
 sig
 asp EM_PSIZE
 loe .2
 zgt *1
 ret EM_PSIZE
1
 asp EM_PSIZE
 loc -1
 loc EM_WSIZE
 loc EM_PSIZE
 cii
 ret EM_PSIZE
 end
