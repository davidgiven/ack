#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $lock
 pro $lock,0
 lol 0
 loc 53
 mon
 zne *1
 loc 0
 ret EM_WSIZE
1
 ste errno
 loc -1
 ret EM_WSIZE
 end
