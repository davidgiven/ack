#include <m2_traps.h>

 mes 2,EM_WSIZE,EM_PSIZE

 exp $addu
 exp $subu
 exp $mulu
#if EM_WSIZE < EM_LSIZE
 exp $addul
 exp $subul
 exp $mulul
#endif

 pro $addu,0
 loc -1
 lol 0
 sbu EM_WSIZE
 lol EM_WSIZE
 cmu EM_WSIZE
 zge *1
 loc M2_UOVFL
 trp
1
 lol 0
 lol EM_WSIZE
 adu EM_WSIZE
 stl EM_WSIZE
 ret 0
 end 0

#if EM_WSIZE < EM_LSIZE
 pro $addul,0
 ldc -1
 ldl 0
 sbu EM_LSIZE
 ldl EM_LSIZE
 cmu EM_LSIZE
 zge *1
 loc M2_UOVFL
 trp
1
 ldl 0
 ldl EM_LSIZE
 adu EM_LSIZE
 sdl EM_LSIZE
 ret 0
 end 0
#endif

 pro $mulu,0
 lol 0
 zeq *1
 loc -1
 lol 0
 dvu EM_WSIZE
 lol EM_WSIZE
 cmu EM_WSIZE
 zge *1
 loc M2_UOVFL
 trp
1
 lol 0
 lol EM_WSIZE
 mlu EM_WSIZE
 stl EM_WSIZE
 ret 0
 end 0

#if EM_WSIZE < EM_LSIZE
 pro $mulul,0
 ldl 0
 ldc 0
 cmu EM_LSIZE
 zeq *1
 ldc -1
 ldl 0
 dvu EM_LSIZE
 ldl EM_LSIZE
 cmu EM_LSIZE
 zge *1
 loc M2_UOVFL
 trp
1
 ldl 0
 ldl EM_LSIZE
 mlu EM_LSIZE
 sdl EM_LSIZE
 ret 0
 end 0
#endif

 pro $subu,0
 lol EM_WSIZE
 lol 0
 cmu EM_WSIZE
 zge *1
 loc M2_UOVFL
 trp
1
 lol EM_WSIZE
 lol 0
 sbu EM_WSIZE
 stl EM_WSIZE
 ret 0
 end 0

#if EM_WSIZE < EM_LSIZE
 pro $subul,0
 ldl EM_LSIZE
 ldl 0
 cmu EM_LSIZE
 zge *1
 loc M2_UOVFL
 trp
1
 ldl EM_LSIZE
 ldl 0
 sbu EM_LSIZE
 sdl EM_LSIZE
 ret 0
 end 0
#endif
