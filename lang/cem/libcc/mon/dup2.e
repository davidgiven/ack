#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $dup2
 pro $dup2,0
 lal 0
 loi 2*EM_WSIZE
 loc 64
 ior EM_WSIZE
 loc 41
 mon
 zeq *1
 ste errno
 loc -1
1
 ret EM_WSIZE
 end
