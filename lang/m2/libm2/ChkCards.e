#include <m2_traps.h>

 mes 2,EM_WSIZE,EM_PSIZE

 pro $addu,0
 loc -1
 lol 0
 sbu EM_WSIZE
 lol EM_WSIZE
 cmu EM_WSIZE
 zle *1
 loc M2_UOVFL
 trp
1
 lol 0
 lol EM_WSIZE
 adu EM_WSIZE
 stl EM_WSIZE
 ret 0
 end 0

 pro $mulu,0
 lol 0
 zeq *1
 loc -1
 lol 0
 dvu EM_WSIZE
 lol EM_WSIZE
 cmu EM_WSIZE
 zle *1
 loc M2_UOVFL
 trp
1
 lol 0
 lol EM_WSIZE
 mlu EM_WSIZE
 stl EM_WSIZE
 ret 0
 end 0
