#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $sigtrp
 pro $sigtrp,0
 ldl 0
 loc 48
 mon
 zeq *1
 ste errno
 loc -1
1
 ret EM_WSIZE
 end
