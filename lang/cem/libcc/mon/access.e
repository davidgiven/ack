#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $access
 pro $access,0
 lal 0
 loi EM_WSIZE+EM_PSIZE
 loc 33
 mon
 zne *1
 loc 0
 ret EM_WSIZE
1
 ste errno
 loc -1
 ret EM_WSIZE
 end
