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


 exp $brk
 pro $brk,0
 lpi $ctch
 sig
 lae .1
 sti EM_PSIZE
 loc 0
 ste .2
 lal 0
 loi EM_PSIZE
 str 2          ; The - possibly - occurring trap is caught
 lae .1
 loi EM_PSIZE
 sig
 asp EM_PSIZE
 loe .2
 zgt *1
 zer EM_PSIZE
 ret EM_PSIZE
1
 loc -1
 loc EM_WSIZE
 loc EM_PSIZE
 cii
 ret EM_PSIZE
 end
