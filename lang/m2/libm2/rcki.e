#
#include <em_abs.h>

 mes 2,EM_WSIZE,EM_PSIZE
 exp $rcki
 pro $rcki, 0

 lal EM_PSIZE
 loi EM_WSIZE
 lal 0
 loi EM_PSIZE
 loi EM_WSIZE
 cmi EM_WSIZE
 zlt *1
 lal EM_PSIZE
 loi EM_WSIZE
 lal 0
 loi EM_PSIZE
 adp EM_WSIZE
 loi EM_WSIZE
 cmi EM_WSIZE
 zgt *1
 ret 0
1
 loc ERANGE
 trp
 ret 0
 end 0
