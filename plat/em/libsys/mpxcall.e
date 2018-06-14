#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $mpxcall
 pro $mpxcall,0
 lal 0
 loi EM_PSIZE+EM_WSIZE
 loc 56
 mon
 zne *1
 loc 0
 ret EM_WSIZE
1
 ste errno
 loc -1
 ret EM_WSIZE
 end
