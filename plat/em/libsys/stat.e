#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $stat
 pro $stat,0
 lal 0
 loi 2*EM_PSIZE
 loc 18
 mon
 zne *1
 loc 0
 ret EM_WSIZE
1
 ste errno
 loc -1
 ret EM_WSIZE
 end
