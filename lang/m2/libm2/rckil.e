#
#include <em_abs.h>

 mes 2,EM_WSIZE,EM_PSIZE
 exp $rckil
 pro $rckil, 0

 lal EM_PSIZE
 loi EM_LSIZE
 lal 0
 loi EM_PSIZE
 loi EM_LSIZE
 cmi EM_LSIZE
 zlt *1
 lal EM_PSIZE
 loi EM_LSIZE
 lal 0
 loi EM_PSIZE
 adp EM_LSIZE
 loi EM_LSIZE
 cmi EM_LSIZE
 zgt *1
 ret 0
1
 loc ERANGE
 trp
 ret 0
 end 0
