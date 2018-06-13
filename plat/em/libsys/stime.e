#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $stime
 pro $stime,0
#if EM_WSIZE<4
 lol 0
 loi 4
#else
 lil 0
#endif
 loc 25
 mon
 zne *1
 loc 0
 ret EM_WSIZE
1
 ste errno
 loc -1
 ret EM_WSIZE
 end
